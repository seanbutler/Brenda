#include "TimeOfDay.h"

TimeOfDay::TimeOfDay() : currentPeriod(DayPeriod::Morning) {
    // Default to morning
}

void TimeOfDay::updateFromHour(int hour) {
    currentPeriod = calculatePeriod(hour);
}

DayPeriod TimeOfDay::calculatePeriod(int hour) const {
    if (hour >= 5 && hour < 8) return DayPeriod::Dawn;
    if (hour >= 8 && hour < 12) return DayPeriod::Morning;
    if (hour >= 12 && hour < 17) return DayPeriod::Afternoon;
    if (hour >= 17 && hour < 20) return DayPeriod::Evening;
    return DayPeriod::Night;
}

std::string TimeOfDay::getPeriodString() const {
    switch (currentPeriod) {
        case DayPeriod::Dawn: return "Dawn";
        case DayPeriod::Morning: return "Morning";
        case DayPeriod::Afternoon: return "Afternoon";
        case DayPeriod::Evening: return "Evening";
        case DayPeriod::Night: return "Night";
    }
    return "Unknown";
}

bool TimeOfDay::isDay() const {
    return currentPeriod == DayPeriod::Dawn || 
           currentPeriod == DayPeriod::Morning || 
           currentPeriod == DayPeriod::Afternoon || 
           currentPeriod == DayPeriod::Evening;
}

bool TimeOfDay::isNight() const {
    return currentPeriod == DayPeriod::Night;
}

bool TimeOfDay::isWorkHours(int hour) const {
    return hour >= 9 && hour < 17;
}

bool TimeOfDay::isSleepTime(int hour) const {
    return hour >= 22 || hour < 6;
}

bool TimeOfDay::isMealTime(int hour) const {
    // Breakfast: 7:00-9:00, Lunch: 12:00-13:00, Dinner: 18:00-20:00
    return (hour >= 7 && hour < 9) || 
           (hour >= 12 && hour < 13) || 
           (hour >= 18 && hour < 20);
}

std::string TimeOfDay::getSuggestedActivity(int hour) const {
    if (isSleepTime(hour)) return "Sleeping";
    if (isWorkHours(hour)) return "Working";
    if (isMealTime(hour)) return "Eating";
    
    switch (currentPeriod) {
        case DayPeriod::Dawn: return "Morning routine";
        case DayPeriod::Morning: return "Socializing";
        case DayPeriod::Afternoon: return "Activities";
        case DayPeriod::Evening: return "Relaxing";
        case DayPeriod::Night: return "Resting";
    }
    return "Idle";
}

bool TimeOfDay::isGoodTimeForWork(int hour) const {
    return isWorkHours(hour);
}

bool TimeOfDay::isGoodTimeForSleep(int hour) const {
    return isSleepTime(hour);
}

bool TimeOfDay::isGoodTimeForLeisure(int hour) const {
    return currentPeriod == DayPeriod::Evening || 
           currentPeriod == DayPeriod::Afternoon ||
           (!isWorkHours(hour) && !isSleepTime(hour));
}
