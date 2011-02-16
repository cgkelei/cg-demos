#include "MathUtil.h"

inline float clamp(float val, float min, float max)
{
	return val < min ? min : (val > max ? max : val);
}
