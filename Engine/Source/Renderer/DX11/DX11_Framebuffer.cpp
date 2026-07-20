#include "aio_pch.hpp"
#include "DX11_Framebuffer.hpp"
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
		// Create Framebuffer
		D3D11_TEXTURE2D_DESC texDesc = {};
		ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
		texDesc.Width = mSpec.width;
		texDesc.Height = mSpec.height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		HRESULT hr = mContext->GetDevice()->CreateTexture2D(&texDesc, nullptr, mBackBuffer.GetAddressOf());
		AIO_ASSERT(SUCCEEDED(hr), "Failed to create back buffer: " + ResultInfo(hr) + "\n");

		// Create the render target view for the color attachment
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = texDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		hr = mContext->GetDevice()->CreateRenderTargetView(mBackBuffer.Get(), &rtvDesc, mFrameBufferRTV.GetAddressOf());
		AIO_ASSERT(SUCCEEDED(hr), "Failed to create render target view: " + ResultInfo(hr) + "\n");

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = texDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		hr = mContext->GetDevice()->CreateShaderResourceView(mBackBuffer.Get(), &shaderResourceViewDesc, mColorAttachmentSRV.GetAddressOf());
		AIO_ASSERT(SUCCEEDED(hr), "Failed to create shader resource view: " + ResultInfo(hr) + "\n");
	}

	void DX11_Framebuffer::Bind()
	{
		mContext->GetDeviceContext()->OMSetRenderTargets(1, mFrameBufferRTV.GetAddressOf(), nullptr);

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

		mBackBuffer->Release();
		mColorAttachmentSRV->Release();
		mFrameBufferRTV->Release();

		Recreate();
	}

	void DX11_Framebuffer::ClearColor(const Vector4& color)
	{
		FLOAT bgColor[] = { color.r, color.g, color.b, color.a};
		mContext->GetDeviceContext()->ClearRenderTargetView(mFrameBufferRTV.Get(), bgColor);
	}
}


