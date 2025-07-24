#pragma once
#include "Entity.h"
#include "./components/PositionComponent.h"
#include "IComponentManager.h"

#include <unordered_map>
#include <memory>
#include <iostream>

class PositionManager : public IComponentManager {
public:
    std::unordered_map<unsigned int, std::unique_ptr<PositionComponent>> positions;

    PositionComponent* create(unsigned int entityUID, float x, float y) {
        auto position = std::make_unique<PositionComponent>(x, y);
        PositionComponent* ptr = position.get();
        positions[entityUID] = std::move(position);
        return ptr;
    }

    PositionComponent* get(unsigned int entityUID) const {
        auto it = positions.find(entityUID);
        return (it != positions.end()) ? it->second.get() : nullptr;
    }

    // void update() override {
    //     // Update logic for positions if needed
    // }

    void dump() override {
        std::cout << "PositionManager Dump:\n";
        if( 

            positions.empty() ) {
            std::cout << "No positions available.\n";
            return;
        }

        std::cout << "Total positions: " << positions.size() << "\n";
        std::cout << "Positions:\n";
        std::cout << "-------------------\n";
        std::cout << "Entity UID | Position (x, y)\n";
        std::cout << "-------------------\n";
        
        for (const auto& pair : positions) {
            const PositionComponent* pos = pair.second.get();
            std::cout << "Entity UID: " << pair.first << " Position: (" << pos->x << ", " << pos->y << ")\n";
        }
    }   

};