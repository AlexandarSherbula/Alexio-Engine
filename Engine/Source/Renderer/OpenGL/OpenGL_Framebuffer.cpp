#include "aio_pch.hpp"
#include "OpenGL_Framebuffer.hpp"
#include "OpenGL_Backend.hpp"

namespace aio
{
	OpenGL_Framebuffer::OpenGL_Framebuffer(const FramebufferSpecification& spec)
	{
		mSpec = spec;
		Recreate();
	}

	void OpenGL_Framebuffer::Recreate()
	{
		glCreateFramebuffers(1, &mID);
		glBindFramebuffer(GL_FRAMEBUFFER, mID);

		glCreateTextures(GL_TEXTURE_2D, 1, &mColorAttachment);
		glBindTexture(GL_TEXTURE_2D, mColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mSpec.width, mSpec.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorAttachment, 0);

		AIO_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mID);
		glViewport(0, 0, mSpec.width, mSpec.height);
	}

	void OpenGL_Framebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_Framebuffer::ClearColor(const Vector4& color)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGL_Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		mSpec.width = width;
		mSpec.height = height;

		if (mID)
		{
			glDeleteTextures(1, &mColorAttachment);
			glDeleteFramebuffers(1, &mID);
		}

		Recreate();
	}
}