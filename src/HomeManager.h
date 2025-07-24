#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include "Grid.h"

struct HomeAssignment {
    std::string zoneName;
    int npcId;
    std::string npcName;
};

class HomeManager {
private:
    std::unordered_map<int, std::string> npcToHome; // NPC ID -> Home zone name
    std::unordered_map<std::string, int> homeToNpc; // Home zone name -> NPC ID
    std::vector<std::string> availableHomes;

public:
    HomeManager() {
        // Initialize with all available home zones
        availableHomes = {
            "Home", "Home_2", "Home_3", "Home_4", "Home_5", "Home_6"
        };
    }

    /**
     * Assigns a home to an NPC
     */
    bool assignHome(int npcId, const std::string& npcName) {
        // Check if NPC already has a home
        if (npcToHome.find(npcId) != npcToHome.end()) {
            return true; // Already has a home
        }

        // Find an available home
        for (const auto& homeName : availableHomes) {
            if (homeToNpc.find(homeName) == homeToNpc.end()) {
                // This home is available
                npcToHome[npcId] = homeName;
                homeToNpc[homeName] = npcId;
                std::cout << "Assigned " << npcName << " (ID: " << npcId << ") to " << homeName << std::endl;
                return true;
            }
        }

        std::cout << "Warning: No available homes for " << npcName << " (ID: " << npcId << ")" << std::endl;
        return false; // No available homes
    }

    /**
     * Gets the home zone name for an NPC
     */
    std::string getNpcHome(int npcId) const {
        auto it = npcToHome.find(npcId);
        return (it != npcToHome.end()) ? it->second : "";
    }

    /**
     * Gets the NPC ID living in a specific home
     */
    int getHomeResident(const std::string& homeName) const {
        auto it = homeToNpc.find(homeName);
        return (it != homeToNpc.end()) ? it->second : -1;
    }

    /**
     * Gets all home assignments
     */
    std::vector<HomeAssignment> getAllAssignments() const {
        std::vector<HomeAssignment> assignments;
        for (const auto& [npcId, homeName] : npcToHome) {
            assignments.push_back({homeName, npcId, "NPC_" + std::to_string(npcId)});
        }
        return assignments;
    }

    /**
     * Removes an NPC's home assignment
     */
    void unassignHome(int npcId) {
        auto it = npcToHome.find(npcId);
        if (it != npcToHome.end()) {
            std::string homeName = it->second;
            npcToHome.erase(it);
            homeToNpc.erase(homeName);
        }
    }

    /**
     * Gets the home zone coordinates for an NPC
     */
    bool getNpcHomeCoordinates(int npcId, const Grid& grid, int& homeX, int& homeY) const {
        std::string homeName = getNpcHome(npcId);
        if (homeName.empty()) return false;

        auto it = grid.zones.find(homeName);
        if (it != grid.zones.end()) {
            const Zone& zone = it->second;
            // Return center of the home zone
            homeX = zone.x + zone.width / 2;
            homeY = zone.y + zone.height / 2;
            return true;
        }
        return false;
    }
};