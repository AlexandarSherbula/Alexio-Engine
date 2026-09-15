#pragma once

#include "Window/Window.hpp"

#include "SDL3/SDL.h"

typedef SDL_Window SDLwindowHandler;

namespace aio
{
	class SDLWindow : public Window
	{
	public:
		SDLWindow(const WindowConfiguration& windowSpec);
		~SDLWindow();
		void Init() override;
		void PollEvents() override;
		void Update() override;

		inline void* GetHandle() const override { return mHandle; }
		void SetFullScreen() override;
		void PixelResize(uint32_t pixelSize) override;

		void SwapBuffers() override;
	private:
		aio::Vector2i mPosition;
		SDLwindowHandler* mHandle;
		
	};
}