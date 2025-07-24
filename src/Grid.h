#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>

#include <fstream>
#include <regex>
#include <nlohmann/json.hpp>

// Forward declaration
class TiledParser;


enum class ObstacleType {
    None,
    Wall,
    Water,
    Forest,
    Grass,
    Path
    // Add more obstacle types as needed
};

struct Cell {
    ObstacleType obstacle = ObstacleType::None;
    int cost = 1; // Default cost for a plain tile
    // Add more fields as needed (terrain, items, etc.)
};

struct Zone {
    int x, y, width, height;
};

class Grid {
public:
    
    Grid(int width, int height)
        : width(width), height(height), cells(width * height) {}

    Cell& at(int x, int y) {
        return cells[y * width + x];
    }

    const Cell& at(int x, int y) const {
        return cells[y * width + x];
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    /**
     * @brief Dumps the grid to console in ASCII format
     * Shows obstacles as '#' and movement costs as numbers
     */
    void dump() const {
        std::cout << "Grid (" << width << "x" << height << "):\n";
        std::cout << "Legend: # = Wall, numbers = movement cost\n\n";
        
        // Print column numbers header
        std::cout << "   ";
        for (int x = 0; x < width; ++x) {
            std::cout << std::setw(3) << x;
        }
        std::cout << "\n";
        
        // Print top border
        std::cout << "  +";
        for (int x = 0; x < width; ++x) {
            std::cout << "---";
        }
        std::cout << "+\n";
        
        // Print each row
        for (int y = 0; y < height; ++y) {
            std::cout << std::setw(2) << y << "|";
            for (int x = 0; x < width; ++x) {
                const Cell& cell = at(x, y); // Use the at() method instead of cells[y][x]
                
                // if (cell.obstacle == ObstacleType::Wall) {
                //     std::cout << " # ";
                // } else 
                {
                    // Show movement cost for non-wall tiles
                    if (cell.cost >= 100) {
                        std::cout << "99+"; // Cap display at 99 for formatting
                    } else {
                        std::cout << std::setw(2) << cell.cost << " ";
                    }
                }
            }
            std::cout << "|\n";
        }
        
        // Print bottom border
        std::cout << "  +";
        for (int x = 0; x < width; ++x) {
            std::cout << "---";
        }
        std::cout << "+\n\n";

        // Print loaded zones
        if (!zones.empty()) {
            std::cout << "Loaded Zones:\n";
            for (const auto& [name, zone] : zones) {
                std::cout << " - " << std::setw(10) << std::left << name
                          << " (x:" << zone.x << ", y:" << zone.y
                          << ", w:" << zone.width << ", h:" << zone.height << ")\n";
            }
            std::cout << "\n";
        }
    }

    bool loadFromJson(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) return false;

        nlohmann::json j;
        try {
            j = nlohmann::json::parse(file, nullptr, true, true);
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "Error parsing environment.json: " << e.what() << std::endl;
            return false;
        }

        this->width = j.value("width", 0);
        this->height = j.value("height", 0);
        this->cells.assign(this->width * this->height, Cell{}); // Re-initialize the cells vector

        // 1. Read tile properties (like cost) into a map for quick lookup
        std::unordered_map<std::string, int> tileCosts;
        if (j.contains("tile_properties"))
        {
            for (auto& [type, props] : j["tile_properties"].items())
            {
                tileCosts[type] = props.value("cost", 1);
            }
        }

        // Helper map for string to enum conversion
        std::unordered_map<std::string, ObstacleType> stringToObstacleType = {
            {"Wall", ObstacleType::Wall},
            {"Water", ObstacleType::Water},
            {"Forest", ObstacleType::Forest},
            {"Grass", ObstacleType::Grass},
            {"Path", ObstacleType::Path}
        };

        // 2. Process all tiles to set their type and cost
        if (j.contains("tiles")) {
            for (const auto& tile : j["tiles"]) {
                int x = tile.value("x", -1);
                int y = tile.value("y", -1);
                std::string typeStr = tile.value("type", "None");

                // Use this->at() or just at() - no more "grid."
                if (x >= 0 && x < getWidth() && y >= 0 && y < getHeight())
                {
                    auto it = stringToObstacleType.find(typeStr);
                    at(x, y).obstacle = (it != stringToObstacleType.end()) ? it->second : ObstacleType::None;
                    at(x, y).cost = tileCosts.count(typeStr) ? tileCosts[typeStr] : 1;
                }
            }
        }

        // 3. Process the zones
        if (j.contains("zones")) {
            for (auto& [name, zone_data] : j["zones"].items()) {
                // Use this->zones or just zones
                zones[name] = {
                    zone_data.value("x", 0),
                    zone_data.value("y", 0),
                    zone_data.value("width", 0),
                    zone_data.value("height", 0)
                };
            }
        }

        return true;
    }

