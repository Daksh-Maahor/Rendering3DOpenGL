cmake_minimum_required(VERSION 3.10)
project(Rendering3D)

set(CMAKE_CXX_STANDARD 17)

# Find OpenGL
find_package(OpenGL REQUIRED)
# Find GLFW
find_package(glfw3 REQUIRED)
# Find GLEW
find_package(GLEW REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Mesh.cpp
    src/Cube.cpp
    src/CharacterController.cpp
    src/Ground.cpp
    src/Terrain.cpp
    src/PerlinNoise.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
include_directories(include)

# Find GLM (OpenGL Mathematics)
find_path(GLM_INCLUDE_DIR glm/glm.hpp
    PATHS
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
)

if(NOT GLM_INCLUDE_DIR)
    message(FATAL_ERROR "GLM not found. Please install GLM.")
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    OpenGL::GL 
    glfw 
    GLEW::GLEW
)

# Include GLM headers
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_INCLUDE_DIR})
