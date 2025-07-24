#include "AISystem.h"
#include <iostream>

// Update the constructor to accept the new manager
AISystem::AISystem(AIManager *aiManager,
                   PositionManager *posManager,
                   MovementManager *moveManager,
                   DescriptionComponentManager *descManager,
                   Pathfinder *pathfinder,
                   Grid *grid,
                   GeminiClient *gemini,
                   InfoBoxManager *infoBoxManager,
                   Clock *clock,
                   HomeManager *homeManager, // Add parameter
                   Uint32 cellSize)
    : aiManager(aiManager),
      posManager(posManager),
      moveManager(moveManager),
      descManager(descManager),
      pathfinder(pathfinder),
      grid(grid),
      gemini(gemini),
      infoBoxManager(infoBoxManager),
      clock(clock),
      homeManager(homeManager), // Initialize member
      cellSize(cellSize)
{
}

void AISystem::update(float deltaTime)
{
    updateAI(deltaTime);
    processArrivingEntities();
    processPlanningEntities();
}

void AISystem::updateAI(float deltaTime)
{
    aiUpdateTimer += deltaTime;
    if (aiUpdateTimer < aiUpdateInterval)
    {
        return; // Not time to think yet
    }
    aiUpdateTimer -= aiUpdateInterval;

    for (auto &[uid, aiState] : aiManager->states)
    {
        auto *movement = moveManager->get(uid);
        auto *position = posManager->get(uid);

        if (!movement || !position)
            continue;

        // Only process AI for entities that are idle or can accept new commands
        if (movement->phase == MovementPhase::IDLE)
        {
            planNextAction(uid);
        }
        else if (movement->phase == MovementPhase::MOVING &&
                 aiState.currentState == AIStateName::MovingToZone)
        {
            // Update info during movement
            auto *infoBox = infoBoxManager->get(uid);
            auto *description = descManager->get(uid);
            std::string entityName = description ? description->name : "Unknown";

            if (infoBox)
            {
                infoBox->text = entityName + "\nMoving to " + aiState.targetZone;
            }
        }
    }
}

void AISystem::processArrivingEntities()
{
    // Use the existing states map instead of getAllEntities()
    for (auto &[entityUID, aiState] : aiManager->states)
    {
        auto *movement = moveManager->get(entityUID);
        auto *position = posManager->get(entityUID);
        auto *infoBox = infoBoxManager->get(entityUID);
        auto *description = descManager->get(entityUID);

        if (!movement || !position)
            continue;

        std::string entityName = description ? description->name : "Unknown";

        // Check if entity has completed a movement step and arrived at destination
        if (aiState.currentState == AIStateName::MovingToZone &&
            movement->phase == MovementPhase::IDLE &&
            !movement->isMoving)
        {

            Point currentCell = {(int)(position->x / cellSize), (int)(position->y / cellSize)};

            // Check if we've reached the target zone
            if (grid->zones.find(aiState.targetZone) != grid->zones.end())
            {
                const auto &zone = grid->zones.at(aiState.targetZone);

                if (currentCell.x >= zone.x && currentCell.x < zone.x + zone.width &&
                    currentCell.y >= zone.y && currentCell.y < zone.y + zone.height)
                {

                    // Arrived at destination zone!
                    aiState.currentState = AIStateName::PerformingActivity;

                    // Update display based on intended activity
                    std::string activity = aiState.intendedActivity.empty() ? "Idle" : aiState.intendedActivity;

                    // Special handling for sleep time
                    const auto &timeOfDay = clock->getTimeOfDay();
                    int currentHour = clock->getHour();

                    if (timeOfDay.isSleepTime(currentHour) &&
                        aiState.targetZone.find("Home") != std::string::npos)
                    {
                        activity = "Sleeping";
                    }

                    if (infoBox)
                    {
                        infoBox->text = entityName + "\n" + activity;
                    }

                    // Set activity duration
                    aiState.activityTimer = 300.0f + (rand() % 600); // 5-15 minutes

                    std::cout << "NPC " << entityUID << " arrived at " << aiState.targetZone << " and is now " << activity << std::endl;
                }
                else
                {
                    // Not at destination yet, continue pathfinding
                    // std::cout << "NPC " << entityUID << " completed movement step but hasn't reached " << aiState.targetZone << " yet. Continuing..." << std::endl;
                    executeMovementPlan(entityUID);
                }
            }
        }
    }
}

