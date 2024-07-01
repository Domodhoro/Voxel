#ifndef SKYBOX_HPP
#define SKYBOX_HPP

class Skybox {
public:
    Skybox() {
        generateMesh();

        vertexArray.bind<Vertex3D>(vertices, indices, 3, 2);
    }

    void draw(ShaderProgram &shaderProgram, Texture &texture, Camera &camera) const {
        glDepthMask(GL_FALSE);

        shaderProgram.use();

        glm::mat4 model = glm::mat4(1.0f);

        shaderProgram.setMat4("Projection", camera.getProjection());
        shaderProgram.setMat4("View", glm::mat4(glm::mat3(camera.getView())));
        shaderProgram.setMat4("Model", model);

        texture.bind("SKYBOX", GL_TEXTURE_CUBE_MAP);
        vertexArray.bind();

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        texture.unbind(GL_TEXTURE_CUBE_MAP);
        vertexArray.unbind();

        glDepthMask(GL_TRUE);
    }
protected:
    VertexArray vertexArray;

    std::vector<Vertex3D> vertices;
    std::vector<unsigned int> indices;

    void generateMesh() {
        auto front = createBlockFrontFace(0.0f, 0.0f, 0.0f);
        auto back = createBlockBackFace(0.0f, 0.0f, 0.0f);
        auto right = createBlockRightFace(0.0f, 0.0f, 0.0f);
        auto left = createBlockLeftFace(0.0f, 0.0f, 0.0f);
        auto up = createBlockUpFace(0.0f, 0.0f, 0.0f);
        auto down = createBlockDownFace(0.0f, 0.0f, 0.0f);

        vertices.insert(vertices.end(), std::begin(front), std::end(front));
        vertices.insert(vertices.end(), std::begin(back), std::end(back));
        vertices.insert(vertices.end(), std::begin(right), std::end(right));
        vertices.insert(vertices.end(), std::begin(left), std::end(left));
        vertices.insert(vertices.end(), std::begin(up), std::end(up));
        vertices.insert(vertices.end(), std::begin(down), std::end(down));

        unsigned int indexArrayData[] = {
            0u,   1u,  3u,  3u,  1u,  2u,
            5u,   4u,  7u,  5u,  7u,  6u,
            8u,   9u, 11u, 11u,  9u, 10u,
            13u, 12u, 15u, 13u, 15u, 14u,
            16u, 17u, 19u, 19u, 17u, 18u,
            21u, 20u, 23u, 21u, 23u, 22u
        };

        indices.insert(indices.end(), std::begin(indexArrayData), std::end(indexArrayData));
    }
};

#endif