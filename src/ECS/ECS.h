#pragma once

/**
 * @file ECS.h
 * @brief Main include file for the new ECS (Entity-Component-System) framework.
 * 
 * This header provides easy access to all ECS components and replaces the 
 * scattered entity/component management system with a centralized architecture.
 * 
 * MIGRATION GUIDE:
 * ===============
 * 
 * OLD APPROACH:
 * - Entities created in Scene::entities (unused std::unique_ptr<Entity>)
 * - Components managed by separate managers (PositionManager, MovementManager, etc.)
 * - Manual iteration through manager maps
 * - Inconsistent EntityID types (uint64_t vs unsigned int)
 * - Systems directly access multiple managers
 * 
 * NEW APPROACH:
 * - Centralized World class manages everything
 * - Unified EntityID type (uint32_t)
 * - Type-safe component access
 * - Proper system architecture with ISystem interface
 * - Automatic entity lifecycle management
 * - Component registry for efficient storage
 * 
 * USAGE EXAMPLE:
 * =============
 * 
 * // Create world and systems
 * ECS::World world;
 * world.addSystem<MovementSystem>(&world);
 * 
 * // Create entity with components
 * ECS::EntityID player = world.createEntity();
 * world.addComponent<PositionComponent>(player, 100.0f, 200.0f);
 * world.addComponent<Movement>(player, 150.0f); // speed
 * world.addComponent<DescriptionComponent>(player, "Hero", "The player character");
 * 
 * // Game loop
 * world.update(deltaTime); // Runs all systems
 * 
 * // Component access
 * auto* pos = world.getComponent<PositionComponent>(player);
 * if (pos) { pos->x += 10.0f; }
 * 
 * // Entity cleanup
 * world.destroyEntity(player); // Removes all components automatically
 */

// Core ECS framework
#include "EntityID.h"
#include "EntityManager.h"
#include "ComponentRegistry.h"
#include "SystemManager.h"
#include "World.h"

// Base interfaces
#include "IComponent.h"

// Existing components (now ECS-compatible)
#include "components/PositionComponent.h"
#include "components/DescriptionComponent.h"
#include "components/Movement.h"
#include "components/InfoBoxComponent.h"
#include "components/AIState.h"

// Example systems
#include "systems/MovementSystem.h"

// Migration utilities
#include "MigrationExample.h"
#include "LegacyManagerAdapter.h"

namespace ECS {
    /**
     * @brief Convenience function to create a fully configured ECS world.
     * @return Configured World instance ready for use.
     */
    inline World createWorld() {
        World world;
        
        // Add default systems here as needed
        // world.addSystem<MovementSystem>(&world);
        // world.addSystem<RenderSystem>(&world);
        // etc.
        
        return world;
    }
    
    /**
     * @brief Migration utility to convert old EntityUID to new EntityID.
     */
    inline EntityID convertLegacyEntityID(unsigned int oldUID) {
        return static_cast<EntityID>(oldUID);
    }
    
    /**
     * @brief Migration utility to convert new EntityID to old EntityUID.
     */
    inline unsigned int convertToLegacyEntityID(EntityID newID) {
        return static_cast<unsigned int>(newID);
    }
}
