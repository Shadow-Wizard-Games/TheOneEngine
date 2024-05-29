#include "RenderTarget.h"

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
        this->frameBuffers.emplace_back(fb[0].frameBufferName, viewportSize.x, viewportSize.y, fb);
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