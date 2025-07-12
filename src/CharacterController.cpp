#include "CharacterController.h"
#include <algorithm>
#include <iostream> // Added for debugging output
#include <cmath> // Add this at the top if not present

CharacterController::CharacterController(Camera& camera, Terrain& terrain) 
    : camera(camera), terrain(terrain), position(0.0f, 1.0f, 0.0f), velocity(0.0f),
      moveSpeed(5.0f), jumpForce(8.0f), gravity(-20.0f), groundLevel(0.0f),
      playerRadius(0.3f), playerHeight(1.8f),
      onGround(true), jumping(false), moving(false), lastTerrainHeight(0.0f) {
    
    // Initialize key states
    for (int i = 0; i < 4; i++) {
        keys[i] = false;
    }
    
    // Set initial camera position
    camera.position = position + glm::vec3(0.0f, 0.5f, 0.0f);
}

void CharacterController::update(float deltaTime) {
    updatePhysics(deltaTime);
    updateCamera();
    handleCollision();
}

void CharacterController::processInput(GLFWwindow* window) {
    // Update key states
    keys[0] = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS; // W
    keys[1] = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS; // A
    keys[2] = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS; // S
    keys[3] = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS; // D
    
    // Jump
    static bool spacePressed = false;
    bool spaceCurrentlyPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    
    if (spaceCurrentlyPressed && !spacePressed && onGround) {
        velocity.y = jumpForce;
        onGround = false;
        jumping = true;
    }
    spacePressed = spaceCurrentlyPressed;
    
    // Calculate movement direction
    glm::vec3 moveDir(0.0f);
    moving = false;
    
    if (keys[0]) { // W - Forward
        moveDir += camera.front;
        moving = true;
    }
    if (keys[2]) { // S - Backward
        moveDir -= camera.front;
        moving = true;
    }
    if (keys[3]) { // D - Right
        moveDir += camera.right;
        moving = true;
    }
    if (keys[1]) { // A - Left
        moveDir -= camera.right;
        moving = true;
    }
    
    // Normalize movement direction and apply speed
    if (glm::length(moveDir) > 0.0f) {
        moveDir = glm::normalize(moveDir);
        // Keep Y component at 0 for ground movement
        moveDir.y = 0.0f;
        velocity.x = moveDir.x * moveSpeed;
        velocity.z = moveDir.z * moveSpeed;
    } else {
        // Apply friction when not moving
        velocity.x *= 0.8f;
        velocity.z *= 0.8f;
    }
}

void CharacterController::processMouseMovement(float xoffset, float yoffset) {
    camera.processMouseMovement(xoffset, yoffset);
}

void CharacterController::updatePhysics(float deltaTime) {
    // Always apply gravity (unless on ground)
    if (!onGround) {
        velocity.y += gravity * deltaTime;
    }
    
    // Apply terminal velocity
    if (velocity.y < -20.0f) {
        velocity.y = -20.0f;
    }
    
    // Sub-stepping: break movement into small steps to prevent tunneling
    const float maxStep = 0.1f; // Maximum movement per sub-step
    float timeLeft = deltaTime;
    glm::vec3 startPos = position;
    while (timeLeft > 0.0f) {
        float step = std::min(maxStep, timeLeft);
        glm::vec3 move = velocity * step;
        glm::vec3 newPosition = position + move;
        position = resolveCollision(position, newPosition);
        timeLeft -= step;
    }
    
    // Emergency correction: if inside a block, push up until not inside
    int emergencyPushes = 0;
    while (checkBlockCollision(position) && emergencyPushes < 10) {
        position.y += 0.1f;
        emergencyPushes++;
        if (emergencyPushes == 1) {
            std::cout << "[WARNING] Player inside block! Emergency push up." << std::endl;
        }
    }
    if (emergencyPushes >= 10) {
        std::cout << "[ERROR] Could not push player out of block after 10 tries!" << std::endl;
    }
}

void CharacterController::updateCamera() {
    // Update camera position to follow character
    camera.position = position + glm::vec3(0.0f, 0.5f, 0.0f);
}

