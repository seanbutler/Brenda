#pragma once
#include "../IComponent.h"

/**
 * @brief Movement phases for coordinating AI and movement systems.
 */
enum class MovementPhase {
    IDLE,       // Entity is not moving, AI can plan new actions
    PLANNING,   // AI is planning next move
    MOVING,     // Entity is actively moving to target
    ARRIVING    // Entity just arrived at target, AI can react
};

/**
 * @brief Component to store a movement target and state.
 */
struct Movement : public ECS::IComponent {
    float targetX;
    float targetY;
    float speed;
    bool isMoving = false;
    MovementPhase phase = MovementPhase::IDLE;
    
    // Phase timing for coordination
    float phaseTimer = 0.0f;
    float planningDuration = 0.5f;  // Time to spend in planning phase
    float arrivingDuration = 0.2f;  // Time to spend in arriving phase

    Movement(float s) : speed(s), targetX(0), targetY(0), isMoving(false) {}

    /**
     * @brief Sets a new target destination and transitions to moving phase.
     */
    void setTarget(float x, float y) {
        targetX = x;
        targetY = y;
        isMoving = true;
        phase = MovementPhase::MOVING;
        phaseTimer = 0.0f;
    }
    
    /**
     * @brief Transitions to planning phase.
     */
    void startPlanning() {
        phase = MovementPhase::PLANNING;
        phaseTimer = 0.0f;
        isMoving = false;
    }
    
    /**
     * @brief Checks if entity can accept new movement commands.
     */
    bool canAcceptNewTarget() const {
        return phase == MovementPhase::IDLE || phase == MovementPhase::ARRIVING;
    }
};

