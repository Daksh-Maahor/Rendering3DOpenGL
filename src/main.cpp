#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Cube.h"
#include "CharacterController.h"
#include "Ground.h"
#include "Terrain.h"

// Global variables
Camera camera(glm::vec3(0.0f, 1.5f, 3.0f));
CharacterController* player = nullptr;
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderCrosshair();

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Cube Renderer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture the cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set initial viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Build and compile shaders
    Shader shader;
    if (!shader.loadFromFiles("shaders/vertex.glsl", "shaders/fragment.glsl")) {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }

    // Create terrain
    Terrain terrain(64, 64, 20.0f); // 64x64 terrain with scale 20
    terrain.setHeightMultiplier(8.0f);
    terrain.setOctaves(6);
    terrain.generate();

    // Create character controller
    player = new CharacterController(camera, terrain);
    
    float startX = 0.0f;
    float startZ = 0.0f;
    float terrainHeight = terrain.getHeightAt(startX, startZ);
    float startY = terrainHeight + 1.0f; // Start 1 unit above terrain
    
    std::cout << "Terrain height at (0,0): " << terrainHeight << std::endl;
    std::cout << "Setting player start position to: (" << startX << ", " << startY << ", " << startZ << ")" << std::endl;
    
    player->setPosition(glm::vec3(startX, startY, startZ));

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input and update
        player->processInput(window);
        player->update(deltaTime);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader
        shader.use();

        // Get current framebuffer size for correct aspect ratio
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = camera.getProjectionMatrix(static_cast<float>(width) / height);
        shader.setMat4("projection", projection);

        // Camera/view transformation
        glm::mat4 view = camera.getViewMatrix();
        shader.setMat4("view", view);

        // Set light properties
        shader.setVec3("lightPos", glm::vec3(10.0f, 20.0f, 10.0f));
        shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("viewPos", camera.position);

        // Render terrain
        terrain.draw(shader);

        // Render crosshair overlay
        renderCrosshair();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Cleanup
    delete player;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (player) {
        player->processMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.zoom -= (float)yoffset;
    if (camera.zoom < 1.0f)
        camera.zoom = 1.0f;
    if (camera.zoom > 45.0f)
        camera.zoom = 45.0f;
}

void renderCrosshair() {
    // Disable depth testing for 2D overlay
    glDisable(GL_DEPTH_TEST);
    
    // Save current matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Draw crosshair
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    
    glBegin(GL_LINES);
    glVertex2f(-0.2f, 0.0f);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.0f, -0.2f);
    glVertex2f(0.0f, 0.2f);
    glEnd();
    
    // Restore matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
}
