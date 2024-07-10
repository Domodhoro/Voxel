shaderSource = {
    chunkVertex = [[
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
    ]],

    chunkFragment = [[
        #version 330 core

        in VS_OUT {
            vec2 Texture;
        } fs_in;

        uniform sampler2D SamplerTexture;

        out vec4 FragColor;

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
    ]],

    skyboxVertex = [[
        #version 330 core

        layout(location = 0) in vec3 Position;

        out VS_OUT {
            vec3 Texture;
        } vs_out;

        uniform mat4 Projection, View, Model;

        void main() {
            gl_Position = Projection * View * Model * vec4(Position, 1.0f);

            vs_out.Texture = Position;
        }
    ]],

    skyboxFragment = [[
        #version 330 core

        in VS_OUT {
            vec3 Texture;
        } fs_in;

        uniform samplerCube Cube;

        out vec4 FragColor;

        void main() {
            FragColor = texture(Cube, fs_in.Texture);
        }
    ]],

    frameBoxVertex = [[
        #version 330 core

        layout(location = 0) in vec3 Position;

        uniform mat4 Projection, View, Model;

        void main() {
            gl_Position = Projection * View * Model * vec4(Position, 1.0f);
        }
    ]],

    frameBoxFragment = [[
        #version 330 core

        out vec4 FragColor;

        uniform vec3 Color;

        void main() {
            FragColor = vec4(Color, 1.0f);
        }
    ]]
}