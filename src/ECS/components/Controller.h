#pragma once
#include <string>

class Controller : public ECS::IComponent{

public:
    std::string type;
    Controller(const std::string& t = "unknown") : type(t) {}

    virtual void update() {};
};