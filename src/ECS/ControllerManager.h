#pragma once
#include <unordered_map>
#include <memory>
#include "Entity.h"
#include "IComponentManager.h"
#include "./components/PlayerController.h"
#include "./components/Controller.h"
#include "MovementManager.h"
#include "PositionManager.h"
#include "../Grid.h" // <-- Add this

class ControllerManager : public IComponentManager {
public:
    std::unordered_map<unsigned int, std::unique_ptr<Controller>> controllers;

    Controller* create(unsigned int entityUID, std::string  type = "unknown" ) {

        Controller* ptr = nullptr;

        if ( type == "player" ) {
            auto ctrl = std::make_unique<PlayerController>();
            ptr = ctrl.get();
            controllers[entityUID] = std::move(ctrl);
            
        } else if ( type == "guardian" ) {
            auto ctrl = std::make_unique<Controller>();
            ptr = ctrl.get();
            controllers[entityUID] = std::move(ctrl);

        } else if ( type == "citizen" ) {
            auto ctrl = std::make_unique<Controller>();
            ptr = ctrl.get();
            controllers[entityUID] = std::move(ctrl);

        } else {
            auto ctrl = std::make_unique<Controller>();
        }

        return ptr;
    }

    Controller* get(unsigned int entityUID) {
        auto it = controllers.find(entityUID);
        return (it != controllers.end()) ? it->second.get() : nullptr;
    }

    // This replaces the old update() method
    void handleEvents(const SDL_Event& event, MovementManager& moveManager, PositionManager& posManager, const Grid& grid, int cellSize) {
        for (auto& [uid, controller] : controllers) {
            if (controller->type == "player") {
                if (auto* playerController = dynamic_cast<PlayerController*>(controller.get())) {
                    playerController->handleEvent(event, uid, moveManager, posManager, grid, cellSize);
                }
            }
        }
    }

    void dump() override {
        for (const auto& [uid, desc] : controllers) {
            if (desc) {
                std::cout << "Entity UID: " << uid << " Controller Type: " << desc->type << std::endl;
            }
        }
    }      
};

