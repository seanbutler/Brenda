#pragma once
#include <string>

// The possible states for an AI entity
enum class AIStateName {
    Idle,               // Doing nothing, waiting for a new goal
    MovingToZone,       // Actively moving towards a destination
    PerformingActivity  // Currently performing an activity at a location
};

struct AIState {
    AIStateName currentState = AIStateName::Idle;
    std::string targetZone; // e.g., "Cafe", "Work"
    std::string intendedActivity; // What the NPC intends to do when they arrive
    float timeInCurrentState = 0.0f;
    float activityTimer = 0.0f; // How long to stay in current activity
};