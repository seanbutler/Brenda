#pragma once

#include <cstdint>

// not sure this needs to be a class
// could be a func that returns a unique ID


namespace ECS
{
    class Entity
    {
    private:
        std::uint64_t uid;
        static std::uint64_t nextUID;

    public:
        Entity() : uid(nextUID++) {}

        std::uint64_t getUID() const { return uid; }
    };

} // namespace ECS
