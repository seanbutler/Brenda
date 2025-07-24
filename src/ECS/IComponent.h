#pragma once

#include "EntityID.h"

namespace ECS
{
    /**
     * @brief Base class for all ECS components.
     * 
     * Components are pure data containers that hold state for entities.
     * They should not contain logic - that belongs in systems.
     */
    class IComponent
    {
    public:
        virtual ~IComponent() = default;
        
        // Components are data-only, no update methods
        // Logic belongs in systems, not components
    };
} // namespace ECS