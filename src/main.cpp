#include <SFML/Graphics.hpp>
#include "phys_engine.h"
#include "game_engine.h"

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");

    GameEngine game_engine;
    GameEngine::debug_options debug_options;
    debug_options.draw_aabbs = true;
    debug_options.draw_velocity_vectors = true;
    game_engine.init(window, debug_options);

    sf::Clock *clock = new sf::Clock(); // Create a clock to track time
    PhysEngine *phys_engine = new PhysEngine({0, 100.0f}); // Initialize physics engine with gravity

    while (window.isOpen())
    {
        // handle input events
        game_engine.handleEvents(window);

        // game logic
        // eventually this will be passed an array of game objects
        game_engine.tick(phys_engine, clock);

        // Clear and Display
        game_engine.draw(window);
    }
}