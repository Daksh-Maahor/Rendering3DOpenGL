#pragma once
#include "Mesh.h"
#include <memory>

class Ground {
public:
    Ground(float size = 20.0f);
    ~Ground();
    
    void draw();
    static std::vector<Vertex> getGroundVertices(float size);
    static std::vector<unsigned int> getGroundIndices();

private:
    std::unique_ptr<Mesh> mesh;
}; 