void CharacterController::handleCollision() {
    // Check if we're on ground by looking for blocks below us
    glm::vec3 groundCheckPos = position;
    groundCheckPos.y -= 0.1f; // Check slightly below our feet
    
    bool wasOnGround = onGround;
    
    // More comprehensive ground check - check multiple points around the player's feet
    bool groundFound = false;
    float checkRadius = playerRadius;
    float step = 0.1f;
    
    for (float x = -checkRadius; x <= checkRadius; x += step) {
        for (float z = -checkRadius; z <= checkRadius; z += step) {
            if (x*x + z*z <= checkRadius*checkRadius) {
                glm::vec3 checkPos = groundCheckPos + glm::vec3(x, 0, z);
                if (terrain.hasBlockAt(static_cast<int>(checkPos.x), 
                                     static_cast<int>(checkPos.y), 
                                     static_cast<int>(checkPos.z))) {
                    groundFound = true;
                    break;
                }
            }
        }
        if (groundFound) break;
    }
    
    onGround = groundFound;
    
    if (onGround && !wasOnGround) {
        // We just landed
        velocity.y = 0.0f;
        jumping = false;
    }
    
    // Update last terrain height for slope detection
    lastTerrainHeight = getTerrainHeightAt(position.x, position.z);
}

bool CharacterController::checkBlockCollision(const glm::vec3& pos) const {
    return checkBlockCollision(pos, playerRadius, playerHeight);
}

bool CharacterController::checkBlockCollision(const glm::vec3& pos, float radius, float height) const {
    // Check the player's bounding box against terrain blocks
    // Player is represented as a cylinder with radius and height
    
    // Use a finer step for more precise collision detection
    float step = 0.1f; // Check every 0.1 units for better precision
    
    // Check from ground level (Y=0) up to the player's head (pos.y + height)
    float startY = pos.y;
    float endY = pos.y + height;
    const int numLevels = static_cast<int>((endY - startY) / step) + 1;
    
    // Debug: Track collision checks
    static int debugCounter = 0;
    bool debugThisFrame = (debugCounter < 20);
    
    if (debugThisFrame) {
        std::cout << "=== Collision Check ===" << std::endl;
        std::cout << "Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
        std::cout << "Radius: " << radius << ", Height: " << height << std::endl;
        std::cout << "Checking " << numLevels << " levels from Y=" << startY << " to Y=" << endY << std::endl;
    }
    
    for (int level = 0; level < numLevels; level++) {
        float checkY = startY + (level * step);
        
        if (debugThisFrame) {
            std::cout << "  Level " << level << " (checkY: " << checkY << ")" << std::endl;
        }
        
        // Check points around the player at this level
        for (float x = -radius; x <= radius; x += step) {
            for (float z = -radius; z <= radius; z += step) {
                // Only check points within the circle
                if (x*x + z*z <= radius*radius) {
                    float bx = std::floor(pos.x + x);
                    float by = std::floor(checkY);
                    float bz = std::floor(pos.z + z);
                    bool hasBlock = terrain.hasBlockAt(static_cast<int>(bx), static_cast<int>(by), static_cast<int>(bz));
                    
                    if (debugThisFrame && hasBlock) {
                        std::cout << "    BLOCK FOUND at (" << bx << ", " << by << ", " << bz << ")" << std::endl;
                    }
                    
                    if (hasBlock) {
                        if (debugThisFrame) {
                            std::cout << "  COLLISION DETECTED!" << std::endl;
                        }
                        return true; // Collision detected
                    }
                }
            }
        }
    }
    
    if (debugThisFrame) {
        std::cout << "  No collision detected" << std::endl;
        debugCounter++;
    }
    
    return false; // No collision
}

bool CharacterController::checkSweptCollision(const glm::vec3& startPos, const glm::vec3& endPos, float radius, float height) const {
    // Check collision along the entire path from start to end position
    const int numSteps = 10; // Check 10 points along the path
    glm::vec3 direction = endPos - startPos;
    float distance = glm::length(direction);
    
    if (distance < 0.001f) {
        return checkBlockCollision(startPos, radius, height);
    }
    
    glm::vec3 normalizedDir = glm::normalize(direction);
    
    for (int i = 0; i <= numSteps; i++) {
        float t = static_cast<float>(i) / numSteps;
        glm::vec3 checkPos = startPos + normalizedDir * (distance * t);
        
        if (checkBlockCollision(checkPos, radius, height)) {
            return true; // Collision detected along the path
        }
    }
    
    return false; // No collision along the path
}

