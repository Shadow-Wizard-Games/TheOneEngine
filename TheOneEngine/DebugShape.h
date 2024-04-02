#ifndef __DEBUG_SHAPE_H__
#define __DEBUG_SHAPE_H__
#pragma once

#include "Defs.h"

class DebugShape
{
public:

    DebugShape() {}
    ~DebugShape() = default;

public:

    std::vector<vec3f> points;
    vec3f center = vec3f(0, 0, 0);
    vec3f color = vec3f(1, 1, 1);
};
#endif // !__DEBUG_SHAPE_H__
