#pragma once

#include <stddef.h>

#include "chunk.h"
#include "utils/math.h"

typedef struct world_t
{
	vector3i_t size;
	size_t chunk_capacity;
	chunk_t* chunks;
} world_t;

world_t allocate_world(size_t size_x, size_t size_y, size_t size_z);
void free_world(world_t* world);
void fill_world(world_t* world);
chunk_t* get_chunk(world_t* world, int x, int y, int z);
void generate_world_chunks_model(world_t* world);
void unload_world_chunks_model(world_t* world);
void render_world(world_t* world);
void render_world_chunks_borders(world_t* world);
