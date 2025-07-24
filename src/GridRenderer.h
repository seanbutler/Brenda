#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Grid.h"
#include "HomeManager.h"  // Add this include

namespace GridRenderer
{
    /**
     * Renders the grid to the given SDL_Renderer.
     *
     * @param renderer The SDL_Renderer to render to.
     * @param grid The Grid object containing cell data.
     * @param cellSize The size of each cell in pixels.
     * @param editMode If true, enables edit mode features like grid lines and hover effect.
     * @param showZones If true, shows zone outlines and indicators.
     */
    void render(SDL_Renderer *renderer, const Grid &grid, int cellSize, bool editMode = false, bool showZones = true)
    {
        // First pass: Render all tiles (same as before)
        for (int y = 0; y < grid.getHeight(); ++y)
        {
            for (int x = 0; x < grid.getWidth(); ++x)
            {
                const Cell &cell = grid.at(x, y);

                SDL_FRect rect = {
                    static_cast<float>(x * cellSize),
                    static_cast<float>(y * cellSize),
                    static_cast<float>(cellSize),
                    static_cast<float>(cellSize)};

                // Set color based on obstacle type
                switch (cell.obstacle)
                {
                case ObstacleType::Wall:
                    SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255); // dark gray
                    break;
                case ObstacleType::Water:
                    SDL_SetRenderDrawColor(renderer, 0, 120, 255, 255); // blue
                    break;
                case ObstacleType::Forest:
                    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); // darker forest green
                    break;
                case ObstacleType::Grass:
                    SDL_SetRenderDrawColor(renderer, 144, 238, 144, 255); // light green grass
                    break;
                case ObstacleType::Path:
                    SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // bright orange path
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 40); // light gray, transparent
                    break;
                }

                SDL_RenderFillRect(renderer, &rect);

