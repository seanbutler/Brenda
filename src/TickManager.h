#pragma once
#include <functional>
#include <vector>
#include <algorithm>

/**
 * @brief Coordinates timed updates across multiple systems.
 * 
 * The TickManager ensures that AI and movement systems are synchronized
 * by providing coordinated tick events and managing update order.
 */
class TickManager {
public:
    using TickCallback = std::function<void(float deltaTime)>;
    
    struct SystemRegistration {
        TickCallback callback;
        float tickInterval;
        float lastTickTime;
        int priority;  // Lower numbers execute first
        
        SystemRegistration(TickCallback cb, float interval, int prio = 0)
            : callback(cb), tickInterval(interval), lastTickTime(0.0f), priority(prio) {}
    };

private:
    std::vector<SystemRegistration> systems;
    float totalTime = 0.0f;
    
public:
    /**
     * @brief Registers a system for coordinated updates.
     * @param callback Function to call on tick
     * @param tickInterval How often to tick this system (in seconds)
     * @param priority Lower numbers execute first (default: 0)
     */
    void registerSystem(TickCallback callback, float tickInterval, int priority = 0) {
        systems.emplace_back(callback, tickInterval, priority);
        
        // Sort by priority to maintain execution order
        std::sort(systems.begin(), systems.end(), 
                 [](const SystemRegistration& a, const SystemRegistration& b) {
                     return a.priority < b.priority;
                 });
    }
    
    /**
     * @brief Updates all registered systems based on their tick intervals.
     */
    void update(float deltaTime) {
        totalTime += deltaTime;
        
        for (auto& system : systems) {
            if (totalTime - system.lastTickTime >= system.tickInterval) {
                float actualDelta = totalTime - system.lastTickTime;
                system.callback(actualDelta);
                system.lastTickTime = totalTime;
            }
        }
    }
    
    /**
     * @brief Forces an immediate tick of all systems.
     */
    void forceTick(float deltaTime) {
        for (auto& system : systems) {
            system.callback(deltaTime);
            system.lastTickTime = totalTime;
        }
    }
    
    /**
     * @brief Gets the current total simulation time.
     */
    float getTotalTime() const { return totalTime; }
    
    /**
     * @brief Resets all system timers.
     */
    void resetTimers() {
        totalTime = 0.0f;
        for (auto& system : systems) {
            system.lastTickTime = 0.0f;
        }
    }
};
