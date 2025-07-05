#ifndef PHYS_ENGINE_H
#define PHYS_ENGINE_H

#include <SFML/Graphics.hpp>

class AABB
{
public:
    AABB();
    sf::Vector2f min; // Minimum corner of the AABB
    sf::Vector2f max; // Maximum corner of the AABB
};

class PhysEngine
{
    sf::Vector2f gravity; // Gravity vector for the physics engine
    bool AABBvsAABB(AABB a, AABB b);
};

class PhysItem
{
public:
    PhysItem();
    AABB aabb; // Axis aligned bounding box for the item
    sf::Vector2f velocity; // Velocity vector for the item

    // Additional member functions can be added here
};

#endif // PHYS_ENGINE_H