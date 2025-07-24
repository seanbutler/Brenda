#pragma once

/**
 * @brief An interface for managing a specific type of component in an ECS architecture.
 *
 * @details The IComponentManager defines the contract for any component manager. Each manager
 * is responsible for handling a single type of component across all entities in the scene.
 * @note In some ECS terminologies, this is also referred to as a "System".
 */
class IComponentManager
{
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~IComponentManager() = default;

    /**
     * @brief Pure virtual update function.
     *
     * This function should be implemented by derived classes to update the state
     * of their managed components each frame.
     */
    // virtual void update() = 0;

    /**
     * @brief Pure virtual dump function for debugging.
     *
     * This function should be implemented by derived classes to print the current
     * state of all managed components to the console.
     */
    virtual void dump() = 0;
};
