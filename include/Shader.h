#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    ~Shader();
    
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    bool loadFromStrings(const std::string& vertexSource, const std::string& fragmentSource);
    void use();
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setMat4(const std::string& name, const glm::mat4& value);

private:
    unsigned int programID;
    std::unordered_map<std::string, int> uniformCache;
    
    bool compileShader(const std::string& source, GLenum type, unsigned int& shaderID);
    bool linkProgram();
    int getUniformLocation(const std::string& name);
}; 