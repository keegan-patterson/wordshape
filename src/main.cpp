#include <SFML/Graphics.hpp>
#include "phys_engine.h"
#include "game_engine.h"

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");

    GameEngine game_engine;
    game_engine.init(window);

    sf::Font font("C:/Windows/Fonts/arial.ttf");
    sf::Text *text = new sf::Text(font);

    sf::Clock clock;
    PhysEngine phys_engine({0, 9.81f}); // Initialize physics engine with gravity
    PhysItem *item = new PhysItem; // Create a physics item

    while (window.isOpen())
    {
        game_engine.handleEvents(window, *item, *text);

        // Business Logic
        // Here you can add any game logic or updates needed before rendering
        phys_engine.applyGravity(*item, clock.restart()); // Apply gravity to the item
        text->setPosition(item->position); // Update text position to match item position

        // Clear and Display
        window.clear();
        window.draw(*text); // Draw the item text
        window.display();
    }
}