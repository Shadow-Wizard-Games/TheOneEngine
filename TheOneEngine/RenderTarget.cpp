#include "RenderTarget.h"
#include "FrameBuffer.h"

RenderTarget::RenderTarget(unsigned int id, DrawMode mode, Camera* camera, glm::vec2 viewportSize, std::vector<std::vector<Attachment>> frameBuffers)
    : id(id),
    mode(mode),
    camera(camera),
    viewportSize(viewportSize)
{
    for (auto fb : frameBuffers)
        this->frameBuffers.emplace_back(viewportSize.x, viewportSize.y, fb);
}

RenderTarget::~RenderTarget() {}

std::vector<FrameBuffer>* RenderTarget::GetFrameBuffers()
{
    return &frameBuffers;
}

FrameBuffer* RenderTarget::GetFrameBuffer(std::string name)
{
    for (auto fb : frameBuffers)
    {
        if (fb.GetName() == name)
            return &fb;
    }
    return nullptr;
}