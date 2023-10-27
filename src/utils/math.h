#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct
{
	int x, y, z;
} vector3i_t;

typedef struct
{
	size_t x, y, z;
} vector3ul_t;


int clamp(int value, int min, int max);
float clampf(float value, float min, float max);
float lerpf(float a, float b, float t);
float smoothstepf(float a, float b, float t);

float perlin_noise_3d(int x, int y, int z);
float perlin_noise_2d(int x, int y, float freq, int depth);

// vector3i_t

bool vecto3i_equals(vector3i_t v, vector3i_t w);
