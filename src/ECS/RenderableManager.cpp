#include "RenderableManager.h"
#include "PositionManager.h"
#include <iostream>

void RenderableManager::renderAll(SDL_Renderer* ren, PositionManager& positionManager) const {
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

void RenderableManager::dump() {
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
