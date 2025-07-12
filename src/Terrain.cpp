#include "Terrain.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Terrain::Terrain(int w, int h, float s) 
    : width(w), height(h), scale(s), baseHeight(2.0f), heightMultiplier(8.0f),
      octaves(4), persistence(0.5f), lacunarity(2.0f), noiseGenerator(42) {
    heightMap.resize(height, std::vector<float>(width, 0.0f));
}

Terrain::~Terrain() = default;

void Terrain::generate() {
    generateHeightMap();
    generateCubes();
}

void Terrain::draw(Shader& shader) {
    for (size_t i = 0; i < cubePositions.size(); i++) {
        drawCubeWithFaceCulling(shader, cubePositions[i], cubeColors[i]);
    }
}

float Terrain::getHeightAt(float x, float z) const {
    // Convert world coordinates to terrain grid coordinates
    // Terrain is centered at (0,0) and goes from -width/2 to width/2
    int gridX = static_cast<int>(x + width / 2.0f);
    int gridZ = static_cast<int>(z + height / 2.0f);
    
    if (!isInBounds(gridX, gridZ)) {
        return baseHeight;
    }
    
    return heightMap[gridZ][gridX];
}

bool Terrain::isInBounds(int x, int z) const {
    return x >= 0 && x < width && z >= 0 && z < height;
}

bool Terrain::hasBlockAt(int x, int y, int z) const {
    // Convert world coordinates to terrain grid coordinates
    int gridX = static_cast<int>(x + width / 2.0f);
    int gridZ = static_cast<int>(z + height / 2.0f);
    int gridY = y;
    
    // Debug: Track block checks
    static int debugCounter = 0;
    bool debugThisCheck = (debugCounter < 50);
    
    if (debugThisCheck) {
        std::cout << "hasBlockAt: world(" << x << "," << y << "," << z << ") -> grid(" << gridX << "," << gridY << "," << gridZ << ")";
        debugCounter++;
    }
    
    // Check if position is in bounds
    if (!isInBounds(gridX, gridZ) || gridY < 0) {
        if (debugThisCheck) {
            std::cout << " -> OUT OF BOUNDS" << std::endl;
        }
        return false;
    }
    
    // Check if there's a block at this position
    bool hasBlock = (gridY < heightMap[gridZ][gridX]);
    
    if (debugThisCheck) {
        std::cout << " -> height=" << heightMap[gridZ][gridX] << " -> " << (hasBlock ? "BLOCK" : "EMPTY") << std::endl;
    }
    
    return hasBlock;
}

void Terrain::setScale(float s) { scale = s; }
void Terrain::setOctaves(int o) { octaves = o; }
void Terrain::setPersistence(float p) { persistence = p; }
void Terrain::setLacunarity(float l) { lacunarity = l; }
void Terrain::setBaseHeight(float h) { baseHeight = h; }
void Terrain::setHeightMultiplier(float m) { heightMultiplier = m; }



void Terrain::generateHeightMap() {
    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
            float sampleX = x / scale;
            float sampleZ = z / scale;
            
            // Use the new PerlinNoise class
            float noiseHeight = noiseGenerator.octaveNoise(sampleX, sampleZ, octaves, persistence, lacunarity);
            
            // Convert to positive height range and round to integer
            heightMap[z][x] = std::round(baseHeight + noiseHeight * heightMultiplier);
        }
    }
}

void Terrain::generateCubes() {
    cubePositions.clear();
    cubeColors.clear();
    
    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
            float terrainHeight = heightMap[z][x];
            
            // Only place cubes above ground level
            if (terrainHeight > 0.0f) {
                // Calculate world position
                float worldX = x - width / 2.0f;
                float worldZ = z - height / 2.0f;
                
                // Place cubes from ground up to the terrain height
                for (int y = 0; y < static_cast<int>(terrainHeight); y++) {
                    cubePositions.push_back(glm::vec3(worldX, y, worldZ));
                    cubeColors.push_back(getTerrainColor(y));
                }
            }
        }
    }
}

glm::vec3 Terrain::getTerrainColor(float height) const {
    // Color based on height - creates natural-looking terrain
    if (height < 1.0f) {
        return glm::vec3(0.6f, 0.4f, 0.2f); // Sand/Brown
    } else if (height < 2.0f) {
        return glm::vec3(0.2f, 0.8f, 0.2f); // Grass
    } else if (height < 4.0f) {
        return glm::vec3(0.4f, 0.6f, 0.2f); // Darker grass
    } else if (height < 6.0f) {
        return glm::vec3(0.5f, 0.5f, 0.5f); // Stone
    } else {
        return glm::vec3(1.0f, 1.0f, 1.0f); // Snow
    }
} 

bool Terrain::isFaceVisible(int x, int y, int z, int face) const {
    // Convert world coordinates to terrain grid coordinates
    int gridX = static_cast<int>(x + width / 2.0f);
    int gridZ = static_cast<int>(z + height / 2.0f);
    
    // Check if the position is in bounds
    if (!isInBounds(gridX, gridZ)) {
        return true; // Render faces at boundaries
    }
    
    // Check if there's a cube at the current position
    if (y >= heightMap[gridZ][gridX]) {
        return false; // No cube here, don't render
    }
    
    // Check adjacent positions based on face direction
    // Face indices: 0=Front, 1=Back, 2=Left, 3=Right, 4=Top, 5=Bottom
    int adjX = gridX, adjY = y, adjZ = gridZ;
    
    switch (face) {
        case 0: adjZ++; break; // Front face (positive Z)
        case 1: adjZ--; break; // Back face (negative Z)
        case 2: adjX--; break; // Left face (negative X)
        case 3: adjX++; break; // Right face (positive X)
        case 4: adjY++; break; // Top face (positive Y)
        case 5: adjY--; break; // Bottom face (negative Y)
    }
    
    // If adjacent position is out of bounds, render the face
    if (!isInBounds(adjX, adjZ)) {
        return true;
    }
    
    // For bottom face, always render if we're at ground level
    if (face == 5 && y == 0) {
        return true;
    }
    
    // For top face, always render if we're at the top of the terrain
    if (face == 4 && y == static_cast<int>(heightMap[gridZ][gridX]) - 1) {
        return true;
    }
    
    // If adjacent position has no cube (y >= height), render the face
    if (adjY >= heightMap[adjZ][adjX]) {
        return true;
    }
    
    // Face is hidden by adjacent cube
    return false;
}

void Terrain::drawCubeWithFaceCulling(Shader& shader, const glm::vec3& position, const glm::vec3& color) {
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);
    int z = static_cast<int>(position.z);
    
    // Create model matrix for this cube
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    
    // Set shader uniforms
    shader.setMat4("model", model);
    shader.setVec3("objectColor", color);
    
    // Draw only visible faces
    for (int face = 0; face < 6; face++) {
        if (isFaceVisible(x, y, z, face)) {
            cube.drawFace(face);
        }
    }
} 