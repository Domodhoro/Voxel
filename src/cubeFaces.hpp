#ifndef CUBE_FACES_HPP
#define CUBE_FACES_HPP

struct Faces {
    bool front, back, right, left, up, down;

    Faces() : front(true), back(true), right(true), left(true), up(true), down(true) { }
};

struct Vertex3D {
    glm::vec3 coordinates;
    glm::vec2 textureCoordinates;
};

using CubeFace = std::array<Vertex3D, 4>;

CubeFace createCubeFrontFace(float x, float y, float z) {
	return CubeFace {
        Vertex3D{{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 0.0f}}
    };
}

CubeFace createCubeBackFace(float x, float y, float z) {
	return CubeFace {
        Vertex3D{{x - 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x - 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
    };
}

CubeFace createCubeRightFace(float x, float y, float z) {
	return CubeFace {
        Vertex3D{{x + 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
    };
}

CubeFace createCubeLeftFace(float x, float y, float z) {
	return CubeFace {
        Vertex3D{{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x - 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x - 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
    };
}

CubeFace createCubeUpFace(float x, float y, float z) {
	return CubeFace {
        Vertex3D{{x - 0.5f, y + 0.5f, z + 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x - 0.5f, y + 0.5f, z - 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y + 0.5f, z - 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y + 0.5f, z + 0.5f}, {1.0f, 0.0f}}
    };
}

CubeFace createCubeDownFace(float x, float y, float z) {
	return CubeFace {
        Vertex3D{{x - 0.5f, y - 0.5f, z + 0.5f}, {0.0f, 0.0f}},
        Vertex3D{{x - 0.5f, y - 0.5f, z - 0.5f}, {0.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z - 0.5f}, {1.0f, 1.0f}},
        Vertex3D{{x + 0.5f, y - 0.5f, z + 0.5f}, {1.0f, 0.0f}}
    };
}

#endif