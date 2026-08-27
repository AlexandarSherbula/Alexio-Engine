#pragma once

#include "Utils/Utilities.hpp"
#include "Utils/Math.hpp"

#include "Texture.hpp"

namespace aio
{
    struct FramebufferTextureSpecification
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(TextureFormat format) : Format(format) {}

        TextureFormat Format;

        TextureWrap SamplerWrap = TextureWrap::Repeat;
		TextureFilter SamplerFilter = TextureFilter::Linear;
    };

    struct FramebufferAttachmentSpecification
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification>& textureSpecs)
            : TextureSpecifications(textureSpecs) {
        }

        std::vector<FramebufferTextureSpecification> TextureSpecifications;
    };

    struct FramebufferSpecification
    {
        uint32_t width = 0;
        uint32_t height = 0;

        FramebufferAttachmentSpecification Attachments;
    };

    class Framebuffer
    {
    public:
        virtual void Recreate() = 0;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void ClearColor(const Vector4& color) = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual void* GetColorAttachmentID(uint32_t index = 0) const = 0;
        virtual uint32_t ReadPixel(const Vector2& mousePos) = 0;
        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
    protected:
        FramebufferSpecification mSpec;
    };
}