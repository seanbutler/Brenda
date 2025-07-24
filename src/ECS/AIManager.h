#pragma once
#include "./components/AIState.h"
#include "IComponentManager.h"
#include <unordered_map>

class AIManager : public IComponentManager {
public:
    std::unordered_map<unsigned int, AIState> states;

    void create(unsigned int uid) {
        states[uid] = AIState{};
    }

    AIState* get(unsigned int uid) {
        if (states.count(uid)) {
            return &states.at(uid);
        }
        return nullptr;
    }

    void dump() override {
        // Optional: implement dump for debugging
    }
};