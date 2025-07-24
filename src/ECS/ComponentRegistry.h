#pragma once

#include "EntityID.h"
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <cassert>

namespace ECS {
    /**
     * @brief Base class for type-erased component storage.
     */
    class IComponentStorage {
    public:
        virtual ~IComponentStorage() = default;
        virtual bool hasComponent(EntityID entityID) const = 0;
        virtual void removeComponent(EntityID entityID) = 0;
        virtual void clear() = 0;
    };
    
    /**
     * @brief Template specialization for storing components of a specific type.
     */
    template<typename ComponentType>
    class ComponentStorage : public IComponentStorage {
    private:
        std::unordered_map<EntityID, std::unique_ptr<ComponentType>> components;
        
    public:
        ComponentType* addComponent(EntityID entityID, std::unique_ptr<ComponentType> component) {
            ComponentType* ptr = component.get();
            components[entityID] = std::move(component);
            return ptr;
        }
        
        ComponentType* getComponent(EntityID entityID) const {
            auto it = components.find(entityID);
            return (it != components.end()) ? it->second.get() : nullptr;
        }
        
        bool hasComponent(EntityID entityID) const override {
            return components.find(entityID) != components.end();
        }
        
        void removeComponent(EntityID entityID) override {
            components.erase(entityID);
        }
        
        void clear() override {
            components.clear();
        }
        
        const std::unordered_map<EntityID, std::unique_ptr<ComponentType>>& getAllComponents() const {
            return components;
        }
    };
    
    /**
     * @brief Central registry for all component types.
     * 
     * This provides type-safe component storage and retrieval,
     * replacing the scattered component management in individual managers.
     */
    class ComponentRegistry {
    private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> storages;
        
        template<typename ComponentType>
        ComponentStorage<ComponentType>* getStorage() {
            std::type_index typeIndex = std::type_index(typeid(ComponentType));
            auto it = storages.find(typeIndex);
            
            if (it == storages.end()) {
                // Create new storage for this component type
                auto storage = std::make_unique<ComponentStorage<ComponentType>>();
                ComponentStorage<ComponentType>* ptr = storage.get();
                storages[typeIndex] = std::move(storage);
                return ptr;
            }
            
            return static_cast<ComponentStorage<ComponentType>*>(it->second.get());
        }
        
        template<typename ComponentType>
        const ComponentStorage<ComponentType>* getStorage() const {
            std::type_index typeIndex = std::type_index(typeid(ComponentType));
            auto it = storages.find(typeIndex);
            
            if (it == storages.end()) {
                return nullptr;
            }
            
            return static_cast<const ComponentStorage<ComponentType>*>(it->second.get());
        }
        
    public:
        /**
         * @brief Adds a component to an entity.
         */
        template<typename ComponentType, typename... Args>
        ComponentType* addComponent(EntityID entityID, Args&&... args) {
            auto component = std::make_unique<ComponentType>(std::forward<Args>(args)...);
            auto* storage = getStorage<ComponentType>();
            return storage->addComponent(entityID, std::move(component));
        }
        
        /**
         * @brief Gets a component from an entity.
         */
        template<typename ComponentType>
        ComponentType* getComponent(EntityID entityID) const {
            std::type_index typeIndex = std::type_index(typeid(ComponentType));
            auto it = storages.find(typeIndex);
            
            if (it == storages.end()) {
                return nullptr;
            }
            
            auto* storage = static_cast<ComponentStorage<ComponentType>*>(it->second.get());
            return storage->getComponent(entityID);
        }
        
        /**
         * @brief Checks if an entity has a specific component type.
         */
        template<typename ComponentType>
        bool hasComponent(EntityID entityID) const {
            std::type_index typeIndex = std::type_index(typeid(ComponentType));
            auto it = storages.find(typeIndex);
            
            if (it == storages.end()) {
                return false;
            }
            
            return it->second->hasComponent(entityID);
        }
        
        /**
         * @brief Removes a component from an entity.
         */
        template<typename ComponentType>
        void removeComponent(EntityID entityID) {
            std::type_index typeIndex = std::type_index(typeid(ComponentType));
            auto it = storages.find(typeIndex);
            
            if (it != storages.end()) {
                it->second->removeComponent(entityID);
            }
        }
        
        /**
         * @brief Removes all components from an entity.
         */
        void removeAllComponents(EntityID entityID);
        
        /**
         * @brief Gets all components of a specific type.
         */
        template<typename ComponentType>
        const std::unordered_map<EntityID, std::unique_ptr<ComponentType>>& getAllComponents() const {
            auto* storage = getStorage<ComponentType>();
            if (!storage) {
                static const std::unordered_map<EntityID, std::unique_ptr<ComponentType>> empty;
                return empty;
            }
            return storage->getAllComponents();
        }
        
        /**
         * @brief Clears all components.
         */
        void clear();
    };
}
