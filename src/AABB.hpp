#ifndef AABB_HPP
#define AABB_HPP

class AABB {
public:
	AABB() : min(0.0f), max(0.0f) { }

    AABB(const glm::vec3 &min, const glm::vec3 &max) : min(min), max(max) { }

    bool intersects(const AABB &other) const {
        bool collisionX = (min.x <= other.max.x && max.x >= other.min.x);
        bool collisionY = (min.y <= other.max.y && max.y >= other.min.y);
        bool collisionZ = (min.z <= other.max.z && max.z >= other.min.z);

        return collisionX && collisionY && collisionZ;
    }
protected:
	glm::vec3 min, max;
};

#endif