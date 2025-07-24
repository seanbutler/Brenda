#include "InfoBoxManager.h"
#include "PositionManager.h"
#include <iostream>
#include <sstream>
#include <vector>

InfoBoxComponent* InfoBoxManager::create(unsigned int uid, const std::string& initialText) {
    auto infoBox = std::make_unique<InfoBoxComponent>();
    infoBox->text = initialText;
    InfoBoxComponent* ptr = infoBox.get();
    infoBoxes[uid] = std::move(infoBox);
    return ptr;
}

InfoBoxComponent* InfoBoxManager::get(unsigned int uid) {
    auto it = infoBoxes.find(uid);
    return (it != infoBoxes.end()) ? it->second.get() : nullptr;
}

void InfoBoxManager::renderAll(SDL_Renderer* renderer, TTF_Font* font, const PositionManager& posManager) {
    SDL_Color textColor = { 0, 0, 0, 255 };             // Black
    SDL_Color boxColor = { 255, 255, 255, 127 };            // White with transparency

    for (const auto& pair : infoBoxes) {
        auto uid = pair.first;
        const auto& infoBox = pair.second;
        auto pos = posManager.get(uid);

        if (infoBox && pos) {
            // Split text into lines
            std::vector<std::string> lines;
            std::stringstream ss(infoBox->text);
            std::string line;
            while (std::getline(ss, line, '\n')) {
                lines.push_back(line);
            }

            if (lines.empty()) continue;

            // Calculate total dimensions needed
            std::vector<SDL_Surface*> lineSurfaces;
            int maxWidth = 0;
            int totalHeight = 0;
            int lineHeight = 0;

            for (const auto& lineText : lines) {
                SDL_Surface* lineSurface = TTF_RenderText_Solid(font, lineText.c_str(), lineText.length(), textColor);
                if (lineSurface) {
                    lineSurfaces.push_back(lineSurface);
                    maxWidth = (lineSurface->w > maxWidth) ? lineSurface->w : maxWidth;
                    totalHeight += lineSurface->h;
                    if (lineHeight == 0) lineHeight = lineSurface->h;
                }
            }

            if (lineSurfaces.empty()) continue;

            // Create a background box with padding
            SDL_FRect boxRect;
            boxRect.w = maxWidth + 8; // 4px padding on each side
            boxRect.h = totalHeight + 4; // 2px padding on top/bottom
            boxRect.x = pos->x - (boxRect.w / 2); // Center the box
            boxRect.y = pos->y - 32 - boxRect.h;  // Position above the entity

            // Draw the background box
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
            SDL_RenderFillRect(renderer, &boxRect);

            // Draw each line of text
            float currentY = boxRect.y + 2;
            for (SDL_Surface* lineSurface : lineSurfaces) {
                SDL_Texture* lineTexture = SDL_CreateTextureFromSurface(renderer, lineSurface);
                if (lineTexture) {
                    SDL_FRect textRect;
                    textRect.w = lineSurface->w;
                    textRect.h = lineSurface->h;
                    textRect.x = boxRect.x + 4; // 4px padding from left
                    textRect.y = currentY;

                    SDL_RenderTexture(renderer, lineTexture, nullptr, &textRect);
                    SDL_DestroyTexture(lineTexture);
                }

                currentY += lineSurface->h;
                SDL_DestroySurface(lineSurface);
            }
        }
    }
}

// implementation for the virtual dump() function.
void InfoBoxManager::dump() {
    std::cout << "--- InfoBoxManager Dump ---\n";
    if (infoBoxes.empty()) {
        std::cout << "No info boxes to display.\n";
        return;
    }

    for (const auto& pair : infoBoxes) {
        std::cout << "Entity UID: " << pair.first 
                  << " | Text: \"" << pair.second->text << "\"\n";
    }
    std::cout << "---------------------------\n";
}