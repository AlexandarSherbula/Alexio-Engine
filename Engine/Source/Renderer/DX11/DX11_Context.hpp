#pragma once

#include "Renderer/GraphicsContext.hpp"

#if defined (AIO_WINDOWS)

#include "Window/Win32_Window.hpp"

namespace aio
{
	class DX11_Context : public GraphicsContext
	{
	public:
		DX11_Context();
		DX11_Context(HWND windowHandle, uint32_t width, uint32_t height);

		void SwapChain() override;

		void ImGuiBackendInit() override;
		void ImGuiBackendBegin() override;
		void ImGuiBackendUpdate() override;
		void ImGuiBackendShutDown() override;

		void CreateRenderTarget();
		void CleanRenderTarget();
		void SetRenderTarget();
		void ClearRenderTargetView(const Vector4& color);

		void ResizeBuffer(const Vector2& size);

		inline Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() const { return mDevice.Get(); }
		inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() const { return mDeviceContext.Get(); }

		D3D_FEATURE_LEVEL GetFeatureLevel() const { return mFeatureLevel; }
	private:
		HWND mWindowHandle;
		uint32_t mWindowWidth;
		uint32_t mWindowHeight;

		D3D_FEATURE_LEVEL mFeatureLevel;

		Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerState;
		Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendState;
	};
}

#else

namespace aio
{
	class DX11_Context : public GraphicsContext
	{
	public:
		DX11_Context();

		void SwapBuffers() override {}

		void ImGuiBackendInit() override     {}
		void ImGuiBackendBegin() override    {}
		void ImGuiBackendUpdate() override   {}
		void ImGuiBackendShutDown() override {}
	};
}

#endif