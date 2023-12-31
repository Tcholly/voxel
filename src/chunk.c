#include "chunk.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/math.h"
#include "utils/mesh.h"
#include "world.h"

static int get_block_index(chunk_t* chunk, size_t x, size_t y, size_t z)
{
	const vector3ul_t chunk_size = chunk->world->chunk_size;
	if (x >= chunk_size.x || y >= chunk_size.y || z >= chunk_size.z)
		return -1;

	return x + y * chunk_size.x + z * chunk_size.x * chunk_size.y;
}

chunk_t allocate_chunk(world_t* world, vector3i_t position)
{
	chunk_t result =
	{
		.position = position,
		.world = world,
	};

	result.blocks = (block_type*)malloc(sizeof(block_type) * world->chunk_size.x * world->chunk_size.y * world->chunk_size.z);
	assert(result.blocks && "Buy more ram");

	return result;
}


void fill_chunk(chunk_t* chunk)
{
	const vector3ul_t chunk_size = chunk->world->chunk_size;
	for (int x = 0; x < (int)chunk_size.x; x++)
	{
		int vx = chunk->position.x * (int)chunk_size.x + x;
		for (int z = 0; z < (int)chunk_size.z; z++)
		{
			int vz = chunk->position.z * (int)chunk_size.z + z;
			// int height = perlin_noise_2d(vx, vz, 0.1f, 3) * (int)chunk_size.y;
			for (int y = 0; y < (int)chunk_size.y; y++)
			{
				float vy = chunk->position.y * chunk_size.y+ y;
				if (perlin_noise_3d(vx, vy, vz) < 0.5f)
				set_block(chunk, x, y, z, BLOCK_TYPE_BASIC, false);
			}
		}
	}
}

block_type get_block(chunk_t* chunk, size_t x, size_t y, size_t z)
{
	if (!chunk)
	{
		printf("WARNING: in get_block -> chunk is null\n");
		return BLOCK_TYPE_NONE;
	}

	const vector3ul_t chunk_size = chunk->world->chunk_size;
	const int index = get_block_index(chunk, x, y, z);

	if (index < 0 || index >= (int)(chunk_size.x * chunk_size.y * chunk_size.z))
	{
		printf("WARNING: in get_block -> out of bounds\n");
		return BLOCK_TYPE_NONE;
	}

	return chunk->blocks[index];
}

void set_block(chunk_t* chunk, size_t x, size_t y, size_t z, block_type block, bool update_meshes)
{
	if (!chunk)
	{
		printf("WARNING: in set_block -> chunk is null\n");
		return;
	}

	const vector3ul_t chunk_size = chunk->world->chunk_size;
	const int index = get_block_index(chunk, x, y, z);
	if (index < 0 || index >= (int)(chunk_size.x * chunk_size.y * chunk_size.z))
	{
		printf("WARNING: in set_block -> out of bounds\n");
		return;
	}

	chunk->blocks[index] = block;

	if (update_meshes)
	{
		update_chunk_model(chunk);

		if (x == 0)
		{
			chunk_t* neighbooring_chunk = get_chunk(chunk->world, chunk->position.x - 1, chunk->position.y, chunk->position.z);
			if (neighbooring_chunk)
				update_chunk_model(neighbooring_chunk);
		}
		else if (x == chunk_size.x - 1)
		{
			chunk_t* neighbooring_chunk = get_chunk(chunk->world, chunk->position.x + 1, chunk->position.y, chunk->position.z);
			if (neighbooring_chunk)
				update_chunk_model(neighbooring_chunk);
		}

		if (y == 0)
		{
			chunk_t* neighbooring_chunk = get_chunk(chunk->world, chunk->position.x, chunk->position.y - 1, chunk->position.z);
			if (neighbooring_chunk)
				update_chunk_model(neighbooring_chunk);
		}
		else if (y == chunk_size.y - 1)
		{
			chunk_t* neighbooring_chunk = get_chunk(chunk->world, chunk->position.x, chunk->position.y + 1, chunk->position.z);
			if (neighbooring_chunk)
				update_chunk_model(neighbooring_chunk);
		}

		if (z == 0)
		{
			chunk_t* neighbooring_chunk = get_chunk(chunk->world, chunk->position.x, chunk->position.y, chunk->position.z - 1);
			if (neighbooring_chunk)
				update_chunk_model(neighbooring_chunk);
		}
		else if (z == chunk_size.z - 1)
		{
			chunk_t* neighbooring_chunk = get_chunk(chunk->world, chunk->position.x, chunk->position.y, chunk->position.z + 1);
			if (neighbooring_chunk)
				update_chunk_model(neighbooring_chunk);
		}
	}
}

block_type get_neighboor_block(chunk_t* chunk, size_t x, size_t y, size_t z, direction_type direction)
{
	const vector3ul_t chunk_size = chunk->world->chunk_size;
	switch (direction)
	{
		case DIRECTION_UP:
			{
				if (y == chunk_size.y - 1)
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

					return get_block(below_chunk, x, chunk_size.y - 1, z);
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

					return get_block(left_chunk, chunk_size.x - 1, y, z);
				}

				return get_block(chunk, x - 1, y, z);
			}
			break;
		case DIRECTION_RIGHT:
			{
				if (x == chunk_size.x - 1)
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

					return get_block(front_chunk, x, y, chunk_size.z - 1);
				}

				return get_block(chunk, x, y, z - 1);
			}
			break;
		case DIRECTION_BACK:
			{
				if (z == chunk_size.z - 1)
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

	const vector3ul_t chunk_size = chunk->world->chunk_size;
	const size_t total_voxel_count = chunk_size.x * chunk_size.y * chunk_size.z;
	const size_t max_vertex_count = (6 * 4) * total_voxel_count;
	const size_t max_indices_count = (6 * 6) * total_voxel_count;
	const size_t max_color_count = (6 * 4) * total_voxel_count;

	mesh.vertices = (float*)malloc((sizeof(float) * 3) * max_vertex_count);
	mesh.indices = (uint16_t*)malloc(sizeof(uint16_t) * max_indices_count);
	mesh.colors = (uint8_t*)malloc(sizeof(uint8_t) * 4 * max_color_count);

	for (size_t x = 0; x < chunk_size.x; x++)
	{
		for (size_t y = 0; y < chunk_size.y; y++)
		{
			for (size_t z = 0; z < chunk_size.z; z++)
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
	const vector3ul_t chunk_size = chunk->world->chunk_size;
	Vector3 position = { chunk->position.x * (int)chunk_size.x, chunk->position.y * (int)chunk_size.y, chunk->position.z * (int)chunk_size.z};
	DrawModel(chunk->model, position, 1.0f, WHITE);
}

void render_chunk_borders(chunk_t* chunk)
{
	const vector3ul_t chunk_size = chunk->world->chunk_size;
	Vector3 position = { (chunk->position.x + 0.5f) * chunk_size.x, (chunk->position.y + 0.5f) * chunk_size.y, (chunk->position.z + 0.5f) * chunk_size.z};
	DrawCubeWires(position, chunk_size.x, chunk_size.y, chunk_size.z, LIGHTGRAY);
}

void free_chunk(chunk_t* chunk)
{
	unload_chunk_model(chunk);
	chunk->position = (vector3i_t){0, 0, 0};
	chunk->world = NULL;
	free(chunk->blocks);
	chunk->blocks = NULL;
}
