#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include "IComponentManager.h"
#include "./components/DescriptionComponent.h" // <-- Updated include

class DescriptionComponentManager : public IComponentManager {
public:
    std::unordered_map<unsigned int, std::unique_ptr<DescriptionComponent>> descriptions;

    DescriptionComponent* create(unsigned int entityUID, const std::string& name, const std::string& text) {
        auto desc = std::make_unique<DescriptionComponent>(name, text);
        DescriptionComponent* ptr = desc.get();
        descriptions[entityUID] = std::move(desc);
        return ptr;
    }

    DescriptionComponent* get(unsigned int entityUID) {
        auto it = descriptions.find(entityUID);
        return (it != descriptions.end()) ? it->second.get() : nullptr;
    }

    void dump() {
        std::cout << "--- Description Components ---" << std::endl;
        for (const auto& pair : descriptions) {
            std::cout << "Entity " << pair.first << ": " << pair.second->name << " - " << pair.second->text << std::endl;
        }
    }

    virtual void update() {
        // for now lets call dump() to show descriptions
        dump();
    };
};