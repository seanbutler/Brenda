#pragma once

#include <cstdint>
#include <limits>

namespace ECS {
    /**
     * @brief Unified entity identifier type used throughout the ECS system.
     * 
     * This replaces the inconsistent use of std::uint64_t and unsigned int
     * across different managers.
     */
    using EntityID = std::uint32_t;
    
    /**
     * @brief Invalid/null entity ID constant.
     */
    constexpr EntityID INVALID_ENTITY = std::numeric_limits<EntityID>::max();
    
    /**
     * @brief Checks if an entity ID is valid.
     */
    constexpr bool isValidEntity(EntityID id) {
        return id != INVALID_ENTITY;
    }
}
