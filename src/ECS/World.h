#pragma once

#include "EntityManager.h"
#include "ComponentRegistry.h"
#include "SystemManager.h"

namespace ECS {
    /**
     * @brief Central ECS World that coordinates entities, components, and systems.
     * 
     * This is the main interface for ECS operations and replaces the scattered
     * entity/component management across the Scene and individual managers.
     */
    class World {
    private:
        EntityManager entityManager;
        ComponentRegistry componentRegistry;
        SystemManager systemManager;
        
    public:
        /**
         * @brief Creates a new entity.
         * @return EntityID The unique identifier for the new entity.
         */
        EntityID createEntity();
        
        /**
         * @brief Destroys an entity and all its components.
         * @param entityID The ID of the entity to destroy.
         * @return bool True if the entity was successfully destroyed.
         */
        bool destroyEntity(EntityID entityID);
        
        /**
         * @brief Checks if an entity exists.
         * @param entityID The ID to check.
         * @return bool True if the entity exists.
         */
        bool isEntityAlive(EntityID entityID) const;
        
        /**
         * @brief Adds a component to an entity.
         * @tparam ComponentType The type of component to add.
         * @param entityID The entity to add the component to.
         * @param args Constructor arguments for the component.
         * @return ComponentType* Pointer to the created component.
         */
        template<typename ComponentType, typename... Args>
        ComponentType* addComponent(EntityID entityID, Args&&... args) {
            if (!entityManager.isAlive(entityID)) {
                return nullptr;
            }
            return componentRegistry.addComponent<ComponentType>(entityID, std::forward<Args>(args)...);
        }
        
        /**
         * @brief Gets a component from an entity.
         * @tparam ComponentType The type of component to get.
         * @param entityID The entity to get the component from.
         * @return ComponentType* Pointer to the component, or nullptr if not found.
         */
        template<typename ComponentType>
        ComponentType* getComponent(EntityID entityID) const {
            return componentRegistry.getComponent<ComponentType>(entityID);
        }
        
        /**
         * @brief Checks if an entity has a specific component.
         * @tparam ComponentType The type of component to check for.
         * @param entityID The entity to check.
         * @return bool True if the entity has the component.
         */
        template<typename ComponentType>
        bool hasComponent(EntityID entityID) const {
            return componentRegistry.hasComponent<ComponentType>(entityID);
        }
        
        /**
         * @brief Removes a component from an entity.
         * @tparam ComponentType The type of component to remove.
         * @param entityID The entity to remove the component from.
         */
        template<typename ComponentType>
        void removeComponent(EntityID entityID) {
            componentRegistry.removeComponent<ComponentType>(entityID);
        }
        
        /**
         * @brief Gets all entities.
         * @return std::vector<EntityID> Vector of all living entity IDs.
         */
        std::vector<EntityID> getAllEntities() const;
        
        /**
         * @brief Gets all components of a specific type.
         * @tparam ComponentType The type of component to get.
         * @return const auto& Reference to the component storage map.
         */
        template<typename ComponentType>
        const auto& getAllComponents() const {
            return componentRegistry.getAllComponents<ComponentType>();
        }
        
        /**
         * @brief Adds a system to the world.
         * @tparam SystemType The type of system to add.
         * @param args Constructor arguments for the system.
         * @return SystemType* Pointer to the created system.
         */
        template<typename SystemType, typename... Args>
        SystemType* addSystem(Args&&... args) {
            return systemManager.addSystem<SystemType>(std::forward<Args>(args)...);
        }
        
        /**
         * @brief Gets a system by type.
         * @tparam SystemType The type of system to get.
         * @return SystemType* Pointer to the system, or nullptr if not found.
         */
        template<typename SystemType>
        SystemType* getSystem() const {
            return systemManager.getSystem<SystemType>();
        }
        
        /**
         * @brief Updates all systems.
         * @param deltaTime Time elapsed since last update.
         */
        void update(float deltaTime);
        
        /**
         * @brief Gets entity count.
         * @return size_t Number of living entities.
         */
        size_t getEntityCount() const;
        
        /**
         * @brief Clears the entire world (entities, components, systems).
         */
        void clear();
        
        // Accessors for individual managers (for migration period)
        EntityManager& getEntityManager() { return entityManager; }
        ComponentRegistry& getComponentRegistry() { return componentRegistry; }
        SystemManager& getSystemManager() { return systemManager; }
    };
}