                // Optionally, draw grid lines for tiles
                if (editMode)
                {
                    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 128);
                    SDL_RenderRect(renderer, &rect);
                }
            }
        }

        // Only render zones if showZones is true
        if (showZones) {
            // Second pass: Draw zone outlines
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red outlines for zones
            for (const auto &[zoneName, zone] : grid.zones)
            {
                SDL_FRect zoneRect = {
                    static_cast<float>(zone.x * cellSize),
                    static_cast<float>(zone.y * cellSize),
                    static_cast<float>(zone.width * cellSize),
                    static_cast<float>(zone.height * cellSize)};

                // Draw thick outline (3 pixels)
                for (int thickness = 0; thickness < 3; ++thickness)
                {
                    SDL_FRect outlineRect = {
                        zoneRect.x - thickness,
                        zoneRect.y - thickness,
                        zoneRect.w + (2 * thickness),
                        zoneRect.h + (2 * thickness)};
                    SDL_RenderRect(renderer, &outlineRect);
                }
            }

            // Third pass: Draw zone indicators with NPC home assignments
            for (const auto &[zoneName, zone] : grid.zones)
            {
                // Draw a small colored square in the top-left corner of each zone
                SDL_FRect labelRect = {
                    static_cast<float>(zone.x * cellSize + 2),
                    static_cast<float>(zone.y * cellSize + 2),
                    static_cast<float>(cellSize / 3),
                    static_cast<float>(cellSize / 3)};

                // Different colors for different zone types
                if (zoneName.find("Home") != std::string::npos)
                {
                    // Check if this home is assigned to an NPC
                    // For now, we'll use different shades of yellow for different homes
                    if (zoneName == "Home") {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255); // Bright yellow - NPC 1
                    } else if (zoneName == "Home_2") {
                        SDL_SetRenderDrawColor(renderer, 255, 200, 100, 255); // Orange-yellow - NPC 2
                    } else if (zoneName == "Home_3") {
                        SDL_SetRenderDrawColor(renderer, 255, 150, 100, 255); // Orange - NPC 3
                    } else if (zoneName == "Home_4") {
                        SDL_SetRenderDrawColor(renderer, 200, 255, 100, 255); // Green-yellow - NPC 4
                    } else if (zoneName == "Home_5") {
                        SDL_SetRenderDrawColor(renderer, 150, 255, 100, 255); // Light green - NPC 5
                    } else if (zoneName == "Home_6") {
                        SDL_SetRenderDrawColor(renderer, 100, 255, 200, 255); // Cyan - NPC 6
                    } else {
                        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 200); // Gray for unassigned homes
                    }
                }
                else if (zoneName.find("Forest") != std::string::npos)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200); // Green for forests
                }
                else if (zoneName.find("Water") != std::string::npos)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 200); // Blue for water
                }
                else if (zoneName.find("Cafe") != std::string::npos)
                {
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 200); // Brown for cafe
                }
                else if (zoneName.find("Stadium") != std::string::npos)
                {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 200); // Magenta for stadium
                }
                else if (zoneName.find("Work") != std::string::npos)
                {
                    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 200); // Dark gray for work
                }
                else if (zoneName.find("Bureau") != std::string::npos)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 100, 200, 200); // Dark blue for bureau
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 200); // Gray for other zones
                }

                SDL_RenderFillRect(renderer, &labelRect);

                // For homes, also draw the NPC ID number
                if (zoneName.find("Home") != std::string::npos) {
                    // Draw a small number to indicate which NPC lives here
                    // This is a simple visual indicator - for full text you'd need font rendering
                    SDL_FRect numberRect = {
                        static_cast<float>(zone.x * cellSize + zone.width * cellSize - cellSize/4),
                        static_cast<float>(zone.y * cellSize + 2),
                        static_cast<float>(cellSize / 6),
                        static_cast<float>(cellSize / 6)};
                    
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black dot for number
                    SDL_RenderFillRect(renderer, &numberRect);
                }
            }
        }

        // Edit mode features (same as before)
        if (editMode)
        {
            // Show mouse hover highlight
            float mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            int hoverGridX = static_cast<int>(mouseX / cellSize);
            int hoverGridY = static_cast<int>(mouseY / cellSize);

            if (hoverGridX >= 0 && hoverGridX < grid.getWidth() &&
                hoverGridY >= 0 && hoverGridY < grid.getHeight())
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128); // Yellow highlight
                SDL_FRect hoverRect = {
                    static_cast<float>(hoverGridX * cellSize),
                    static_cast<float>(hoverGridY * cellSize),
                    static_cast<float>(cellSize),
                    static_cast<float>(cellSize)};
                SDL_RenderFillRect(renderer, &hoverRect);
            }
        }
    }

    /**
     * Enhanced version that can render zone labels with NPC home assignments
     */
    void renderWithLabels(SDL_Renderer *renderer, const Grid &grid, int cellSize, TTF_Font *font, bool editMode = false, bool showZones = true, HomeManager* homeManager = nullptr)
    {
        // First render everything as normal
        render(renderer, grid, cellSize, editMode, showZones);

        // Then add text labels if font is provided and zones are visible
        if (font && showZones)
        {
            SDL_Color textColor = {255, 255, 255, 255};   // White text
            SDL_Color bgColor = {0, 0, 0, 180};           // Semi-transparent black background

            for (const auto &[zoneName, zone] : grid.zones)
            {
                std::string displayText = zoneName;
                
                // For homes, add the NPC name if assigned
                if (homeManager && zoneName.find("Home") != std::string::npos) {
                    int residentId = homeManager->getHomeResident(zoneName);
                    if (residentId != -1) {
                        displayText += "\n(NPC " + std::to_string(residentId) + ")";
                    } else {
                        displayText += "\n(Vacant)";
                    }
                }

                // Create text surface
                SDL_Surface *textSurface = TTF_RenderText_Solid(font, displayText.c_str(), displayText.length(), textColor);
                if (textSurface)
                {
                    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    if (textTexture)
                    {
                        // Position label in center of zone
                        float labelX = zone.x * cellSize + (zone.width * cellSize - textSurface->w) / 2.0f;
                        float labelY = zone.y * cellSize + (zone.height * cellSize - textSurface->h) / 2.0f;

                        // Draw background rectangle
                        SDL_FRect bgRect = {
                            labelX - 4,
                            labelY - 2,
                            static_cast<float>(textSurface->w + 8),
                            static_cast<float>(textSurface->h + 4)};

                        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
                        SDL_RenderFillRect(renderer, &bgRect);

                        // Draw text
                        SDL_FRect textRect = {
                            labelX,
                            labelY,
                            static_cast<float>(textSurface->w),
                            static_cast<float>(textSurface->h)};

                        SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);

                        SDL_DestroyTexture(textTexture);
                    }
                    SDL_DestroySurface(textSurface);
                }
            }
        }
    }
}
