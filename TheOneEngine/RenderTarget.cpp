#include "RenderTarget.h"
#include "EngineCore.h"
#include "Window.h"

RenderTarget::RenderTarget(unsigned int id, std::string name, DrawMode mode, Camera* camera, glm::vec2 viewportSize, std::vector<std::vector<Attachment>> frameBuffers, bool active)
    : id(id),
    name(name),
    mode(mode),
    camera(camera),
    viewportSize(viewportSize),
    active(active)
{
    frameBuffers.reserve(frameBuffers.size() * sizeof(FrameBuffer));

    for (auto& fb : frameBuffers)
    {
        if (mode == DrawMode::BUILD_DEBUG || mode == DrawMode::BUILD_RELEASE && fb.back().frameBufferName == "uiBuffer")
            this->frameBuffers.emplace_back(fb[0].frameBufferName, engine->window->GetWidth(), engine->window->GetHeight(), fb);
        else
            this->frameBuffers.emplace_back(fb[0].frameBufferName, viewportSize.x, viewportSize.y, fb);
    }
}

RenderTarget::~RenderTarget() {}

std::vector<FrameBuffer>* RenderTarget::GetFrameBuffers()
{
    return &frameBuffers;
}

FrameBuffer* RenderTarget::GetFrameBuffer(std::string name)
{
    for (auto &fb : frameBuffers)
    {
        if (fb.GetName() == name)
            return &fb;
    }
    return nullptr;
}