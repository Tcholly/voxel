#include "chunk.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils/mesh.h"
#include "world.h"

void fill_chunk(chunk_t* chunk)
{
	for (size_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (size_t z = 0; z < CHUNK_SIZE; z++)
		{
			int variation = rand() % 5;
			for (size_t y = 0; y < 10 + (size_t)variation; y++)
			{
				set_block(chunk, x, y, z, BLOCK_TYPE_BASIC);
			}
		}
	}
}

block_type get_block(chunk_t* chunk, size_t x, size_t y, size_t z)
{
	return chunk->blocks[x][y][z];
}

void set_block(chunk_t* chunk, size_t x, size_t y, size_t z, block_type block)
{
	chunk->blocks[x][y][z] = block;
	// TODO: Update chunk mesh
}

block_type get_neighboor_block(chunk_t* chunk, size_t x, size_t y, size_t z, direction_type direction)
{
	switch (direction)
	{
		case DIRECTION_UP:
			{
				if (y == CHUNK_SIZE - 1)
				{
					chunk_t* above_chunk = get_chunk(chunk->world, chunk->position.x, chunk->position.y + 1, chunk->position.z);
					if (!above_chunk)
						return BLOCK_TYPE_NONE;
					
					return get_block(above_chunk, x, 0, z);
				}

				return get_block(chunk, x, y + 1, z);
			}
			break;
		case DIRECTION_DOWN:
			{
				if (y == 0)
				{
					chunk_t* below_chunk = get_chunk(chunk->world, chunk->position.x, chunk->position.y - 1, chunk->position.z);
					if (!below_chunk)
						return BLOCK_TYPE_NONE;

					return get_block(below_chunk, x, CHUNK_SIZE - 1, z);
				}

				return get_block(chunk, x, y - 1, z);
			}
			break;
		case DIRECTION_LEFT:
			{
				if (x == 0)
				{
					chunk_t* left_chunk = get_chunk(chunk->world, chunk->position.x - 1, chunk->position.y, chunk->position.z);
					if (!left_chunk)
						return BLOCK_TYPE_NONE;

					return get_block(left_chunk, CHUNK_SIZE - 1, y, z);
				}

				return get_block(chunk, x - 1, y, z);
			}
			break;
		case DIRECTION_RIGHT:
			{
				if (x == CHUNK_SIZE - 1)
				{
					chunk_t* right_chunk = get_chunk(chunk->world, chunk->position.x + 1, chunk->position.y, chunk->position.z);
					if (!right_chunk)
						return BLOCK_TYPE_NONE;

					return get_block(right_chunk, 0, y, z);
				}

				return get_block(chunk, x + 1, y, z);
			}
			break;
		case DIRECTION_FRONT:
			{
				if (z == 0)
				{
					chunk_t* front_chunk = get_chunk(chunk->world, chunk->position.x, chunk->position.y, chunk->position.z - 1);
					if (!front_chunk)
						return BLOCK_TYPE_NONE;

					return get_block(front_chunk, x, y, CHUNK_SIZE - 1);
				}

				return get_block(chunk, x, y, z - 1);
			}
			break;
		case DIRECTION_BACK:
			{
				if (z == CHUNK_SIZE - 1)
				{
					chunk_t* back_chunk = get_chunk(chunk->world, chunk->position.x, chunk->position.y, chunk->position.z + 1);
					if (!back_chunk)
						return BLOCK_TYPE_NONE;

					return get_block(back_chunk, x, y, 0);
				}

				return get_block(chunk, x, y, z + 1);
			}
			break;

		default:
			assert(0 && "UNREACHABLE");
	}
}

void generate_chunk_model(chunk_t* chunk)
{
	Mesh mesh = {0};
	size_t max_vertex_count = (6 * 4) * (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
	size_t max_indices_count = (6 * 6) * (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
	size_t max_color_count = (6 * 4) * (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

	mesh.vertices = (float*)malloc((sizeof(float) * 3) * max_vertex_count);
	mesh.indices = (uint16_t*)malloc(sizeof(uint16_t) * max_indices_count);
	mesh.colors = (uint8_t*)malloc(sizeof(uint8_t) * 4 * max_color_count);

	for (size_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (size_t y = 0; y < CHUNK_SIZE; y++)
		{
			for (size_t z = 0; z < CHUNK_SIZE; z++)
			{
				block_type block = get_block(chunk, x, y, z);
				if (block == BLOCK_TYPE_NONE) continue;

				// down left right front back
				if (get_neighboor_block(chunk, x, y, z, DIRECTION_UP) == BLOCK_TYPE_NONE)
				{
					Vector3 vertices[] =
					{
						{x    , y + 1, z    },
						{x + 1, y + 1, z    },
						{x    , y + 1, z + 1},
						{x + 1, y + 1, z + 1}
					};
					add_mesh_quad_data(&mesh, vertices, PINK);
				}
				if (get_neighboor_block(chunk, x, y, z, DIRECTION_DOWN) == BLOCK_TYPE_NONE)
				{
					Vector3 vertices[] =
					{
						{x    , y, z    },
						{x    , y, z + 1},
						{x + 1, y, z    },
						{x + 1, y, z + 1}
					};
					add_mesh_quad_data(&mesh, vertices, PURPLE);
				}

				if (get_neighboor_block(chunk, x, y, z, DIRECTION_LEFT) == BLOCK_TYPE_NONE)
				{
					Vector3 vertices[] =
					{
						{x, y    , z    },
						{x, y + 1, z    },
						{x, y    , z + 1},
						{x, y + 1, z + 1}
					};
					add_mesh_quad_data(&mesh, vertices, BROWN);
				}
				if (get_neighboor_block(chunk, x, y, z, DIRECTION_RIGHT) == BLOCK_TYPE_NONE)
				{
					Vector3 vertices[] =
					{
						{x + 1, y    , z    },
						{x + 1, y    , z + 1},
						{x + 1, y + 1, z    },
						{x + 1, y + 1, z + 1}
					};
					add_mesh_quad_data(&mesh, vertices, LIME);
				}

				if (get_neighboor_block(chunk, x, y, z, DIRECTION_FRONT) == BLOCK_TYPE_NONE)
				{
					Vector3 vertices[] =
					{
						{x    , y    , z},
						{x + 1, y    , z},
						{x    , y + 1, z},
						{x + 1, y + 1, z}
					};
					add_mesh_quad_data(&mesh, vertices, YELLOW);
				}
				if (get_neighboor_block(chunk, x, y, z, DIRECTION_BACK) == BLOCK_TYPE_NONE)
				{
					Vector3 vertices[] =
					{
						{x    , y    , z + 1},
						{x    , y + 1, z + 1},
						{x + 1, y    , z + 1},
						{x + 1, y + 1, z + 1}
					};
					add_mesh_quad_data(&mesh, vertices, GRAY);
				}
			}
		}
	}

	UploadMesh(&mesh, false);
	chunk->model = LoadModelFromMesh(mesh);
}

void update_chunk_model(chunk_t* chunk)
{
	unload_chunk_model(chunk);
	generate_chunk_model(chunk);
}

void unload_chunk_model(chunk_t* chunk)
{
	// UploadMesh and UnloadModel frees the allocated arrays -> vertices indices colors
	UnloadModel(chunk->model);
}

void render_chunk(chunk_t* chunk)
{
	Vector3 position = { chunk->position.x * CHUNK_SIZE, chunk->position.y * CHUNK_SIZE, chunk->position.z * CHUNK_SIZE };
	DrawModel(chunk->model, position, 1.0f, WHITE);
}
