#pragma once
#include <vector>
#include <cmath>

class PerlinNoise {
public:
    PerlinNoise(unsigned int seed = 0);
    
    // Generate noise value at given coordinates
    float noise(float x, float y) const;
    float noise(float x, float y, float z) const;
    
    // Generate octave noise (multiple frequencies)
    float octaveNoise(float x, float y, int octaves, float persistence, float lacunarity) const;
    
    // Set seed for reproducible results
    void setSeed(unsigned int seed);

private:
    std::vector<int> p;
    
    float fade(float t) const;
    float lerp(float t, float a, float b) const;
    float grad(int hash, float x, float y, float z) const;
}; 