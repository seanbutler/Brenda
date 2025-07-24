#include "Clock.h"
#include <sstream>
#include <iomanip>

Clock::Clock(float timeScale) 
    : totalSeconds(0.0f)
    , hour(8)  // Start at 8 AM
    , minute(0)
    , day(1)
    , timeOfDay()
    , timeScale(timeScale)
    , paused(false) 
{
    // Initialize time of day with starting hour
    timeOfDay.updateFromHour(hour);
}

void Clock::update(float deltaTime) {
    if (paused) return;
    
    totalSeconds += deltaTime * timeScale;
    updateTimeComponents();
}

void Clock::updateTimeComponents() {
    // Convert total seconds to hours, minutes, and days
    int totalMinutes = static_cast<int>(totalSeconds / 60.0f);
    int totalHours = totalMinutes / 60;
    
    // minute = totalMinutes % 60;
    // hour = (8 + totalHours) % 24;  // Start at 8 AM
    // day = 1 + (8 + totalHours) / 24;  // Day advances every 24 hours

    minute = totalMinutes % 60;
    hour = (totalHours) % 24;  // Start at 8 AM
    day = 1 + (totalHours) / 24;  // Day advances every 24 hours
    

    // Update time of day based on current hour
    timeOfDay.updateFromHour(hour);
}

std::string Clock::getTimeString() const {
    std::stringstream ss;
    ss << "Day " << day << " - " 
       << std::setfill('0') << std::setw(2) << hour << ":" 
       << std::setfill('0') << std::setw(2) << minute;
    return ss.str();
}
