#include "math.h"

#include <assert.h>

int clamp(int value, int min, int max)
{
	assert(min < max);
	if (value > max) return max;
	if (value < min) return min;
	return value;
}

float clampf(float value, float min, float max)
{
	assert(min < max);
	if (value > max) return max;
	if (value < min) return min;
	return value;
}
