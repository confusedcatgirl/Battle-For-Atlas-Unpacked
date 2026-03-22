import "shaders/modules/graphics/environment.h"

float IsUIPass()
{
	return Gfx_Environment.isInUIPass ? 1.0 : 0.0;
}
