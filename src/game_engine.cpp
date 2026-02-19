#include "game_engine.h"
#include <SFML/Graphics.hpp>

void GameEngine::buildBoundaries()
{
    // Floor
    PhysItem *boundary1 = new PhysItem(PhysItem::Type::Obstacle);
    boundary1->position = {10, 1000};
    boundary1->velocity = {0, 0};
    boundary1->setAABB({0, 0}, {940, 10});
    boundary1->mass = 1000000.0f;
    boundary1->restitution = 0.5f;
    boundary1->base_aabb.is_set = true;
    boundary1->definePolygonRectangle({{0, 0}, {940, 0}, {940, 10}, {0, 10}});

    // Left wall
    PhysItem *boundary2 = new PhysItem(PhysItem::Type::Obstacle);
    boundary2->position = {10, 11};
    boundary2->velocity = {0, 0};
    boundary2->setAABB({0, 0}, {10, 940});
    boundary2->mass = 1000000.0f;
    boundary2->restitution = 0.5f;
    boundary2->base_aabb.is_set = true;
    boundary2->definePolygonRectangle({{0, 0}, {10, 0}, {10, 940}, {0, 940}});

    // Right wall
    PhysItem *boundary3 = new PhysItem(PhysItem::Type::Obstacle);
    boundary3->position = {940, 11};
    boundary3->velocity = {0, 0};
    boundary3->setAABB({0, 0}, {10, 940});
    boundary3->mass = 1000000.0f;
    boundary3->restitution = 0.5f;
    boundary3->base_aabb.is_set = true;
    boundary3->definePolygonRectangle({{0, 0}, {10, 0}, {10, 940}, {0, 940}});

    // Ceiling
    PhysItem *boundary4 = new PhysItem(PhysItem::Type::Obstacle);
    boundary4->position = {10, 0};
    boundary4->velocity = {0, 0};
    boundary4->setAABB({0, 0}, {940, 10});
    boundary4->mass = 1000000.0f;
    boundary4->restitution = 0.5f;
    boundary4->base_aabb.is_set = true;
    boundary4->definePolygonRectangle({{0, 0}, {940, 0}, {940, 10}, {0, 10}});

    this->entities.push_back(boundary1);
    this->entities.push_back(boundary2);
    this->entities.push_back(boundary3);
    this->entities.push_back(boundary4);
}

void GameEngine::init(sf::RenderWindow &window, GameEngine::debug_options debug_settings)
{
    window.setFramerateLimit(144);
    window.setVerticalSyncEnabled(true);

    this->debug_settings = debug_settings; // Set the debug mode based on the parameter

    this->entities.clear(); // Clear any existing entities

    buildBoundaries();

    PhysItem *item1 = new PhysItem(PhysItem::Type::TextBlock);
    item1->starting_position = {50, 100};
    item1->starting_velocity = {200, -30};
    item1->setAABB({0, 0}, {100, 100});

    PhysItem *item2 = new PhysItem(PhysItem::Type::TextBlock);
    item2->starting_position = {300, 100};
    item2->starting_velocity = {-20, 0};
    item2->setAABB({0, 0}, {100, 100});

    this->entities.push_back(item1); // Add the item to the list of entities
    this->entities.push_back(item2); // Add the item to the list of entities
}

void GameEngine::handleEvents(sf::RenderWindow &window)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        if (event->is<sf::Event::TextEntered>())
        {
            const sf::Event::TextEntered *t_event = event->getIf<sf::Event::TextEntered>();
            int i = 0;
            for (auto entity : entities)
            {
                if (entity->item_type == PhysItem::Type::TextBlock)
                {
                    entity->text->setString(std::string(1, static_cast<char>(t_event->unicode + i)));
                    entity->text->setCharacterSize(100);
                    entity->text->setFillColor(sf::Color::White);
                    entity->text->setPosition({0, 0});
                    entity->position = entity->starting_position; // Reset position to starting position
                    entity->velocity = entity->starting_velocity; // Reset velocity to starting velocity
                    entity->base_aabb.is_set = true;              // Mark AABB as set
                    i++;
                }
            }
        }
    }
}

void GameEngine::tick(PhysEngine *phys_engine, sf::Clock *clock)
{
    auto deltaTime = clock->restart();

    for (size_t i = 0; i < entities.size(); i++)
    {
        for (size_t j = i + 1; j < entities.size(); j++)
        {
            PhysItem *item = entities[i];
            PhysItem *other_item = entities[j];

            if (phys_engine->AABBvsAABB(item->getAABB(), other_item->getAABB()))
            {
                if (item->base_aabb.is_set && other_item->base_aabb.is_set)
                {
                    phys_engine->ResolveCollision(item, other_item);
                }
            }
        }

        if (entities[i]->item_type == PhysItem::Type::TextBlock)
        {
            phys_engine->applyGravity(entities[i], deltaTime);
        }
    }
}

void GameEngine::draw(sf::RenderWindow &window)
{
    // Draw the item and text to the window
    window.clear();
    for (auto entity : entities)
    {
        if (entity->text)
        {
            entity->text->setPosition(entity->position); // Update text position to match item position
            window.draw(*entity->text);                  // Draw the text representation of the item
        }

        entity->polygon.setPosition(entity->position);
        window.draw(entity->polygon); // Draw the polygon representation of the item

        if (debug_settings.draw_aabbs)
        {
            entity->rectangle.setPosition(entity->position);
            window.draw(entity->rectangle); // Draw the rectangle representation of the item
        }
        if (debug_settings.draw_velocity_vectors)
        {
            sf::VertexArray line(sf::PrimitiveType::Lines, 2);
            // Set the first point (start of the line)
            line[0].position = entity->position;
            line[0].color = sf::Color::Red;

            // Set the second point (end of the line)
            line[1].position = entity->position + entity->velocity;
            line[1].color = sf::Color::Red;

            window.draw(line);
        }
    }
    window.display();
}