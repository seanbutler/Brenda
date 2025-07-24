#pragma once
#include <unordered_map>
#include <memory>
#include <cmath>
#include <vector>
#include "IComponentManager.h"
#include "./components/Movement.h"

// Forward declaration to break circular dependency
class PositionManager;

class MovementManager : public IComponentManager {
public:
    std::unordered_map<unsigned int, std::unique_ptr<Movement>> movements;

    Movement* create(unsigned int entityUID, float speed) {
        auto move = std::make_unique<Movement>(speed);
        Movement* ptr = move.get();
        movements[entityUID] = std::move(move);
        return ptr;
    }

    Movement* get(unsigned int entityUID) {
        auto it = movements.find(entityUID);
        return (it != movements.end()) ? it->second.get() : nullptr;
    }

    void update(PositionManager& posManager, float deltaTime, float timeScale = 1.0f);
    
private:
    void updateMovement(unsigned int uid, Movement* movement, PositionManager& posManager, 
                       float deltaTime, float timeScale = 1.0f);
    
public:
    /**
     * @brief Gets all entities in a specific movement phase.
     */
    std::vector<unsigned int> getEntitiesInPhase(MovementPhase phase) {
        std::vector<unsigned int> entities;
        for (const auto& [uid, movement] : movements) {
            if (movement->phase == phase) {
                entities.push_back(uid);
            }
        }
        return entities;
    }
    
    /**
     * @brief Checks if an entity can accept new movement commands.
     */
    bool canEntityMove(unsigned int entityUID) {
        auto* movement = get(entityUID);
        return movement ? movement->canAcceptNewTarget() : true;
    }

    void dump() override {}
};