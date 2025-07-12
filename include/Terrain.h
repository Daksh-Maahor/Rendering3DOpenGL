#pragma once
#include "Cube.h"
#include "Shader.h"
#include "PerlinNoise.h"
#include <vector>
#include <glm/glm.hpp>

class Terrain {
public:
    Terrain(int width, int height, float scale = 1.0f);
    ~Terrain();
    
    void generate();
    void draw(Shader& shader);
    float getHeightAt(float x, float z) const;
    bool isInBounds(int x, int z) const;
    bool hasBlockAt(int x, int y, int z) const;
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    // Terrain properties
    void setScale(float scale);
    void setOctaves(int octaves);
    void setPersistence(float persistence);
    void setLacunarity(float lacunarity);
    void setBaseHeight(float baseHeight);
    void setHeightMultiplier(float heightMultiplier);

private:
    int width, height;
    float scale;
    float baseHeight;
    float heightMultiplier;
    int octaves;
    float persistence;
    float lacunarity;
    
    std::vector<std::vector<float>> heightMap;
    std::vector<glm::vec3> cubePositions;
    std::vector<glm::vec3> cubeColors;
    
    Cube cube;
    PerlinNoise noiseGenerator;
    
    void generateHeightMap();
    void generateCubes();
    glm::vec3 getTerrainColor(float height) const;
    bool isFaceVisible(int x, int y, int z, int face) const;
    void drawCubeWithFaceCulling(Shader& shader, const glm::vec3& position, const glm::vec3& color);
}; 