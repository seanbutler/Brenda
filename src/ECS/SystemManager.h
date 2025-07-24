#pragma once

#include "EntityManager.h"
#include "ComponentRegistry.h"
#include <functional>
#include <vector>

namespace ECS {
    /**
     * @brief Base class for ECS systems.
     */
    class ISystem {
    public:
        virtual ~ISystem() = default;
        virtual void update(float deltaTime) = 0;
        virtual const char* getName() const = 0;
    };
    
    /**
     * @brief Manages system execution order and lifecycle.
     */
    class SystemManager {
    private:
        std::vector<std::unique_ptr<ISystem>> systems;
        
    public:
        /**
         * @brief Adds a system to the manager.
         */
        template<typename SystemType, typename... Args>
        SystemType* addSystem(Args&&... args) {
            auto system = std::make_unique<SystemType>(std::forward<Args>(args)...);
            SystemType* ptr = system.get();
            systems.push_back(std::move(system));
            return ptr;
        }
        
        /**
         * @brief Updates all systems in order.
         */
        void updateSystems(float deltaTime);
        
        /**
         * @brief Gets a system by type.
         */
        template<typename SystemType>
        SystemType* getSystem() const {
            for (const auto& system : systems) {
                if (auto* typedSystem = dynamic_cast<SystemType*>(system.get())) {
                    return typedSystem;
                }
            }
            return nullptr;
        }
        
        /**
         * @brief Clears all systems.
         */
        void clear();
    };
}
