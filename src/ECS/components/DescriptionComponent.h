#pragma once
#include <string>
#include "../IComponent.h"

/**
 * @brief Component to store a descriptive string for an entity.
 */
struct DescriptionComponent : public ECS::IComponent {
    std::string name;
    std::string text;
    DescriptionComponent(const std::string& n, const std::string& d) : name(n), text(d) {}
};