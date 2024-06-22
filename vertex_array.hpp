#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

struct Vertex {
    glm::vec3 coordinates;
    glm::vec2 texture_coordinates;
};

class VertexArray {
public:
	VertexArray() {
		glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
	}

	void bind(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices) {
		glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.at(0), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.at(0), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)offsetof(Vertex, coordinates));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)offsetof(Vertex, texture_coordinates));
        glEnableVertexAttribArray(1);
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