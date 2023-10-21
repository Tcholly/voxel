#include "math.h"

#include <assert.h>
#include <math.h>

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

float perlin_noise_3d(int x, int y, int z)
{
	float s1 = sinf(x / 5.0f);
	float s2 = sinf(y / 5.0f);
	float s3 = cosf(z / 5.0f);

	float sum = s1 + s2 + s3;
	return (sum + 3.0f) / 6.0f;
}

// vector3i_t

bool vecto3i_equals(vector3i_t v, vector3i_t w)
{
	return v.x == w.x && v.y == w.y && v.z == w.z;
}
