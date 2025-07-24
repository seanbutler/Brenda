#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Grid.h"

/**
 * @brief Parser for Tiled Map Editor JSON files
 * 
 * Supports loading tile maps created with Tiled (https://www.mapeditor.org/)
 * and converting them to our Grid format.
 */
class TiledParser {
public:
    struct TiledTileset {
        int firstgid;
        std::string name;
        int tilewidth;
        int tileheight;
        int tilecount;
        int columns;
        std::string image;
        std::unordered_map<int, std::string> tileTypes; // tile ID -> tile type name
        std::unordered_map<int, int> tileCosts; // tile ID -> movement cost
    };

    struct TiledLayer {
        std::string name;
        std::string type;
        int width;
        int height;
        std::vector<int> data; // tile IDs
        bool visible = true;
    };

    struct TiledObjectGroup {
        std::string name;
        std::string type;
        std::vector<nlohmann::json> objects; // zone/object data
        bool visible = true;
    };

    /**
     * @brief Load a Tiled JSON file and populate the given Grid
     * 
     * @param filename Path to the Tiled JSON file
     * @param grid Grid object to populate
     * @return true if successful, false otherwise
     */
    static bool loadTiledMap(const std::string& filename, Grid& grid) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Failed to open Tiled file: " << filename << std::endl;
            return false;
        }

        nlohmann::json j;
        try {
            j = nlohmann::json::parse(file);
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "Error parsing Tiled JSON: " << e.what() << std::endl;
            return false;
        }

        // Validate this is a Tiled map
        if (!j.contains("type") || j["type"] != "map") {
            std::cerr << "Not a valid Tiled map file" << std::endl;
            return false;
        }

        // Get map dimensions
        int mapWidth = j.value("width", 0);
        int mapHeight = j.value("height", 0);
        int tileWidth = j.value("tilewidth", 32);
        int tileHeight = j.value("tileheight", 32);

        std::cout << "Loading Tiled map: " << mapWidth << "x" << mapHeight 
                  << " (tile size: " << tileWidth << "x" << tileHeight << ")" << std::endl;

        // Resize grid to match map
        grid = Grid(mapWidth, mapHeight);

        // Load tilesets
        std::vector<TiledTileset> tilesets;
        if (j.contains("tilesets")) {
            for (const auto& tilesetRef : j["tilesets"]) {
                TiledTileset tileset;
                tileset.firstgid = tilesetRef.value("firstgid", 1);
                
                // Handle embedded vs external tilesets
                if (tilesetRef.contains("source")) {
                    // External tileset - would need to load from separate file
                    std::cerr << "External tilesets not yet supported" << std::endl;
                    continue;
                } else {
                    // Embedded tileset
                    tileset.name = tilesetRef.value("name", "");
                    tileset.tilewidth = tilesetRef.value("tilewidth", tileWidth);
                    tileset.tileheight = tilesetRef.value("tileheight", tileHeight);
                    tileset.tilecount = tilesetRef.value("tilecount", 0);
                    tileset.columns = tilesetRef.value("columns", 1);
                    tileset.image = tilesetRef.value("image", "");
                }

                // Load tile properties and types
                if (tilesetRef.contains("tiles")) {
                    for (const auto& tileData : tilesetRef["tiles"]) {
                        int tileId = tileData.value("id", 0);
                        int globalId = tileset.firstgid + tileId;
                        
                        if (tileData.contains("type")) {
                            tileset.tileTypes[globalId] = tileData["type"];
                        }
                        
                        if (tileData.contains("properties")) {
                            for (const auto& prop : tileData["properties"]) {
                                if (prop.contains("name") && prop["name"] == "cost") {
                                    tileset.tileCosts[globalId] = prop.value("value", 1);
                                }
                            }
                        }
                    }
                }

                tilesets.push_back(tileset);
                std::cout << "Loaded tileset: " << tileset.name << " (firstgid=" << tileset.firstgid << ")" << std::endl;
            }
        }

        // Load layers
        if (j.contains("layers")) {
            for (const auto& layerJson : j["layers"]) {
                std::string layerType = layerJson.value("type", "");
                std::string layerName = layerJson.value("name", "");
                bool visible = layerJson.value("visible", true);

                if (!visible) continue; // Skip invisible layers

                if (layerType == "tilelayer") {
                    // Tile layer - contains the actual tile data
                    processTileLayer(layerJson, grid, tilesets);
                } else if (layerType == "objectgroup") {
                    // Object layer - contains zones, spawn points, etc.
                    processObjectGroup(layerJson, grid);
                }
            }
        }

        return true;
    }

