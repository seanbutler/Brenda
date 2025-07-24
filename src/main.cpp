#include "./ECS/Entity.h"
#include "Scene.h"
#include "./ECS/RenderableManager.h"
#include "GridRenderer.h"
#include "Grid.h"
#include "GeminiClient.h"
#include "Pathfinder.h"
#include "AISystem.h"
#include "./ECS/AIManager.h"
#include "./ECS/InfoBoxManager.h"
#include "Clock.h"
#include "TickManager.h" // Add TickManager include
#include "TiledParser.h" // Add TiledParser include

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include "HomeManager.h"  // Add this include



// Helper function to load the API key
std::string loadApiKey(const std::string& configPath) {
    std::ifstream configFile(configPath);
    if (!configFile) {
        std::cerr << "Error: Could not open " << configPath << std::endl;
        return "";
    }
    nlohmann::json configJson;
    configFile >> configJson;
    return configJson.value("gemini_api_key", "");
}

// Helper function to render clock display
void renderClock(SDL_Renderer* renderer, TTF_Font* font, const Clock& clock) {
    SDL_Color textColor = { 0, 0, 0, 255 };  // Black text
    SDL_Color bgColor = { 255, 255, 255, 200 };  // White background with transparency
    
    std::string timeStr = clock.getTimeString() + " (" + clock.getTimeOfDay().getPeriodString() + ")";
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, timeStr.c_str(), timeStr.length(), textColor);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_FRect bgRect;
        bgRect.w = surface->w + 10;
        bgRect.h = surface->h + 6;
        bgRect.x = 10;
        bgRect.y = 10;
        
        SDL_FRect textRect;
        textRect.w = surface->w;
        textRect.h = surface->h;
        textRect.x = bgRect.x + 5;
        textRect.y = bgRect.y + 3;
        
        // Draw background
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(renderer, &bgRect);
        
        // Draw text
        SDL_RenderTexture(renderer, texture, nullptr, &textRect);
        SDL_DestroyTexture(texture);
    }
    
    SDL_DestroySurface(surface);
}

