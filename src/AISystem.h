#pragma once
#include "./ECS/AIManager.h"
#include "./ECS/PositionManager.h"
#include "./ECS/MovementManager.h"
#include "./ECS/DescriptionComponentManager.h"
#include "Pathfinder.h"
#include "Grid.h"
#include "GeminiClient.h"
#include "ECS/InfoBoxManager.h"
#include "Clock.h"
#include "TimeOfDay.h"
#include "HomeManager.h"


class AISystem
{
public:
    AISystem(AIManager *aiManager,
             PositionManager *posManager,
             MovementManager *moveManager,
             DescriptionComponentManager *descManager,
             Pathfinder *pathfinder,
             Grid *grid,
             GeminiClient *gemini,
             InfoBoxManager *infoBoxManager,
             Clock *clock,
             HomeManager *homeManager,
             Uint32 cellSize);

    void update(float deltaTime);
    void updateAI(float deltaTime);
    void processArrivingEntities();
    void processPlanningEntities();

private:
    AIManager *aiManager;
    PositionManager *posManager;
    MovementManager *moveManager;
    DescriptionComponentManager *descManager;
    Pathfinder *pathfinder;
    Grid *grid;
    GeminiClient *gemini;
    InfoBoxManager *infoBoxManager;
    Clock *clock;
    HomeManager *homeManager;
    Uint32 cellSize;
    
    // Add timer for AI updates
    float aiUpdateTimer = 0.0f;
    float aiUpdateInterval = 0.5f;
    
    // Helper methods
    void planNextAction(unsigned int entityUID);
    void executeMovementPlan(unsigned int entityUID); // Add this declaration
    void handleArrival(unsigned int entityUID); // Add this declaration too if it's missing
    std::string getHomeZone(unsigned int npcId);
    std::string getRandomLeisureZone();
    bool shouldReplanActivity(unsigned int entityUID);
};