#include "phys_engine.h"

AABB::AABB() :min({0, 0}), max({0, 0}) {}

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

PhysItem::PhysItem(){
    aabb = AABB();
    velocity = {0, 0};
}