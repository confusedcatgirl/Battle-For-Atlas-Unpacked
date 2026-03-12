import "shaders/modules/graphics/onward/common/foliage_constants.h"
MR_Sampler2D GrassMaskLookup : OW_GrassMaskLookup;
MR_Sampler2D ForceFieldLookup : OW_ForceFieldLookup { wrap = clamp, filter = linear };
