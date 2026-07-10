#include "aio_pch.hpp"
#include "Texture.hpp"

#if defined (AIO_LINUX)
	#define STBI_NO_SIMD
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

#include "Renderer/Renderer.hpp"
#include "OpenGL/OpenGL_Texture.hpp"
#include "DX11/DX11_Texture.hpp"

namespace aio
{
	std::unordered_map<std::string, Ref<Texture>> Texture::sTextures;

	Ref<Texture> Texture::Create(const TextureSpecification& specification, const std::filesystem::path& filepath, std::string name)
	{
		if (filepath != "")
		{
			if (std::filesystem::exists(filepath))
			{
				if (name == "")
					name = GetFileName(filepath);
			}
			else
			{
				AIO_LOG_ERROR("Filepath '{0}' doesn't exist", filepath.string());
				AIO_DEBUG_BREAK();
			}
		}

		CHECK_API(
			return CreateRef<OpenGL_Texture>(specification, filepath, name),
			return CreateRef<DX11_Texture>(specification, filepath, name)
		);

		return nullptr;
	}

	Ref<Texture> Texture::CreateAsset(const TextureSpecification& specification, const std::string& imageFile, std::string name)
	{
		std::filesystem::path textureFilePath = imageFile;

		if (imageFile != "")
		{
			textureFilePath = ASSETS_DIRECTORY / "images" / imageFile;

			AIO_ASSERT(std::filesystem::exists(textureFilePath), "File or filepath '" + textureFilePath.string() + "' not found");

			if (name == "")
				name = GetFileName(textureFilePath);

		}
		
		auto texture = Texture::Create(specification, textureFilePath, name);
		Add(texture, name);
		return texture;
	}

	Ref<Texture> Texture::Get(const std::string& name)
	{
		AIO_ASSERT(Exists(name), "Texture not found!");
		return sTextures[name];
	}

	void Texture::Add(const Ref<Texture>& texture, std::string name)
	{
		AIO_ASSERT(!Exists(name), "Texture already exists!");
		sTextures[name] = texture;
	}

	bool Texture::Exists(const std::string& name)
	{
		return sTextures.find(name) != sTextures.end();
	}
}


