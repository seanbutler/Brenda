#include "ControllerManager.h"
#include "MovementManager.h"
#include "PositionManager.h"
#include "../Grid.h"

void ControllerManager::handleEvents(const SDL_Event& event, MovementManager& moveManager, 
                                   PositionManager& posManager, const Grid& grid, int cellSize) {
    // Debug output for key events
    if (event.type == SDL_EVENT_KEY_DOWN) {
        std::cout << "ControllerManager: Key event detected, checking " << controllers.size() << " controllers" << std::endl;
    }
    
    for (auto& [uid, controller] : controllers) {
        if (controller->type == "player") {
            if (event.type == SDL_EVENT_KEY_DOWN) {
                std::cout << "ControllerManager: Found player controller for entity " << uid << std::endl;
            }
            if (auto* playerController = dynamic_cast<PlayerController*>(controller.get())) {
                playerController->handleEvent(event, uid, moveManager, posManager, grid, cellSize);
            }
        }
    }
}

void ControllerManager::dump() {
    for (const auto& [uid, desc] : controllers) {
        if (desc) {
            std::cout << "Entity UID: " << uid << " Controller Type: " << desc->type << std::endl;
        }
    }
}
