#include "game_engine.h"

void GameEngine::init(sf::RenderWindow &window)
{
    window.setFramerateLimit(144);
    window.setVerticalSyncEnabled(true);

    entities.clear(); // Clear any existing entities
    PhysItem *item1 = new PhysItem(PhysItem::Type::TextBlock);
    item1->starting_position = {100, 100};
    item1->starting_velocity = {10, 0};
    item1->base_aabb.min = {-10, -10};
    item1->base_aabb.max = {10, 10};
    PhysItem *item2 = new PhysItem(PhysItem::Type::TextBlock);
    item2->starting_position = {150, 100};
    item2->starting_velocity = {-10, 0};
    item2->base_aabb.min = {-10, -10};
    item2->base_aabb.max = {10, 10};
    PhysItem *item3 = new PhysItem(PhysItem::Type::Obstacle);
    item3->starting_position = {0, 1000};
    item3->position = item3->starting_position; // Set position to starting position
    item3->starting_velocity = {0, 0};
    item3->velocity = item3->starting_velocity; // Set velocity to starting velocity
    item3->base_aabb.min = {-960, -5};
    item3->base_aabb.max = {960, 5};
    item3->mass = 1000000.0f;       // Set a large mass for the obstacle to make it immovable
    item3->restitution = 0.5f;      // Set restitution to 0
    item3->base_aabb.is_set = true; // Mark AABB as set

    entities.push_back(item1); // Add the item to the list of entities
    entities.push_back(item2); // Add the item to the list of entities
    entities.push_back(item3); // Add the obstacle to the list of entities
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
    // This function can be used to update the game state
    auto deltaTime = clock->restart();
    for (auto item : entities)
    {
        for (auto other_item : entities)
        {
            if (item != other_item && phys_engine->AABBvsAABB(item->getAABB(), other_item->getAABB()) && item->base_aabb.is_set && other_item->base_aabb.is_set)
            {
                phys_engine->ResolveCollision(item, other_item); // Resolve collision between items
            }
        }
        if (item->item_type == PhysItem::Type::TextBlock)
        {
            phys_engine->applyGravity(item, deltaTime); // Apply gravity to each item
        }
        item->text->setPosition(item->position); // Update text position to match item position
    }
}

void GameEngine::draw(sf::RenderWindow &window)
{
    // Draw the item and text to the window
    window.clear();
    for (auto entity : entities)
    {
        if (entity->item_type == PhysItem::Type::TextBlock && entity->text)
        {
            window.draw(*entity->text); // Draw the text representation of the item
        }
        if (entity->item_type == PhysItem::Type::Obstacle)
        {
            entity->rectangle.setPosition(entity->position);
            entity->rectangle.setSize(sf::Vector2f(entity->base_aabb.max.x - entity->base_aabb.min.x, entity->base_aabb.max.y - entity->base_aabb.min.y));
            entity->rectangle.setFillColor(sf::Color::Red);
            entity->rectangle.setOutlineColor(sf::Color::Red);
            entity->rectangle.setOutlineThickness(5);
            window.draw(entity->rectangle); // Draw the rectangle representation of the obstacle
        }
    }
    window.display();
}