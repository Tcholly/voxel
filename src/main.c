#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

#include "chunk.h"
#include "world.h"

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
		.target = {0.0f, 20.0f, 1.0f},
		.position = {0.0f, 20.0f, 0.0f}
	};

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
		Vector2 mouse_delta = Vector2Scale(GetMouseDelta(), 0.01f);
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
						world.chunks[i].position.x * CHUNK_SIZE,
						world.chunks[i].position.y * CHUNK_SIZE,
						world.chunks[i].position.z * CHUNK_SIZE
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
				size_t voxel_x = (int)voxel_center.x - world.chunks[chunk_index].position.x * CHUNK_SIZE;
				size_t voxel_y = (int)voxel_center.y - world.chunks[chunk_index].position.y * CHUNK_SIZE;
				size_t voxel_z = (int)voxel_center.z - world.chunks[chunk_index].position.z * CHUNK_SIZE;
				printf("%d -- %f, %f, %f -> %zu, %zu, %zu\n", chunk_index, voxel_center.x, voxel_center.y, voxel_center.z, voxel_x, voxel_y, voxel_z);
				
				set_block(&world.chunks[chunk_index], voxel_x, voxel_y, voxel_z, BLOCK_TYPE_NONE, true);
				update_chunk_model(&world.chunks[chunk_index]);
			}

		}

		BeginDrawing();
		ClearBackground(WHITE);

		BeginMode3D(camera);
		
		render_world(&world);

		// DrawLine3D(camera.target, (Vector3){camera.target.x + 1.0f, camera.target.y, camera.target.z}, BLUE);
		// DrawLine3D(camera.target, (Vector3){camera.target.x, camera.target.y + 1.0f, camera.target.z}, RED);
		// DrawLine3D(camera.target, (Vector3){camera.target.x, camera.target.y, camera.target.z + 1.0f}, GREEN);

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
