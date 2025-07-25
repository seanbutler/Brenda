#pragma once
#include <unordered_map>
#include <memory>
#include <iostream>
#include "Entity.h"
#include "IComponentManager.h"
#include "./components/PlayerController.h"
#include "./components/Controller.h"
#include <SDL3/SDL.h>

// Forward declarations to break circular dependencies
class MovementManager;
class PositionManager;
class Grid;

class ControllerManager : public IComponentManager {
public:
    std::unordered_map<unsigned int, std::unique_ptr<Controller>> controllers;

    Controller* create(unsigned int entityUID, std::string  type = "unknown" ) {

        Controller* ptr = nullptr;
        std::cout << "Creating controller for entity " << entityUID << " with type: " << type << std::endl;

        if ( type == "player" ) {
            auto ctrl = std::make_unique<PlayerController>();
            ptr = ctrl.get();
            controllers[entityUID] = std::move(ctrl);
            std::cout << "Created PlayerController for entity " << entityUID << std::endl;
            
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
    void handleEvents(const SDL_Event& event, MovementManager& moveManager, 
                     PositionManager& posManager, const Grid& grid, int cellSize);

    void dump() override;      
};

