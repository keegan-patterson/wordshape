#include "phys_engine.h"
#include <cmath>

// AABB
AABB::AABB() :min({0, 0}), max({0, 0}) {}


// PhysItem
PhysItem::PhysItem(Type item_type, std::string font_path){
    base_aabb = AABB();
    velocity = {0, 0};
    font = new sf::Font(font_path);
    text = new sf::Text(*font);
    this->item_type = item_type;
    if (item_type == Type::TextBlock) {
        // text->setString("A");
        // text->setCharacterSize(100);
        // text->setFillColor(sf::Color::White);
        // text->setPosition({0, 0});

        definePolygonHexagon();
        computeInertia(); // Polygon and default mass=1 are both set; compute now
        // Debugging aabbs
        rectangle.setOutlineColor(sf::Color::Red);
        rectangle.setOutlineThickness(1);
        rectangle.setFillColor(sf::Color::Transparent);
    } if(item_type == Type::Obstacle) {
        // Debugging aabbs
        rectangle.setOutlineColor(sf::Color::Blue);
        rectangle.setOutlineThickness(1);
        rectangle.setFillColor(sf::Color::Transparent);
    }
}

void PhysItem::computeInertia()
{
    // Compute moment of inertia about the origin using the triangle decomposition.
    // Polygon vertices must be centered around (0,0) — i.e. definePolygon*() places
    // the COM at the local origin — so I_origin == I_com and no parallel axis
    // correction is needed. localCOM is set to (0,0) to reflect this invariant.
    // For static boundary bodies set inverseInertia = 0 directly; don't call this.

    int n = static_cast<int>(polygon.getPointCount());
    int count = (n > 1 && polygon.getPoint(n - 1) == polygon.getPoint(0)) ? n - 1 : n;

    if (count < 3 || mass <= 0.0f)
    {
        localCOM = {0.0f, 0.0f};
        inverseInertia = 0.0f;
        return;
    }

    localCOM = {0.0f, 0.0f}; // Vertices are centered; COM is at the local origin.

    float numerator   = 0.0f;
    float denominator = 0.0f;

    for (int i = 0; i < count; i++)
    {
        sf::Vector2f p1 = polygon.getPoint(i);
        sf::Vector2f p2 = polygon.getPoint((i + 1) % count);

        float cross = std::abs(p1.x * p2.y - p1.y * p2.x);
        numerator += cross * (PhysHelpers::dot(p1, p1)
                            + PhysHelpers::dot(p1, p2)
                            + PhysHelpers::dot(p2, p2));
        denominator += cross;
    }

    if (denominator < 1e-10f)
    {
        inverseInertia = 0.0f;
        return;
    }

    float I = (mass / 6.0f) * (numerator / denominator);
    inverseInertia = (I > 1e-10f) ? 1.0f / I : 0.0f;
}


// PhysEngine
PhysEngine::PhysEngine() : gravity({0, 0}) {}
PhysEngine::PhysEngine(sf::Vector2f gravity) : gravity(gravity) {}

bool PhysEngine::AABBvsAABB(AABB a, AABB b)
{
    // Exit with no intersection if found separated along an axis
    if (a.max.x < b.min.x || a.min.x > b.max.x)
        return false;
    if (a.max.y < b.min.y || a.min.y > b.max.y)
        return false;
    // No separating axis found, therefore there is at least one overlapping axis
    return true;
}