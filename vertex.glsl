#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 Texture;

out VS_OUT {
    vec2 Texture;
} vs_out;

uniform mat4 Projection, View, Model;

void main() {
    gl_Position = Projection * View * Model * vec4(Position, 1.0f);
    
    vs_out.Texture = Texture;
}