private:
    static void processTileLayer(const nlohmann::json& layer, Grid& grid, const std::vector<TiledTileset>& tilesets) {
        std::string layerName = layer.value("name", "");
        int layerWidth = layer.value("width", 0);
        int layerHeight = layer.value("height", 0);
        
        std::cout << "Processing tile layer: " << layerName << std::endl;

        if (layer.contains("data")) {
            std::vector<int> tileData = layer["data"];
            
            for (int y = 0; y < layerHeight && y < grid.getHeight(); ++y) {
                for (int x = 0; x < layerWidth && x < grid.getWidth(); ++x) {
                    int index = y * layerWidth + x;
                    if (index >= tileData.size()) continue;
                    
                    int gid = tileData[index];
                    if (gid == 0) continue; // Empty tile
                    
                    // Find which tileset this tile belongs to
                    const TiledTileset* tileset = nullptr;
                    for (auto it = tilesets.rbegin(); it != tilesets.rend(); ++it) {
                        if (gid >= it->firstgid) {
                            tileset = &(*it);
                            break;
                        }
                    }
                    
                    if (!tileset) continue;
                    
                    // Set tile properties
                    Cell& cell = grid.at(x, y);
                    
                    // Set obstacle type based on tile type
                    if (tileset->tileTypes.count(gid)) {
                        std::string tileType = tileset->tileTypes.at(gid);
                        cell.obstacle = stringToObstacleType(tileType);
                    }
                    
                    // Set movement cost
                    if (tileset->tileCosts.count(gid)) {
                        cell.cost = tileset->tileCosts.at(gid);
                    } else {
                        // Default costs based on obstacle type
                        cell.cost = getDefaultCost(cell.obstacle);
                    }
                }
            }
        }
    }

    static void processObjectGroup(const nlohmann::json& layer, Grid& grid) {
        std::string layerName = layer.value("name", "");
        std::cout << "Processing object group: " << layerName << std::endl;

        if (layer.contains("objects")) {
            for (const auto& obj : layer["objects"]) {
                std::string name = obj.value("name", "");
                std::string type = obj.value("type", "");
                float x = obj.value("x", 0.0f);
                float y = obj.value("y", 0.0f);
                float width = obj.value("width", 0.0f);
                float height = obj.value("height", 0.0f);

                // Convert pixel coordinates to tile coordinates
                // (assuming 32x32 tiles, but could be made configurable)
                int tileX = static_cast<int>(x / 32);
                int tileY = static_cast<int>(y / 32);
                int tileWidth = static_cast<int>(width / 32);
                int tileHeight = static_cast<int>(height / 32);

                // Add zone to grid
                if (!name.empty() && tileWidth > 0 && tileHeight > 0) {
                    grid.zones[name] = {tileX, tileY, tileWidth, tileHeight};
                    std::cout << "Added zone: " << name << " (" << tileX << "," << tileY 
                              << " " << tileWidth << "x" << tileHeight << ")" << std::endl;
                }
            }
        }
    }

    static ObstacleType stringToObstacleType(const std::string& typeStr) {
        static std::unordered_map<std::string, ObstacleType> mapping = {
            {"Wall", ObstacleType::Wall},
            {"wall", ObstacleType::Wall},
            {"Water", ObstacleType::Water},
            {"water", ObstacleType::Water},
            {"Forest", ObstacleType::Forest},
            {"forest", ObstacleType::Forest},
            {"Grass", ObstacleType::Grass},
            {"grass", ObstacleType::Grass},
            {"Path", ObstacleType::Path},
            {"path", ObstacleType::Path},
            {"None", ObstacleType::None},
            {"none", ObstacleType::None}
        };
        
        auto it = mapping.find(typeStr);
        return (it != mapping.end()) ? it->second : ObstacleType::None;
    }

    static int getDefaultCost(ObstacleType obstacle) {
        switch (obstacle) {
            case ObstacleType::Wall: return 9999;
            case ObstacleType::Water: return 50;
            case ObstacleType::Forest: return 10;
            case ObstacleType::Grass: return 1;
            case ObstacleType::Path: return 0;
            default: return 1;
        }
    }
};
