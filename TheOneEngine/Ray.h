#ifndef __RAY_H__
#define __RAY_H__
#pragma once

#include "Defs.h"

struct AABBox;

class Ray
{
public:
    Ray();
    Ray(const glm::vec3& origin, const glm::vec3& direction);

    bool Intersects(const AABBox& bb) const;
    bool Intersects(const AABBox& bb, float& t) const;
    bool IntersectsTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float& t) const;

    glm::vec3 Origin, Direction;
};

#endif // !__RAY_H__