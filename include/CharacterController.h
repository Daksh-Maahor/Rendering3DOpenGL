#pragma once
#include "Camera.h"
#include "Terrain.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class CharacterController {
public:
    CharacterController(Camera& camera, Terrain& terrain);
    
    void update(float deltaTime);
    void processInput(GLFWwindow* window);
    void processMouseMovement(float xoffset, float yoffset);
    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const;
    void setSpeed(float speed);
    float getSpeed() const;
    
    // Movement state
    bool isMoving() const;
    bool isJumping() const;
    bool isOnGround() const;
    
    // Physics
    void setGravity(float gravity);
    void setJumpForce(float jumpForce);
    void setGroundLevel(float groundLevel);

private:
    Camera& camera;
    Terrain& terrain;
    glm::vec3 position;
    glm::vec3 velocity;
    
    // Movement
    float moveSpeed;
    float jumpForce;
    float gravity;
    float groundLevel;
    
    // Collision
    float playerRadius;
    float playerHeight;
    
    // State
    bool onGround;
    bool jumping;
    bool moving;
    float lastTerrainHeight;
    
    // Input
    bool keys[4]; // W, A, S, D
    
    void updatePhysics(float deltaTime);
    void updateCamera();
    void handleCollision();
    float getTerrainHeightAt(float x, float z) const;
    
    // Collision detection methods
    bool checkBlockCollision(const glm::vec3& pos) const;
    bool checkBlockCollision(const glm::vec3& pos, float radius, float height) const;
    bool checkSweptCollision(const glm::vec3& startPos, const glm::vec3& endPos, float radius, float height) const;
    glm::vec3 resolveCollision(const glm::vec3& oldPos, const glm::vec3& newPos) const;
    bool isPositionValid(const glm::vec3& pos) const;
}; 