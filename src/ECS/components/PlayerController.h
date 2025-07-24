#pragma once

#include "Controller.h"
#include <SDL3/SDL.h>

// Forward declarations to break circular dependencies
class MovementManager;
class PositionManager;
class Grid;

/**
 * @class PlayerController
 * @brief A controller that translates user keyboard input into entity movement.
 */
class PlayerController : public Controller
{
public:
    PlayerController() : Controller("player") {}

    /**
     * @brief Handles SDL keyboard events to move the player entity.
     * @param event The SDL_Event to process.
     * @param uid The UID of the entity this controller belongs to.
     * @param moveManager Reference to the MovementManager.
     * @param posManager Reference to the PositionManager.
     * @param grid The game grid, for collision checks.
     * @param cellSize The size of a grid cell in pixels.
     */
    void handleEvent(const SDL_Event& event, unsigned int uid, 
                    MovementManager& moveManager, PositionManager& posManager, 
                    const Grid& grid, int cellSize);
};