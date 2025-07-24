#pragma once

#include "World.h"
#include "components/PositionComponent.h"
#include "components/DescriptionComponent.h"
#include "components/Movement.h"
#include "components/InfoBoxComponent.h"
#include "components/AIState.h"

namespace ECS {
    /**
     * @brief Adapter class to provide backward compatibility during migration.
     * 
     * This allows existing code to continue working while we gradually 
     * migrate to the new ECS World system.
     */
    class LegacyManagerAdapter {
    private:
        World* world;
        
    public:
        explicit LegacyManagerAdapter(World* ecsWorld) : world(ecsWorld) {}
        
        // Position component compatibility
        struct PositionManagerCompat {
            World* world;
            
            PositionComponent* create(unsigned int entityUID, float x, float y) {
                EntityID id = static_cast<EntityID>(entityUID);
                return world->addComponent<PositionComponent>(id, x, y);
            }
            
            PositionComponent* get(unsigned int entityUID) const {
                EntityID id = static_cast<EntityID>(entityUID);
                return world->getComponent<PositionComponent>(id);
            }
            
            void dump() {
                // Implementation for debugging
            }
        };
        
        // Description component compatibility  
        struct DescriptionManagerCompat {
            World* world;
            
            DescriptionComponent* create(unsigned int entityUID, const std::string& name, const std::string& text) {
                EntityID id = static_cast<EntityID>(entityUID);
                return world->addComponent<DescriptionComponent>(id, name, text);
            }
            
            DescriptionComponent* get(unsigned int entityUID) const {
                EntityID id = static_cast<EntityID>(entityUID);
                return world->getComponent<DescriptionComponent>(id);
            }
            
            void dump() {
                // Implementation for debugging
            }
        };
        
        // Movement component compatibility
        struct MovementManagerCompat {
            World* world;
            
            Movement* create(unsigned int entityUID, float speed) {
                EntityID id = static_cast<EntityID>(entityUID);
                return world->addComponent<Movement>(id, speed);
            }
            
            Movement* get(unsigned int entityUID) const {
                EntityID id = static_cast<EntityID>(entityUID);
                return world->getComponent<Movement>(id);
            }
            
            void update(PositionManagerCompat& posManager, float deltaTime, float timeScale = 1.0f) {
                // Migration note: This logic should move to a MovementSystem
                // For now, keep existing logic working
            }
        };
        
        // InfoBox component compatibility
        struct InfoBoxManagerCompat {
            World* world;
            
            InfoBoxComponent* create(unsigned int entityUID, const std::string& initialText = "") {
                EntityID id = static_cast<EntityID>(entityUID);
                return world->addComponent<InfoBoxComponent>(id, initialText);
            }
            
            InfoBoxComponent* get(unsigned int entityUID) const {
                EntityID id = static_cast<EntityID>(entityUID);
                return world->getComponent<InfoBoxComponent>(id);
            }
        };
        
        // AI component compatibility
        struct AIManagerCompat {
            World* world;
            
            AIState* create(unsigned int entityUID) {
                EntityID id = static_cast<EntityID>(entityUID);
                return world->addComponent<AIState>(id);
            }
            
            AIState* get(unsigned int entityUID) const {
                EntityID id = static_cast<EntityID>(entityUID);
                return world->getComponent<AIState>(id);
            }
        };
        
        // Create compatibility managers
        PositionManagerCompat getPositionManager() { return {world}; }
        DescriptionManagerCompat getDescriptionManager() { return {world}; }
        MovementManagerCompat getMovementManager() { return {world}; }
        InfoBoxManagerCompat getInfoBoxManager() { return {world}; }
        AIManagerCompat getAIManager() { return {world}; }
        
        // Entity management
        unsigned int createEntity() {
            return static_cast<unsigned int>(world->createEntity());
        }
        
        bool destroyEntity(unsigned int entityUID) {
            EntityID id = static_cast<EntityID>(entityUID);
            return world->destroyEntity(id);
        }
    };
}
