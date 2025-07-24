#include "Scene.h"

#include "ECS/DescriptionComponentManager.h"
#include "ECS/ControllerManager.h"
#include "ECS/MovementManager.h"
#include "./ECS/AIManager.h"
#include "./ECS/InfoBoxManager.h" // <-- Add include

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

bool Scene::loadFromFile(
    const std::string &filename,
    PositionManager *posManager,
    RenderableManager *renderManager,
    DescriptionComponentManager *descManager,
    ControllerManager *controllerManager,
    MovementManager *moveManager,
    AIManager *aiManager,
    InfoBoxManager *infoBoxManager,    Uint32 cellSize)
{

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open scene file " << filename << std::endl;
        return false;
    }

    nlohmann::json j;
    try
    {
        j = nlohmann::json::parse(file, nullptr, true, true);
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "Error parsing scene file: " << e.what() << std::endl;
        return false;
    }

    // Check if the root object contains the "entities" key
    if (!j.contains("entities"))
    {
        std::cerr << "Error: " << filename << " does not contain 'entities' array." << std::endl;
        return false;
    }

    // Iterate over the array associated with the "entities" key
    for (const auto &entity_data : j["entities"])
    {
        if (!entity_data.contains("id"))
            continue;
        unsigned int uid = entity_data["id"];

        // Description
        if (entity_data.contains("description"))
        {
            if (descManager) {
                std::string name = entity_data["description"].value("name", "");
                std::string longDesc = entity_data["description"].value("long", "");
                descManager->create(uid, name, longDesc);
            }
        }

        // Position
        if (entity_data.contains("position"))
        {
            if (posManager)
            {
                posManager->create(uid,
                                   entity_data["position"].value("x", 0.0f),
                                   entity_data["position"].value("y", 0.0f));
            }
        }

        // Controller
        if (entity_data.contains("controller"))
        {
            if (controllerManager)
            {
                // Get the controller type from the JSON data
                std::string controllerType = entity_data["controller"].value("type", "unknown");
                // Pass the type to the create method
                controllerManager->create(uid, controllerType);
            }
        }

        // Movement (for player/AI target-based movement)
        if (entity_data.contains("movement"))
        {
            if (moveManager)
            {
                moveManager->create(uid, entity_data["movement"].value("speed", 0.0f));
            }
        }

        // --- Add AI State Component ---
        if (entity_data.contains("ai_state"))
        {
            if (aiManager)
                aiManager->create(uid);
            // If an entity has AI, it gets an infobox with their name.
            if (infoBoxManager) {
                std::string name = entity_data["description"].value("name", "Unknown");
                infoBoxManager->create(uid, name + "\nIdle");
            }
        }

        // Shape/Renderable
        if (entity_data.contains("shape"))
        {
            if (renderManager)
            {
                auto shape_data = entity_data["shape"];
                auto color_data = shape_data["color"];
                SDL_Color color = {
                    (Uint8)color_data[0].get<int>(),
                    (Uint8)color_data[1].get<int>(),
                    (Uint8)color_data[2].get<int>(),
                    (Uint8)color_data[3].get<int>()};

                if (shape_data.value("type", "") == "circle") {
                    renderManager->createCircle(uid, 
                        entity_data["position"].value("x", 0.0f),
                        entity_data["position"].value("y", 0.0f),
                        color);
                }
            }
        }
    }

    return true;
}
