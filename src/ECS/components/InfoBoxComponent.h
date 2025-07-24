#pragma once
#include <string>
#include "../IComponent.h"

/**
 * @struct InfoBoxComponent
 * @brief Holds the dynamic text to be displayed for an entity.
 */
struct InfoBoxComponent : public ECS::IComponent {
    std::string text;
    
    InfoBoxComponent(const std::string& t = "") : text(t) {}
};

