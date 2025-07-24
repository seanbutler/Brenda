#include "MovementManager.h"
#include "PositionManager.h"

void MovementManager::update(PositionManager& posManager, float deltaTime, float timeScale) {
    for (auto& [uid, movement] : movements) {
        // Update phase timer (scales with time)
        movement->phaseTimer += deltaTime * timeScale;
        
        // Handle phase transitions
        switch (movement->phase) {
            case MovementPhase::PLANNING:
                if (movement->phaseTimer >= movement->planningDuration) {
                    movement->phase = MovementPhase::IDLE;
                    movement->phaseTimer = 0.0f;
                }
                break;
                
            case MovementPhase::ARRIVING:
                if (movement->phaseTimer >= movement->arrivingDuration) {
                    movement->phase = MovementPhase::IDLE;
                    movement->phaseTimer = 0.0f;
                }
                break;
                
            case MovementPhase::MOVING:
                updateMovement(uid, movement.get(), posManager, deltaTime, timeScale);
                break;
                
            case MovementPhase::IDLE:
                // Entity is idle, no movement updates needed
                break;
        }
    }
}

void MovementManager::updateMovement(unsigned int uid, Movement* movement, PositionManager& posManager, 
                                    float deltaTime, float timeScale) {
    auto* pos = posManager.get(uid);
    if (!pos) return;

    float dx = movement->targetX - pos->x;
    float dy = movement->targetY - pos->y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Calculate how far we should move in this single frame (scaled by time)
    float moveDistance = movement->speed * deltaTime * timeScale;

    // If the remaining distance is less than this frame's movement,
    // snap to the target and transition to arriving phase.
    if (distance <= moveDistance) {
        pos->x = movement->targetX;
        pos->y = movement->targetY;
        movement->isMoving = false;
        movement->phase = MovementPhase::ARRIVING;
        movement->phaseTimer = 0.0f;
    } else {
        // Otherwise, move towards the target by the calculated distance
        pos->x += (dx / distance) * moveDistance;
        pos->y += (dy / distance) * moveDistance;
    }
}
