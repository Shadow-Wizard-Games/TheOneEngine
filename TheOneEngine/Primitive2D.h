#ifndef __PRIMITIVE2D_H__
#define __PRIMITIVE2D_H__
#pragma once

#include "Component.h"

enum PrimitiveType
{
    None = 0,
    Quad,
    Circle,
    Line
};

class Primitive2D : public Component
{
public:
    Primitive2D(std::shared_ptr<GameObject> containerGO, PrimitiveType type = PrimitiveType::None);
    Primitive2D(std::shared_ptr<GameObject> containerGO, Primitive2D* ref);
    virtual ~Primitive2D();

    void DrawComponent(Camera* camera);

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

public:
    PrimitiveType prType;
    glm::vec4 color;
    float width;
    glm::vec3 p1, p2, offset;
};

#endif // __PRIMITIVE2D_H__