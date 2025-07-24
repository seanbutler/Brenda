#pragma once

#include "EntityID.h"
#include <vector>
#include <unordered_set>
#include <queue>

namespace ECS {
    /**
     * @brief Manages entity creation, destruction, and lifecycle.
     * 
     * This centralizes entity management and provides a consistent
     * interface for entity operations across the entire ECS system.
     */
    class EntityManager {
    private:
        EntityID nextEntityID = 1; // Start from 1, 0 reserved for invalid
        std::unordered_set<EntityID> livingEntities;
        std::queue<EntityID> freeEntityIDs; // Reuse destroyed entity IDs
        
    public:
        /**
         * @brief Creates a new entity and returns its ID.
         * @return EntityID The unique identifier for the new entity.
         */
        EntityID createEntity();
        
        /**
         * @brief Destroys an entity and marks its ID for reuse.
         * @param entityID The ID of the entity to destroy.
         * @return bool True if the entity was successfully destroyed, false if ID was invalid.
         */
        bool destroyEntity(EntityID entityID);
        
        /**
         * @brief Checks if an entity with the given ID exists.
         * @param entityID The ID to check.
         * @return bool True if the entity exists, false otherwise.
         */
        bool isAlive(EntityID entityID) const;
        
        /**
         * @brief Gets the total number of living entities.
         * @return size_t Number of entities currently alive.
         */
        size_t getEntityCount() const;
        
        /**
         * @brief Gets all living entity IDs.
         * @return std::vector<EntityID> Vector containing all living entity IDs.
         */
        std::vector<EntityID> getAllEntities() const;
        
        /**
         * @brief Clears all entities (for cleanup/reset).
         */
        void clear();
    };
}
