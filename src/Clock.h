#pragma once
#include <string>
#include "TimeOfDay.h"

class Clock {
public:
    Clock(float timeScale = 60.0f); // 1 real second = 60 sim seconds (1 real minute = 1 sim hour)
    
    void update(float deltaTime);
    
    // Time getters
    int getHour() const { return hour; }
    int getMinute() const { return minute; }
    int getDay() const { return day; }
    float getTotalSeconds() const { return totalSeconds; }
    
    // Time of day access
    const TimeOfDay& getTimeOfDay() const { return timeOfDay; }
    TimeOfDay& getTimeOfDay() { return timeOfDay; }
    
    // Convenience methods
    std::string getTimeString() const;
    
    // Time progression control
    void setTimeScale(float scale) { timeScale = scale; }
    float getTimeScale() const { return timeScale; }
    void pause() { paused = true; }
    void resume() { paused = false; }
    bool isPaused() const { return paused; }

private:
    float totalSeconds;  // Total simulation seconds elapsed
    int hour;           // 0-23
    int minute;         // 0-59
    int day;            // Day counter starting from 1
    
    TimeOfDay timeOfDay; // Encapsulated time of day logic
    
    float timeScale;    // How fast simulation time moves vs real time
    bool paused;        // Whether time is paused
    
    void updateTimeComponents();
};
