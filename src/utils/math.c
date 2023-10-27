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

float lerpf(float a, float b, float t)
{
	return (1.0f - t) * a + t * b;
}

float smoothstepf(float a, float b, float t)
{
	t = clampf((t - a) / (b - a), 0.0f, 1.0f);
	
	return lerpf(a, b, t * t * (3.0f - 2.0f * t));
}

float perlin_noise_3d(int x, int y, int z)
{
	float s1 = sinf(x / 5.0f);
	float s2 = sinf(y / 5.0f);
	float s3 = cosf(z / 5.0f);

	float sum = s1 + s2 + s3;
	return (sum + 3.0f) / 6.0f;
}

static const int  SEED = 1;

static const unsigned char  HASH[] = {
    208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
    185,248,251,245,28,124,204,204,76,36,220,107,28,234,163,202,224,245,128,167,204,
    9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
    70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
    203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
    164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
    228,108,245,148,140,40,35,195,38,58,207,65,215,253,65,85,208,76,62,3,237,55,89,
    232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
    193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
    101,120,99,3,186,86,99,41,237,203,111,79,1,135,158,42,30,154,120,67,87,167,
    135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
    114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219
};

static int noise2(int x, int y)
{
	int  y_index = (y + SEED) % 256;
	if (y_index < 0)
		y_index += 256;

	int  x_index = (HASH[y_index] + x) % 256;
	if (x_index < 0)
		x_index += 256;

	return HASH[x_index];
}

static float perlin_noise_2d_helper(float x, float y)
{
    const int  x_int = floorf(x);
    const int  y_int = floorf(y);
    const float  x_frac = x - x_int;
    const float  y_frac = y - y_int;
    const int  s = noise2(x_int    , y_int);
    const int  t = noise2(x_int + 1, y_int);
    const int  u = noise2(x_int    , y_int + 1);
    const int  v = noise2(x_int + 1, y_int + 1);
    const float  low = smoothstepf(s, t, x_frac);
    const float  high = smoothstepf(u, v, x_frac);
    const float  result = smoothstepf(low, high, y_frac);
    return result;
}

float perlin_noise_2d(int x, int y, float freq, int depth)
{
    float xa = x * freq;
    float ya = y * freq;
    float amp = 1.0f;
    float fin = 0;
    float div = 0.0f;
    for (int i = 0; i < depth; i++)
    {
        div += 256 * amp;
        fin += perlin_noise_2d_helper(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }
    return (fin / div);
}

// vector3i_t

bool vecto3i_equals(vector3i_t v, vector3i_t w)
{
	return v.x == w.x && v.y == w.y && v.z == w.z;
}
