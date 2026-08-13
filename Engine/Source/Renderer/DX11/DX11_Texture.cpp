#include "aio_pch.hpp"

#if defined (AIO_WINDOWS)
#include "DX11_Texture.hpp"
#include "Core/Application.hpp"
#include "stb_image.hpp"

#include "Utils/FileUtils.hpp"

namespace aio
{
	uint32_t DX11_Texture::sID = 1;

	static DXGI_FORMAT DXGIFormatFromImageFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::RED8UN:               return DXGI_FORMAT_R8_UNORM;
		case ImageFormat::RED8UI:               return DXGI_FORMAT_R8_UINT;
		case ImageFormat::RED16UI:              return DXGI_FORMAT_R16_UINT;
		case ImageFormat::RED32UI:              return DXGI_FORMAT_R32_UINT;
		case ImageFormat::RED32F:               return DXGI_FORMAT_R32_FLOAT;
		case ImageFormat::RG8:                  return DXGI_FORMAT_R8G8_UNORM;
		case ImageFormat::RG16F:                return DXGI_FORMAT_R16G16_FLOAT;
		case ImageFormat::RG32F:                return DXGI_FORMAT_R32G32_FLOAT;
		case ImageFormat::RGB:                  // DX11 lacks RGB-only
		case ImageFormat::RGBA:                 return DXGI_FORMAT_R8G8B8A8_UNORM;
		case ImageFormat::RGBA16F:              return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case ImageFormat::RGBA32F:              return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ImageFormat::B10R11G11UF:          return DXGI_FORMAT_R11G11B10_FLOAT;
		case ImageFormat::SRGB:                 
		case ImageFormat::SRGBA:                return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case ImageFormat::DEPTH32FSTENCIL8UINT: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		case ImageFormat::DEPTH32F:             return DXGI_FORMAT_D32_FLOAT;
		case ImageFormat::DEPTH24STENCIL8:      return DXGI_FORMAT_D24_UNORM_S8_UINT;
		default:                                return DXGI_FORMAT_UNKNOWN;
		}
	}

	static D3D11_FILTER DXFilterFromTextureFilter(TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::Linear:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		case TextureFilter::Nearest:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;

		default:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}
	}

	static D3D11_TEXTURE_ADDRESS_MODE DXWrapFromTextureWrap(TextureWrap wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Clamp:  return D3D11_TEXTURE_ADDRESS_CLAMP; 
		case TextureWrap::Repeat: return D3D11_TEXTURE_ADDRESS_WRAP;
		default:                  return D3D11_TEXTURE_ADDRESS_WRAP;
		}
	}

	DX11_Texture::DX11_Texture(const TextureSpecification& specification, const std::filesystem::path& filepath, std::string name)
	{
		mSpecification = specification;
		mID = sID;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGIFormatFromImageFormat(mSpecification.Format);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));

		sampDesc.Filter = DXFilterFromTextureFilter(mSpecification.SamplerFilter);
		sampDesc.AddressU = DXWrapFromTextureWrap(mSpecification.SamplerWrap);
		sampDesc.AddressV = DXWrapFromTextureWrap(mSpecification.SamplerWrap);
		sampDesc.AddressW = DXWrapFromTextureWrap(mSpecification.SamplerWrap);
		sampDesc.MipLODBias = 0.0f;
		sampDesc.MaxAnisotropy = 1;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampDesc.BorderColor[0] = 0;
		sampDesc.BorderColor[1] = 0;
		sampDesc.BorderColor[2] = 0;
		sampDesc.BorderColor[3] = 0;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		D3D11_SUBRESOURCE_DATA textureSubData;
		ZeroMemory(&textureSubData, sizeof(textureSubData));

		if (filepath == "")
		{
			mContext = std::dynamic_pointer_cast<DX11_Context>(Application::Get().GetAppWindow()->GetContext());

			textureDesc.Width = mSpecification.Width;
			textureDesc.Height = mSpecification.Height;

			ID3D11Texture2D* textureObject;
			ZeroMemory(&textureObject, sizeof(textureObject));

			uint32_t whiteTexture = 0xffffffff;

			textureSubData.pSysMem = &whiteTexture;
			textureSubData.SysMemPitch = mSpecification.Width * 4;

			HRESULT hr = mContext->GetDevice()->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());
			AIO_ASSERT(SUCCEEDED(hr), "Failed to create sampler state: " + ResultInfo(hr) + "\n");

			hr = mContext->GetDevice()->CreateTexture2D(&textureDesc, &textureSubData, mTextureBuffer.GetAddressOf());
			AIO_ASSERT(SUCCEEDED(hr), "Failed to create white texture: " + ResultInfo(hr) + "\n");

			hr = mContext->GetDevice()->CreateShaderResourceView(mTextureBuffer.Get(), nullptr, mTextureResource.GetAddressOf());
			AIO_ASSERT(SUCCEEDED(hr), "Failed to create texture shader resource view: " + ResultInfo(hr) + "\n");
		}
		else
		{
			int32_t width, height, channels;
			stbi_uc* data = nullptr;
			data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);

			mContext = std::dynamic_pointer_cast<DX11_Context>(Application::Get().GetAppWindow()->GetContext());

			if (data)
			{
				mName = name;
				mSpecification.Width = width;
				mSpecification.Height = height;

				textureDesc.Width = mSpecification.Width;
				textureDesc.Height = mSpecification.Height;

				textureSubData.pSysMem = data;
				textureSubData.SysMemPitch = mSpecification.Width * channels;

				HRESULT hr = mContext->GetDevice()->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());
				AIO_ASSERT(SUCCEEDED(hr), "Failed to create sampler state: " + ResultInfo(hr) + "\n");
				
				hr = mContext->GetDevice()->CreateTexture2D(&textureDesc, &textureSubData, mTextureBuffer.GetAddressOf());
				AIO_ASSERT(SUCCEEDED(hr), "Failed to create a texture: " + ResultInfo(hr) + "\n");

				hr = mContext->GetDevice()->CreateShaderResourceView(mTextureBuffer.Get(), nullptr, mTextureResource.GetAddressOf());
				AIO_ASSERT(SUCCEEDED(hr), "Failed to create texture shader resource view: " + ResultInfo(hr) + "\n");
			}
		}
		sID++;
	}

	DX11_Texture::~DX11_Texture()
	{
	}

	void DX11_Texture::Bind(uint32_t slot) const
	{
		mContext->GetDeviceContext()->PSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		mContext->GetDeviceContext()->PSSetShaderResources(slot, 1, mTextureResource.GetAddressOf());
	}

	void DX11_Texture::Unbind() const
	{
	}

	void DX11_Texture::SetData(const void* data, uint32_t size)
	{
	}
}
#endif