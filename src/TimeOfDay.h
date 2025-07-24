#pragma once
#include <string>

enum class DayPeriod {
    Dawn,       // 5:00 - 8:00
    Morning,    // 8:00 - 12:00
    Afternoon,  // 12:00 - 17:00
    Evening,    // 17:00 - 20:00
    Night       // 20:00 - 5:00
};

class TimeOfDay {
public:
    TimeOfDay();
    
    // Update the time of day based on current hour
    void updateFromHour(int hour);
    
    // Getters
    DayPeriod getCurrentPeriod() const { return currentPeriod; }
    std::string getPeriodString() const;
    
    // Convenience methods
    bool isDay() const;
    bool isNight() const;
    bool isWorkHours(int hour) const;      // 9:00 - 17:00
    bool isSleepTime(int hour) const;      // 22:00 - 6:00
    bool isMealTime(int hour) const;       // 7:00-9:00, 12:00-13:00, 18:00-20:00
    
    // Activity suggestions based on time
    std::string getSuggestedActivity(int hour) const;
    bool isGoodTimeForWork(int hour) const;
    bool isGoodTimeForSleep(int hour) const;
    bool isGoodTimeForLeisure(int hour) const;

private:
    DayPeriod currentPeriod;
    
    DayPeriod calculatePeriod(int hour) const;
};
