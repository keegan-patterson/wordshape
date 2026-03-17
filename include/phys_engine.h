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

    // Two crossed vectors return a scalar
    static float CrossProduct( const sf::Vector2f& a, const sf::Vector2f& b )
    {
        return a.x * b.y - a.y * b.x;
    }

    // More exotic (but necessary) forms of the cross product
    // with a vector a and scalar s, both returning a vector
    static sf::Vector2f CrossProduct( const sf::Vector2f& a, float s )
    {
        return sf::Vector2f( s * a.y, -s * a.x );
    }
    static sf::Vector2f CrossProduct( float s, const sf::Vector2f& a )
    {
        return sf::Vector2f( -s * a.y, s * a.x );
    }

    // Rotate a local-space point by angle (radians, clockwise in screen coords)
    static sf::Vector2f rotatePoint(const sf::Vector2f& point, float angle)
    {
        float c = std::cos(angle);
        float s = std::sin(angle);
        return sf::Vector2f(point.x * c - point.y * s,
                            point.x * s + point.y * c);
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
    sf::Vector2f starting_position; // Starting position, used for resets
    sf::Vector2f velocity;          // Velocity vector for the item
    sf::Vector2f starting_velocity; // Starting velocity, used for resets
    float orientation = 0.0f;           // Current rotation angle in radians
    float starting_orientation = 0.0f;  // Starting orientation, used for resets
    float angularVelocity = 0.0f;           // Rotational velocity in radians/sec
    float starting_angular_velocity = 0.0f; // Starting angular velocity, used for resets
    float restitution = 0.5f;       // Coefficient of restitution for the item
    float mass = 1.0f;              // Mass of the item, default is 1.0f
    float inverseInertia = 0.0f;    // 1/I — call computeInertia() to populate; 0 = static body
    sf::Vector2f localCOM = {0.0f, 0.0f}; // Polygon centroid in local space, computed by computeInertia()
    float staticFrictionCoefficient = 0.3f;  // Static friction coefficient for the item
    float dynamicFrictionCoefficient = 0.2f; // Dynamic friction coefficient for the item

    sf::Font *font = nullptr;
    sf::Text *text = nullptr; // Text representation of the item, if applicable
    Type item_type;           // Type of the item, default is Obstacle

    // Compute inverseInertia from the current polygon and mass.
    // Call after definePolygon*() and after setting mass.
    // For static boundary bodies, leave inverseInertia = 0 instead.
    void computeInertia();

    sf::RectangleShape rectangle; // Rectangle shape for rendering the item
    sf::ConvexShape polygon;      // Polygon shape for rendering the item, if applicable

    AABB getAABB() const
    {
        // If the item has no polygon vertices, fall back to the static base_aabb.
        // Otherwise, compute the AABB from the actual rotated polygon vertices.
        // This is essential once objects spin: the static base_aabb no longer
        // encloses the rotated shape, causing AABBvsAABB to miss collisions and
        // objects to phase through boundaries.
        int n = static_cast<int>(polygon.getPointCount());
        if (n == 0)
        {
            AABB aabb;
            aabb.min = position + base_aabb.min;
            aabb.max = position + base_aabb.max;
            return aabb;
        }

        float minX =  std::numeric_limits<float>::max();
        float minY =  std::numeric_limits<float>::max();
        float maxX = -std::numeric_limits<float>::max();
        float maxY = -std::numeric_limits<float>::max();

        int count = (n > 1 && polygon.getPoint(n - 1) == polygon.getPoint(0)) ? n - 1 : n;
        for (int i = 0; i < count; i++)
        {
            sf::Vector2f p = PhysHelpers::rotatePoint(polygon.getPoint(i), orientation) + position;
            if (p.x < minX) minX = p.x;
            if (p.y < minY) minY = p.y;
            if (p.x > maxX) maxX = p.x;
            if (p.y > maxY) maxY = p.y;
        }

        AABB aabb;
        aabb.min = { minX, minY };
        aabb.max = { maxX, maxY };
        aabb.is_set = true;
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
        // Vertices centered around (0,0). position therefore equals the world-space COM.
        polygon.setPointCount(7);
        polygon.setPoint(0, sf::Vector2f(-25, -43));
        polygon.setPoint(1, sf::Vector2f( 25, -43));
        polygon.setPoint(2, sf::Vector2f( 50,   0));
        polygon.setPoint(3, sf::Vector2f( 25,  43));
        polygon.setPoint(4, sf::Vector2f(-25,  43));
        polygon.setPoint(5, sf::Vector2f(-50,   0));
        polygon.setPoint(6, sf::Vector2f(-25, -43)); // close
        polygon.setFillColor(sf::Color::Green);
        polygon.setOutlineColor(sf::Color::Black);
    }

    void definePolygonRectangle(sf::Vector2f size)
    {
        // Vertices centered around (0,0) for the given width/height.
        // position therefore equals the world-space COM of the rectangle.
        float hw = size.x / 2.0f;
        float hh = size.y / 2.0f;
        polygon.setPointCount(5);
        polygon.setPoint(0, sf::Vector2f(-hw, -hh));
        polygon.setPoint(1, sf::Vector2f( hw, -hh));
        polygon.setPoint(2, sf::Vector2f( hw,  hh));
        polygon.setPoint(3, sf::Vector2f(-hw,  hh));
        polygon.setPoint(4, sf::Vector2f(-hw, -hh)); // close
        polygon.setFillColor(sf::Color::Green);
        polygon.setOutlineColor(sf::Color::Green);
    }

    sf::Vector2f getProjection(const sf::Vector2f &axis) const
    {
        float min = 0.0f;
        float max = 0.0f;
        // Project the polygon onto the given axis, accounting for rotation
        for (int i = 0; i < (int)polygon.getPointCount(); i++)
        {
            sf::Vector2f point = PhysHelpers::rotatePoint(polygon.getPoint(i), orientation) + position;
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
        // Get the SAT axes for the polygon, rotated to match current orientation
        std::vector<sf::Vector2f> axes;
        int pointsToTraverse = polygon.getPointCount();
        if(polygon.getPoint(polygon.getPointCount() - 1) == polygon.getPoint(0))
        {
            pointsToTraverse--; // Remove the last point if it is the same as the first
        }

        for (int i = 0; i < pointsToTraverse; i++)
        {
            // Rotate both endpoints (relative to localCOM) so axes reflect current orientation
            sf::Vector2f point1 = PhysHelpers::rotatePoint(polygon.getPoint(i), orientation);
            sf::Vector2f point2 = PhysHelpers::rotatePoint(
                polygon.getPoint((i + 1) % polygon.getPointCount()), orientation);
            sf::Vector2f edge = point2 - point1;
            // Perpendicular vector to the edge (the SAT axis)
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
        // Integrate linear velocity and position
        sf::Vector2f initialVelocity = item->velocity;
        item->velocity += gravity * deltaTime.asSeconds();
        item->position += ((item->velocity + initialVelocity) / 2.0f) * deltaTime.asSeconds();

        // Integrate angular velocity into orientation
        item->orientation += item->angularVelocity * deltaTime.asSeconds();
    }

    void ResolveCollision(PhysItem *A, PhysItem *B)
    {
        auto mtvOpt = SeparatingAxisTheorem(*A, *B);
        if (!mtvOpt.has_value())
            return;

        sf::Vector2f normal = mtvOpt.value().normalized();

        // Find the approximate contact point as the midpoint of the two support points.
        // The support point of A along +normal is the vertex of A deepest into B,
        // and vice-versa for B along -normal.
        auto supportPoint = [&](const PhysItem& shape, const sf::Vector2f& dir) -> sf::Vector2f
        {
            float best = -std::numeric_limits<float>::max();
            sf::Vector2f bestPt;
            int n = shape.polygon.getPointCount();
            int count = (n > 1 && shape.polygon.getPoint(n - 1) == shape.polygon.getPoint(0))
                        ? n - 1 : n;
            for (int i = 0; i < count; i++)
            {
                sf::Vector2f p = PhysHelpers::rotatePoint(shape.polygon.getPoint(i), shape.orientation)
                                 + shape.position;
                float proj = PhysHelpers::dot(p, dir);
                if (proj > best) { best = proj; bestPt = p; }
            }
            return bestPt;
        };
        // Use the incident vertex — the deepest penetrating vertex of B into A — as the
        // contact point. Averaging both support points (A's corner + B's vertex) breaks
        // for boundary-hexagon collisions: the boundary's support vertex is a far corner
        // of the large rectangle, giving a lever arm that's hundreds of units off-center.
        // This inflates rBCrossN² in the impulse denominator by ~100x, making the impulse
        // too weak to prevent sinking. Using only B's incident vertex gives the correct
        // contact location in all cases.
        sf::Vector2f contactPoint = supportPoint(*B, -normal);

        // Lever arms from each COM to the contact point
        sf::Vector2f rA = contactPoint - A->position;
        sf::Vector2f rB = contactPoint - B->position;

        // Relative velocity at the contact point, including rotational contributions:
        //   v_contact = v_com + omega x r   (2D: omega x r = (-omega*ry, omega*rx))
        sf::Vector2f vA = A->velocity + PhysHelpers::CrossProduct(A->angularVelocity, rA);
        sf::Vector2f vB = B->velocity + PhysHelpers::CrossProduct(B->angularVelocity, rB);
        sf::Vector2f rv = vB - vA;

        float velAlongNormal = PhysHelpers::dot(rv, normal);
        if (velAlongNormal > 0)
            return; // Already separating

        float e = std::min(A->restitution, B->restitution);

        // Scalar cross products of the lever arms with the collision normal.
        // These appear squared in the denominator as the rotational resistance term.
        float rACrossN = PhysHelpers::CrossProduct(rA, normal);
        float rBCrossN = PhysHelpers::CrossProduct(rB, normal);

        float j = -(1.0f + e) * velAlongNormal;
        j /= (1.0f / A->mass + 1.0f / B->mass
              + rACrossN * rACrossN * A->inverseInertia
              + rBCrossN * rBCrossN * B->inverseInertia);

        sf::Vector2f impulse = j * normal;

        // Apply linear impulse
        A->velocity -= (1.0f / A->mass) * impulse;
        B->velocity += (1.0f / B->mass) * impulse;

        // Apply angular impulse: delta_omega = (1/I) * (r x impulse)
        A->angularVelocity -= A->inverseInertia * PhysHelpers::CrossProduct(rA, impulse);
        B->angularVelocity += B->inverseInertia * PhysHelpers::CrossProduct(rB, impulse);

        // --- Friction ---
        // Recalculate relative velocity at contact after the normal impulse
        vA = A->velocity + PhysHelpers::CrossProduct(A->angularVelocity, rA);
        vB = B->velocity + PhysHelpers::CrossProduct(B->angularVelocity, rB);
        rv = vB - vA;

        sf::Vector2f tangent = rv - PhysHelpers::dot(rv, normal) * normal;
        if (tangent.lengthSquared() < 1e-10f)
            return; // No tangential relative motion — skip friction
        tangent = tangent.normalized();

        float rACrossT = PhysHelpers::CrossProduct(rA, tangent);
        float rBCrossT = PhysHelpers::CrossProduct(rB, tangent);

        float jt = -PhysHelpers::dot(rv, tangent);
        jt /= (1.0f / A->mass + 1.0f / B->mass
               + rACrossT * rACrossT * A->inverseInertia
               + rBCrossT * rBCrossT * B->inverseInertia);

        float muS = (A->staticFrictionCoefficient + B->staticFrictionCoefficient) / 2.0f;
        float muD = (A->dynamicFrictionCoefficient + B->dynamicFrictionCoefficient) / 2.0f;

        sf::Vector2f frictionImpulse;
        if (std::abs(jt) < j * muS)
            frictionImpulse = jt * tangent;           // Static friction — clamp to cone
        else
            frictionImpulse = std::copysign(1.0f, jt) * j * muD * tangent; // Kinetic friction

        A->velocity -= (1.0f / A->mass) * frictionImpulse;
        B->velocity += (1.0f / B->mass) * frictionImpulse;
        A->angularVelocity -= A->inverseInertia * PhysHelpers::CrossProduct(rA, frictionImpulse);
        B->angularVelocity += B->inverseInertia * PhysHelpers::CrossProduct(rB, frictionImpulse);

        // --- Positional correction ---
        // Impulses fix velocities but leave any existing overlap untouched.
        // Each frame gravity pushes the object slightly deeper, the overlap grows,
        // and the body visibly sinks. Linear projection corrects positions directly,
        // scaled by inverse-mass so static bodies (1/mass ≈ 0) never move.
        // A small slop threshold prevents jitter on resting contacts.
        const float percent = 0.4f;  // fraction of penetration to correct per frame
        const float slop    = 0.5f;  // pixels of overlap tolerated before correcting
        float penetration   = mtvOpt.value().length();
        float correctionMag = std::max(penetration - slop, 0.0f)
                              / (1.0f / A->mass + 1.0f / B->mass)
                              * percent;
        sf::Vector2f correction = correctionMag * normal;
        A->position -= (1.0f / A->mass) * correction;
        B->position += (1.0f / B->mass) * correction;
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