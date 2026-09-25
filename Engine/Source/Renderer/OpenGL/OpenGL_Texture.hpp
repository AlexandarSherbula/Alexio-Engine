#pragma once

#include "Renderer/Texture.hpp"
#include <cstdint>

namespace aio
{
	GLenum ConvertToGLFormat(TextureFormat format);
	GLenum ConvertToGLWrap(TextureWrap wrap);
	GLenum ConvertToGLFilter(TextureFilter filter);

	class OpenGL_Texture : public Texture
	{
	public:
		OpenGL_Texture(const std::filesystem::path& filepath, const TextureConfiguration& specification = {}, std::string name = "");

		~OpenGL_Texture();

		void Bind(uint32_t slot) const override;
		void Unbind() const override;

		void SetData(const void* data, uint32_t size) override;
		inline void* GetHandle() const override { return reinterpret_cast<void*>(mID); }
	private:
		GLenum mUploadTextureFormat;
	};
}