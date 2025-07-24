#pragma once

#include "IComponentManager.h"
#include "./components/InfoBoxComponent.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

// Forward declaration to break circular dependency
class PositionManager;

class InfoBoxManager : public IComponentManager {
public:
    std::unordered_map<unsigned int, std::unique_ptr<InfoBoxComponent>> infoBoxes;

    InfoBoxComponent* create(unsigned int uid, const std::string& initialText = "");
    InfoBoxComponent* get(unsigned int uid);
    void renderAll(SDL_Renderer* renderer, TTF_Font* font, const PositionManager& posManager);
    void dump() override;
};


