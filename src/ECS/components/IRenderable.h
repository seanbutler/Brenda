#pragma once
#include <SDL3/SDL.h>


// Note: This interface is designed to be implemented by any class that needs to be rendered.
// It provides a pure virtual function `render` that takes an SDL_Renderer pointer.
// Classes implementing this interface should define their own rendering logic in the `render` method.
// This allows for polymorphic behavior when rendering different types of objects in the ECS system.


class IRenderable {
public:
    IRenderable(SDL_FRect r)    
        : rect(r)   
    {        
    }
    virtual ~IRenderable() = default;
    virtual void render(SDL_Renderer* ren) const = 0;

    SDL_FRect rect;     // all shapes can be represented by a rectangle

};

