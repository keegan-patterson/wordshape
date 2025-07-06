#include <SFML/Graphics.hpp>
#include "phys_engine.h"
#include "game_engine.h"

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");

    GameEngine game_engine;
    game_engine.init(window);

    sf::Font font("C:/Windows/Fonts/arial.ttf");
    sf::Text text = sf::Text(font);

    while (window.isOpen())
    {
        game_engine.handleEvents(window, text);

        // Business Logic
        // Here you can add any game logic or updates needed before rendering
        auto xy = text.getPosition();
        xy.y += 1;
        text.setPosition(xy);

        // Clear and Display
        window.clear();
        window.draw(text);
        window.display();
    }
}