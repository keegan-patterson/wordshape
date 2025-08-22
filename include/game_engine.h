#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <SFML/Graphics.hpp>
#include "phys_engine.h"

class GameEngine
{
private:
    std::vector<PhysItem*> entities; // Comprehensive list of game objects with physics
    bool debug_mode = false;

public:
    GameEngine() = default;

    // Initialize methods
    void init(sf::RenderWindow& window, bool debug_mode = false);
    void buildBoundaries();

    // Handle events in the game loop
    void handleEvents(sf::RenderWindow& window);

    void tick(PhysEngine* phys_engine, sf::Clock* clock);

    void draw(sf::RenderWindow& window);
};

#endif // GAME_ENGINE_H