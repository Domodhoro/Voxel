#ifndef CHUNK_MANAGER_HPP
#define CHUNK_MANAGER_HPP

class ChunkManager {
public:
	ChunkManager() {
		chunks.emplace_back(glm::vec3(0.0f));
	}

	void addChunk(const glm::vec3 &position) {
		chunks.emplace_back(position);
	}

	bool checkCollision(Camera &camera) {
		for (auto chunk : chunks) {
			if (chunk.checkCollision(camera)) {
                return true;
            }
		}

		return false;
	}

	void render(ShaderProgram &shaderProgram, Texture &textures, Camera &camera) const {
		for (auto chunk : chunks) {
			if (chunk.renderLimit(camera, RENDER_LIMIT_DISTANCE)) {
                chunk.render(shaderProgram, textures, camera);
            }
		}
	}
protected:
	std::vector<Chunk> chunks;
};

#endif