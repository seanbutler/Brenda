#include "ControllerManager.h"
#include "MovementManager.h"
#include "PositionManager.h"
#include "../Grid.h"

void ControllerManager::handleEvents(const SDL_Event& event, MovementManager& moveManager, 
                                   PositionManager& posManager, const Grid& grid, int cellSize) {
    for (auto& [uid, controller] : controllers) {
        if (controller->type == "player") {
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
