#pragma once

#include <unordered_map>
#include <memory>
#include "./IComponentManager.h"
#include "./PositionManager.h"
#include "./components/IRenderable.h"
#include "./components/Square.h"
#include "./components/Circle.h"
#include "Entity.h"


class RenderableManager : public IComponentManager {
public:
    // Map from Entity UID to IRenderable component
    std::unordered_map<unsigned int, std::unique_ptr<IRenderable>> renderables;

    // Factory method for quads
    Square* createQuad(unsigned int entityUID, float x, float y, SDL_Color color) {
        auto square = std::make_unique<Square>(x, y, 32, color);
        Square* ptr = square.get();
        renderables[entityUID] = std::move(square);
        return ptr;
    }

    // Factory method for circles
    Circle* createCircle(unsigned int entityUID, float x, float y, SDL_Color color) {
        auto circle = std::make_unique<Circle>(x, y, 16, color);
        Circle* ptr = circle.get();
        renderables[entityUID] = std::move(circle);
        return ptr;
    }

    void renderAll(SDL_Renderer* ren, PositionManager& positionManager) const {
        for (const auto& [uid, r] : renderables) {
            if (r) {
                auto pos = positionManager.get(uid);
                if (pos) {
                    r->rect.x = pos->x - (r->rect.w / 2);
                    r->rect.y = pos->y - (r->rect.h / 2);
                }
                r->render(ren);
            }
        }
    }

    void dump() override {
        std::cout << "RenderableManager Dump:\n";
        for (const auto& pair : renderables) {
            const IRenderable* renderable = pair.second.get();
            if (renderable) {
                std::cout << "Entity UID: " << pair.first 
                          << " Renderable Rect: (" 
                          << renderable->rect.x << ", " 
                          << renderable->rect.y << ", "
                          << renderable->rect.w << ", "
                          << renderable->rect.h << ")\n";
            }
        }
    }   

    // void update() override {
    //     // Update logic for renderables if needed
    // }
};