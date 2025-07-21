#ifndef PHYS_ENGINE_H
#define PHYS_ENGINE_H

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

class PhysHelpers{
    public:
    static float dot(const sf::Vector2f& a, const sf::Vector2f& b) {
        return a.x * b.x + a.y * b.y;
    }
};

class AABB
{
public:
    AABB();
    sf::Vector2f min; // Minimum corner of the AABB
    sf::Vector2f max; // Maximum corner of the AABB
    sf::Vector2f getCenter() const {
        // Calculate the center of the AABB
        return (min + max) / 2.0f;
    }
    bool is_set = false;
};

class PhysItem
{
public:
    enum class Type {
        TextBlock, // Represents a text block item
        Obstacle // Represents an obstacle item
    };

    PhysItem(Type item_type, std::string font_path = "C:/Windows/Fonts/arial.ttf");

    AABB base_aabb;                 // Axis aligned bounding box for the item

    sf::Vector2f position;     // Position of the item in the world
    sf::Vector2f starting_position;     // Position of the item in the world
    sf::Vector2f velocity;     // Velocity vector for the item
    sf::Vector2f starting_velocity;     // Velocity vector for the item
    float restitution = 0.5f;  // Coefficient of restitution for the item
    float mass = 1.0f;         // Mass of the item, default is 1.0f

    sf::Font *font = nullptr;
    sf::Text *text = nullptr;  // Text representation of the item, if applicable
    Type item_type; // Type of the item, default is Obstacle
    AABB getAABB() const {
        // Return the AABB of the item
        AABB aabb;
        aabb.min = position + base_aabb.min;
        aabb.max = position + base_aabb.max;
        return aabb;
    }
    void setAABB(sf::Vector2f min, sf::Vector2f max){
        base_aabb.min = min;
        base_aabb.max = max;
        rectangle.setSize(max - min);
    }
    sf::RectangleShape rectangle; // Rectangle shape for rendering the item
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

    void ResolveCollision( PhysItem *A, PhysItem *B )
    {
        // Calculate relative velocity 
        sf::Vector2f rv = B->velocity - A->velocity;

        // Calculate relative velocity in terms of the normal direction 
        sf::Vector2f normal = (B->getAABB().getCenter() - A->getAABB().getCenter()).normalized();
        float velAlongNormal = PhysHelpers::dot(rv, normal);

        // Do not resolve if velocities are separating 
        if(velAlongNormal > 0)
            return;

        // Calculate restitution 
        float e = std::min( A->restitution, B->restitution);

        // Calculate impulse scalar 
        float j = -(1 + e) * velAlongNormal;
        j /= 1 / A->mass + 1 / B->mass;
  
        // Apply impulse 
        sf::Vector2f impulse = j * normal;
        A->velocity -= 1 / A->mass * impulse;
        B->velocity += 1 / B->mass * impulse;
    }
};

#endif // PHYS_ENGINE_H