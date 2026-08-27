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

        inline void* GetColorAttachmentID(uint32_t index = 0) const override { AIO_ASSERT(index < mColorAttachments.size());  return reinterpret_cast<void*>(mColorAttachments[index]); }
        int32_t ReadPixel(const Vector2& mousePos, int32_t index) override;
    private:
        uint32_t mID;
        std::vector<uint32_t> mColorAttachments, mDepthAttachments;
    };
}
