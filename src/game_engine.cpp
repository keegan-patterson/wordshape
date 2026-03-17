#include "game_engine.h"
#include <SFML/Graphics.hpp>

void GameEngine::buildBoundaries()
{
    // Floor — position is the COM (center) of the rectangle
    PhysItem *boundary1 = new PhysItem(PhysItem::Type::Obstacle);
    boundary1->position = {480, 1005};
    boundary1->velocity = {0, 0};
    boundary1->setAABB({-470, -5}, {470, 5});
    boundary1->mass = 1000000.0f;
    boundary1->restitution = 0.5f;
    boundary1->base_aabb.is_set = true;
    boundary1->inverseInertia = 0.0f; // Static body — never rotates
    boundary1->definePolygonRectangle({940, 10});

    // Left wall
    PhysItem *boundary2 = new PhysItem(PhysItem::Type::Obstacle);
    boundary2->position = {15, 481};
    boundary2->velocity = {0, 0};
    boundary2->setAABB({-5, -470}, {5, 470});
    boundary2->mass = 1000000.0f;
    boundary2->restitution = 0.5f;
    boundary2->base_aabb.is_set = true;
    boundary2->inverseInertia = 0.0f; // Static body — never rotates
    boundary2->definePolygonRectangle({10, 940});

    // Right wall
    PhysItem *boundary3 = new PhysItem(PhysItem::Type::Obstacle);
    boundary3->position = {945, 481};
    boundary3->velocity = {0, 0};
    boundary3->setAABB({-5, -470}, {5, 470});
    boundary3->mass = 1000000.0f;
    boundary3->restitution = 0.5f;
    boundary3->base_aabb.is_set = true;
    boundary3->inverseInertia = 0.0f; // Static body — never rotates
    boundary3->definePolygonRectangle({10, 940});

    // Ceiling
    PhysItem *boundary4 = new PhysItem(PhysItem::Type::Obstacle);
    boundary4->position = {480, 5};
    boundary4->velocity = {0, 0};
    boundary4->setAABB({-470, -5}, {470, 5});
    boundary4->mass = 1000000.0f;
    boundary4->restitution = 0.5f;
    boundary4->base_aabb.is_set = true;
    boundary4->inverseInertia = 0.0f; // Static body — never rotates
    boundary4->definePolygonRectangle({940, 10});

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
    item1->starting_position = {100, 143}; // World-space COM (vertices are centered around origin)
    item1->starting_velocity = {200, -30};
    item1->starting_angular_velocity = 2.0f; // ~115 deg/sec clockwise
    item1->setAABB({-50, -43}, {50, 43});
    item1->base_aabb.is_set = true; // Mark AABB as set so collision resolution fires
    // computeInertia() was already called in the TextBlock constructor

    PhysItem *item2 = new PhysItem(PhysItem::Type::TextBlock);
    item2->starting_position = {350, 143}; // World-space COM (vertices are centered around origin)
    item2->starting_velocity = {-20, 0};
    item2->starting_angular_velocity = -1.5f; // ~86 deg/sec counter-clockwise
    item2->setAABB({-50, -43}, {50, 43});
    item2->base_aabb.is_set = true; // Mark AABB as set so collision resolution fires
    // computeInertia() was already called in the TextBlock constructor

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
                    entity->orientation = entity->starting_orientation;         // Reset orientation
                    entity->angularVelocity = entity->starting_angular_velocity; // Reset spin
                    entity->base_aabb.is_set = true;              // Mark AABB as set
                    i++;
                }
            }
        } if (event->is<sf::Event::MouseButtonPressed>())
        {
            const sf::Event::MouseButtonPressed *m_event = event->getIf<sf::Event::MouseButtonPressed>();
            if (m_event->button == sf::Mouse::Button::Left)
            {
                PhysItem *new_item = new PhysItem(PhysItem::Type::TextBlock);
                new_item->position = sf::Vector2f(m_event->position.x, m_event->position.y);
                new_item->velocity = {0, 0};
                new_item->setAABB({0, 0}, {100, 100});
                new_item->base_aabb.is_set = true;              // Mark AABB as set
                // computeInertia() was already called in the TextBlock constructor
                this->entities.push_back(new_item);
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

        entity->polygon.setPosition(entity->position); // position is world COM; polygon vertices are centered
        entity->polygon.setRotation(sf::radians(entity->orientation));
        window.draw(entity->polygon);

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