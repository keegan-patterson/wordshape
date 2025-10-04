#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <SFML/Graphics.hpp>
#include "phys_engine.h"

class GameEngine
{
public:
struct debug_options
{
    bool draw_aabbs = false;
    bool draw_velocity_vectors = false;
};

private:
    std::vector<PhysItem*> entities; // Comprehensive list of game objects with physics
    debug_options debug_settings; // Debug settings for rendering

public:
    GameEngine() = default;

    // Initialize methods
    void init(sf::RenderWindow& window, GameEngine::debug_options debug_settings);
    void buildBoundaries();

    // Handle events in the game loop
    void handleEvents(sf::RenderWindow& window);

    void tick(PhysEngine* phys_engine, sf::Clock* clock);

    void draw(sf::RenderWindow& window);
};

#endif // GAME_ENGINE_H