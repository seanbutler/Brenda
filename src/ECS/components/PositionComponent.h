#pragma once

#include "../IComponent.h"

class PositionComponent : public ECS::IComponent {
    public:

    PositionComponent() = default;
    PositionComponent(float x = 0, float y = 0) : x(x), y(y) {}

    virtual ~PositionComponent() = default;
    float x, y;
};
