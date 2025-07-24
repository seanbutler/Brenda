#pragma once
#include <string>
#include "../IComponent.h"

class Controller : public ECS::IComponent {

public:
    std::string type;
    Controller(const std::string& t = "unknown") : type(t) {}

    virtual void update() {};
};