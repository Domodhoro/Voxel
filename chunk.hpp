#ifndef CHUNK_HPP
#define CHUNK_HPP

enum class BLOCK_TYPE {
    AIR,
    GRASS,
    DIRT
};

struct Faces {
    bool front, back, right, left, up, down;
};

class Chunk {
public:
    Chunk(glm::vec3 position) : position(position) {
    	blocks.resize(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_LENGTH);

    	std::fill(blocks.begin(), blocks.end(), BLOCK_TYPE::GRASS);

    	unsigned int i = 0u;

    	for (int x = 0; x != CHUNK_WIDTH; x++) {
    		for (int y = 0; y != CHUNK_HEIGHT; y++) {
    			for (int z = 0; z != CHUNK_LENGTH; z++) {
    				if (get_block_type(x, y, z) != BLOCK_TYPE::AIR) {
    					Faces faces = otimization(x, y, z);
	    				
	    				generate_mesh(x, y, z, i, faces);
			        }
    			}
    		}
    	}

        vertex_array.bind(vertices, indices);
    }

    void draw(Shader &shader, Texture &texture, Camera &camera) const {
        shader.use();
        texture.bind();
        vertex_array.bind();

        glm::mat4 model = glm::mat4(1.0f);

        model = translate(model, position);

        shader.set_mat4("Projection", camera.get_projection());
        shader.set_mat4("View", camera.get_view());
        shader.set_mat4("Model", model);
        
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        vertex_array.unbind();
    }
protected:
	glm::vec3 position;
    Vertex_Array vertex_array;
    std::vector<BLOCK_TYPE> blocks;
    std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	BLOCK_TYPE get_block_type(int x, int y, int z) const {
		return blocks.at(x + y * CHUNK_WIDTH + z * CHUNK_WIDTH * CHUNK_HEIGHT);
	}

	Faces otimization(int x, int y, int z) {
		Faces faces = {
			true, true, true, true, true, true
		};

    	if (x > 0 && get_block_type(x - 1, y, z) != BLOCK_TYPE::AIR) {
			faces.left = false;
		}

        if (y > 0 && get_block_type(x, y - 1, z) != BLOCK_TYPE::AIR) {
        	faces.down = false;
        }

        if (z > 0 && get_block_type(x, y, z - 1) != BLOCK_TYPE::AIR) {
        	faces.front = false;
        }

        if (x < (CHUNK_WIDTH - 1) && get_block_type(x + 1, y, z) != BLOCK_TYPE::AIR) {
        	faces.right = false;
        }

        if (y < (CHUNK_HEIGHT - 1) && get_block_type(x, y + 1, z) != BLOCK_TYPE::AIR) {
        	faces.up = false;
        }

        if (z < (CHUNK_LENGTH - 1) && get_block_type(x, y, z + 1) != BLOCK_TYPE::AIR) {
        	faces.back = false;
        }

        return faces;
	}

	void generate_mesh(int x, int y, int z, unsigned int &i, Faces &faces) {
		if (faces.front) {
			Vertex front[] = {
				{{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f}},
		        {{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
		        {{x + 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 1.0f}},
		        {{x + 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 0.0f}}
			};

			vertices.insert(vertices.end(), std::begin(front), std::end(front));

			unsigned int front_indices[] = {
	        	i + 0u, i + 1u, i + 3u, i + 3u, i + 1u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(front_indices), std::end(front_indices));

	        i += 4u;
		}

		if (faces.back) {
			Vertex back[] = {
				{{x - 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 0.0f}},
		        {{x - 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 1.0f}},
		        {{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 1.0f}},
		        {{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
			};

			vertices.insert(vertices.end(), std::begin(back), std::end(back));

			unsigned int back_indices[] = {
	        	i + 1u, i + 0u, i + 3u, i + 1u, i + 3u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(back_indices), std::end(back_indices));

			i += 4u;
		}

		if (faces.right) {
			Vertex right[] = {
				{{x + 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f}},
		        {{x + 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
		        {{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 1.0f}},
		        {{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
			};

			vertices.insert(vertices.end(), std::begin(right), std::end(right));

			unsigned int right_indices[] = {
	        	i + 0u, i + 1u, i + 3u, i + 3u, i + 1u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(right_indices), std::end(right_indices));

			i += 4u;
		}

		if (faces.left) {
			Vertex left[] = {
				{{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f}},
		        {{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
		        {{x - 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 1.0f}},
		        {{x - 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
			};

			vertices.insert(vertices.end(), std::begin(left), std::end(left));

			unsigned int left_indices[] = {
	        	i + 1u, i + 0u, i + 3u, i + 1u, i + 3u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(left_indices), std::end(left_indices));

			i += 4u;
		}

		if (faces.up) {
			Vertex up[] = {
				{{x - 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 0.0f}},
		        {{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 1.0f}},
		        {{x + 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 1.0f}},
		        {{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
			};

			vertices.insert(vertices.end(), std::begin(up), std::end(up));

			unsigned int up_indices[] = {
	        	i + 0u, i + 1u, i + 3u, i + 3u, i + 1u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(up_indices), std::end(up_indices));

			i += 4u;
		}

		if (faces.down) {
			Vertex down[] = {
				{{x - 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 0.0f}},
		        {{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
		        {{x + 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 1.0f}},
		        {{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 0.0f}}
			};

			vertices.insert(vertices.end(), std::begin(down), std::end(down));

			unsigned int down_indices[] = {
	        	i + 1u, i + 0u, i + 3u, i + 1u, i + 3u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(down_indices), std::end(down_indices));

			i += 4u;
		}
    }
};

#endif