#pragma once

#include "World.h"
#include "components/PositionComponent.h"
#include "components/DescriptionComponent.h"
#include "components/Movement.h"
#include "components/InfoBoxComponent.h"
#include "components/AIState.h"
#include "systems/MovementSystem.h"
#include <iostream>

namespace ECS {
    /**
     * @brief Migration example showing how to transition from old managers to new ECS.
     * 
     * This demonstrates the step-by-step process of migrating existing entity
     * management code to use the new centralized ECS World system.
     */
    class MigrationExample {
    private:
        World world;
        
    public:
        /**
         * @brief Example of creating entities using the new ECS framework.
         */
        void demonstrateNewEntityCreation() {
            // OLD WAY (scattered across Scene and managers):
            // - Create Entity object in Scene::entities vector (not used)
            // - Call positionManager->create(uid, x, y)
            // - Call descriptionManager->create(uid, name, desc)
            // - Call movementManager->create(uid, speed)
            // - etc. for each component type
            
            // NEW WAY (centralized in World):
            EntityID npc = world.createEntity();
            
            // Add components to the entity
            world.addComponent<PositionComponent>(npc, 100.0f, 200.0f);
            world.addComponent<DescriptionComponent>(npc, "Guard", "A vigilant guard");
            world.addComponent<Movement>(npc, 50.0f); // 50 pixels/second speed
            world.addComponent<InfoBoxComponent>(npc, "Guard\nPatrolling");
            world.addComponent<AIState>(npc);
            
            // Entity is now fully created with all components
        }
        
        /**
         * @brief Example of component access using the new system.
         */
        void demonstrateComponentAccess() {
            EntityID entity = world.createEntity();
            world.addComponent<PositionComponent>(entity, 0.0f, 0.0f);
            
            // OLD WAY:
            // auto* pos = positionManager->get(entityUID);
            // if (pos) { pos->x = 50.0f; }
            
            // NEW WAY:
            auto* pos = world.getComponent<PositionComponent>(entity);
            if (pos) {
                pos->x = 50.0f;
                pos->y = 75.0f;
            }
            
            // Type-safe component queries
            if (world.hasComponent<Movement>(entity)) {
                auto* movement = world.getComponent<Movement>(entity);
                movement->setTarget(100.0f, 100.0f);
            }
        }
        
        /**
         * @brief Example of system setup and execution.
         */
        void demonstrateSystemUsage() {
            // Add systems to the world
            auto* movementSystem = world.addSystem<MovementSystem>(&world);
            
            // Create test entities
            EntityID entity1 = world.createEntity();
            world.addComponent<PositionComponent>(entity1, 0.0f, 0.0f);
            auto* movement1 = world.addComponent<Movement>(entity1, 100.0f);
            movement1->setTarget(200.0f, 200.0f);
            
            EntityID entity2 = world.createEntity();
            world.addComponent<PositionComponent>(entity2, 50.0f, 50.0f);
            auto* movement2 = world.addComponent<Movement>(entity2, 75.0f);
            movement2->setTarget(300.0f, 100.0f);
            
            // Update the world (runs all systems)
            float deltaTime = 0.016f; // ~60 FPS
            world.update(deltaTime);
            
            // Systems automatically process all relevant entities
            // No need to manually iterate through manager maps
        }
        
        /**
         * @brief Example of entity lifecycle management.
         */
        void demonstrateEntityLifecycle() {
            // Create entity
            EntityID entity = world.createEntity();
            world.addComponent<PositionComponent>(entity, 100.0f, 100.0f);
            world.addComponent<DescriptionComponent>(entity, "Temp", "Temporary entity");
            
            // Check if entity exists
            if (world.isEntityAlive(entity)) {
                std::cout << "Entity " << entity << " is alive" << std::endl;
            }
            
            // Remove specific component
            world.removeComponent<DescriptionComponent>(entity);
            
            // Destroy entire entity (removes all components automatically)
            world.destroyEntity(entity);
            
            // Entity ID is now invalid and may be reused
            if (!world.isEntityAlive(entity)) {
                std::cout << "Entity " << entity << " has been destroyed" << std::endl;
            }
        }
        
        /**
         * @brief Shows backward compatibility approach.
         */
        void demonstrateBackwardCompatibility() {
            // During migration, you can still use existing patterns:
            // 1. Keep existing manager code working
            // 2. Gradually replace manager calls with World calls
            // 3. Move system logic from managers to proper ECS systems
            // 4. Eventually remove old managers completely
            
            // For now, existing code can continue to work alongside new ECS code
        }
        
        World& getWorld() { return world; }
    };
}
