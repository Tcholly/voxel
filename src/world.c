#include "world.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

world_t allocate_world(size_t size_x, size_t size_y, size_t size_z)
{
	world_t result = {0};
	result.chunk_capacity = size_x * size_y * size_z;
	result.size = (vector3i_t){size_x, size_y, size_z};
	result.chunks = (chunk_t*)malloc(result.chunk_capacity * sizeof(chunk_t));
	assert(result.chunks && "Buy more ram");

	return result;
}

void free_world(world_t* world)
{
	free(world->chunks);
	world->chunk_capacity = 0;
	world->size = (vector3i_t){0, 0, 0};
	world->chunks = NULL;
}

void fill_world(world_t* world)
{
	size_t i = 0;
	for (int x = 0; x < world->size.x; x++)
	{
		for (int y = 0; y < world->size.y; y++)
		{
			for (int z = 0; z < world->size.z; z++)
			{
				world->chunks[i].world = world;
				world->chunks[i].position = (vector3i_t){x, y, z};
				fill_chunk(&world->chunks[i]);
				i++;
			}
		}
	}
}

chunk_t* get_chunk(world_t* world, int x, int y, int z)
{
	// TODO: hashmap or something
	for (size_t i = 0; i < world->chunk_capacity; i++)
	{
		chunk_t* chunk = &world->chunks[i];
		if (chunk->position.x == x && chunk->position.y == y && chunk->position.z == z)
			return chunk;
	}

	return NULL;
}

void generate_world_chunks_model(world_t* world)
{
	for (size_t i = 0; i < world->chunk_capacity; i++)
		generate_chunk_model(&world->chunks[i]);
}

void unload_world_chunks_model(world_t* world)
{
	for (size_t i = 0; i < world->chunk_capacity; i++)
		unload_chunk_model(&world->chunks[i]);
}

void render_world(world_t* world)
{
	for (size_t i = 0; i < world->chunk_capacity; i++)
		render_chunk(&world->chunks[i]);
}
