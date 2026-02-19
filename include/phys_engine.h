#ifndef PHYS_ENGINE_H
#define PHYS_ENGINE_H

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>

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
    float staticFrictionCoefficient = 0.3f; // Static friction coefficient for the item
    float dynamicFrictionCoefficient = 0.2f; // Dynamic friction coefficient for the item

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
        polygon.setPoint(0, sf::Vector2f(25, 0));
        polygon.setPoint(1, sf::Vector2f(75, 0));
        polygon.setPoint(2, sf::Vector2f(100, 43));
        polygon.setPoint(3, sf::Vector2f(75, 86));
        polygon.setPoint(4, sf::Vector2f(25, 86));
        polygon.setPoint(5, sf::Vector2f(0, 43));
        polygon.setPoint(6, sf::Vector2f(25, 0));
        polygon.setFillColor(sf::Color::Green);
        polygon.setOutlineColor(sf::Color::Black);
    }

    void definePolygonRectangle(std::vector<sf::Vector2f> points)
    {
        polygon.setPointCount(5);
        polygon.setPoint(0, points[0]);
        polygon.setPoint(1, points[1]);
        polygon.setPoint(2, points[2]);
        polygon.setPoint(3, points[3]);
        polygon.setPoint(4, points[0]); // Close the polygon
        polygon.setFillColor(sf::Color::Green);
        polygon.setOutlineColor(sf::Color::Green);
    }

    sf::Vector2f getProjection(const sf::Vector2f &axis) const
    {
        float min = 0.0f;
        float max = 0.0f;
        // Project the polygon onto the given axis
        for (int i = 0; i < polygon.getPointCount(); i++)
        {
            sf::Vector2f point = polygon.getPoint(i);
            point = position + point; // Transform point to world coordinates
            float projection = PhysHelpers::dot(point, axis.normalized());
            if (i == 0)
            {
                min = projection;
                max = projection;
            }
            else
            {
                if (projection < min)
                    min = projection;
                if (projection > max)
                    max = projection;
            }
        }
        return sf::Vector2f(min, max);
    }

    std::vector<sf::Vector2f> getTestableAxes() const
    {
        // Get the axes for the polygon
        std::vector<sf::Vector2f> axes;
        int pointsToTraverse = polygon.getPointCount();
        if(polygon.getPoint(polygon.getPointCount() - 1) == polygon.getPoint(0))
        {
            pointsToTraverse--; // Remove the last point if it is the same as the first
        }

        for (int i = 0; i < pointsToTraverse; i++)
        {
            sf::Vector2f point1 = polygon.getPoint(i);
            sf::Vector2f point2 = polygon.getPoint((i + 1) % polygon.getPointCount());
            sf::Vector2f edge = point2 - point1;
            // Perpendicular vector to the edge
            sf::Vector2f axis(-edge.y, edge.x);
            axes.push_back(axis);
        }
        return axes;
    }
};

class PhysEngine
{
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
        sf::Vector2f rv = B->velocity - A->velocity;

        auto mtvOpt = SeparatingAxisTheorem(*A, *B);
        if (!mtvOpt.has_value())
            return;

        sf::Vector2f normal = mtvOpt.value().normalized();
        float velAlongNormal = PhysHelpers::dot(rv, normal);

        if (velAlongNormal > 0)
            return;

        float e = std::min(A->restitution, B->restitution);

        float j = -(1.0f + e) * velAlongNormal;
        j /= (1.0f / A->mass + 1.0f / B->mass);

        sf::Vector2f impulse = j * normal;
        A->velocity -= (1.0f / A->mass) * impulse;
        B->velocity += (1.0f / B->mass) * impulse;

        // Recalculate relative velocity after normal impulse
        rv = B->velocity - A->velocity;

        sf::Vector2f tangent = rv - PhysHelpers::dot(rv, normal) * normal;
        if (tangent.lengthSquared() < 1e-10f)
            return; // No tangential component, skip friction

        tangent = tangent.normalized();

        float jt = -PhysHelpers::dot(rv, tangent);
        jt /= (1.0f / A->mass + 1.0f / B->mass);

        float muS = (A->staticFrictionCoefficient + B->staticFrictionCoefficient) / 2.0f;
        float muD = (A->dynamicFrictionCoefficient + B->dynamicFrictionCoefficient) / 2.0f;

        sf::Vector2f frictionImpulse;
        if (std::abs(jt) < j * muS)
        {
            frictionImpulse = jt * tangent;
        }
        else
        {
            frictionImpulse = std::copysign(1.0f, jt) * j * muD * tangent;
        }

        A->velocity -= (1.0f / A->mass) * frictionImpulse;
        B->velocity += (1.0f / B->mass) * frictionImpulse;
    }

    std::optional<sf::Vector2f> SeparatingAxisTheorem(PhysItem shape1, PhysItem shape2)
    {
        float overlap = std::numeric_limits<float>::max();
        sf::Vector2f smallest;

        std::vector<sf::Vector2f> axes1 = shape1.getTestableAxes();
        std::vector<sf::Vector2f> axes2 = shape2.getTestableAxes();

        auto testAxes = [&](const std::vector<sf::Vector2f>& axes) -> bool
        {
            for (const auto& axis : axes)
            {
                sf::Vector2f p1 = shape1.getProjection(axis);
                sf::Vector2f p2 = shape2.getProjection(axis);

                if (p1.x >= p2.y || p2.x >= p1.y)
                    return false; // Separating axis found, no collision

                float o = std::min(p1.y, p2.y) - std::max(p1.x, p2.x);

                if (o < overlap)
                {
                    overlap = o;
                    smallest = axis;
                }
            }
            return true;
        };

        if (!testAxes(axes1) || !testAxes(axes2))
            return std::nullopt;

        sf::Vector2f direction = shape2.position - shape1.position;
        sf::Vector2f mtv = smallest.normalized() * overlap;
        if (PhysHelpers::dot(direction, mtv) < 0)
            mtv = -mtv;

        return mtv;
    }
};

#endif // PHYS_ENGINE_H