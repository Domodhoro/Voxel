#!/bin/bash

source_file="main.cpp"
executable="Voxel"

if [ -f "$executable" ]; then
    rm "$executable"
fi

g++ -std=c++17 -o "$executable" "$source_file" -lm -lSDL2 -lSDL2_image -lglfw -lGL -lGLEW "liblua54.a"

if [ $? -eq 0 ]; then
    "./$executable"
else
    echo "Erro durante a compilação."
fi
