#pragma once
#include <SDL3/SDL.h>
#include "../IComponent.h"
#include "./IRenderable.h"

class ShapeRenderableComponent : public ECS::IComponent, public IRenderable {

public:
    SDL_Color color;    // ALL shapes do have a color in our world

    ShapeRenderableComponent(SDL_FRect r, SDL_Color c )
        : IRenderable(r)
        , color(c)
    {

    }

};