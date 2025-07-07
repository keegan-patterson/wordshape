#ifndef PHYS_ENGINE_H
#define PHYS_ENGINE_H

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

class AABB
{
public:
    AABB();
    sf::Vector2f min; // Minimum corner of the AABB
    sf::Vector2f max; // Maximum corner of the AABB
};

class PhysItem
{
public:
    PhysItem();
    AABB aabb;             // Axis aligned bounding box for the item
    sf::Vector2f position; // Position of the item in the world
    sf::Vector2f velocity; // Velocity vector for the item

    void applyForce(sf::Vector2f force) {
        // Apply a force to the item, modifying its velocity
        velocity += force;
    }
};

class PhysEngine
{
public:
    PhysEngine();
    PhysEngine(sf::Vector2f gravity);
    sf::Vector2f gravity; // Gravity vector for the physics engine
    bool AABBvsAABB(AABB a, AABB b);
    void applyGravity(PhysItem* item, sf::Time deltaTime) {
        // Apply gravity to the item based on the time step
        sf::Vector2f initialVelocity = item->velocity;
        item->velocity += gravity * deltaTime.asSeconds();
        item->position += ((item->velocity + initialVelocity) / 2.0f) * deltaTime.asSeconds();
    }
};

#endif // PHYS_ENGINE_H