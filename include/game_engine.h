#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <SFML/Graphics.hpp>
#include "phys_engine.h"

class GameEngine
{
private:
    std::vector<PhysItem*> entities; // Comprehensive list of game objects with physics

public:
    GameEngine() = default;

    // Initialize the game engine with a window
    void init(sf::RenderWindow& window) {
        window.setFramerateLimit(144);
        window.setVerticalSyncEnabled(true);

        entities.clear(); // Clear any existing entities
        PhysItem* item1 = new PhysItem(PhysItem::Type::TextBlock);
        item1->starting_position = {100, 100};
        item1->starting_velocity = {10, 0};
        item1->base_aabb.min = {-10, -10};
        item1->base_aabb.max = {10, 10};
        PhysItem* item2 = new PhysItem(PhysItem::Type::TextBlock);
        item2->starting_position = {150, 100};
        item2->starting_velocity = {-10, 0};
        item2->base_aabb.min = {-10, -10};
        item2->base_aabb.max = {10, 10};

        entities.push_back(item1); // Add the item to the list of entities
        entities.push_back(item2); // Add the item to the list of entities
    }

    // Handle events in the game loop
    void handleEvents(sf::RenderWindow& window) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (event->is<sf::Event::TextEntered>()) {
                const sf::Event::TextEntered* t_event = event->getIf<sf::Event::TextEntered>();
                int i = 0;
                for(auto entity : entities) {
                    if (entity->item_type == PhysItem::Type::TextBlock) {
                        entity->text->setString(std::string(1, static_cast<char>(t_event->unicode + i)));
                        entity->text->setCharacterSize(100);
                        entity->text->setFillColor(sf::Color::White);
                        entity->text->setPosition({0, 0});
                        entity->position = entity->starting_position; // Reset position to starting position
                        entity->velocity = entity->starting_velocity; // Reset velocity to starting velocity
                        entity->base_aabb.is_set = true; // Mark AABB as set
                        i++;
                    }
                }
            }
        }
    }

    void tick(PhysEngine* phys_engine, sf::Clock* clock) {
        // This function can be used to update the game state
        auto deltaTime = clock->restart();
        for (auto item : entities) {
            for(auto other_item : entities) {
                if (item != other_item && phys_engine->AABBvsAABB(item->getAABB(), other_item->getAABB()) && item->base_aabb.is_set && other_item->base_aabb.is_set) {
                    phys_engine->ResolveCollision(item, other_item); // Resolve collision between items
                }
            }
            phys_engine->applyGravity(item, deltaTime); // Apply gravity to each item
            item->text->setPosition(item->position); // Update text position to match item position
        }
    }

    void draw(sf::RenderWindow& window) {
        // Draw the item and text to the window
        window.clear();
        for(auto entity : entities) {
            if (entity->item_type == PhysItem::Type::TextBlock && entity->text) {
                window.draw(*entity->text); // Draw the text representation of the item
            }
        }
        window.display();
    }
};

#endif // GAME_ENGINE_H