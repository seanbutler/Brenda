#pragma once
#include <unordered_map>
#include <memory>
#include <cmath>
#include <vector>
#include "IComponentManager.h"
#include "./components/Movement.h"
#include "PositionManager.h"

class MovementManager : public IComponentManager {
public:
    std::unordered_map<unsigned int, std::unique_ptr<Movement>> movements;

    Movement* create(unsigned int entityUID, float speed) {
        auto move = std::make_unique<Movement>(speed);
        Movement* ptr = move.get();
        movements[entityUID] = std::move(move);
        return ptr;
    }

    Movement* get(unsigned int entityUID) {
        auto it = movements.find(entityUID);
        return (it != movements.end()) ? it->second.get() : nullptr;
    }

    void update(PositionManager& posManager, float deltaTime, float timeScale = 1.0f) {
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
    
private:
    void updateMovement(unsigned int uid, Movement* movement, PositionManager& posManager, float deltaTime, float timeScale = 1.0f) {
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
    
public:
    /**
     * @brief Gets all entities in a specific movement phase.
     */
    std::vector<unsigned int> getEntitiesInPhase(MovementPhase phase) {
        std::vector<unsigned int> entities;
        for (const auto& [uid, movement] : movements) {
            if (movement->phase == phase) {
                entities.push_back(uid);
            }
        }
        return entities;
    }
    
    /**
     * @brief Checks if an entity can accept new movement commands.
     */
    bool canEntityMove(unsigned int entityUID) {
        auto* movement = get(entityUID);
        return movement ? movement->canAcceptNewTarget() : true;
    }

    void dump() override {}
};