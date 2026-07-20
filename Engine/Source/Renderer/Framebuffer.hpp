#pragma once

#include "Utils/Utilities.hpp"
#include "Utils/Math.hpp"

namespace aio
{
    struct FramebufferSpecification
    {
        uint32_t width = 0;
        uint32_t height = 0;
    };

    class Framebuffer
    {
    public:
        virtual void Recreate() = 0;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void ClearColor(const Vector4& color) = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual void* GetColorAttachmentID() const = 0;
        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
    protected:
        FramebufferSpecification mSpec;
    };
}