void AISystem::processPlanningEntities()
{
    // Use the existing states map instead of getAllEntities()
    for (auto &[entityUID, aiState] : aiManager->states)
    {
        auto *movement = moveManager->get(entityUID);
        auto *position = posManager->get(entityUID);
        auto *infoBox = infoBoxManager->get(entityUID);
        auto *description = descManager->get(entityUID);

        if (!movement || !position)
            continue;

        std::string entityName = description ? description->name : "Unknown";

        // Handle entities that are currently performing activities
        if (aiState.currentState == AIStateName::PerformingActivity)
        {
            // Decrease activity timer
            aiState.activityTimer -= 16.67f; // Assuming ~60 FPS (16.67ms per frame)

            // Check if it's time to plan next action
            const auto &timeOfDay = clock->getTimeOfDay();
            int currentHour = clock->getHour();

            // During sleep time, keep sleeping if at home
            if (timeOfDay.isSleepTime(currentHour) &&
                aiState.targetZone.find("Home") != std::string::npos)
            {

                if (infoBox)
                {
                    infoBox->text = entityName + "\nSleeping";
                }
                continue; // Don't plan new action, keep sleeping
            }

            // Check if activity timer expired or conditions changed
            if (aiState.activityTimer <= 0 || shouldReplanActivity(entityUID))
            {
                planNextAction(entityUID);
            }
        }

        // Handle planning phase completion -> START MOVEMENT
        if (aiState.currentState == AIStateName::MovingToZone &&
            movement->phase == MovementPhase::PLANNING)
        {

            // Check if planning is complete (planning phase timer expired)
            if (movement->phaseTimer >= movement->planningDuration)
            {
                // Execute the movement plan - this is the missing piece!
                executeMovementPlan(entityUID);

                if (infoBox)
                {
                    std::string activity = aiState.intendedActivity.empty() ? "Moving" : "Moving to " + aiState.intendedActivity;
                    infoBox->text = entityName + "\n" + activity;
                }
            }
        }
    }
}

void AISystem::planNextAction(unsigned int entityUID)
{
    auto *aiState = aiManager->get(entityUID);
    auto *movement = moveManager->get(entityUID);
    auto *position = posManager->get(entityUID);
    auto *infoBox = infoBoxManager->get(entityUID);
    auto *description = descManager->get(entityUID);

    if (!aiState || !movement || !position)
        return;

    std::string entityName = description ? description->name : "Unknown";

    // Choose activities based on time of day
    const auto &timeOfDay = clock->getTimeOfDay();
    int currentHour = clock->getHour();

    std::string activity = "Idle";
    std::string targetZone = "";

    if (timeOfDay.isSleepTime(currentHour))
    {
        activity = "Sleeping";
        targetZone = getHomeZone(entityUID);
    }
    else if (timeOfDay.isWorkHours(currentHour))
    {
        activity = "Working";
        targetZone = "Work";
    }
    else if (timeOfDay.isMealTime(currentHour))
    {
        activity = "Eating";
        targetZone = "Cafe";
    }
    else if (timeOfDay.getCurrentPeriod() == DayPeriod::Evening)
    {
        activity = "Relaxing";
        targetZone = getRandomLeisureZone();
    }
    else
    {
        activity = timeOfDay.getSuggestedActivity(currentHour);
        // For random activities, exclude homes from selection
        std::vector<std::string> nonHomeZones;
        for (const auto &[zoneName, zone] : grid->zones)
        {
            if (zoneName.find("Home") == std::string::npos)
            {
                nonHomeZones.push_back(zoneName);
            }
        }

        if (!nonHomeZones.empty())
        {
            int randomIndex = rand() % nonHomeZones.size();
            targetZone = nonHomeZones[randomIndex];
        }
    }

    // Store the intended activity for later use
    aiState->intendedActivity = activity;

    if (!targetZone.empty() && grid->zones.find(targetZone) != grid->zones.end())
    {
        aiState->targetZone = targetZone;
        aiState->currentState = AIStateName::MovingToZone;

        // Start planning phase before movement
        movement->startPlanning();
        // if (infoBox) infoBox->text = entityName + "\nPlanning to " + targetZone;
    }
    else
    {
        // No valid target zone, just idle
        if (infoBox)
            infoBox->text = entityName + "\n" + activity;
    }
}

