#pragma once

#include "Renderer/Framebuffer.hpp"

#if defined (AIO_WINDOWS)
#include <d3d11.h>
#include <wrl/client.h>

#include "DX11_Context.hpp"

namespace aio
{
    struct DX11_ColorAttachment
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTV;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
    };

    class DX11_Framebuffer : public Framebuffer
    {
    public:
        DX11_Framebuffer(const FramebufferSpecification& spec);

        void Recreate() override;

        void Bind() override;
        void Unbind() override;

        void Resize(uint32_t width, uint32_t height) override;
        void ClearColor(const Vector4& color) override;

        inline void* GetColorAttachmentID(uint32_t index = 0) const override { return mColorAttachments[index].SRV.Get(); }
        int32_t ReadPixel(const Vector2& mousePos, int32_t index) override;
    private:
        std::vector<DX11_ColorAttachment> mColorAttachments;
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

        void ClearColor(const Vector4& color) override {}

        void Resize(uint32_t width, uint32_t height) override {}

        inline void* GetColorAttachmentID(uint32_t index = 0) const override { return nullptr; }
        uint32_t ReadPixel(const Vector2& mousePos) override { return 0; }
    };
}

#endif