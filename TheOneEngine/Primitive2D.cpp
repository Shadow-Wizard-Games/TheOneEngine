#include "Primitive2D.h"
#include "Renderer2D.h"
#include "Transform.h"
#include "GameObject.h"


Primitive2D::Primitive2D(std::shared_ptr<GameObject> containerGO, PrimitiveType type)
    : Component(containerGO, ComponentType::Primitive2D),
    prType(type),
    p1(0.0f), p2(0.0f), offset(0.0f), width(0.0f),
    color(1.0f)
{
}

Primitive2D::Primitive2D(std::shared_ptr<GameObject> containerGO, Primitive2D* ref)
    : Component(containerGO, ComponentType::Primitive2D),
    prType(ref->prType),
    p1(ref->p1), p2(ref->p2), offset(ref->offset), width(ref->width),
    color(ref->color)
{
}

Primitive2D::~Primitive2D()
{
}

void Primitive2D::DrawComponent(Camera* camera) 
{
    if (prType == PrimitiveType::None)
        return;

    std::shared_ptr<GameObject> containerGO = GetContainerGO();
    Transform* t = containerGO.get()->GetComponent<Transform>();
    p1 = t->GetGlobalPosition();
    p2 = p1 + glm::vec3(t->GetGlobalRotation() * vec4(offset, 1.0));
    switch (prType)
    {
    case Quad:
        Renderer2D::DrawQuad(BatchType::WORLD, t->GetGlobalTransform(), color);
        break;
    case Circle:
        Renderer2D::DrawCircle(BatchType::WORLD, t->GetGlobalTransform(), color);
        break;
    case Line:
        Renderer2D::SetLineWidth(width);
        Renderer2D::DrawLine(BatchType::WORLD, p1, p2, color);
        break;
    }
}

json Primitive2D::SaveComponent()
{
    json prJSON;

    prJSON["Name"] = name;
    prJSON["Type"] = type;
    prJSON["LightType"] = prType;
    if (auto pGO = containerGO.lock())
    {
        prJSON["ParentUID"] = pGO.get()->GetUID();
    }
    prJSON["UID"] = UID;
    prJSON["Color"] = { color.r, color.g, color.b, color.a};
    prJSON["Offset"] = { offset.x, offset.y, offset.z };

    return prJSON;
}

void Primitive2D::LoadComponent(const json& prJSON)
{
    if (prJSON.contains("Name"))
    {
        name = prJSON["Name"];
    }
    if (prJSON.contains("LightType"))
    {
        prType = prJSON["LightType"];
    }
    if (prJSON.contains("UID"))
    {
        UID = prJSON["UID"];
    }
    if (prJSON.contains("Color"))
    {
        color.r = prJSON["Color"][0];
        color.g = prJSON["Color"][1];
        color.b = prJSON["Color"][2];
        color.a = prJSON["Color"][3];
    }
    if (prJSON.contains("Offset"))
    {
        offset.x = prJSON["Offset"][0];
        offset.y = prJSON["Offset"][1];
        offset.z = prJSON["Offset"][2];
    }
}