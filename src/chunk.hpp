#ifndef CHUNK_HPP
#define CHUNK_HPP

enum class BLOCK_TYPE {
	NONE,
    AIR,
    GRASS,
    DIRT,
    STONE
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

        vertexArray.bind(vertices, indices);
    }

    void setBlockType(int x, int y, int z, BLOCK_TYPE blockType) {
	    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_LENGTH) {
	        blocks[x + y * CHUNK_WIDTH + z * CHUNK_WIDTH * CHUNK_HEIGHT] = blockType;
	    }
	}

    BLOCK_TYPE getBlockType(int x, int y, int z) const {
    	if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_LENGTH) {
	        return blocks.at(x + y * CHUNK_WIDTH + z * CHUNK_WIDTH * CHUNK_HEIGHT);
	    }

	    return BLOCK_TYPE::NONE;
	}

	bool checkCollision(Camera &camera) {
        AABB cameraAABB = camera.getAABB();

        for (int x = 0; x != CHUNK_WIDTH; x++) {
            for (int y = 0; y != CHUNK_HEIGHT; y++) {
                for (int z = 0; z != CHUNK_LENGTH; z++) {
                    if (getBlockType(x, y, z) != BLOCK_TYPE::AIR) {
                    	glm::vec3 blockPosition(x, y, z), halfSize(0.5f);

                    	blockPosition += position;

                        AABB blockAABB { 
                        	blockPosition - halfSize,
                        	blockPosition + halfSize
                        };

                        bool collisionX = cameraAABB.max.x >= blockAABB.min.x && blockAABB.max.x >= cameraAABB.min.x;
                        bool collisionY = cameraAABB.max.y >= blockAABB.min.y && blockAABB.max.y >= cameraAABB.min.y;
                        bool collisionZ = cameraAABB.max.z >= blockAABB.min.z && blockAABB.max.z >= cameraAABB.min.z;

                        if (collisionX && collisionY && collisionZ) {
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

	bool renderLimit(Camera &camera, float renderLimitDistance) {
	    glm::vec3 cameraPosition = camera.getPosition();
	    glm::vec3 chunkCenter = position + glm::vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_LENGTH) / 2.0f;

	    float distance = glm::distance(cameraPosition, chunkCenter);

	    if (distance < renderLimitDistance) {
	        return true;
	    }

	    return false;
	}

    void render(ShaderProgram &shaderProgram, Texture &texture, Camera &camera) const {
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
		Faces faces;

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
			CubeFace front = createCubeFrontFace(x, y, z);

			unsigned int frontIndices[] = {
	        	i + 0u, i + 1u, i + 3u, 
	        	i + 3u, i + 1u, i + 2u
	        };

	        vertices.insert(vertices.end(), std::begin(front), std::end(front));

	        indices.insert(indices.end(), std::begin(frontIndices), std::end(frontIndices));

	        i += 4u;
		}

		if (faces.back) {
			CubeFace back = createCubeBackFace(x, y, z);

			unsigned int backIndices[] = {
	        	i + 1u, i + 0u, i + 3u, 
	        	i + 1u, i + 3u, i + 2u
	        };

	        vertices.insert(vertices.end(), std::begin(back), std::end(back));

	        indices.insert(indices.end(), std::begin(backIndices), std::end(backIndices));

			i += 4u;
		}

		if (faces.right) {
			CubeFace right = createCubeRightFace(x, y, z);

			unsigned int rightIndices[] = {
	        	i + 0u, i + 1u, i + 3u, 
	        	i + 3u, i + 1u, i + 2u
	        };

	        vertices.insert(vertices.end(), std::begin(right), std::end(right));

	        indices.insert(indices.end(), std::begin(rightIndices), std::end(rightIndices));

			i += 4u;
		}

		if (faces.left) {
			CubeFace left = createCubeLeftFace(x, y, z);

			unsigned int leftIndices[] = {
	        	i + 1u, i + 0u, i + 3u, 
	        	i + 1u, i + 3u, i + 2u
	        };

	        vertices.insert(vertices.end(), std::begin(left), std::end(left));

	        indices.insert(indices.end(), std::begin(leftIndices), std::end(leftIndices));

			i += 4u;
		}

		if (faces.up) {
			CubeFace up = createCubeUpFace(x, y, z);

			unsigned int upIndices[] = {
	        	i + 0u, i + 1u, i + 3u, 
	        	i + 3u, i + 1u, i + 2u
	        };

	        vertices.insert(vertices.end(), std::begin(up), std::end(up));

	        indices.insert(indices.end(), std::begin(upIndices), std::end(upIndices));

			i += 4u;
		}

		if (faces.down) {
			CubeFace down = createCubeDownFace(x, y, z);

			unsigned int downIndices[] = {
	        	i + 1u, i + 0u, i + 3u, 
	        	i + 1u, i + 3u, i + 2u
	        };

	        vertices.insert(vertices.end(), std::begin(down), std::end(down));
	        
	        indices.insert(indices.end(), std::begin(downIndices), std::end(downIndices));

			i += 4u;
		}
    }
};

#endif