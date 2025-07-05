#include "phys_engine.h"

AABB::AABB() : x_min(0), y_min(0), x_max(0), y_max(0) {}

bool PhysEngine::AABBvsAABB(AABB a, AABB b)
{
    // Exit with no intersection if found separated along an axis
    if (a.x_max < b.x_min || a.x_min > b.x_max)
        return false;
    if (a.y_max < b.y_min || a.y_min > b.y_max)
        return false;
    // No separating axis found, therefore there is at least one overlapping axis
    return true;
}

PhysItem::PhysItem() : aabb(AABB()) {}