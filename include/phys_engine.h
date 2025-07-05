#ifndef PHYS_ENGINE_H
#define PHYS_ENGINE_H

class AABB
{
    public:
    AABB() = default;                            // Default constructor
    float x_min, y_min; // Minimum x and y coordinates
    float x_max, y_max; // Maximum x and y coordinates
};

class PhysEngine
{
    bool AABBvsAABB(AABB a, AABB b);
};

class PhysItem
{
    AABB aabb; // Axis aligned bounding box for the item

public:
    PhysItem() = default;                            // Default constructor
    PhysItem(const PhysItem &) = default;            // Copy constructor
    PhysItem(PhysItem &&) = default;                 // Move constructor
    PhysItem &operator=(const PhysItem &) = default; // Copy assignment operator
    PhysItem &operator=(PhysItem &&) = default;      // Move assignment operator
    ~PhysItem() = default;                           // Destructor

    // Additional member functions can be added here
};

#endif // PHYS_ENGINE_H