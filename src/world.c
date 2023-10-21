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
	for (size_t i = 0; i < world->chunk_capacity; i++)
		free_chunk(&world->chunks[i]);

	free(world->chunks);
	world->chunk_capacity = 0;
	world->size = (vector3i_t){0, 0, 0};
	world->chunks = NULL;
	world->min_chunks = (vector3i_t){0, 0, 0};
	world->max_chunks = (vector3i_t){0, 0, 0};
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
	world->min_chunks = (vector3i_t){0, 0, 0};
	world->max_chunks = (vector3i_t){world->size.x - 1, world->size.y - 1, world->size.z - 1};
}

chunk_t* get_chunk(world_t* world, int x, int y, int z)
{
	if (x < world->min_chunks.x || x > world->max_chunks.x)
		return NULL;
	if (y < world->min_chunks.y || y > world->max_chunks.y)
		return NULL;
	if (z < world->min_chunks.z || z > world->max_chunks.z)
		return NULL;

	// TODO: hashmap or something
	for (size_t i = 0; i < world->chunk_capacity; i++)
	{
		chunk_t* chunk = &world->chunks[i];
		if (chunk->position.x == x && chunk->position.y == y && chunk->position.z == z)
			return chunk;
	}

	return NULL;
}

int get_chunk_index(world_t* world, int x, int y, int z)
{
	if (x < world->min_chunks.x || x > world->max_chunks.x)
		return -1;
	if (y < world->min_chunks.y || y > world->max_chunks.y)
		return -1;
	if (z < world->min_chunks.z || z > world->max_chunks.z)
		return -1;

	// TODO: hashmap or something
	for (size_t i = 0; i < world->chunk_capacity; i++)
	{
		chunk_t* chunk = &world->chunks[i];
		if (chunk->position.x == x && chunk->position.y == y && chunk->position.z == z)
			return i;
	}

	return -1;
}

static void replace_chunk(world_t* world, int chunk_index, int new_x, int new_y, int new_z)
{
	chunk_t* chunk = &world->chunks[chunk_index];

	free_chunk(chunk);
	chunk->world = world;
	chunk->position = (vector3i_t){new_x, new_y, new_z};
	fill_chunk(chunk);
	generate_chunk_model(chunk);
}

void unload_and_load_new_chunks(world_t* world, int dir_x, int dir_y, int dir_z)
{
	if (dir_x > 0)
	{
		for (size_t i = 0; i < world->chunk_capacity; i++)
		{
			chunk_t* chunk = &world->chunks[i];
			if (chunk->position.x == world->min_chunks.x)
				replace_chunk(world, i, world->max_chunks.x + 1, chunk->position.y, chunk->position.z);
		}

		world->max_chunks.x += 1;
		world->min_chunks.x += 1;
	}
	else if (dir_x < 0)
	{
		for (size_t i = 0; i < world->chunk_capacity; i++)
		{
			chunk_t* chunk = &world->chunks[i];
			if (chunk->position.x == world->max_chunks.x)
				replace_chunk(world, i, world->min_chunks.x - 1, chunk->position.y, chunk->position.z);
		}

		world->max_chunks.x -= 1;
		world->min_chunks.x -= 1;
	}

	if (dir_y > 0)
	{
		for (size_t i = 0; i < world->chunk_capacity; i++)
		{
			chunk_t* chunk = &world->chunks[i];
			if (chunk->position.y == world->min_chunks.y)
				replace_chunk(world, i, chunk->position.x, world->max_chunks.y + 1, chunk->position.z);
		}

		world->max_chunks.y += 1;
		world->min_chunks.y += 1;
	}
	else if (dir_y < 0)
	{
		for (size_t i = 0; i < world->chunk_capacity; i++)
		{
			chunk_t* chunk = &world->chunks[i];
			if (chunk->position.y == world->max_chunks.y)
				replace_chunk(world, i, chunk->position.x, world->min_chunks.y - 1, chunk->position.z);
		}

		world->max_chunks.y -= 1;
		world->min_chunks.y -= 1;
	}

	if (dir_z > 0)
	{
		for (size_t i = 0; i < world->chunk_capacity; i++)
		{
			chunk_t* chunk = &world->chunks[i];
			if (chunk->position.z == world->min_chunks.z)
				replace_chunk(world, i, chunk->position.x, chunk->position.y, world->max_chunks.z + 1);
		}

		world->max_chunks.z += 1;
		world->min_chunks.z += 1;
	}
	else if (dir_z < 0)
	{
		for (size_t i = 0; i < world->chunk_capacity; i++)
		{
			chunk_t* chunk = &world->chunks[i];
			if (chunk->position.z == world->max_chunks.z)
				replace_chunk(world, i, chunk->position.x, chunk->position.y, world->min_chunks.z - 1);
		}

		world->max_chunks.z -= 1;
		world->min_chunks.z -= 1;
	}
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

void render_world_chunks_borders(world_t* world)
{
	for (size_t i = 0; i < world->chunk_capacity; i++)
		render_chunk_borders(&world->chunks[i]);
}
