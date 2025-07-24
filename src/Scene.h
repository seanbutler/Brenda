#pragma once

#include "./ECS/Entity.h"
#include "./ECS/PositionManager.h"
#include "./ECS/RenderableManager.h"
#include "./ECS/DescriptionComponentManager.h"
#include "./ECS/ControllerManager.h"
#include "./ECS/MovementManager.h"
#include "./ECS/AIManager.h"

#include <vector>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>
#include <fstream>

class InfoBoxManager; // Forward declare

class Scene
{
public:
    std::vector<std::unique_ptr<ECS::Entity>> entities;

    bool loadFromFile(const std::string &filename,
                      PositionManager *positionManager,
                      RenderableManager *renderManager,
                      DescriptionComponentManager *descriptionManager,
                      ControllerManager *controllerManager,
                      MovementManager *movementManager,
                      AIManager *aiManager,
                      InfoBoxManager *infoBoxManager, // Add parameter
                      Uint32 cellSize);
};