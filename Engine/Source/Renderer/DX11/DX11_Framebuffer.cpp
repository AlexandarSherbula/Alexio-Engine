#include "aio_pch.hpp"

#if defined (AIO_WINDOWS)
#include "DX11_Framebuffer.hpp"
#include "DX11_Texture.hpp"
#include "Core/Application.hpp"

namespace aio
{
	DX11_Framebuffer::DX11_Framebuffer(const FramebufferSpecification& spec)
	{
		mSpec = spec;
		mContext = std::dynamic_pointer_cast<DX11_Context>(Application::Get().GetAppWindow()->GetContext());
		Recreate();
	}

	void DX11_Framebuffer::Recreate()
	{
		if (mSpec.Attachments.TextureSpecifications.size())
		{
			mColorAttachments.resize(mSpec.Attachments.TextureSpecifications.size());

			for (size_t i = 0; i < mColorAttachments.size(); i++)
			{
				// Create Framebuffer
				D3D11_TEXTURE2D_DESC texDesc = {};
				ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
				texDesc.Width = mSpec.width;
				texDesc.Height = mSpec.height;
				texDesc.MipLevels = 1;
				texDesc.ArraySize = 1;
				texDesc.Format = ConvertToDXGIFormat(mSpec.Attachments.TextureSpecifications[i].Format);
				texDesc.SampleDesc.Count = 1;
				texDesc.SampleDesc.Quality = 0;
				texDesc.Usage = D3D11_USAGE_DEFAULT;
				texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				texDesc.CPUAccessFlags = 0;
				texDesc.MiscFlags = 0;

				HRESULT hr = mContext->GetDevice()->CreateTexture2D(&texDesc, nullptr, mColorAttachments[i].Texture.GetAddressOf());
				AIO_ASSERT(SUCCEEDED(hr), "Failed to create back buffer: " + ResultInfo(hr) + "\n");

				// Create the render target view for the color attachment
				D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
				rtvDesc.Format = texDesc.Format;
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D.MipSlice = 0;

				hr = mContext->GetDevice()->CreateRenderTargetView(mColorAttachments[i].Texture.Get(), &rtvDesc, mColorAttachments[i].RTV.GetAddressOf());
				AIO_ASSERT(SUCCEEDED(hr), "Failed to create render target view: " + ResultInfo(hr) + "\n");

				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
				shaderResourceViewDesc.Format = texDesc.Format;
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
				shaderResourceViewDesc.Texture2D.MipLevels = 1;

				hr = mContext->GetDevice()->CreateShaderResourceView(mColorAttachments[i].Texture.Get(), &shaderResourceViewDesc, mColorAttachments[i].SRV.GetAddressOf());
				AIO_ASSERT(SUCCEEDED(hr), "Failed to create shader resource view: " + ResultInfo(hr) + "\n");
			}
		}
	}

	void DX11_Framebuffer::Bind()
	{
		std::vector<ID3D11RenderTargetView*> rtvs;
		rtvs.reserve(mColorAttachments.size());

		for (auto& attachment : mColorAttachments)
			rtvs.push_back(attachment.RTV.Get());

		mContext->GetDeviceContext()->OMSetRenderTargets(
			(UINT)rtvs.size(),
			rtvs.data(),
			nullptr);

		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (FLOAT)mSpec.width;
		viewport.Height = (FLOAT)mSpec.height;

		mContext->GetDeviceContext()->RSSetViewports(1, &viewport);
	}

	void DX11_Framebuffer::Unbind()
	{
		mContext->SetRenderTarget();
	}

	void DX11_Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		mSpec.width = width;
		mSpec.height = height;

		for (size_t i = 0; i < mColorAttachments.size(); i++)
		{
			mColorAttachments[i].Texture.Reset();
			mColorAttachments[i].SRV.Reset();
			mColorAttachments[i].RTV.Reset();
		}

		Recreate();
	}

	void DX11_Framebuffer::ClearColor(const Vector4& color)
	{
		FLOAT bgColor[] = { color.r, color.g, color.b, color.a};
		FLOAT idClear[] = { -1.0f, 0.0f, 0.0f, 0.0f };
		mContext->GetDeviceContext()->ClearRenderTargetView(mColorAttachments[0].RTV.Get(), bgColor);
		mContext->GetDeviceContext()->ClearRenderTargetView(mColorAttachments[1].RTV.Get(), idClear);
	}

	static bool TextureStagedForPixelReading = false;
	static D3D11_TEXTURE2D_DESC desc = {};
	static Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture;

	int32_t DX11_Framebuffer::ReadPixel(const Vector2& mousePos, int32_t index)
	{
		if (!TextureStagedForPixelReading ||
			desc.Width != mSpec.width ||
			desc.Height != mSpec.height)
		{
			mColorAttachments[index].Texture->GetDesc(&desc);

			desc.Usage = D3D11_USAGE_STAGING;
			desc.BindFlags = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.MiscFlags = 0;

			mContext->GetDevice()->CreateTexture2D(&desc, nullptr, &stagingTexture);

			if (!TextureStagedForPixelReading)
				TextureStagedForPixelReading = true;
		}

		mContext->GetDeviceContext()->CopyResource(
			stagingTexture.Get(),
			mColorAttachments[1].Texture.Get()
		);

		D3D11_MAPPED_SUBRESOURCE mapped = {};
		HRESULT hr = mContext->GetDeviceContext()->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mapped);

		AIO_ASSERT(SUCCEEDED(hr), "Failed to map staging texture: " + ResultInfo(hr) + "\n");

		int32_t* data = reinterpret_cast<int32_t*>(mapped.pData);

		// Compute pixel index
		int32_t pitch = mapped.RowPitch / sizeof(int32_t);
		int32_t pixel = data[static_cast<int32_t>(mousePos.y) * pitch + static_cast<int32_t>(mousePos.x)];

		mContext->GetDeviceContext()->Unmap(stagingTexture.Get(), 0);

		return pixel;
	}
}
#endif // AIO_WINDOWS