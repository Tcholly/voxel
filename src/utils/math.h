#pragma once

#include <stdbool.h>

typedef struct
{
	int x, y, z;
} vector3i_t;


int clamp(int value, int min, int max);
float clampf(float value, float min, float max);

float perlin_noise_3d(int x, int y, int z);

// vector3i_t

bool vecto3i_equals(vector3i_t v, vector3i_t w);
