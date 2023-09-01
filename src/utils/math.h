#pragma once

typedef struct
{
	int x, y, z;
} vector3i_t;


int clamp(int value, int min, int max);
float clampf(float value, float min, float max);
