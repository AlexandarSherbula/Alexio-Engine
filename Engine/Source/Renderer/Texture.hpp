#pragma once

#include <filesystem>
#include <unordered_map>

namespace aio
{
	enum class TextureFormat 
	{
		None = 0, 
		RED8UN, 
		RED8I,
		RED8UI, 
		RED16I,
		RED16UI, 
		RED32I,
		RED32UI, 
		RED32F, 
		RG8, 
		RG16F, 
		RG32F, 
		RGB, 
		RGBA, 
		RGBA16F, 
		RGBA32F, 
		B10R11G11UF, 
		SRGB,
		SRGBA,
		DEPTH32FSTENCIL8UINT,
		DEPTH32F,
		DEPTH24STENCIL8,
		// Defaults
		Depth = DEPTH24STENCIL8, 
	};

	enum class TextureWrap
	{
		Clamp,
		Repeat
	};

	enum class TextureFilter
	{
		Linear,
		Nearest
	};

	struct TextureConfiguration
	{
		TextureFormat Format = TextureFormat::RGBA;
		uint32_t Width = 1;
		uint32_t Height = 1;
		TextureWrap SamplerWrap = TextureWrap::Repeat;
		TextureFilter SamplerFilter = TextureFilter::Linear;

		bool GenerateMips = true;
	};

	class Texture
	{
	public:
		virtual ~Texture() {}

		inline uint32_t GetWidth() const { return mCFG.Width; }
		inline uint32_t GetHeight() const { return mCFG.Height; }
		inline uint32_t GetID() const { return mID; }

		virtual void Bind(uint32_t slot) const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;
		virtual void* GetHandle() const = 0;

		inline TextureConfiguration& GetSpecs() { return mCFG; }

		static Ref<Texture> Create(const std::filesystem::path& filepath = "", const TextureConfiguration& cfg = {}, std::string name = "");
		static Ref<Texture> CreateAsset(const std::string& imageFile = "", const TextureConfiguration& cfg = {}, std::string name = "");

		static Ref<Texture> Get(const std::string& name);
		static void Add(const Ref<Texture>& texture, std::string name = "");
		static bool Exists(const std::string& name);
	protected:
		TextureConfiguration mCFG;
		uint32_t mID;
		std::string mName;

		static std::unordered_map<std::string, Ref<Texture>> sTextures;
	};
}
