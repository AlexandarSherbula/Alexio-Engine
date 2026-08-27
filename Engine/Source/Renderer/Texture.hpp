#pragma once

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

	struct TextureSpecification
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

		inline uint32_t GetWidth() const { return mSpecification.Width; }
		inline uint32_t GetHeight() const { return mSpecification.Height; }
		inline uint32_t GetID() const { return mID; }

		virtual void Bind(uint32_t slot) const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		inline TextureSpecification& GetSpecs() { return mSpecification; }

		static Ref<Texture> Create(const TextureSpecification& specification, const std::filesystem::path& filepath = "", std::string name = "");
		static Ref<Texture> CreateAsset(const TextureSpecification& specification, const std::string& imageFile = "", std::string name = "");

		static Ref<Texture> Get(const std::string& name);
		static void Add(const Ref<Texture>& texture, std::string name = "");
		static bool Exists(const std::string& name);
	protected:
		TextureSpecification mSpecification;
		uint32_t mID;
		std::string mName;

		static std::unordered_map<std::string, Ref<Texture>> sTextures;
	};
}
