#pragma once

#include "Renderer/Framebuffer.hpp"

namespace aio
{
    class OpenGL_Framebuffer : public Framebuffer
    {
    public:
        OpenGL_Framebuffer(const FramebufferSpecification& spec);

        void Recreate() override;

        void Bind() override;
        void Unbind() override;

        void ClearColor(const Vector4& color) override;

        void Resize(uint32_t width, uint32_t height) override;

        inline void* GetColorAttachmentID() const override { return reinterpret_cast<void*>(mColorAttachment); }
    private:
        uint32_t mID;
        uint32_t mColorAttachment, mDepthAttachment;
    };
}
