#ifndef PHYS_ENGINE_H
#define PHYS_ENGINE_H

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

class PhysHelpers
{
public:
    static float dot(const sf::Vector2f &a, const sf::Vector2f &b)
    {
        return a.x * b.x + a.y * b.y;
    }
};

class AABB
{
public:
    AABB();
    sf::Vector2f min; // Minimum corner of the AABB
    sf::Vector2f max; // Maximum corner of the AABB
    sf::Vector2f getCenter() const
    {
        // Calculate the center of the AABB
        return (min + max) / 2.0f;
    }
    bool is_set = false;
};

class PhysItem
{
public:
    enum class Type
    {
        TextBlock, // Represents a text block item
        Obstacle   // Represents an obstacle item
    };

    PhysItem(Type item_type, std::string font_path = "C:/Windows/Fonts/arial.ttf");

    AABB base_aabb; // Axis aligned bounding box for the item

    sf::Vector2f position;          // Position of the item in the world
    sf::Vector2f starting_position; // Position of the item in the world
    sf::Vector2f velocity;          // Velocity vector for the item
    sf::Vector2f starting_velocity; // Velocity vector for the item
    float restitution = 0.5f;       // Coefficient of restitution for the item
    float mass = 1.0f;              // Mass of the item, default is 1.0f

    sf::Font *font = nullptr;
    sf::Text *text = nullptr; // Text representation of the item, if applicable
    Type item_type;           // Type of the item, default is Obstacle

    sf::RectangleShape rectangle; // Rectangle shape for rendering the item
    sf::ConvexShape polygon;      // Polygon shape for rendering the item, if applicable

    AABB getAABB() const
    {
        // Return the AABB of the item
        AABB aabb;
        aabb.min = position + base_aabb.min;
        aabb.max = position + base_aabb.max;
        return aabb;
    }
    void setAABB(sf::Vector2f min, sf::Vector2f max)
    {
        base_aabb.min = min;
        base_aabb.max = max;
        rectangle.setSize(max - min);
    }

    void definePolygonHexagon()
    {
        polygon.setPointCount(7);
        polygon.setPoint(0, sf::Vector2f(-25, 0));
        polygon.setPoint(1, sf::Vector2f(25, 0));
        polygon.setPoint(2, sf::Vector2f(50, 37.5));
        polygon.setPoint(3, sf::Vector2f(25, 75));
        polygon.setPoint(4, sf::Vector2f(-25, 75));
        polygon.setPoint(5, sf::Vector2f(-50, 37.5));
        polygon.setPoint(6, sf::Vector2f(-25, 0));
        polygon.setFillColor(sf::Color::Green);
        polygon.setOutlineColor(sf::Color::Black);
    }

};

class PhysEngine
{
    int OverlappingAxis(AABB a, AABB b);
    sf::Vector2f NormalVectorFromAxis(int axis);

public:
    PhysEngine();
    PhysEngine(sf::Vector2f gravity);
    sf::Vector2f gravity; // Gravity vector for the physics engine
    bool AABBvsAABB(AABB a, AABB b);

    void applyGravity(PhysItem *item, sf::Time deltaTime)
    {
        // Apply gravity to the item based on the time step
        sf::Vector2f initialVelocity = item->velocity;
        item->velocity += gravity * deltaTime.asSeconds();
        item->position += ((item->velocity + initialVelocity) / 2.0f) * deltaTime.asSeconds();
    }

    void ResolveCollision(PhysItem *A, PhysItem *B)
    {
        // Calculate relative velocity
        sf::Vector2f rv = B->velocity - A->velocity;

        // Calculate relative velocity in terms of the normal direction
        // TODO: This needs to be replaced with SAT (Separating Axis Theorem)
        sf::Vector2f normal;
        if (A->item_type == PhysItem::Type::Obstacle || B->item_type == PhysItem::Type::Obstacle)
        {
            // Resolve collison with obstacles differently
            // Find which side collision is happening on
            int axis = OverlappingAxis(A->getAABB(), B->getAABB());
            normal = NormalVectorFromAxis(axis);
        }
        else
        {
            normal = (B->getAABB().getCenter() - A->getAABB().getCenter()).normalized();
        }
        float velAlongNormal = PhysHelpers::dot(rv, normal);

        // Do not resolve if velocities are separating
        if (velAlongNormal > 0)
            return;

        // Calculate restitution
        float e = std::min(A->restitution, B->restitution);

        // Calculate impulse scalar
        float j = -(1 + e) * velAlongNormal;
        j /= 1 / A->mass + 1 / B->mass;

        // Apply impulse
        sf::Vector2f impulse = j * normal;
        A->velocity -= 1 / A->mass * impulse;
        B->velocity += 1 / B->mass * impulse;
    }

    void SeparatingAxisTheorem(PhysItem shape1, PhysItem shape2)
    {

        // sf::Vector2f axes[2]; //For the moment only use two axes because bounding boxes have 2 axes
        // for (int i = 0; i < std::size(axes); i++)
        // {
        //     sf::Vector2f axis = axes[i];
        //     // project both shapes onto the axis
        //     Projection p1 = shape1.project(axis);
        //     Projection p2 = shape2.project(axis);
        //     // do the projections overlap?
        //     if (!p1.overlap(p2))
        //     {
        //         // then we can guarantee that the shapes do not overlap
        //         return false;
        //     }
        // }
    }
};

#endif // PHYS_ENGINE_H