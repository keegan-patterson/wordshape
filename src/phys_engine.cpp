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
    if (item_type == Type::TextBlock) {
        // text->setString("A");
        // text->setCharacterSize(100);
        // text->setFillColor(sf::Color::White);
        // text->setPosition({0, 0});

        definePolygonHexagon();
        // Debugging aabbs
        rectangle.setOutlineColor(sf::Color::Red);
        rectangle.setOutlineThickness(1);
        rectangle.setFillColor(sf::Color::Transparent );
    } else if (item_type == Type::Obstacle) {
        rectangle.setSize(sf::Vector2f(100, 50));
        rectangle.setOutlineColor(sf::Color::Red);
        rectangle.setOutlineThickness(5);
        rectangle.setFillColor(sf::Color::Red);
    }
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

int PhysEngine::OverlappingAxis(AABB a, AABB b){
    if(a.max.x > b.min.x){
        if(a.min.x < b.max.x){
            return 1; // A overlaps B on X axis, A is to the left of B
        }
    }
    if(a.max.y > b.min.y){
        if(a.min.y < b.max.y){
            return 2; // A overlaps B on Y axis, A is above B
        }
    }
    if(b.max.x > a.min.x){
        if(b.min.x < a.max.x){
            return 3; // B overlaps A on X axis, B is to the left of A
        }
    }
    if (b.max.y > a.min.y){
        if(b.min.y < a.max.y){
            return 4; // B overlaps A on Y axis, B is above A
        }
    }
    return 0;
}

sf::Vector2f PhysEngine::NormalVectorFromAxis(int axis) {
    switch (axis) {
        case 1: return {1, 0}; // A overlaps B on X axis, A is to the left of B
        case 2: return {0, 1}; // A overlaps B on Y axis, A is above B
        case 3: return {-1, 0}; // B overlaps A on X axis, B is to the left of A
        case 4: return {0, -1}; // B overlaps A on Y axis, B is above A
        default: return {0, 0}; // No valid axis
    }
}