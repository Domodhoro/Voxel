#ifndef BLOCK_FACES_HPP
#define BLOCK_FACES_HPP

struct Faces {
    bool front, back, right, left, up, down;
};

struct Vertex3D {
    glm::vec3 coordinates;
    glm::vec2 textureCoordinates;
};

std::array<Vertex3D, 4> createBlockFrontFace(float x, float y, float z) {
	std::array<Vertex3D, 4> front = {
        Vertex3D{{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 0.0f}}
    };

    return front;
}

std::array<Vertex3D, 4> createBlockBackFace(float x, float y, float z) {
	std::array<Vertex3D, 4> back = {
        Vertex3D{{x - 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x - 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
    };

    return back;
}

std::array<Vertex3D, 4> createBlockRightFace(float x, float y, float z) {
	std::array<Vertex3D, 4> right = {
        Vertex3D{{x + 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
    };

    return right;
}

std::array<Vertex3D, 4> createBlockLeftFace(float x, float y, float z) {
	std::array<Vertex3D, 4> left = {
        Vertex3D{{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x - 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x - 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
    };

    return left;
}

std::array<Vertex3D, 4> createBlockUpFace(float x, float y, float z) {
	std::array<Vertex3D, 4> up = {
        Vertex3D{{x - 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
    };

    return up;
}

std::array<Vertex3D, 4> createBlockDownFace(float x, float y, float z) {
	std::array<Vertex3D, 4> down = {
        Vertex3D{{x - 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 0.0f}}
    };

    return down;
}

#endif