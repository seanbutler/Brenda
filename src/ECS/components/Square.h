#pragma once
#include "ShapeRenderableComponent.h"

class Square : public ShapeRenderableComponent {
public:

    Square(float x, float y, float size, SDL_Color c)
    : ShapeRenderableComponent({x, y, size, size}, c)
    {
    }

    void render(SDL_Renderer* ren) const override {
        SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(ren, &rect);
    }
};