#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

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

		BeginDrawing();
		ClearBackground(WHITE);

		BeginMode3D(camera);
		
		render_world(&world);

		// DrawLine3D(camera.target, (Vector3){camera.target.x + 1.0f, camera.target.y, camera.target.z}, BLUE);
		// DrawLine3D(camera.target, (Vector3){camera.target.x, camera.target.y + 1.0f, camera.target.z}, RED);
		// DrawLine3D(camera.target, (Vector3){camera.target.x, camera.target.y, camera.target.z + 1.0f}, GREEN);

		EndMode3D();

		DrawFPS(5, 5);
		
		EndDrawing();
	}



	free_world(&world);
	CloseWindow();
	return 0;
}
