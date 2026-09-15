#include "aio_pch.hpp"

#include "SDL_Window.hpp"
#include "Win32_Window.hpp"

#include "Renderer/Renderer.hpp"

namespace aio
{
    Window::~Window()
    {
    }

    Scope<Window> Window::Create(const WindowConfiguration& windowSpec)
    {
        CHECK_API(
            return CreateScope<SDLWindow>(windowSpec),
            return CreateScope<Win32_Window>(windowSpec)
        );
        return nullptr;
    }
}