int main(int argc, char *argv[])
{

    // --- Load API Key and Create Gemini Client ---
    std::string apiKey = loadApiKey("config.json");
    if (apiKey.empty() || apiKey == "YOUR_GEMINI_API_KEY_HERE") {
        std::cerr << "FATAL: Gemini API key is not set in config.json" << std::endl;
        return 1;
    }
    GeminiClient gemini(apiKey);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow("SDL3 Project", 1280, 960, 0);
    if (win == nullptr)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(win, NULL);
    if (sdl_renderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << std::endl;
        return 1;
    }

    // Load a font
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
    if (!font) {
        std::cerr << "TTF_OpenFont Error: " << std::endl;
        return 1;
    }

    SDL_Event e;
    bool quit = false;

    // --- Manager Declarations ---
    PositionManager positionManager;
    RenderableManager renderManager;
    DescriptionComponentManager descriptionManager;
    HomeManager homeManager; // This should now work
    AIManager aiManager; // Add this line
    MovementManager movementManager; // Add this if not already declared
    ControllerManager controllerManager; // Add this if not already declared
    InfoBoxManager infoBoxManager; // Add this if not already declared
    
    // --- System Declarations ---
    Uint32 cellSize = 32; // Size of each grid cell in pixels
    Grid grid(20, 15);
    
    // Try to load as Tiled map first, then fall back to custom format
    bool mapLoaded = false;
    
    // Check if a Tiled map file exists
    std::ifstream tiledFile("map.json");
    if (tiledFile.good()) {
        tiledFile.close();
        std::cout << "Found map.json, attempting to load as Tiled map..." << std::endl;
        if (grid.loadFromTiled("map.json")) {
            std::cout << "Successfully loaded Tiled map!" << std::endl;
            mapLoaded = true;
        } else {
            std::cout << "Failed to load as Tiled map, falling back to environment.json" << std::endl;
        }
    }
    
    // Fall back to custom environment format
    if (!mapLoaded) {
        std::cout << "Loading custom environment.json format..." << std::endl;
        if (!grid.loadFromJson("environment.json")) {
            std::cerr << "Failed to load environment.json" << std::endl;
            return 1;
        }
        std::cout << "Successfully loaded custom format!" << std::endl;
    }
    
    // Dump the grid to console to verify it loaded correctly
    grid.dump();

    Pathfinder pathfinder(grid);
    Scene scene;
    Clock simClock(300.0f); 
    
    // Create TickManager for coordinated updates
    TickManager tickManager;
    
    // 1 real second = 300 sim seconds (fast time for testing)
    AISystem aiSystem(&aiManager, &positionManager, &movementManager, 
                      &descriptionManager, &pathfinder, &grid, &gemini, 
                      &infoBoxManager, &simClock, &homeManager, // Add homeManager parameter
                      cellSize);
    
    // Register systems with TickManager for coordinated updates
    // AI system runs at lower frequency for planning
    tickManager.registerSystem([&aiSystem](float dt) { aiSystem.updateAI(dt); }, 0.5f, 1);
    
    // Movement system runs at higher frequency for smooth movement, scaled by time
    tickManager.registerSystem([&movementManager, &positionManager, &simClock](float dt) { 
        // Calculate movement time scale: normalize to 1.0 for "normal" speed (60.0f)
        float timeScale = simClock.getTimeScale() / 60.0f;
        movementManager.update(positionManager, dt, timeScale);
    }, 0.016f, 2); // ~60 FPS for smooth movement
    
    // Clock always updates at frame rate
    tickManager.registerSystem([&simClock](float dt) { simClock.update(dt); }, 0.0f, 0);

    // --- Scene Loading ---
    std::string sceneFile = "entities.json";
    if (!scene.loadFromFile(sceneFile, &positionManager, &renderManager,
                            &descriptionManager, &controllerManager,
                            &movementManager, &aiManager, 
                            &infoBoxManager, // <-- Pass to Scene
                            cellSize)) 
    {
        std::cerr << "Failed to load " << sceneFile << std::endl;
    }

    // renderManager.dump();
    // positionManager.dump();
    // movementManager.dump();
    descriptionManager.dump();

    // Assign homes to NPCs
    int npcCount = 0;
    for (auto& [entityUID, aiState] : aiManager.states) {
        homeManager.assignHome(entityUID, "NPC_" + std::to_string(entityUID));
        npcCount++;
    }
    
    // Print home assignments
    std::cout << "\n=== Home Assignments ===" << std::endl;
    auto assignments = homeManager.getAllAssignments();
    for (const auto& assignment : assignments) {
        std::cout << assignment.npcName << " lives in " << assignment.zoneName << std::endl;
    }
    std::cout << "========================\n" << std::endl;
    
    // Force initial AI planning for all NPCs
    std::cout << "Starting initial AI planning..." << std::endl;
    for (auto& [entityUID, aiState] : aiManager.states) {
        aiState.currentState = AIStateName::Idle; // Make sure they start idle
        auto* movement = movementManager.get(entityUID);
        if (movement) {
            movement->phase = MovementPhase::IDLE;
        }
    }
    
    const int targetFPS = 30;
    const int frameDelay = 1000 / targetFPS;

    Uint32 frameStart = 0;
    Uint32 lastFrameTime = 0;
    float deltaTime = 0.0f;

    // --- Game Loop Variables ---
    bool isPaused = false;
    bool editMode = false; // Add edit mode flag
    bool showZones = true; // Add zone visibility flag (default on)
    
    while (!quit) {
        frameStart = SDL_GetTicks();
        deltaTime = (frameStart - lastFrameTime) / 1000.0f;
        lastFrameTime = frameStart;

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                quit = true;
            }
            
            // Handle mouse clicks for tile editing (only in edit mode)
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && editMode) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    // Convert mouse coordinates to grid coordinates
                    int gridX = e.button.x / cellSize;
                    int gridY = e.button.y / cellSize;
                    
                    // Cycle the tile type
                    grid.cycleTileType(gridX, gridY);
                    
                    std::cout << "Clicked tile (" << gridX << ", " << gridY << ") - cycled to next type" << std::endl;
                }
                if (e.button.button == SDL_BUTTON_RIGHT) {
                    // Save the current grid state
                    grid.saveToJson("environment.json");
                    std::cout << "Grid saved to environment.json" << std::endl;
                }
            }
            
            // Handle key presses for mode toggles FIRST
            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.scancode == SDL_SCANCODE_E) {
                    editMode = !editMode; // Toggle edit mode
                    if (editMode) {
                        std::cout << "--- Edit Mode Enabled ---\n";
                        std::cout << "Left click to cycle tile types, Right click to save\n";
                    } else {
                        std::cout << "--- Edit Mode Disabled ---\n";
                    }
                }
                if (e.key.scancode == SDL_SCANCODE_Z) {
                    showZones = !showZones; // Toggle zone visibility
                    if (showZones) {
                        std::cout << "--- Zone Visibility Enabled ---\n";
                    } else {
                        std::cout << "--- Zone Visibility Disabled ---\n";
                    }
                }
                if (e.key.scancode == SDL_SCANCODE_P) {
                    isPaused = !isPaused; // Toggle pause state
                    if (isPaused) {
                        std::cout << "--- Game Paused ---\n";
                        simClock.pause();
                    } else {
                        std::cout << "--- Game Resumed ---\n";
                        simClock.resume();
                    }
                }
                // Time speed controls
                if (e.key.scancode == SDL_SCANCODE_1) {
                    simClock.setTimeScale(60.0f);   // Normal speed
                    std::cout << "Time speed: Normal (1x) - Movement scale: 1.0x\n";
                }
                if (e.key.scancode == SDL_SCANCODE_2) {
                    simClock.setTimeScale(300.0f);  // 5x speed
                    std::cout << "Time speed: Fast (5x) - Movement scale: 5.0x\n";
                }
                if (e.key.scancode == SDL_SCANCODE_3) {
                    simClock.setTimeScale(600.0f);  // 10x speed
                    std::cout << "Time speed: Very Fast (10x) - Movement scale: 10.0x\n";
                }
                if (e.key.scancode == SDL_SCANCODE_0) {
                    simClock.setTimeScale(10.0f);   // Slow speed for debugging
                    std::cout << "Time speed: Slow (debug) - Movement scale: 0.17x\n";
                }
            }

            // Handle player input ALWAYS (not just when unpaused and not in edit mode)
            // Player should be able to move even when game is paused or in edit mode
            controllerManager.handleEvents(e, movementManager, positionManager, grid, cellSize);
        }

        // Only update game logic systems if the game is not paused
        if (!isPaused) {
            // --- COORDINATED SYSTEM UPDATES ---
            // Use TickManager for synchronized updates
            tickManager.update(deltaTime);
        }
        
        // ALWAYS update movement system (even when paused) so player can move
        // Fix: Use the correct method signature with all required parameters
        float timeScale = simClock.getTimeScale() / 60.0f; // Normalize time scale
        movementManager.update(positionManager, deltaTime, timeScale);
        
        // Process phase-specific AI logic after movement updates
        aiSystem.processArrivingEntities();
        aiSystem.processPlanningEntities();

        // --- RENDER (Always run this) ---
        SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
        SDL_RenderClear(sdl_renderer);
        GridRenderer::renderWithLabels(sdl_renderer, grid, cellSize, font, editMode, showZones, &homeManager); // Pass homeManager
        renderManager.renderAll(sdl_renderer, positionManager);
        
        // Render edit mode indicator
        if (editMode) {
            SDL_Color editColor = { 255, 0, 0, 255 };  // Red text
            SDL_Surface* surface = TTF_RenderText_Solid(font, "EDIT MODE - E to toggle, Left click to cycle tiles, Right click to save", 
                                                       strlen("EDIT MODE - E to toggle, Left click to cycle tiles, Right click to save"), editColor);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);
                if (texture) {
                    SDL_FRect textRect = { 10, 60, static_cast<float>(surface->w), static_cast<float>(surface->h) };
                    SDL_RenderTexture(sdl_renderer, texture, nullptr, &textRect);
                    SDL_DestroyTexture(texture);
                }
                SDL_DestroySurface(surface);
            }
        }
        
        // Render zone visibility indicator
        if (!showZones) {
            SDL_Color zoneColor = { 0, 255, 0, 255 };  // Green text
            SDL_Surface* surface = TTF_RenderText_Solid(font, "Zones Hidden - Z to toggle", 
                                                       strlen("Zones Hidden - Z to toggle"), zoneColor);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);
                if (texture) {
                    SDL_FRect textRect = { 10, 80, static_cast<float>(surface->w), static_cast<float>(surface->h) };
                    SDL_RenderTexture(sdl_renderer, texture, nullptr, &textRect);
                    SDL_DestroyTexture(texture);
                }
                SDL_DestroySurface(surface);
            }
        }
        
        // Render the infoboxes on top
        infoBoxManager.renderAll(sdl_renderer, font, positionManager);
        
        // Render the clock display
        renderClock(sdl_renderer, font, simClock);

        SDL_RenderPresent(sdl_renderer);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay)
        {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // Cleanup
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
