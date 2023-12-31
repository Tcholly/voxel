#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

#include "chunk.h"
#include "world.h"

vector3i_t get_chunk_pos(world_t* world, Vector3 position)
{
	vector3i_t result =
	{
		.x = position.x / world->chunk_size.x,
		.y = 0,
		.z = position.z / world->chunk_size.y,
	};

	return result;
}


int main(void)
{
	InitWindow(800, 640, "voxel");
	SetTargetFPS(60);
	DisableCursor();

	world_t world = allocate_world(16, 1, 16);

	Camera3D camera =
	{
		.projection = CAMERA_PERSPECTIVE,
		.fovy = 45.0f,
		.up = {0.0f, 1.0f, 0.0f},
		.target = {128.0f, 20.0f, 129.0f},
		.position = {128.0f, 20.0f, 128.0f}
	};

	vector3i_t previous_chunk = get_chunk_pos(&world, camera.position);

	bool debug_information = false;

	fill_world(&world);
	generate_world_chunks_model(&world);

	while (!WindowShouldClose())
	{
		Vector3 look_direction = 
		{
			camera.target.x - camera.position.x,
			0.0f,
			camera.target.z - camera.position.z,
		};
		look_direction = Vector3Normalize(look_direction);

		Vector3 movement = {0};
		float speed = 10.0f;

		if (IsKeyDown(KEY_W))
			movement = Vector3Add(movement, look_direction);
		if (IsKeyDown(KEY_S))
			movement = Vector3Subtract(movement, look_direction);
		if (IsKeyDown(KEY_A))
			movement = Vector3Add(movement, (Vector3){look_direction.z, 0.0f, -look_direction.x});
		if (IsKeyDown(KEY_D))
			movement = Vector3Subtract(movement, (Vector3){look_direction.z, 0.0f, -look_direction.x});
		// 	camera.position.z += 5.0f * GetFrameTime();
		if (IsKeyDown(KEY_LEFT_SHIFT))
			movement.y -= 1.0f;
		if (IsKeyDown(KEY_SPACE))
			movement.y += 1.0f;
		if (IsKeyDown(KEY_LEFT_CONTROL))
			speed *= 4.0f;


		Vector3 camera_direction = Vector3Subtract(camera.target, camera.position);
		Vector2 mouse_delta = Vector2Scale(GetMouseDelta(), 0.005f);
		Vector3 perpendicular_axis = Vector3CrossProduct(camera_direction, (Vector3){0.0f, 1.0f, 0.0f});
		Quaternion q1 = QuaternionFromAxisAngle((Vector3){0.0f, 1.0f, 0.0f}, -mouse_delta.x);
		Quaternion q2 = QuaternionFromAxisAngle(perpendicular_axis, -mouse_delta.y);
		Quaternion q = QuaternionMultiply(q1, q2);
		camera_direction = Vector3RotateByQuaternion(camera_direction, q);

		float dt = GetFrameTime();
		camera.position.x += movement.x * speed * dt;
		camera.position.y += movement.y * speed * dt;
		camera.position.z += movement.z * speed * dt;

    	camera.target = Vector3Add(camera.position, camera_direction);


		if (IsKeyPressed(KEY_F))
		{
			fill_world(&world);
			unload_world_chunks_model(&world);
			generate_world_chunks_model(&world);
		}

		if (IsKeyPressed(KEY_G))
			debug_information = !debug_information;

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			Vector2 center = {
				.x = GetScreenWidth() / 2.0f,
				.y = GetScreenHeight() / 2.0f,
			};
			Ray ray = GetMouseRay(center, camera);
			float distance = FLT_MAX;
			int chunk_index = -1;
			RayCollision collision = {0};

			for (size_t i = 0; i < world.chunk_capacity; i++)
			{
				Matrix transform = MatrixTranslate(
						world.chunks[i].position.x * world.chunk_size.x,
						world.chunks[i].position.y * world.chunk_size.y,
						world.chunks[i].position.z * world.chunk_size.z
					);
				RayCollision ray_collision = GetRayCollisionMesh(ray, world.chunks[i].model.meshes[0], transform);
				if (ray_collision.hit && ray_collision.distance < distance)
				{
					distance = ray_collision.distance;
					collision = ray_collision;
					chunk_index = i;
				}
			}

			if (collision.hit)
			{
				Vector3 voxel_center = Vector3Subtract(collision.point, Vector3Scale(collision.normal, 0.5f));
				size_t voxel_x = (int)voxel_center.x - world.chunks[chunk_index].position.x * world.chunk_size.x;
				size_t voxel_y = (int)voxel_center.y - world.chunks[chunk_index].position.y * world.chunk_size.y;
				size_t voxel_z = (int)voxel_center.z - world.chunks[chunk_index].position.z * world.chunk_size.z;
				
				set_block(&world.chunks[chunk_index], voxel_x, voxel_y, voxel_z, BLOCK_TYPE_NONE, true);
			}
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			Vector2 center = {
				.x = GetScreenWidth() / 2.0f,
				.y = GetScreenHeight() / 2.0f,
			};
			Ray ray = GetMouseRay(center, camera);
			float distance = FLT_MAX;
			int chunk_index = -1;
			RayCollision collision = {0};

			for (size_t i = 0; i < world.chunk_capacity; i++)
			{
				Matrix transform = MatrixTranslate(
						world.chunks[i].position.x * world.chunk_size.x,
						world.chunks[i].position.y * world.chunk_size.y,
						world.chunks[i].position.z * world.chunk_size.z
					);
				RayCollision ray_collision = GetRayCollisionMesh(ray, world.chunks[i].model.meshes[0], transform);
				if (ray_collision.hit && ray_collision.distance < distance)
				{
					distance = ray_collision.distance;
					collision = ray_collision;
					chunk_index = i;
				}
			}

			if (collision.hit)
			{
				Vector3 voxel_center = Vector3Add(collision.point, Vector3Scale(collision.normal, 0.5f));
				chunk_t* chunk = &world.chunks[chunk_index];

				int voxel_x = (int)voxel_center.x - chunk->position.x * world.chunk_size.x;
				int voxel_y = (int)voxel_center.y - chunk->position.y * world.chunk_size.y;
				int voxel_z = (int)voxel_center.z - chunk->position.z * world.chunk_size.z;

				if (voxel_x < 0)
				{
					chunk = get_chunk(&world, chunk->position.x - 1, chunk->position.y, chunk->position.z);
					voxel_x = world.chunk_size.x - 1;
				}
				else if (voxel_x >= (int)world.chunk_size.x)
				{
					chunk = get_chunk(&world, chunk->position.x + 1, chunk->position.y, chunk->position.z);
					voxel_x = 0;
				}
				if (voxel_y < 0)
				{
					chunk = get_chunk(&world, chunk->position.x, chunk->position.y - 1, chunk->position.z);
					voxel_y = world.chunk_size.y - 1;
				}
				else if (voxel_y >= (int)world.chunk_size.y)
				{
					chunk = get_chunk(&world, chunk->position.x, chunk->position.y + 1, chunk->position.z);
					voxel_y = 0;
				}
				if (voxel_z < 0)
				{
					chunk = get_chunk(&world, chunk->position.x, chunk->position.y, chunk->position.z - 1);
					voxel_z = world.chunk_size.z - 1;
				}
				else if (voxel_z >= (int)world.chunk_size.z)
				{
					chunk = get_chunk(&world, chunk->position.x, chunk->position.y, chunk->position.z + 1);
					voxel_z = 0;
				}
				
				
				set_block(chunk, voxel_x, voxel_y, voxel_z, BLOCK_TYPE_BASIC, true);
			}
		}

		// Chunk_loading
		vector3i_t current_chunk = get_chunk_pos(&world, camera.position);
		if (camera.position.x < 0) current_chunk.x -= 1;
		if (camera.position.z < 0) current_chunk.z -= 1;

		if (!vecto3i_equals(previous_chunk, current_chunk))
		{
			int direction_x = current_chunk.x - previous_chunk.x;
			int direction_z = current_chunk.z - previous_chunk.z;

			unload_and_load_new_chunks(&world, direction_x, 0, direction_z);
		
			previous_chunk = current_chunk;
		}


		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(camera);
		
		render_world(&world);

		if (debug_information)
		{
			DrawLine3D(camera.target, (Vector3){camera.target.x + 0.2f, camera.target.y, camera.target.z}, BLUE);
			DrawLine3D(camera.target, (Vector3){camera.target.x, camera.target.y + 0.2f, camera.target.z}, RED);
			DrawLine3D(camera.target, (Vector3){camera.target.x, camera.target.y, camera.target.z + 0.2f}, GREEN);

			render_world_chunks_borders(&world);
		}

		EndMode3D();

		Vector2 center = {
			.x = GetScreenWidth() / 2.0f,
			.y = GetScreenHeight() / 2.0f,
		};
		DrawCircleV(center, 3.0f, BLACK);

		DrawFPS(5, 5);
		
		EndDrawing();
	}


	free_world(&world);
	CloseWindow();
	return 0;
}
