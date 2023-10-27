#pragma once

#include <stddef.h>

#include "chunk.h"
#include "utils/math.h"

typedef struct world_t
{
	vector3ul_t size;
	vector3ul_t chunk_size;
	size_t chunk_capacity;
	chunk_t* chunks;
	vector3i_t max_chunks;
	vector3i_t min_chunks;
} world_t;

world_t allocate_world(size_t size_x, size_t size_y, size_t size_z);
void free_world(world_t* world);
void fill_world(world_t* world);
chunk_t* get_chunk(world_t* world, int x, int y, int z);
int get_chunk_index(world_t* world, int x, int y, int z);
// Assumes dir are either -1, 0, 1
void unload_and_load_new_chunks(world_t* world, int dir_x, int dir_y, int dir_z);
void generate_world_chunks_model(world_t* world);
void unload_world_chunks_model(world_t* world);
void render_world(world_t* world);
void render_world_chunks_borders(world_t* world);
