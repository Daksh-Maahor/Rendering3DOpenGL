#pragma once
#include "Mesh.h"
#include <memory>

class Cube {
public:
    Cube();
    ~Cube();
    
    void draw();
    void drawFace(int faceIndex);
    static std::vector<Vertex> getCubeVertices();
    static std::vector<unsigned int> getCubeIndices();
    static std::vector<unsigned int> getFaceIndices(int faceIndex);

private:
    std::unique_ptr<Mesh> mesh;
}; 