#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec2 Texture;
} fs_in;

uniform sampler2D SamplerTexture;

highp vec2 sample(highp vec2 tex, float size, float x, float y) {
    return vec2((tex.x + x) * size, (tex.y + y) * size);
}

void main() {
    highp vec4 Color = vec4(1.0f);

    Color = texture(SamplerTexture, sample(fs_in.Texture, 0.25f, 0.0f, 3.0f));

    if (Color.r == 1.0f && Color.g != 1.0f && Color.b == 1.0f) {
        discard;
    }

    FragColor = Color;
}