#include "aio_pch.hpp"

#if defined (AIO_WINDOWS)

#include "DX11_Context.hpp"
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

namespace aio
{
	DX11_Context::DX11_Context()
	{
	}

	DX11_Context::DX11_Context(HWND windowHandle, uint32_t width, uint32_t height)
	{
		mWindowHandle = windowHandle;
		mWindowWidth = width;
		mWindowHeight = height;

		DXGI_SWAP_CHAIN_DESC scd;
		ZeroMemory(&scd, sizeof(scd));
		scd.BufferCount = 2;
		scd.BufferDesc.Width = 0;
		scd.BufferDesc.Height = 0;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.OutputWindow = mWindowHandle;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		const D3D_FEATURE_LEVEL featureLevelArray[4] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };

		UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef AIO_DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // AIO_DEBUG

		HRESULT hr;
		hr = D3D11CreateDeviceAndSwapChain(nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			flags,
			featureLevelArray,
			2,
			D3D11_SDK_VERSION,
			&scd,
			mSwapChain.GetAddressOf(),
			mDevice.GetAddressOf(),
			&mFeatureLevel,
			mDeviceContext.GetAddressOf());
		AIO_ASSERT(SUCCEEDED(hr), "Failed to create device and swapChain: " + ResultInfo(hr) + "\n");

		CreateRenderTarget();

		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = mWindowWidth;
		viewport.Height = mWindowHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		mDeviceContext->RSSetViewports(1, &viewport);

		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;

		hr = mDevice->CreateRasterizerState(&rasterizerDesc, mRasterizerState.GetAddressOf());
		AIO_ASSERT(SUCCEEDED(hr), "Failed to create rasterizer state: " + ResultInfo(hr) + "\n");

		mDeviceContext->RSSetState(mRasterizerState.Get());

		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

		hr = mDevice->CreateBlendState(&blendDesc, mBlendState.GetAddressOf());
		AIO_ASSERT(SUCCEEDED(hr), "Failed to create blend state: " + ResultInfo(hr) + "\n");

		mDeviceContext->OMSetBlendState(mBlendState.Get(), nullptr, 0xffffff);

		mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), nullptr);
	}

	void DX11_Context::SwapChain()
	{
		mSwapChain->Present((UINT)mVSync, 0);
	}

	void DX11_Context::ImGuiBackendInit()
	{
		ImGui_ImplWin32_Init(mWindowHandle);
		ImGui_ImplDX11_Init(mDevice.Get(), mDeviceContext.Get());
	}

	void DX11_Context::ImGuiBackendBegin()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void DX11_Context::ImGuiBackendUpdate()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void DX11_Context::ImGuiBackendShutDown()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
	}

	void DX11_Context::CreateRenderTarget()
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(backBuffer.GetAddressOf()));
		mDevice->CreateRenderTargetView(backBuffer.Get(), NULL, mRenderTargetView.GetAddressOf());
	}
	
	void DX11_Context::CleanRenderTarget()
	{
		if (mRenderTargetView)
			mRenderTargetView->Release();
	}
	
	void DX11_Context::SetRenderTarget()
	{
		mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), nullptr);
	}

	void DX11_Context::ClearRenderTargetView(const Vector4& color)
	{
		const FLOAT clearColor[] = {color.r, color.g, color.b, color.a};
		mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), clearColor);
	}

	void DX11_Context::ResizeBuffer(const Vector2& size)
	{
		CleanRenderTarget();
		mSwapChain->ResizeBuffers(0, size.x, size.y, DXGI_FORMAT_UNKNOWN, 0);
		CreateRenderTarget();

		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (FLOAT)size.x;
		viewport.Height = (FLOAT)size.y;

		mDeviceContext->RSSetViewports(1, &viewport);
	}
}
#endif // AIO_WINDOWS