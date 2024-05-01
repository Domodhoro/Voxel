#ifndef BLOCK_HPP
#define BLOCK_HPP

struct Vertex {
    glm::vec3 coordinates;
    glm::vec2 texture_coordinates;
};

class Block {
public:
    Block() {
    	vertices = {
	        {{-0.5f, 0.5f,-0.5f}, {0.0f, 0.0f}},
	        {{-0.5f,-0.5f,-0.5f}, {0.0f, 1.0f}},
	        {{ 0.5f,-0.5f,-0.5f}, {1.0f, 1.0f}},
	        {{ 0.5f, 0.5f,-0.5f}, {1.0f, 0.0f}},
	        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
	        {{-0.5f,-0.5f, 0.5f}, {0.0f, 1.0f}},
	        {{ 0.5f,-0.5f, 0.5f}, {1.0f, 1.0f}},
	        {{ 0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
	        {{ 0.5f, 0.5f,-0.5f}, {0.0f, 0.0f}},
	        {{ 0.5f,-0.5f,-0.5f}, {0.0f, 1.0f}},
	        {{ 0.5f,-0.5f, 0.5f}, {1.0f, 1.0f}},
	        {{ 0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
	        {{-0.5f, 0.5f,-0.5f}, {0.0f, 0.0f}},
	        {{-0.5f,-0.5f,-0.5f}, {0.0f, 1.0f}},
	        {{-0.5f,-0.5f, 0.5f}, {1.0f, 1.0f}},
	        {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
	        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
	        {{-0.5f, 0.5f,-0.5f}, {0.0f, 1.0f}},
	        {{ 0.5f, 0.5f,-0.5f}, {1.0f, 1.0f}},
	        {{ 0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
	        {{-0.5f,-0.5f, 0.5f}, {0.0f, 0.0f}},
	        {{-0.5f,-0.5f,-0.5f}, {0.0f, 1.0f}},
	        {{ 0.5f,-0.5f,-0.5f}, {1.0f, 1.0f}},
	        {{ 0.5f,-0.5f, 0.5f}, {1.0f, 0.0f}}
	    };

	    indices = {
	         0u,  1u,  3u,  3u,  1u,  2u,
	         5u,  4u,  7u,  5u,  7u,  6u,
	         8u,  9u, 11u, 11u,  9u, 10u,
	        13u, 12u, 15u, 13u, 15u, 14u,
	        16u, 17u, 19u, 19u, 17u, 18u,
	        21u, 20u, 23u, 21u, 23u, 22u
	    };

        setup();

        vertices.clear();
    	indices.clear();
    }

    void draw(Shader &shader, Image &image, Camera &camera) const {
        image.use();
        shader.use();

        glBindVertexArray(VAO);

        shader.set_mat4("Projection", camera.get_projection());
        shader.set_mat4("View", camera.get_view());
        
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(0));
    }
protected:
    unsigned int VAO = 0u, VBO = 0u, EBO = 0u, count = 0u;
    std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	void setup() {
		glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.at(0), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.at(0), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        count = indices.size();
	}
};

#endif