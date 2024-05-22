#include "Framebuffer.h"
#include "Log.h"

#include "GL/glew.h"

#include "../TheOneEngine/EngineCore.h"

static const unsigned int s_MaxFramebufferSize = 8192;

FrameBuffer::FrameBuffer(std::string name, int width, int height, std::vector<Attachment> attachments)
{
	this->width = width;
	this->height = height;
    this->attachments = attachments;
    GenerateFrameBuffer();
}

FrameBuffer::~FrameBuffer()
{
    for (const auto& attachment : attachments)
        glDeleteTextures(1, &attachment.textureId);

    GLCALL(glDeleteFramebuffers(1, &FBO));
}

void FrameBuffer::Bind(bool clear)
{
	GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
	GLCALL(glViewport(0, 0, width, height));
	if (clear) {
		GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	}
}

void FrameBuffer::Unbind()
{
	GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::GenerateFrameBuffer()
{
    // Delete previous attachments and framebuffer
    // Reset id
    if (FBO)
    {       
        for (const auto& attachment : attachments)
            GLCALL(glDeleteTextures(1, &attachment.textureId));

        GLCALL(glDeleteFramebuffers(1, &FBO));

        for (auto& attachment : attachments)
            attachment.textureId = 0;
    }

    // Create and bind framebuffer
    GLCALL(glGenFramebuffers(1, &FBO));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));

    // Create and attach attachments
    GLenum drawBuffers[MAX_ATTACHMENTS];
    unsigned int drawBufferIndex = 0;

    for (auto& attachment : attachments)
    {
        switch (attachment.type)
        {
            case Attachment::Type::RGBA8:
                GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &attachment.textureId));
                GLCALL(glBindTexture(GL_TEXTURE_2D, attachment.textureId));
                GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
                TextureParameters();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + drawBufferIndex, GL_TEXTURE_2D, attachment.textureId, 0);
                drawBuffers[drawBufferIndex++] = GL_COLOR_ATTACHMENT0 + drawBufferIndex;
                break;

            case Attachment::Type::RGB16F:
                GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &attachment.textureId));
                GLCALL(glBindTexture(GL_TEXTURE_2D, attachment.textureId));
                GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr));
                TextureParameters();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + drawBufferIndex, GL_TEXTURE_2D, attachment.textureId, 0);
                drawBuffers[drawBufferIndex++] = GL_COLOR_ATTACHMENT0 + drawBufferIndex;
                break;

            case Attachment::Type::DEPTH:
                GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &attachment.textureId));
                GLCALL(glBindTexture(GL_TEXTURE_2D, attachment.textureId));
                GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
                TextureParameters();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, attachment.textureId, 0);
                break;

            case Attachment::Type::DEPTH_STENCIL:
                GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &attachment.textureId));
                GLCALL(glBindTexture(GL_TEXTURE_2D, attachment.textureId));
                GLCALL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height));
                TextureParameters();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, attachment.textureId, 0);
                break;

            default: break;
        }
    }

    // Set draw buffers
    if (drawBufferIndex)
    {
        GLCALL(glDrawBuffers(drawBufferIndex, drawBuffers));
    }
    else
    {
        GLCALL(glDrawBuffer(GL_NONE));
        GLCALL(glReadBuffer(GL_NONE));
    }

    // Check framebuffer completeness
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);

    // Unbind framebuffer
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::TextureParameters()
{   
    // hekbas TODO: set filtering type on choice
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
}

void FrameBuffer::Resize(unsigned int newWidth, unsigned int newHeight)
{
	if (newWidth < 0 || newHeight < 0 || newWidth > s_MaxFramebufferSize || newHeight > s_MaxFramebufferSize)
	{
		LOG(LogType::LOG_INFO, "Attempted to rezize framebuffer to %d, %d", width, height);
		return;
	}
	width = newWidth;
	height = newHeight;

    GenerateFrameBuffer();
}

void FrameBuffer::Clear(ClearBit flag, glm::vec4 color)
{
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GLCALL(glClearColor(color.r, color.g, color.b, color.a));
    GLCALL(glClear(static_cast<unsigned int>(flag)));
}

unsigned int FrameBuffer::GetAttachmentTexture(const std::string& name) const
{
    for (const auto& attachment : attachments)
    {
        if (attachment.name == name)
            return attachment.textureId;
    }
    return 0;
}