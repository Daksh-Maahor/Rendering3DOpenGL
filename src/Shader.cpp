#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

Shader::Shader() : programID(0) {}

Shader::~Shader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

bool Shader::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::string usedVertexPath = vertexPath;
    std::string usedFragmentPath = fragmentPath;

    // Try original path
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
        // Try ../ path
        vShaderFile.close();
        fShaderFile.close();
        vShaderFile.open("../" + vertexPath);
        fShaderFile.open("../" + fragmentPath);
        usedVertexPath = "../" + vertexPath;
        usedFragmentPath = "../" + fragmentPath;
        if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
            std::cerr << "Failed to open shader files: " << vertexPath << ", " << fragmentPath << std::endl;
            return false;
        }
    }
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vShaderFile.close();
    fShaderFile.close();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    std::cout << "Loaded vertex shader from: " << usedVertexPath << " (" << vertexCode.size() << " bytes)\n";
    std::cout << "Loaded fragment shader from: " << usedFragmentPath << " (" << fragmentCode.size() << " bytes)\n";
    return loadFromStrings(vertexCode, fragmentCode);
}

bool Shader::loadFromStrings(const std::string& vertexSource, const std::string& fragmentSource) {
    unsigned int vertex, fragment;
    
    if (!compileShader(vertexSource, GL_VERTEX_SHADER, vertex)) {
        return false;
    }
    
    if (!compileShader(fragmentSource, GL_FRAGMENT_SHADER, fragment)) {
        glDeleteShader(vertex);
        return false;
    }
    
    programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    
    if (!linkProgram()) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return true;
}

bool Shader::compileShader(const std::string& source, GLenum type, unsigned int& shaderID) {
    shaderID = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
        return false;
    }
    
    return true;
}

bool Shader::linkProgram() {
    glLinkProgram(programID);
    
    int success;
    char infoLog[512];
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    
    if (!success) {
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "Program linking error: " << infoLog << std::endl;
        return false;
    }
    
    return true;
}

void Shader::use() {
    glUseProgram(programID);
}

void Shader::setBool(const std::string& name, bool value) {
    glUniform1i(getUniformLocation(name), (int)value);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

int Shader::getUniformLocation(const std::string& name) {
    if (uniformCache.find(name) != uniformCache.end()) {
        return uniformCache[name];
    }
    
    int location = glGetUniformLocation(programID, name.c_str());
    uniformCache[name] = location;
    return location;
}
