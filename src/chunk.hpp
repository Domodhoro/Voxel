#ifndef CHUNK_HPP
#define CHUNK_HPP

enum class BLOCK_TYPE {
    AIR, GRASS, DIRT
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
    				if (getBlockType(x, y, z) != BLOCK_TYPE::AIR) {
    					Faces faces = otimization(x, y, z);
	    				
	    				generateMesh(x, y, z, i, faces);
			        }
    			}
    		}
    	}

        vertexArray.bind<Vertex3D>(vertices, indices, 3, 2);
    }

    void setBlockType(int x, int y, int z, BLOCK_TYPE blockType) {
	    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_LENGTH) {
	        blocks[x + y * CHUNK_WIDTH + z * CHUNK_WIDTH * CHUNK_HEIGHT] = blockType;
	    }
	}

    BLOCK_TYPE getBlockType(int x, int y, int z) const {
		return blocks.at(x + y * CHUNK_WIDTH + z * CHUNK_WIDTH * CHUNK_HEIGHT);
	}

	bool checkCollision(Camera &camera) const {
		glm::vec3 cameraPos = camera.getPosition(), falfSize = glm::vec3(0.5f, 1.0f, 0.5f);

	    AABB cameraAABB(cameraPos - falfSize, cameraPos + falfSize);

	    for (int x = 0; x < CHUNK_WIDTH; x++) {
	        for (int y = 0; y < CHUNK_HEIGHT; y++) {
	            for (int z = 0; z < CHUNK_LENGTH; z++) {
	                if (getBlockType(x, y, z) != BLOCK_TYPE::AIR) {
	                    glm::vec3 blockPos = position + glm::vec3(x, y, z) - 0.5f;

	                    AABB blockAABB(blockPos, blockPos + glm::vec3(1.0f));

	                    if (cameraAABB.intersects(blockAABB)) {
	                        return true;
	                    }
	                }
	            }
	        }
	    }

	    return false;
	}

    void draw(ShaderProgram &shaderProgram, Texture &texture, Camera &camera) const {
        shaderProgram.use();

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, position);

        shaderProgram.setMat4("Projection", camera.getProjection());
        shaderProgram.setMat4("View", camera.getView());
        shaderProgram.setMat4("Model", model);

        texture.bind("BLOCKS", GL_TEXTURE_2D);
        vertexArray.bind();
        
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        texture.unbind(GL_TEXTURE_2D);
        vertexArray.unbind();
    }
protected:
	glm::vec3 position;

    VertexArray vertexArray;

    std::vector<BLOCK_TYPE> blocks;
    std::vector<Vertex3D> vertices;
	std::vector<unsigned int> indices;

	Faces otimization(int x, int y, int z) {
		Faces faces = {
			true, true, true, true, true, true
		};

    	if (x > 0 && getBlockType(x - 1, y, z) != BLOCK_TYPE::AIR) {
			faces.left = false;
		}

        if (y > 0 && getBlockType(x, y - 1, z) != BLOCK_TYPE::AIR) {
        	faces.down = false;
        }

        if (z > 0 && getBlockType(x, y, z - 1) != BLOCK_TYPE::AIR) {
        	faces.front = false;
        }

        if (x < (CHUNK_WIDTH - 1) && getBlockType(x + 1, y, z) != BLOCK_TYPE::AIR) {
        	faces.right = false;
        }

        if (y < (CHUNK_HEIGHT - 1) && getBlockType(x, y + 1, z) != BLOCK_TYPE::AIR) {
        	faces.up = false;
        }

        if (z < (CHUNK_LENGTH - 1) && getBlockType(x, y, z + 1) != BLOCK_TYPE::AIR) {
        	faces.back = false;
        }

        return faces;
	}

	void generateMesh(int x, int y, int z, unsigned int &i, Faces &faces) {
		if (faces.front) {
			auto front = createBlockFrontFace(x, y, z);

			vertices.insert(vertices.end(), std::begin(front), std::end(front));

			unsigned int frontIndices[] = {
	        	i + 0u, i + 1u, i + 3u, i + 3u, i + 1u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(frontIndices), std::end(frontIndices));

	        i += 4u;
		}

		if (faces.back) {
			auto back = createBlockBackFace(x, y, z);

			vertices.insert(vertices.end(), std::begin(back), std::end(back));

			unsigned int backIndices[] = {
	        	i + 1u, i + 0u, i + 3u, i + 1u, i + 3u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(backIndices), std::end(backIndices));

			i += 4u;
		}

		if (faces.right) {
			auto right = createBlockRightFace(x, y, z);

			vertices.insert(vertices.end(), std::begin(right), std::end(right));

			unsigned int rightIndices[] = {
	        	i + 0u, i + 1u, i + 3u, i + 3u, i + 1u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(rightIndices), std::end(rightIndices));

			i += 4u;
		}

		if (faces.left) {
			auto left = createBlockLeftFace(x, y, z);

			vertices.insert(vertices.end(), std::begin(left), std::end(left));

			unsigned int leftIndices[] = {
	        	i + 1u, i + 0u, i + 3u, i + 1u, i + 3u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(leftIndices), std::end(leftIndices));

			i += 4u;
		}

		if (faces.up) {
			auto up = createBlockUpFace(x, y, z);

			vertices.insert(vertices.end(), std::begin(up), std::end(up));

			unsigned int upIndices[] = {
	        	i + 0u, i + 1u, i + 3u, i + 3u, i + 1u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(upIndices), std::end(upIndices));

			i += 4u;
		}

		if (faces.down) {
			auto down = createBlockDownFace(x, y, z);

			vertices.insert(vertices.end(), std::begin(down), std::end(down));

			unsigned int downIndices[] = {
	        	i + 1u, i + 0u, i + 3u, i + 1u, i + 3u, i + 2u
	        };

	        indices.insert(indices.end(), std::begin(downIndices), std::end(downIndices));

			i += 4u;
		}
    }
};

#endif