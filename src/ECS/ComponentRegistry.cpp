#include "ComponentRegistry.h"

namespace ECS {
    
    void ComponentRegistry::removeAllComponents(EntityID entityID) {
        for (auto& [typeIndex, storage] : storages) {
            storage->removeComponent(entityID);
        }
    }
    
    void ComponentRegistry::clear() {
        for (auto& [typeIndex, storage] : storages) {
            storage->clear();
        }
    }
}
