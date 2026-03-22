#!/usr/bin/env python3
"""
Starlink: Battle for Atlas - SDF Extractor
Based on rouge_sdf by m0xf (The Division version),
adapted for Snowdrop v0x17 (Starlink PC).
"""

import sys
import os
import zlib
import struct
from pathlib import Path

CHUNK_SIZE = 0x10000  # 65536

def zlib_decompress(data: bytes) -> bytes:
    try:
        return zlib.decompress(data)
    except zlib.error:
        try:
            return zlib.decompress(data, -15)
        except zlib.error as e:
            raise IOError(f"zlib decompression failed: {e}")


# ---------------------------------------------------------------------------
# File manager
# ---------------------------------------------------------------------------
class SdfFileManager:
    def __init__(self, base_dir: Path, base_name: str):
        self.base_dir = base_dir
        self.base_name = base_name
        self._handles = {}
        self._sizes   = {}

    def _pkg_path(self, package_id: int) -> Path:
        layer = chr(ord('A') + package_id // 1000)
        return self.base_dir / f"{self.base_name}-{layer}-{package_id:04d}.sdfdata"

    def size(self, package_id: int) -> int:
        if package_id not in self._sizes:
            p = self._pkg_path(package_id)
            if p.exists():
                sz = p.stat().st_size
                self._sizes[package_id] = 0 if sz <= 5 else sz
            else:
                self._sizes[package_id] = 0
        return self._sizes[package_id]

    def read(self, package_id: int, offset: int, length: int) -> bytes:
        if length == 0:
            return b""
        if package_id not in self._handles:
            self._handles[package_id] = open(self._pkg_path(package_id), "rb")
        fh = self._handles[package_id]
        fh.seek(offset)
        data = fh.read(length)
        if len(data) != length:
            raise IOError(f"Short read from pkg {package_id}: wanted {length}, got {len(data)} at {offset:#x}")
        return data

    def read_spanning(self, package_id: int, offset: int, length: int) -> bytes:
        """Read bytes spanning multiple sdfdata files, skipping placeholders."""
        result = bytearray()
        remaining = length
        cur_id  = package_id
        cur_off = offset
        while remaining > 0:
            sz = self.size(cur_id)
            if sz == 0:
                cur_id += 1
                cur_off = 0
                continue
            available = sz - cur_off
            if available <= 0:
                cur_id += 1
                cur_off = 0
                continue
            to_read = min(remaining, available)
            result += self.read(cur_id, cur_off, to_read)
            remaining -= to_read
            cur_id += 1
            cur_off = 0
        return bytes(result)

    def close(self):
        for fh in self._handles.values():
            fh.close()
        self._handles.clear()


# ---------------------------------------------------------------------------
# TOC parser  (mirrors rouge_sdf main.cpp exactly)
# ---------------------------------------------------------------------------
class SdfToc:
    def __init__(self, toc_path: Path):
        self.toc_path = toc_path
        with open(toc_path, "rb") as f:
            self.raw = f.read()
        self.pos = 0
        self.dds_headers = []   # list of (usedBytes, bytes[0x94])
        self.toc_block   = b""
        self.toc_pos     = 0

    # -- raw file readers ----------------------------------------------------
    def _u8(self):  v = self.raw[self.pos];                                    self.pos += 1; return v
    def _u32(self): v = struct.unpack_from("<I", self.raw, self.pos)[0];       self.pos += 4; return v
    def _u64(self): v = struct.unpack_from("<Q", self.raw, self.pos)[0];       self.pos += 8; return v
    def _read(self, n): d = self.raw[self.pos:self.pos+n];                     self.pos += n; return d

    # -- toc block readers ---------------------------------------------------
    def _tb(self):  v = self.toc_block[self.toc_pos];                          self.toc_pos += 1; return v
    def _tb_read(self, n): d = self.toc_block[self.toc_pos:self.toc_pos+n];   self.toc_pos += n; return d

    def _tb_varint(self, n):
        """Read n-byte little-endian integer from toc block."""
        raw = self.toc_block[self.toc_pos:self.toc_pos+n]
        self.toc_pos += n
        return int.from_bytes(raw, "little")

    # -- header --------------------------------------------------------------
    def parse_header(self):
        # v0x17 layout (Starlink PC):
        # [00] magic
        # [04] version
        # [08] decomp_size_filetree
        # [0c] comp_size_metadata   (small block: DDS headers etc.)
        # [10] comp_size_filetree   (large block: file tree)
        # [14] zero
        # [18] block1count
        # [1c] dds_index_count
        # [20] SdfTocId (48 bytes)
        # [50] sign_flag
        # [51] signature (0x140 bytes if sign_flag != 0)
        # [191] metadata block (compressed, comp_size=[0c])
        # [191+comp_metadata] filetree block (compressed, comp_size=[10])
        magic = self._read(4)
        assert magic == b"WEST", f"Bad magic: {magic}"
        self.version       = self._u32()
        decomp_filetree    = self._u32()   # [08]
        comp_metadata      = self._u32()   # [0c]
        comp_filetree      = self._u32()   # [10]
        _zero              = self._u32()   # [14]
        _block1count       = self._u32()   # [18]
        _dds_index_count   = self._u32()   # [1c]

        # SdfTocId (48 bytes) + sign_flag + optional signature
        self._read(48)
        sign_flag = self._u8()
        if sign_flag:
            self._read(0x140)

        # Metadata block: contains DDS headers at the end
        meta_comp = self._read(comp_metadata)
        meta = zlib_decompress(meta_comp)

        # DDS entries start where the first "DDS " magic appears (minus 4 for usedBytes).
        # Each entry: u32 usedBytes + 0x94 bytes = 152 bytes total.
        DDS_ENTRY  = 4 + 0x94  # 152
        DDS_MAGIC  = b"DDS "
        first_magic = meta.find(DDS_MAGIC)
        if first_magic < 4:
            raise ValueError(f"DDS magic not found in metadata block (pos={first_magic})")
        dds_offset = first_magic - 4  # start of first usedBytes field
        dds_count  = (len(meta) - dds_offset) // DDS_ENTRY
        self.dds_headers = []
        for i in range(dds_count):
            off  = dds_offset + i * DDS_ENTRY
            used = struct.unpack_from("<I", meta, off)[0]
            data = meta[off+4 : off+4+0x94]
            self.dds_headers.append((used, data))

        # File tree block
        filetree_comp = self._read(comp_filetree)
        self.toc_block = zlib_decompress(filetree_comp)
        assert len(self.toc_block) == decomp_filetree, \
            f"TOC decomp mismatch: {len(self.toc_block)} != {decomp_filetree}"

        print(f"  Version:     {self.version:#x}")
        print(f"  DDS headers: {dds_count}  (from {len(meta)} byte metadata block)")
        print(f"  TOC block:   {decomp_filetree} bytes decompressed")

    # -- file tree parser (mirrors rouge_sdf FileTree::ParseNames) -----------
    def parse_names(self, name=""):
        ch = self._tb()

        if 1 <= ch <= 0x1f:
            # String segment
            part = self._tb_read(ch).decode("latin-1")
            yield from self.parse_names(name + part)

        elif ord('A') <= ch <= ord('Z'):
            ch     -= ord('A')
            count1  = ch & 7
            flag1   = (ch >> 3) & 1

            if count1:
                _strange_id = self._tb_varint(4)
                ch2        = self._tb()
                byte_count = ch2 & 3
                _byte_val  = ch2 >> 2
                dds_type   = self._tb_varint(byte_count)  # index into dds_headers

                for chunk_index in range(count1):
                    ch3 = self._tb()
                    comp_size_bytes   = (ch3 & 3) + 1
                    offset_bytes      = (ch3 >> 2) & 7
                    has_compression   = (ch3 >> 5) & 1

                    decomp_size = self._tb_varint(comp_size_bytes)
                    comp_size   = 0
                    if has_compression:
                        comp_size = self._tb_varint(comp_size_bytes)

                    pkg_offset = self._tb_varint(offset_bytes) if offset_bytes else 0
                    pkg_id     = self._tb_varint(2)

                    # Page sizes for multi-page blocks
                    comp_size_array = []
                    if has_compression:
                        page_count = (decomp_size + 0xffff) >> 16
                        if page_count > 1:
                            for _ in range(page_count):
                                comp_size_array.append(self._tb_varint(2))

                    # fileId – always present (rouge_sdf reads it for all versions)
                    _file_id = self._tb_varint(4)

                    # If single compressed block, put comp_size in array
                    if not comp_size_array and has_compression:
                        comp_size_array.append(comp_size)

                    use_dds = (byte_count != 0 and chunk_index == 0)
                    append  = (chunk_index != 0)

                    yield {
                        "name":       name,
                        "pkg_id":     pkg_id,
                        "pkg_offset": pkg_offset,
                        "decomp":     decomp_size,
                        "chunks":     comp_size_array,   # empty = uncompressed
                        "dds_type":   dds_type,
                        "use_dds":    use_dds,
                        "append":     append,
                    }

            if flag1:
                ch3 = self._tb()
                # skip ch3 pairs of bytes
                self._tb_read(ch3 * 2)

        else:
            # Branch node (search tree)
            offset = self._tb_varint(4)
            # Save current position, parse left subtree, then jump to offset
            saved = self.toc_pos
            yield from self.parse_names(name)
            self.toc_pos = offset
            yield from self.parse_names(name)


# ---------------------------------------------------------------------------
# Extractor
# ---------------------------------------------------------------------------
def extract(toc_path: Path, out_dir: Path):
    print(f"Parsing TOC: {toc_path}")
    toc = SdfToc(toc_path)
    toc.parse_header()

    base_name = toc_path.stem  # "sdf"
    mgr = SdfFileManager(toc_path.parent, base_name)
    out_dir.mkdir(parents=True, exist_ok=True)

    ok = skipped = errors = 0

    for entry in toc.parse_names():
        name       = entry["name"]
        pkg_id     = entry["pkg_id"]
        offset     = entry["pkg_offset"]
        decomp     = entry["decomp"]
        chunks     = entry["chunks"]
        dds_type   = entry["dds_type"]
        use_dds    = entry["use_dds"]
        append     = entry["append"]

        if not name:
            skipped += 1
            continue

        if mgr.size(pkg_id) == 0:
            skipped += 1
            continue

        out_path = out_dir / name.replace("\\", os.sep)
        out_path.parent.mkdir(parents=True, exist_ok=True)

        try:
            # --- Decompress / read data ---
            if not chunks:
                # Uncompressed
                data = mgr.read_spanning(pkg_id, offset, decomp)
            else:
                # Compressed (one or more pages)
                result    = bytearray()
                cur_off   = offset
                remaining = decomp
                for chunk_comp in chunks:
                    page_decomp = min(CHUNK_SIZE, remaining)
                    if chunk_comp == 0 or chunk_comp == page_decomp:
                        # Stored uncompressed
                        block = mgr.read_spanning(pkg_id, cur_off, page_decomp)
                        result += block
                        cur_off += page_decomp
                    else:
                        block = mgr.read_spanning(pkg_id, cur_off, chunk_comp)
                        result += zlib_decompress(block)
                        cur_off += chunk_comp
                    remaining -= page_decomp
                data = bytes(result)

            # --- Prepend DDS header if needed ---
            if use_dds and dds_type < len(toc.dds_headers):
                used, hdr_bytes = toc.dds_headers[dds_type]
                data = hdr_bytes[:used] + data

            # --- Write output ---
            if append:
                with open(out_path, "ab") as f:
                    f.write(data)
            else:
                out_path.write_bytes(data)

            ok += 1
            if ok % 500 == 0:
                print(f"  {ok} files extracted...")

        except Exception as e:
            print(f"  ERROR [{name}]: {e}")
            errors += 1

    mgr.close()
    print(f"\nDone: {ok} extracted, {skipped} skipped, {errors} errors")


# ---------------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python sdf_extract.py <path/to/sdf.sdftoc> <output_dir>")
        sys.exit(1)
    extract(Path(sys.argv[1]), Path(sys.argv[2]))