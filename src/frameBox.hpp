#ifndef FRAME_BOX_HPP
#define FRAME_BOX_HPP

class FrameBox {
public:
	FrameBox() : position(0.0f) {
		generateMesh();

		vertexArray.bind(vertices, indices);
	}

	void setPosition(const glm::vec3 &position) {
		this->position = position;
	}

	void render(ShaderProgram &shaderProgram, Camera &camera, const glm::vec3 &color, const glm::vec3 &scale) const {
        shaderProgram.use();

        shaderProgram.setVec3("Color", color);

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::scale(model, scale);
        model = glm::translate(model, position);
        //model = glm::rotate(model, glm::radians(45.0f * (float)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));

        shaderProgram.setMat4("Projection", camera.getProjection());
        shaderProgram.setMat4("View", camera.getView());
        shaderProgram.setMat4("Model", model);

        vertexArray.bind();

        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);

        vertexArray.unbind();
    }
protected:
	glm::vec3 position;

	VertexArray vertexArray;

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

	void generateMesh() {
		FrameCube frameBox = createFrameBox();

		vertices.insert(vertices.end(), std::begin(frameBox), std::end(frameBox));

		unsigned int indexArrayData[] = {
			0u, 1u, 1u, 2u, 2u, 3u, 3u, 0u,
		    4u, 5u, 5u, 6u, 6u, 7u, 7u, 4u,
		    0u, 4u, 1u, 5u, 2u, 6u, 3u, 7u
		};

		indices.insert(indices.end(), std::begin(indexArrayData), std::end(indexArrayData));
    }
};

#endif