void AISystem::executeMovementPlan(unsigned int entityUID)
{
    auto *aiState = aiManager->get(entityUID);
    auto *movement = moveManager->get(entityUID);
    auto *position = posManager->get(entityUID);

    if (!aiState || !movement || !position)
        return;

    if (aiState->currentState == AIStateName::MovingToZone)
    {
        Point currentCell = {(int)(position->x / cellSize), (int)(position->y / cellSize)};

        // Check if target zone exists
        if (grid->zones.find(aiState->targetZone) == grid->zones.end())
        {
            // Target zone doesn't exist, go idle
            aiState->currentState = AIStateName::Idle;
            movement->phase = MovementPhase::IDLE;
            return;
        }

        const auto &zone = grid->zones.at(aiState->targetZone);

        // Check if we're already in the target zone
        if (currentCell.x >= zone.x && currentCell.x < zone.x + zone.width &&
            currentCell.y >= zone.y && currentCell.y < zone.y + zone.height)
        {
            // Already in target zone, transition to performing activity
            aiState->currentState = AIStateName::PerformingActivity;
            movement->phase = MovementPhase::IDLE;
            aiState->activityTimer = 300.0f + (rand() % 600); // 5-15 minutes
            return;
        }

        // Find path to the zone and set movement target
        auto path = pathfinder->findPathToZone(currentCell, aiState->targetZone);

        if (!path.empty() && path.size() > 1)
        {
            // Move to the next step in the path
            Point nextStep = path[1]; // path[0] is current position, path[1] is next step
            float targetX = nextStep.x * cellSize + (cellSize / 2.0f);
            float targetY = nextStep.y * cellSize + (cellSize / 2.0f);

            // Set the target and start moving
            movement->setTarget(targetX, targetY);

            // CRITICAL FIX: Start the movement phase
            movement->phase = MovementPhase::MOVING;
            movement->phaseTimer = 0.0f; // Reset timer
            movement->isMoving = true;   // Make sure movement flag is set

            std::cout << "NPC " << entityUID << " moving to (" << nextStep.x << ", " << nextStep.y << ") towards " << aiState->targetZone << std::endl;
        }
        else
        {
            // No path found, return to idle
            std::cout << "No path found for NPC " << entityUID << " to " << aiState->targetZone << std::endl;
            aiState->currentState = AIStateName::Idle;
            movement->phase = MovementPhase::IDLE;
        }
    }
}

// Helper method to find a home zone for sleeping
std::string AISystem::getHomeZone(unsigned int npcId)
{
    // First, try to get the NPC's assigned home
    if (homeManager)
    {
        std::string assignedHome = homeManager->getNpcHome(npcId);
        if (!assignedHome.empty())
        {
            return assignedHome;
        }
    }

    // Fallback: Look for zones with "Home" in the name (should not happen if HomeManager is working)
    for (const auto &[zoneName, zone] : grid->zones)
    {
        if (zoneName.find("Home") != std::string::npos)
        {
            return zoneName;
        }
    }

    // Final fallback to any zone if no home found
    if (!grid->zones.empty())
    {
        return grid->zones.begin()->first;
    }
    return "";
}

// Helper method to find leisure zones for evening activities
std::string AISystem::getRandomLeisureZone()
{
    std::vector<std::string> leisureZones;

    // Collect zones that are good for leisure activities (excluding homes)
    for (const auto &[zoneName, zone] : grid->zones)
    {
        if ((zoneName.find("Cafe") != std::string::npos ||
             zoneName.find("Forest") != std::string::npos ||
             zoneName.find("Water") != std::string::npos ||
             zoneName.find("Stadium") != std::string::npos) &&
            zoneName.find("Home") == std::string::npos)
        { // Exclude homes
            leisureZones.push_back(zoneName);
        }
    }

    // Return random leisure zone, or any non-home zone if none found
    if (!leisureZones.empty())
    {
        int randomIndex = rand() % leisureZones.size();
        return leisureZones[randomIndex];
    }
    else
    {
        // Fallback to any non-home zone
        std::vector<std::string> nonHomeZones;
        for (const auto &[zoneName, zone] : grid->zones)
        {
            if (zoneName.find("Home") == std::string::npos)
            {
                nonHomeZones.push_back(zoneName);
            }
        }
        if (!nonHomeZones.empty())
        {
            int randomIndex = rand() % nonHomeZones.size();
            return nonHomeZones[randomIndex];
        }
    }

    return "";
}

// Helper method to determine if we should replan the current activity
bool AISystem::shouldReplanActivity(unsigned int entityUID)
{
    auto *aiState = aiManager->get(entityUID);
    if (!aiState)
        return false;

    const auto &timeOfDay = clock->getTimeOfDay();
    int currentHour = clock->getHour();

    // Always replan if the time period has changed significantly
    if (timeOfDay.isSleepTime(currentHour) && aiState->targetZone.find("Home") == std::string::npos)
    {
        return true; // Should go home to sleep
    }

    if (timeOfDay.isWorkHours(currentHour) && aiState->targetZone != "Work")
    {
        return true; // Should go to work
    }

    if (timeOfDay.isMealTime(currentHour) && aiState->targetZone != "Cafe")
    {
        return true; // Should go eat
    }

    return false;
}