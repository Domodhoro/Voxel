#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

class VertexArray {
public:
	VertexArray() {
		glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
	}

	void bind(std::vector<glm::vec3> &vertices, std::vector<unsigned int> &indices) {
		glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.at(0), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.at(0), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0u);
	}

	void bind(std::vector<Vertex3D> &vertices, std::vector<unsigned int> &indices) {
		glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), &vertices.at(0), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.at(0), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex3D), (void*)offsetof(Vertex3D, coordinates));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex3D), (void*)offsetof(Vertex3D, textureCoordinates));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0u);
	}

	void bind() const {
		glBindVertexArray(VAO);
	}

	void unbind() const {
		glBindVertexArray(0u);
	}
protected:
	unsigned int VAO = 0u, VBO = 0u, EBO = 0u;
};

#endif