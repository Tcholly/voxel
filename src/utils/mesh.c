#include "mesh.h"

#include <stdlib.h>
#include <stdint.h>


// TODO: Change from Vector3[4] to position and normal
void add_mesh_quad_data(Mesh* mesh, Vector3 vertices[4], Color color)
{

	size_t vertex_start_index = mesh->vertexCount * 3;
	float* vertices_array = mesh->vertices + vertex_start_index;
	for (size_t i = 0; i < 4; i++)
	{
		vertices_array[0] = vertices[i].x;
		vertices_array[1] = vertices[i].y;
		vertices_array[2] = vertices[i].z;
		vertices_array += 3;
	}
	
	size_t indices_start_index = mesh->triangleCount * 3;
	uint16_t base_index = mesh->vertexCount;
	uint16_t* indices = mesh->indices + indices_start_index;

	indices[0] = base_index + 0;
	indices[1] = base_index + 2;
	indices[2] = base_index + 1;
	indices[3] = base_index + 2;
	indices[4] = base_index + 3;
	indices[5] = base_index + 1;

	size_t color_start_index = mesh->vertexCount * 4;
	uint8_t* colors = mesh->colors + color_start_index;
	for (size_t i = 0; i < 4; i++)
	{
		colors[4 * i + 0] = color.r;
		colors[4 * i + 1] = color.g;
		colors[4 * i + 2] = color.b;
		colors[4 * i + 3] = color.a;
	}

	mesh->vertexCount += 4;
	mesh->triangleCount += 2;
}
