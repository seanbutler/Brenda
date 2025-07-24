#include "PlayerController.h"
#include "../MovementManager.h"
#include "../PositionManager.h"
#include "../../Grid.h"
#include "Movement.h"
#include <iostream>

void PlayerController::handleEvent(const SDL_Event& event, unsigned int uid, 
                                  MovementManager& moveManager, PositionManager& posManager, 
                                  const Grid& grid, int cellSize)
{
    auto *movement = moveManager.get(uid);
    if (!movement || movement->isMoving)
    {
        return;
    }

    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        auto *pos = posManager.get(uid);
        if (!pos) return;

        // Determine the entity's current grid cell from its pixel position
        int currentGridX = static_cast<int>(pos->x / cellSize);
        int currentGridY = static_cast<int>(pos->y / cellSize);

        // Determine the target grid cell based on keyboard input
        int targetGridX = currentGridX;
        int targetGridY = currentGridY;
        bool key_pressed = false;

        switch (event.key.scancode)
        {
        case SDL_SCANCODE_W: targetGridY--; key_pressed = true; break;
        case SDL_SCANCODE_S: targetGridY++; key_pressed = true; break;
        case SDL_SCANCODE_A: targetGridX--; key_pressed = true; break;
        case SDL_SCANCODE_D: targetGridX++; key_pressed = true; break;
        default: break;
        }

        if (key_pressed)
        {
            // Check if the target cell is within grid bounds and is walkable
            if (targetGridX >= 0 && targetGridX < grid.getWidth() &&
                targetGridY >= 0 && targetGridY < grid.getHeight() &&
                grid.at(targetGridX, targetGridY).obstacle == ObstacleType::None)
            {
                // Calculate the pixel coordinates of the CENTER of the target cell
                float targetPixelX = targetGridX * cellSize + (cellSize / 2.0f);
                float targetPixelY = targetGridY * cellSize + (cellSize / 2.0f);

                // Command the movement manager to move to the calculated center
                movement->setTarget(targetPixelX, targetPixelY);
                
                // CRITICAL FIX: Actually start the movement
                movement->phase = MovementPhase::MOVING;
                movement->phaseTimer = 0.0f;
                movement->isMoving = true;
                
                // Debug output
                std::cout << "Player moving from (" << currentGridX << "," << currentGridY 
                          << ") to (" << targetGridX << "," << targetGridY << ")" << std::endl;
            }
            else
            {
                std::cout << "Cannot move to (" << targetGridX << "," << targetGridY 
                          << ") - out of bounds or blocked" << std::endl;
            }
        }
    }
}
