#include "game_engine.h"

void GameEngine::buildBoundaries()
{
    PhysItem *boundary1 = new PhysItem(PhysItem::Type::Obstacle);
    boundary1->position = {0, 1000};
    boundary1->velocity = {0, 0};
    boundary1->setAABB({-960, -5}, {960, 5});
    boundary1->mass = 1000000.0f;       
    boundary1->restitution = 0.5f;      
    boundary1->base_aabb.is_set = true; 

    PhysItem *boundary2 = new PhysItem(PhysItem::Type::Obstacle);
    boundary2->position = {10, 0}; 
    boundary2->velocity = {0, 0};
    boundary2->setAABB({-5, -540}, {5, 540});
    boundary2->mass = 1000000.0f;       
    boundary2->restitution = 0.5f;      
    boundary2->base_aabb.is_set = true; 

    this->entities.push_back(boundary1); 
    this->entities.push_back(boundary2); 
    // I am a comment, delete me!
}

void GameEngine::init(sf::RenderWindow &window, bool debug_mode)
{
    window.setFramerateLimit(144);
    window.setVerticalSyncEnabled(true);

    this->debug_mode = debug_mode; // Set the debug mode based on the parameter

    this->entities.clear(); // Clear any existing entities

    buildBoundaries();

    PhysItem *item1 = new PhysItem(PhysItem::Type::TextBlock);
    item1->starting_position = {0, 100};
    item1->starting_velocity = {10, 0};
    item1->setAABB({-50, 0}, {50, 100});

    PhysItem *item2 = new PhysItem(PhysItem::Type::TextBlock);
    item2->starting_position = {300, 100};
    item2->starting_velocity = {-10, 0};
    item2->setAABB({-50, 0}, {50, 100});

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
    // This function can be used to update the game state
    auto deltaTime = clock->restart();
    for (auto item : entities)
    {
        for (auto other_item : entities)
        {
            if (item != other_item && phys_engine->AABBvsAABB(item->getAABB(), other_item->getAABB()))
            {
                // std::cout << "Checking collision between item at position: " << other_item->position.x << ", " << other_item->position.y << std::endl;
                if (item->base_aabb.is_set && other_item->base_aabb.is_set)
                {
                    std::cout << "Collision detected between items at positions: "
                              << item->position.x << ", " << item->position.y << " and "
                              << other_item->position.x << ", " << other_item->position.y << std::endl;
                    std::optional<sf::Vector2f> mtv = phys_engine->SeparatingAxisTheorem(*item, *other_item);
                    if (mtv.has_value())
                    {
                        // Move the items apart based on the minimum translation vector
                        if(item->item_type != PhysItem::Type::Obstacle){
                            item->velocity += *mtv;
                        }
                        if(other_item->item_type != PhysItem::Type::Obstacle){
                            other_item->velocity -= *mtv;
                        }
                    }
                    //phys_engine->ResolveCollision(item, other_item); // Resolve collision between items
                }
            }
        }
        if (item->item_type == PhysItem::Type::TextBlock)
        {
            phys_engine->applyGravity(item, deltaTime); // Apply gravity to each item
        }
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
            entity->text->setPosition(entity->position); // Update text position to match item position
            window.draw(*entity->text);                  // Draw the text representation of the item
            entity->polygon.setPosition(entity->position);
            window.draw(entity->polygon); // Draw the polygon representation of the item
        }
        if (debug_mode || entity->item_type == PhysItem::Type::Obstacle)
        {
            entity->rectangle.setPosition(entity->position);
            window.draw(entity->rectangle); // Draw the rectangle representation of the obstacle
        }
    }
    window.display();
}