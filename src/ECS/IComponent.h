#pragma once

namespace ECS
{
    class IComponent
    {
    public:
        virtual ~IComponent() = default;
        // void update() const = 0;
        // virtual const char* getType() const = 0;
    };
} // namespace ECS