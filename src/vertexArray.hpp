#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

class VertexArray {
public:
	VertexArray() {
		glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
	}

	template<typename T>
	void bind(std::vector<T> &vertices, std::vector<unsigned int> &indices, int attribPointer1, int attribPointer2) {
		glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), &vertices.at(0), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.at(0), GL_STATIC_DRAW);

        glVertexAttribPointer(0, attribPointer1, GL_FLOAT, false, sizeof(T), (void*)offsetof(T, coordinates));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, attribPointer2, GL_FLOAT, false, sizeof(T), (void*)offsetof(T, textureCoordinates));
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