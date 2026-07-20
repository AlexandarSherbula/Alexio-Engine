#include "aio_pch.hpp"
#include "Framebuffer.hpp"

#include "Renderer.hpp"
#include "OpenGL/OpenGL_Framebuffer.hpp"
#include "DX11/DX11_Framebuffer.hpp"

namespace aio
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		CHECK_API
		(
			return CreateRef<OpenGL_Framebuffer>(spec),
			return CreateRef<DX11_Framebuffer>(spec)
		);

		return nullptr;
	}
}


