#include "EntityManager.h"
#include <algorithm>

namespace ECS {
    
    EntityID EntityManager::createEntity() {
        EntityID newID;
        
        if (!freeEntityIDs.empty()) {
            // Reuse a previously destroyed entity ID
            newID = freeEntityIDs.front();
            freeEntityIDs.pop();
        } else {
            // Create a new ID
            newID = nextEntityID++;
            
            // Handle overflow (extremely unlikely with uint32_t)
            if (newID == INVALID_ENTITY) {
                newID = 1; // Wrap around to 1, skip 0 (invalid)
                nextEntityID = 2;
            }
        }
        
        livingEntities.insert(newID);
        return newID;
    }
    
    bool EntityManager::destroyEntity(EntityID entityID) {
        if (!isValidEntity(entityID) || livingEntities.find(entityID) == livingEntities.end()) {
            return false; // Entity doesn't exist
        }
        
        livingEntities.erase(entityID);
        freeEntityIDs.push(entityID); // Mark for reuse
        return true;
    }
    
    bool EntityManager::isAlive(EntityID entityID) const {
        return isValidEntity(entityID) && livingEntities.find(entityID) != livingEntities.end();
    }
    
    size_t EntityManager::getEntityCount() const {
        return livingEntities.size();
    }
    
    std::vector<EntityID> EntityManager::getAllEntities() const {
        return std::vector<EntityID>(livingEntities.begin(), livingEntities.end());
    }
    
    void EntityManager::clear() {
        livingEntities.clear();
        while (!freeEntityIDs.empty()) {
            freeEntityIDs.pop();
        }
        nextEntityID = 1;
    }
}
