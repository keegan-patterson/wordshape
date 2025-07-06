#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <SFML/Graphics.hpp>
#include "phys_engine.h"

class GameEngine
{
public:
    GameEngine() = default;

    // Initialize the game engine with a window
    void init(sf::RenderWindow& window) {
        window.setFramerateLimit(144);
        window.setVerticalSyncEnabled(true);
    }

    // Handle events in the game loop
    void handleEvents(sf::RenderWindow& window, PhysItem& item, sf::Text& text) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (event->is<sf::Event::TextEntered>()) {
                const sf::Event::TextEntered* t_event = event->getIf<sf::Event::TextEntered>();
                text.setString(std::string(1, static_cast<char>(t_event->unicode)));
                text.setCharacterSize(100);
                text.setFillColor(sf::Color::White);
                text.setPosition({0, 0});
                item.position = {100,100};
                item.velocity = {0, 0};
            }
        }
    }
};

#endif // GAME_ENGINE_H