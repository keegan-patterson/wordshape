#include <SFML/Graphics.hpp>
#include "phys_engine.h"
#include "game_engine.h"

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");

    GameEngine game_engine;
    game_engine.init(window);

    sf::Clock *clock = new sf::Clock(); // Create a clock to track time
    PhysEngine *phys_engine = new PhysEngine({0, 9.81f}); // Initialize physics engine with gravity

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