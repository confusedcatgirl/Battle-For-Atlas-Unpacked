import "shaders/MRender/MR_ViewConstants.h"
import "shaders/MRender/MR_DepthBuffer.h"

MR_Sampler2D SSR_FramebufferLate : MR_Framebuffer0 { wrap = clamp, filter = point };
MR_Sampler2D SSR_FramebufferEarly : MR_Framebuffer1 { wrap = clamp, filter = point };
MR_Sampler2D SSR_FramebufferOpaque : MR_Framebuffer2 { wrap = clamp, filter = point };
MR_Sampler2D SSR_Noise { texture = "textures/random16x16.tga", filter = point };
