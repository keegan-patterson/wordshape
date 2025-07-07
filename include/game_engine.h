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

    void tick(PhysEngine* phys_engine, PhysItem* item, sf::Text* text, sf::Clock* clock) {
        // This function can be used to update the game state
        phys_engine->applyGravity(item, clock->restart()); // Apply gravity to the item
        text->setPosition(item->position); // Update text position to match item position
    }

    void draw(sf::RenderWindow& window, sf::Text& text) {
        // Draw the item and text to the window
        window.clear();
        window.draw(text); // Draw the item text
        window.display();
    }
};

#endif // GAME_ENGINE_H