#include "SystemManager.h"

namespace ECS {
    
    void SystemManager::updateSystems(float deltaTime) {
        for (auto& system : systems) {
            system->update(deltaTime);
        }
    }
    
    void SystemManager::clear() {
        systems.clear();
    }
}
