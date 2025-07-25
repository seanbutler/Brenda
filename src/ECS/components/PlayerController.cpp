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
    if (!movement) {
        std::cout << "PlayerController: No movement component for entity " << uid << std::endl;
        return;
    }
    
    if (movement->isMoving) {
        // Allow debug output to see if this is the issue
        if (event.type == SDL_EVENT_KEY_DOWN) {
            std::cout << "PlayerController: Entity " << uid << " is already moving, ignoring input" << std::endl;
        }
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
        case SDL_SCANCODE_W: targetGridY--; key_pressed = true; 
            std::cout << "W key pressed - attempting to move UP to (" << targetGridX << "," << targetGridY << ")" << std::endl;
            break;
        case SDL_SCANCODE_S: targetGridY++; key_pressed = true; 
            std::cout << "S key pressed - attempting to move DOWN to (" << targetGridX << "," << targetGridY << ")" << std::endl;
            break;
        case SDL_SCANCODE_A: targetGridX--; key_pressed = true; 
            std::cout << "A key pressed - attempting to move LEFT to (" << targetGridX << "," << targetGridY << ")" << std::endl;
            break;
        case SDL_SCANCODE_D: targetGridX++; key_pressed = true; 
            std::cout << "D key pressed - attempting to move RIGHT to (" << targetGridX << "," << targetGridY << ")" << std::endl;
            break;
        default: break;
        }

        if (key_pressed)
        {
            // Debug: Show what we're checking
            std::cout << "Checking movement to (" << targetGridX << "," << targetGridY << ")" << std::endl;
            std::cout << "Grid bounds: " << grid.getWidth() << "x" << grid.getHeight() << std::endl;
            
            // Check if the target cell is within grid bounds
            if (targetGridX >= 0 && targetGridX < grid.getWidth() &&
                targetGridY >= 0 && targetGridY < grid.getHeight()) {
                
                const Cell& targetCell = grid.at(targetGridX, targetGridY);
                std::cout << "Target cell obstacle type: " << (int)targetCell.obstacle << " (Wall=" << (int)ObstacleType::Wall << ")" << std::endl;
                
                // Allow movement unless the target is a Wall (same logic as Pathfinder)
                if (targetCell.obstacle != ObstacleType::Wall) {
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
                } else {
                    std::cout << "Cannot move to (" << targetGridX << "," << targetGridY 
                              << ") - blocked by Wall" << std::endl;
                }
            } else {
                std::cout << "Cannot move to (" << targetGridX << "," << targetGridY 
                          << ") - out of bounds" << std::endl;
            }
        }
    }
}
