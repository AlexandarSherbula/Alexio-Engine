#pragma once

#include "Renderer/Framebuffer.hpp"

#if defined (AIO_WINDOWS)
#include <d3d11.h>
#include <wrl/client.h>

#include "DX11_Context.hpp"

namespace aio
{
    class DX11_Framebuffer : public Framebuffer
    {
    public:
        DX11_Framebuffer(const FramebufferSpecification& spec);

        void Recreate() override;

        void Bind() override;
        void Unbind() override;

        void Resize(uint32_t width, uint32_t height) override;
        void ClearColor(const Vector4& color) override;

        inline void* GetColorAttachmentID() const override { return mColorAttachmentSRV.Get(); }
    private:
        Microsoft::WRL::ComPtr<ID3D11Texture2D> mBackBuffer;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mFrameBufferRTV;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mColorAttachmentSRV;

        Ref<DX11_Context> mContext;
    };
}

#else

namespace aio
{
    class DX11_Framebuffer : public Framebuffer
    {
    public:
        DX11_Framebuffer(const FramebufferSpecification& spec) {}

        void Recreate() override {}

        void Bind() override {}
        void Unbind() override {}

        void Resize(uint32_t width, uint32_t height) override {}

        inline void* GetColorAttachmentID() const override { return nullptr; }
    };
}

#endif