glm::vec3 CharacterController::resolveCollision(const glm::vec3& oldPos, const glm::vec3& newPos) const {
    glm::vec3 resolvedPos = oldPos;
    glm::vec3 movement = newPos - oldPos;
    
    // Debug: Track collision resolution
    static int debugCounter = 0;
    bool debugThisFrame = (debugCounter < 10);
    
    if (debugThisFrame) {
        std::cout << "=== Collision Resolution ===" << std::endl;
        std::cout << "Old pos: (" << oldPos.x << ", " << oldPos.y << ", " << oldPos.z << ")" << std::endl;
        std::cout << "New pos: (" << newPos.x << ", " << newPos.y << ", " << newPos.z << ")" << std::endl;
        std::cout << "Movement: (" << movement.x << ", " << movement.y << ", " << movement.z << ")" << std::endl;
    }
    
    // Use swept collision detection to prevent tunneling
    if (checkSweptCollision(oldPos, newPos, playerRadius, playerHeight)) {
        if (debugThisFrame) {
            std::cout << "SWEPT COLLISION DETECTED! Blocking movement." << std::endl;
        }
        
        // Movement is blocked, try to find the closest safe position
        const int maxAttempts = 20;
        float stepSize = 0.05f;
        
        for (int attempt = 1; attempt <= maxAttempts; attempt++) {
            float t = 1.0f - (attempt * stepSize);
            if (t < 0.0f) t = 0.0f;
            
            glm::vec3 testPos = oldPos + movement * t;
            
            if (!checkBlockCollision(testPos, playerRadius, playerHeight)) {
                resolvedPos = testPos;
                if (debugThisFrame) {
                    std::cout << "Found safe position at t=" << t << ": (" << resolvedPos.x << ", " << resolvedPos.y << ", " << resolvedPos.z << ")" << std::endl;
                }
                break;
            }
        }
        
        // Stop velocity in the direction of collision
        if (glm::length(movement) > 0.001f) {
            glm::vec3 moveDir = glm::normalize(movement);
            if (std::abs(moveDir.x) > 0.1f) {
                const_cast<CharacterController*>(this)->velocity.x = 0.0f;
            }
            if (std::abs(moveDir.z) > 0.1f) {
                const_cast<CharacterController*>(this)->velocity.z = 0.0f;
            }
            if (std::abs(moveDir.y) > 0.1f) {
                const_cast<CharacterController*>(this)->velocity.y = 0.0f;
            }
        }
    } else {
        // No collision, allow full movement
        resolvedPos = newPos;
        if (debugThisFrame) {
            std::cout << "No collision, allowing full movement" << std::endl;
        }
    }
    
    if (debugThisFrame) {
        std::cout << "Final resolved pos: (" << resolvedPos.x << ", " << resolvedPos.y << ", " << resolvedPos.z << ")" << std::endl;
        debugCounter++;
    }
    
    return resolvedPos;
}

bool CharacterController::isPositionValid(const glm::vec3& pos) const {
    return !checkBlockCollision(pos);
}

float CharacterController::getTerrainHeightAt(float x, float z) const {
    return terrain.getHeightAt(x, z);
}

void CharacterController::setPosition(const glm::vec3& pos) {
    // Ensure we don't place the player inside blocks
    if (checkBlockCollision(pos)) {
        // Try to find a safe position above
        glm::vec3 safePos = pos;
        for (int i = 1; i <= 10; i++) {
            safePos.y = pos.y + i;
            if (!checkBlockCollision(safePos)) {
                position = safePos;
                camera.position = position + glm::vec3(0.0f, 0.5f, 0.0f);
                return;
            }
        }
        // If we can't find a safe position, just use the original
        std::cout << "Warning: Could not find safe spawn position!" << std::endl;
    }
    
    position = pos;
    camera.position = position + glm::vec3(0.0f, 0.5f, 0.0f);
}

glm::vec3 CharacterController::getPosition() const {
    return position;
}

void CharacterController::setSpeed(float speed) {
    moveSpeed = speed;
}

float CharacterController::getSpeed() const {
    return moveSpeed;
}

bool CharacterController::isMoving() const {
    return moving;
}

bool CharacterController::isJumping() const {
    return jumping;
}

bool CharacterController::isOnGround() const {
    return onGround;
}

void CharacterController::setGravity(float g) {
    gravity = g;
}

void CharacterController::setJumpForce(float force) {
    jumpForce = force;
}

void CharacterController::setGroundLevel(float level) {
    groundLevel = level;
} 