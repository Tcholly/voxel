#!/bin/sh

set -xe

gcc -Wall -Wextra -ggdb -o voxel src/main.c src/world.c src/chunk.c src/utils/math.c src/utils/mesh.c -Isrc -IDependencies/raylib/include -LDependencies/raylib/lib -lm -lraylib

if [ "$1" = "run" ]; then
	./voxel
fi
