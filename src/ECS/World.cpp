#include "World.h"

namespace ECS {
    
    EntityID World::createEntity() {
        return entityManager.createEntity();
    }
    
    bool World::destroyEntity(EntityID entityID) {
        if (!entityManager.isAlive(entityID)) {
            return false;
        }
        
        // Remove all components from the entity
        componentRegistry.removeAllComponents(entityID);
        
        // Destroy the entity
        return entityManager.destroyEntity(entityID);
    }
    
    bool World::isEntityAlive(EntityID entityID) const {
        return entityManager.isAlive(entityID);
    }
    
    std::vector<EntityID> World::getAllEntities() const {
        return entityManager.getAllEntities();
    }
    
    void World::update(float deltaTime) {
        systemManager.updateSystems(deltaTime);
    }
    
    size_t World::getEntityCount() const {
        return entityManager.getEntityCount();
    }
    
    void World::clear() {
        systemManager.clear();
        componentRegistry.clear();
        entityManager.clear();
    }
}
