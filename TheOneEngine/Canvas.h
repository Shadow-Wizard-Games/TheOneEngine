#ifndef __CANVAS_H__
#define __CANVAS_H__
#pragma once

#include "Component.h"
#include "ItemUI.h"

#include "Texture.h"

#include <random>

class Canvas : public Component
{

public:

    Canvas(std::shared_ptr<GameObject> containerGO);
    Canvas(std::shared_ptr<GameObject> containerGO, Canvas* ref);
    virtual ~Canvas();

    void Update(double dt) override;

    template <typename TUI>
    unsigned int AddItemUI()
    {
        std::unique_ptr<ItemUI> newItemUI = std::make_unique<TUI>(containerGO.lock());
        unsigned int tempID = newItemUI->GetID();
        uiElements.push_back(std::move(newItemUI));

        return tempID;
    }

    template <typename TUI>
    unsigned int AddItemUI(std::string path)
    {
        std::unique_ptr<ItemUI> newItemUI = std::make_unique<TUI>(containerGO.lock(), path);
        unsigned int tempID = newItemUI->GetID();
        uiElements.push_back(std::move(newItemUI));

        return tempID;
    }

    template <typename TUI>
    unsigned int AddCopiedItemUI(TUI* ref)
    {
        std::unique_ptr<ItemUI> newItemUI = std::make_unique<TUI>(ref);
        unsigned int tempID = newItemUI->GetID();
        uiElements.push_back(std::move(newItemUI));

        return tempID;
    }

    template <typename TUI>
    TUI* GetItemUI(unsigned int id)
    {
        for (const auto& component : uiElements)
        {
            if (dynamic_cast<TUI*>(component.get()))
            {
                if (component.get()->GetID() == id)
                {
                    return static_cast<TUI*>(component.get());
                }
            }
        }
        return nullptr;
    }

    bool RemoveItemUI(unsigned int id);

    void DrawComponent(Camera* camera);

    void CanvasFlicker(bool flicker);

    // @Get / Set --------------------------------
    Rect2D GetRect() const;

    void SetSize(float width, float height);

public:

    json SaveComponent();
    void LoadComponent(const json& canvasJSON);

    //return a vector of all the uiElements (IT DOES NOT return uiElements vector, only its content as a sepparated vector)
    std::vector<ItemUI*> GetUiElements();
    //return real variable of uiElements vector
    std::vector<std::unique_ptr<ItemUI>>& GetUiElementsPtr();

    bool debugDraw = true;

private:
    Rect2D rect = { 0,0,2,2 };
    std::vector<std::unique_ptr<ItemUI>> uiElements;
    std::vector<bool> uiPrintables;

    bool flicker = false;
    double flickerCd = 0.0f;
    int flickerOrder = 0;
    bool flickeringBackground = true;

    //random stuff
    std::random_device rd;
    std::mt19937 gen;
    std::bernoulli_distribution dist;
};

#endif //  !__CANVAS_H__