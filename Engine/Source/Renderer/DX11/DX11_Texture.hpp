#pragma once

#include "Renderer/Texture.hpp"
#if defined(AIO_WINDOWS)

#include "DX11_Context.hpp"

#include <d3d11.h>
#include <wrl/client.h>

namespace aio
{
	DXGI_FORMAT ConvertToDXGIFormat(TextureFormat format);
	D3D11_FILTER ConvertToDXFilter(TextureFilter filter);
	D3D11_TEXTURE_ADDRESS_MODE ConvertToDXWrap(TextureWrap wrap);

	class DX11_Texture : public Texture
	{
	public:
		DX11_Texture(const TextureSpecification& specification, const std::filesystem::path& filepath, std::string name = "");
		~DX11_Texture();

		void Bind(uint32_t slot) const override;
		void Unbind() const override;

		void SetData(const void* data, uint32_t size) override;
	private:
		static uint32_t sID;
	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureResource;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mTextureBuffer;

		Ref<DX11_Context> mContext;
	};
}
#else
namespace aio
{
	class DX11_Texture : public Texture
	{
	public:
		DX11_Texture(uint32_t width, uint32_t height, uint32_t whiteTexture) {}
		DX11_Texture(const std::string& filepath) {}
		~DX11_Texture() {}
	
		void Bind(uint32_t slot) const override {}
		void Unbind() const override {}
	
		void SetData(const void* data, uint32_t size) override {}
	};
}
#endif