#include "aio_pch.hpp"
#include "OpenGL_Framebuffer.hpp"
#include "OpenGL_Backend.hpp"
#include "OpenGL_Texture.hpp"

#include "Input/Input.hpp"

namespace aio
{
	OpenGL_Framebuffer::OpenGL_Framebuffer(const FramebufferSpecification& spec)
	{
		mSpec = spec;
		Recreate();
	}

	void OpenGL_Framebuffer::Recreate()
	{
		if (mSpec.Attachments.TextureSpecifications.size())
		{
			mColorAttachments.resize(mSpec.Attachments.TextureSpecifications.size());

			glCreateFramebuffers(1, &mID);
			glBindFramebuffer(GL_FRAMEBUFFER, mID);

			for (size_t i = 0; i < mColorAttachments.size(); i++)
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &mColorAttachments[i]);
				glBindTexture(GL_TEXTURE_2D, mColorAttachments[i]);

				switch (mSpec.Attachments.TextureSpecifications[i].Format)
				{
				case TextureFormat::RGBA: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mSpec.width, mSpec.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); break;
				case TextureFormat::RED32I: glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, mSpec.width, mSpec.height, 0, GL_RED_INTEGER, GL_INT, nullptr); break;
				}

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ConvertToGLFilter(mSpec.Attachments.TextureSpecifications[i].SamplerFilter));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ConvertToGLFilter(mSpec.Attachments.TextureSpecifications[i].SamplerFilter));

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mColorAttachments[i], 0);
			}

			std::vector<GLenum> drawBuffers;
			for (size_t i = 0; i < mColorAttachments.size(); i++)
				drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);

			glDrawBuffers(drawBuffers.size(), drawBuffers.data());

			AIO_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		
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
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Clear COLOR ATTACHMENT 1 (ID buffer) to -1
		int clearValue = -1;
		glClearBufferiv(GL_COLOR, 1, &clearValue);
	}

	void OpenGL_Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		mSpec.width = width;
		mSpec.height = height;

		if (mID)
		{
			for (size_t i = 0; i < mColorAttachments.size(); i++)
			{
				glDeleteTextures(1, &mColorAttachments[i]);
			}
			glDeleteFramebuffers(1, &mID);
		}

		Recreate();
	}

	int32_t OpenGL_Framebuffer::ReadPixel(const Vector2& mousePos, int32_t index)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + index);

		int32_t pixel = -1;
		glReadPixels(mousePos.x, mousePos.y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);

		return pixel;
	}
}