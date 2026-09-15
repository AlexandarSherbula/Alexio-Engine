#pragma once

#include "Utils/Utilities.hpp"
#include "Utils/Math.hpp"

#include "Texture.hpp"

namespace aio
{
    struct FramebufferTextureConfiguration
    {
        FramebufferTextureConfiguration() = default;
        FramebufferTextureConfiguration(TextureFormat format) : Format(format) {}

        TextureFormat Format;

        TextureWrap SamplerWrap = TextureWrap::Repeat;
		TextureFilter SamplerFilter = TextureFilter::Linear;
    };

    struct FramebufferAttachmentConfiguration
    {
        FramebufferAttachmentConfiguration() = default;
        FramebufferAttachmentConfiguration(const std::initializer_list<FramebufferTextureConfiguration>& textureCFGs)
            : TextureConfigurations(textureCFGs) {
        }

        std::vector<FramebufferTextureConfiguration> TextureConfigurations;
    };

    struct FramebufferSpecification
    {
        uint32_t width = 0;
        uint32_t height = 0;

        FramebufferAttachmentConfiguration Attachments;
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
        virtual int32_t ReadPixel(const Vector2& mousePos, int32_t index) = 0;
        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
    protected:
        FramebufferSpecification mSpec;
    };
}