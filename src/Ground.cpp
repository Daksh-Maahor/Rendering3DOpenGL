#include "Ground.h"

Ground::Ground(float size) {
    mesh = std::make_unique<Mesh>(getGroundVertices(size), getGroundIndices());
}

Ground::~Ground() = default;

void Ground::draw() {
    mesh->draw();
}

std::vector<Vertex> Ground::getGroundVertices(float size) {
    float halfSize = size * 0.5f;
    std::vector<Vertex> vertices = {
        // Ground plane (facing up)
        {{-halfSize, 0.0f, -halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ halfSize, 0.0f, -halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ halfSize, 0.0f,  halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-halfSize, 0.0f,  halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
    };
    
    return vertices;
}

std::vector<unsigned int> Ground::getGroundIndices() {
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0
    };
    
    return indices;
}
