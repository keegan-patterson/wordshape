#include "phys_engine.h"

// AABB
AABB::AABB() :min({0, 0}), max({0, 0}) {}


// PhysItem
PhysItem::PhysItem(Type item_type, std::string font_path){
    base_aabb = AABB();
    velocity = {0, 0};
    font = new sf::Font(font_path);
    text = new sf::Text(*font);
    this->item_type = item_type;
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