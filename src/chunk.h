#pragma once

#include <raylib.h>
#include <stddef.h>

#include "utils/math.h"

#define CHUNK_SIZE 16

typedef enum
{
	BLOCK_TYPE_NONE,
	BLOCK_TYPE_BASIC,

	BLOCK_TYPE_MAX
} block_type;

typedef enum
{
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	DIRECTION_FRONT,
	DIRECTION_BACK
} direction_type;

typedef struct
{
	vector3i_t position;
	// [x][y][z]
	block_type blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	Model model;

	// reference to the world they belong to
	struct world_t* world;
} chunk_t;

void fill_chunk(chunk_t* chunk);
void set_block(chunk_t* chunk, size_t x, size_t y, size_t z, block_type block, bool update_meshes);
block_type get_block(chunk_t* chunk, size_t x, size_t y, size_t z);
block_type get_neighboor_block(chunk_t* chunk, size_t x, size_t y, size_t z, direction_type direction);
void generate_chunk_model(chunk_t* chunk);
void update_chunk_model(chunk_t* chunk);
void unload_chunk_model(chunk_t* chunk);
void render_chunk(chunk_t* chunk);
