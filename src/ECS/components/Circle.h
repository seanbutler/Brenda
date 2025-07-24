#pragma once
#include "ShapeRenderableComponent.h"

class Circle : public ShapeRenderableComponent
{
public:
    float radius;

    Circle(float x, float y, float r, SDL_Color c)
        : ShapeRenderableComponent({x, y, r * 2, r * 2}, c), radius(r)
    {
    }

    void render(SDL_Renderer *renderer) const
    {
        int centerX = static_cast<int>(rect.x + rect.w / 2);
        int centerY = static_cast<int>(rect.y + rect.h / 2);
        int radius = static_cast<int>(rect.w / 2);

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

        // Draw filled circle
        for (int w = 0; w < rect.w; ++w)
        {
            for (int h = 0; h < rect.h; ++h)
            {
                int dx = w - radius;
                int dy = h - radius;
                if ((dx * dx + dy * dy) <= (radius * radius))
                {
                    SDL_RenderPoint(renderer, centerX + dx, centerY + dy);
                }
            }
        }
    }
};