    /**
     * @brief Load grid from a Tiled Map Editor JSON file
     * 
     * @param filename Path to the .json file exported from Tiled
     * @return true if successful, false otherwise
     */
    bool loadFromTiled(const std::string& filename);

    /**
     * @brief Cycles a tile to the next type when clicked
     */
    void cycleTileType(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        
        Cell& cell = at(x, y);
        
        // Define the cycle order
        switch (cell.obstacle) {
            case ObstacleType::None:
                cell.obstacle = ObstacleType::Grass;
                cell.cost = 10;
                break;
            case ObstacleType::Grass:
                cell.obstacle = ObstacleType::Path;
                cell.cost = 5;
                break;
            case ObstacleType::Path:
                cell.obstacle = ObstacleType::Forest;
                cell.cost = 20;
                break;
            case ObstacleType::Forest:
                cell.obstacle = ObstacleType::Water;
                cell.cost = 100;
                break;
            case ObstacleType::Water:
                cell.obstacle = ObstacleType::Wall;
                cell.cost = 9999;
                break;
            case ObstacleType::Wall:
                cell.obstacle = ObstacleType::None;
                cell.cost = 1;
                break;
        }
    }
    
    /**
     * @brief Save current grid state back to environment.json
     */
    void saveToJson(const std::string& filename) {
        nlohmann::json j;
        
        // Add width and height
        j["width"] = width;
        j["height"] = height;
        
        // Add tile_properties (preserve original structure)
        j["tile_properties"] = {
            {"Wall", {{"cost", 9999}}},
            {"Forest", {{"cost", 20}}},
            {"Water", {{"cost", 50}}},
            {"Grass", {{"cost", 10}}},
            {"Path", {{"cost", 5}}},
            {"Plain", {{"cost", 0}}}
        };
        
        // Convert cells back to tiles array
        nlohmann::json tiles = nlohmann::json::array();
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const Cell& cell = at(x, y);
                if (cell.obstacle != ObstacleType::None) {
                    std::string type;
                    switch (cell.obstacle) {
                        case ObstacleType::Wall: type = "Wall"; break;
                        case ObstacleType::Water: type = "Water"; break;
                        case ObstacleType::Forest: type = "Forest"; break;
                        case ObstacleType::Grass: type = "Grass"; break;
                        case ObstacleType::Path: type = "Path"; break;
                        default: continue;
                    }
                    tiles.push_back({{"type", type}, {"x", x}, {"y", y}});
                }
            }
        }
        j["tiles"] = tiles;
        
        // Add zones (preserve existing zones from original file)
        j["zones"] = {
            {"Cafe", {{"x", 1}, {"y", 1}, {"width", 4}, {"height", 4}}},
            {"Bureau", {{"x", 20}, {"y", 2}, {"width", 4}, {"height", 3}}},
            {"Work", {{"x", 8}, {"y", 7}, {"width", 10}, {"height", 8}}},
            {"Home", {{"x", 25}, {"y", 10}, {"width", 2}, {"height", 2}}},
            {"Home_2", {{"x", 30}, {"y", 10}, {"width", 2}, {"height", 2}}},
            {"Home_3", {{"x", 35}, {"y", 10}, {"width", 2}, {"height", 2}}},
            {"Home_4", {{"x", 25}, {"y", 16}, {"width", 2}, {"height", 2}}},
            {"Home_5", {{"x", 29}, {"y", 15}, {"width", 4}, {"height", 4}}},
            {"Home_6", {{"x", 34}, {"y", 15}, {"width", 4}, {"height", 4}}},
            {"Stadium", {{"x", 15}, {"y", 22}, {"width", 10}, {"height", 6}}},
            {"Forest_0", {{"x", 5}, {"y", 10}, {"width", 3}, {"height", 3}}},
            {"Forest_1", {{"x", 30}, {"y", 5}, {"width", 3}, {"height", 3}}},
            {"Forest_2", {{"x", 5}, {"y", 25}, {"width", 4}, {"height", 4}}},
            {"Forest_3", {{"x", 35}, {"y", 20}, {"width", 5}, {"height", 3}}},
            {"Water_1", {{"x", 15}, {"y", 3}, {"width", 2}, {"height", 2}}},
            {"Water_2", {{"x", 25}, {"y", 5}, {"width", 3}, {"height", 2}}},
            {"Water_3", {{"x", 2}, {"y", 20}, {"width", 9}, {"height", 1}}},
            {"Water_4", {{"x", 28}, {"y", 22}, {"width", 4}, {"height", 4}}}
        };
        
        std::ofstream file(filename);
        file << j.dump(2); // Pretty print with 2-space indentation
    }
    
    /**
     * @brief A map to store named rectangular zones.
     * The key is the zone name (e.g., "Forest", "Cafe").
     */
    std::unordered_map<std::string, Zone> zones;


private:
    int width, height;
    std::vector<Cell> cells;

};


