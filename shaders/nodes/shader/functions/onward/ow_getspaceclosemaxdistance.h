import "shaders/modules/graphics/onward/ow_view_constants.h"

float OW_GetSpaceCloseMaxDistance()
{
	return 1.0f / OW_ViewConstants.classification[OW_SPACE_CLOSE_CATEGORY].x;
}
