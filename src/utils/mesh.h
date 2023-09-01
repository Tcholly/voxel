#pragma once

#include <raylib.h>

// Assumes that the arrays (vertices, indices, colors) have enough capacity
void add_mesh_quad_data(Mesh* mesh, Vector3 vertices[4], Color color);
