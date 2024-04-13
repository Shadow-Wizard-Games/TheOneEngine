#include "Framebuffer.h"
#include "Log.h"

#include "GL/glew.h"

#include "../TheOneEngine/EngineCore.h"

static const unsigned int s_MaxFramebufferSize = 8192;

FrameBuffer::FrameBuffer(int newWidth, int newHeight, bool depth) : initialized(true), depthActive(depth)
{
	width = newWidth;
	height = newHeight;

	Reset(depth);
}

FrameBuffer::~FrameBuffer()
{
	if (initialized) {
		GLCALL(glDeleteTextures(1, &colorAttachment));
		GLCALL(glDeleteTextures(1, &depthAttachment));
		GLCALL(glDeleteFramebuffers(1, &FBO));
	}
}

void FrameBuffer::Bind(bool clear)
{
	GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
	GLCALL(glViewport(0, 0, width, height));
	if (clear) {
		GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	}
}

void FrameBuffer::Unbind()
{
	GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::Reset(bool depth)
{
	if (FBO)
	{
		GLCALL(glDeleteTextures(1, &colorAttachment));
		GLCALL(glDeleteTextures(1, &depthAttachment));
		GLCALL(glDeleteFramebuffers(1, &FBO));

		colorAttachment = 0;
		depthAttachment = 0;
	}

	// FRAMEBUFFER
	GLCALL(glCreateFramebuffers(1, &FBO));
	GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));

	// Color texture
	GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &colorAttachment));
	GLCALL(glBindTexture(GL_TEXTURE_2D, colorAttachment));

	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0));

	if (depth) {
		// Depth attachment
		GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &depthAttachment));
		GLCALL(glBindTexture(GL_TEXTURE_2D, depthAttachment));

		GLCALL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0));
	}

	// Check framebuffer status
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG(LogType::LOG_WARNING, "Framebuffer not completed");
	}
	else
		LOG(LogType::LOG_OK, "Framebuffer completed");

	GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	assert(status == GL_FRAMEBUFFER_COMPLETE); //36053
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

	Reset(depthActive);
}

void FrameBuffer::Clear(glm::vec4 color)
{
	GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
	GLCALL(glClearColor(color.r, color.g, color.b, color.a));
	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

void FrameBuffer::ClearBuffer(int value)
{
	GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
	//GLCALL(glClearTexImage(colorAttachment, 0, GL_RGBA8, GL_INT, &value));
	GLCALL(glGetString(GL_VERSION));
}

