#include "aio_pch.hpp"
#include "OpenGL_Texture.hpp"

#include "stb_image.hpp"

#include "Utils/FileUtils.hpp"

namespace aio
{
	static GLenum GLFormatFromImageFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::RED8UN:               return GL_R8;
		case ImageFormat::RED8UI:               return GL_R8UI;
		case ImageFormat::RED16UI:              return GL_R16UI;
		case ImageFormat::RED32UI:              return GL_R32UI;
		case ImageFormat::RED32F:               return GL_R32F;
		case ImageFormat::RG8:                  return GL_RG8;
		case ImageFormat::RG16F:                return GL_RG16F;
		case ImageFormat::RG32F:                return GL_RG32F;
		case ImageFormat::RGB:                  return GL_RGB8;
		case ImageFormat::RGBA:                 return GL_RGBA8;
		case ImageFormat::RGBA16F:              return GL_RGBA16F;
		case ImageFormat::RGBA32F:              return GL_RGBA32F;
		case ImageFormat::B10R11G11UF:          return GL_R11F_G11F_B10F;
		case ImageFormat::SRGB:                 return GL_SRGB8;
		case ImageFormat::SRGBA:                return GL_SRGB8_ALPHA8;
		case ImageFormat::DEPTH32FSTENCIL8UINT: return GL_DEPTH32F_STENCIL8;
		case ImageFormat::DEPTH32F:             return GL_DEPTH_COMPONENT32F;
		case ImageFormat::DEPTH24STENCIL8:      return GL_DEPTH24_STENCIL8;
		default:                                return 0;
		}
	}

	static GLenum GLWrapFromTextureWrap(TextureWrap wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Clamp:  return GL_CLAMP_TO_EDGE;
		case TextureWrap::Repeat: return GL_REPEAT;
		default:                  return GL_CLAMP_TO_EDGE; // sensible fallback
		}
	}

	static GLenum GLFilterFromTextureFilter(TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::Linear:  return GL_LINEAR;
		case TextureFilter::Nearest: return GL_NEAREST;
		default:                     return GL_LINEAR;
		}
	}

	OpenGL_Texture::OpenGL_Texture(const TextureSpecification& specification, const std::filesystem::path& filepath, std::string name)
	{
		mSpecification = specification;

		if (filepath == "")
		{
			mUploadImageFormat = GL_RGBA;

			glCreateTextures(GL_TEXTURE_2D, 1, &mID);
			glTextureStorage2D(mID, 1, GLFormatFromImageFormat(mSpecification.Format), mSpecification.Width, mSpecification.Height);

			uint32_t whiteTexture = 0xffffffff;
			glTextureSubImage2D(mID, 0, 0, 0, mSpecification.Width, mSpecification.Height, mUploadImageFormat, GL_UNSIGNED_BYTE, &whiteTexture);
		}
		else
		{
			int32_t width, height, channels;
			stbi_uc* data = nullptr;
			data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);

			if (data)
			{
				mName = name;
				mSpecification.Width = width;
				mSpecification.Height = height;

				if (channels == 4)
				{
					mSpecification.Format = ImageFormat::RGBA;
					mUploadImageFormat = GL_RGBA;
				}
				else if (channels == 3)
				{
					mSpecification.Format = ImageFormat::RGB;
					mUploadImageFormat = GL_RGB;
				}

				AIO_ASSERT(GLFormatFromImageFormat(mSpecification.Format) & mUploadImageFormat, "Format not supported!");

				glCreateTextures(GL_TEXTURE_2D, 1, &mID);
				glTextureStorage2D(mID, 1, GLFormatFromImageFormat(mSpecification.Format), mSpecification.Width, mSpecification.Height);

				glTextureSubImage2D(mID, 0, 0, 0, mSpecification.Width, mSpecification.Height, mUploadImageFormat, GL_UNSIGNED_BYTE, data);

				stbi_image_free(data);
			}
		}

		glTextureParameteri(mID, GL_TEXTURE_MIN_FILTER, GLFilterFromTextureFilter(mSpecification.SamplerFilter));
		glTextureParameteri(mID, GL_TEXTURE_MAG_FILTER, GLFilterFromTextureFilter(mSpecification.SamplerFilter));

		glTextureParameteri(mID, GL_TEXTURE_WRAP_S, GLWrapFromTextureWrap(mSpecification.SamplerWrap));
		glTextureParameteri(mID, GL_TEXTURE_WRAP_T, GLWrapFromTextureWrap(mSpecification.SamplerWrap));
	}

	OpenGL_Texture::~OpenGL_Texture()
	{
		glDeleteTextures(1, &mID);
	}

	void OpenGL_Texture::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, mID);
	}

	void OpenGL_Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGL_Texture::SetData(const void* data, uint32_t size)
	{
		uint32_t bpp = mUploadImageFormat == GL_RGBA ? 4 : 3;
		AIO_ASSERT(size == mSpecification.Width * mSpecification.Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(mID, 0, 0, 0, mSpecification.Width, mSpecification.Height, mUploadImageFormat, GL_UNSIGNED_BYTE, data);
	}
}

