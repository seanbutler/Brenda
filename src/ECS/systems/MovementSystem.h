#pragma once

#include "../SystemManager.h"
#include "../World.h"
#include "../components/PositionComponent.h"
#include "../components/Movement.h"
#include <cmath>

namespace ECS {
    /**
     * @brief Example system showing how to use the new ECS framework.
     * 
     * This system updates entity positions based on their movement components.
     * It demonstrates the proper separation of data (components) and logic (systems).
     */
    class MovementSystem : public ISystem {
    private:
        World* world;
        
    public:
        explicit MovementSystem(World* ecsWorld) : world(ecsWorld) {}
        
        void update(float deltaTime) override {
            // Get all movement components
            const auto& movements = world->getAllComponents<Movement>();
            
            for (const auto& [entityID, movement] : movements) {
                if (!movement || !movement->isMoving) {
                    continue;
                }
                
                // Get the position component for this entity
                auto* position = world->getComponent<PositionComponent>(entityID);
                if (!position) {
                    continue; // Entity has movement but no position
                }
                
                // Update position based on movement
                updateEntityPosition(entityID, position, movement.get(), deltaTime);
            }
        }
        
        const char* getName() const override {
            return "MovementSystem";
        }
        
    private:
        void updateEntityPosition(EntityID entityID, PositionComponent* position, 
                                Movement* movement, float deltaTime) {
            // Calculate movement direction
            float dx = movement->targetX - position->x;
            float dy = movement->targetY - position->y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance < 1.0f) {
                // Arrived at target
                position->x = movement->targetX;
                position->y = movement->targetY;
                movement->isMoving = false;
                movement->phase = MovementPhase::ARRIVING;
                movement->phaseTimer = 0.0f;
                return;
            }
            
            // Normalize direction and apply speed
            float moveDistance = movement->speed * deltaTime;
            if (moveDistance > distance) {
                moveDistance = distance; // Don't overshoot
            }
            
            position->x += (dx / distance) * moveDistance;
            position->y += (dy / distance) * moveDistance;
        }
    };
}
