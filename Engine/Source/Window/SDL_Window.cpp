#include "aio_pch.hpp"
#include "SDL_Window.hpp"

#include "Renderer/OpenGL/OpenGL_Context.hpp"

#include <backends/imgui_impl_sdl3.h>

namespace aio
{
	static void ProcessEvents(SDL_Event& sdl_event, aio::SDLWindow* window);


	SDLWindow::SDLWindow(const WindowSpecifications& windowSpec)
	{
		mSpecs.title = windowSpec.title;
		mSpecs.width = mProjectionSize.x = windowSpec.width;
		mSpecs.height = mProjectionSize.y = windowSpec.height;
		mSpecs.vSync = windowSpec.vSync;
		mSpecs.isFullScreen = windowSpec.isFullScreen;
		mSpecs.eventCallback = windowSpec.eventCallback;

		AIO_ASSERT(SDL_Init(SDL_INIT_VIDEO), "Failed to initalize SDL");

		mHandle = SDL_CreateWindow(mSpecs.title, mSpecs.width, mSpecs.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		AIO_ASSERT(mHandle, "Failed to create a window: {0}\n", SDL_GetError());

		if (mSpecs.isFullScreen)
		{
			// Store current window position and size
			SDL_GetWindowPosition(mHandle, &mPosition.x, &mPosition.y);
			SDL_GetWindowSize(mHandle, &mSpecs.width, &mSpecs.height);

			if (!SDL_SetWindowFullscreen(mHandle, true))
				AIO_LOG_ERROR("Failed to enable fullscreen: {0}", SDL_GetError());
		}

		mGraphicsContext = CreateRef<OpenGL_Context>(mHandle);
		mGraphicsContext->SetVSync(mSpecs.vSync);
	}

	SDLWindow::~SDLWindow()
	{
		if (mHandle != nullptr)
		{
			SDL_DestroyWindow(mHandle);
			SDL_Quit();
			mHandle = nullptr;
		}
	}

	void SDLWindow::Init()
	{
	}

	void SDLWindow::PollEvents()
	{
		SDL_Event sdl_event;
		while (SDL_PollEvent(&sdl_event))
		{
			ImGui_ImplSDL3_ProcessEvent(&sdl_event);

			ProcessEvents(sdl_event, this);
		}
	}

	void SDLWindow::Update()
	{
	}
	
	void SDLWindow::SetFullScreen()
	{
		mSpecs.isFullScreen = !mSpecs.isFullScreen;

		if (mSpecs.isFullScreen)
		{
			// Store current window position and size
			SDL_GetWindowPosition(mHandle, &mPosition.x, &mPosition.y);
			SDL_GetWindowSize(mHandle, &mSpecs.width, &mSpecs.height);

			if (!SDL_SetWindowFullscreen(mHandle, true))
				AIO_LOG_ERROR("Failed to enable fullscreen: {0}", SDL_GetError());
		}
		else
		{
			// Exit fullscreen
			if (!SDL_SetWindowFullscreen(mHandle, false))
				AIO_LOG_ERROR("Failed to disable fullscreen: {0}", SDL_GetError());

			// Restore previous position and size
			SDL_SetWindowPosition(mHandle, mPosition.x, mPosition.y);
			SDL_SetWindowSize(mHandle, mSpecs.width, mSpecs.height);
		}
	}

	void SDLWindow::PixelResize(uint32_t pixelSize)
	{
		if (!mSpecs.isFullScreen)
		{
			mSpecs.pixelWidth = pixelSize;
			mSpecs.pixelHeight = pixelSize;

			mSpecs.width = mProjectionSize.x * mSpecs.pixelWidth;
			mSpecs.height = mProjectionSize.y * mSpecs.pixelHeight;

			SDL_DisplayID displayId = SDL_GetPrimaryDisplay(); // or SDL_GetDisplayForWindow(window)

			SDL_Rect bounds;
			if (SDL_GetDisplayBounds(displayId, &bounds))
			{
				int32_t monitorWidth = bounds.w;
				int32_t monitorHeight = bounds.h;

				mPosition = { monitorWidth / 2.0f - mSpecs.width / 2.0f, monitorHeight / 2.0f - mSpecs.height / 2.0f };
			}
			else
			{
				AIO_LOG_ERROR("Failed to get display bounds: {0}", SDL_GetError());
				AIO_DEBUG_BREAK();
			}

			SDL_SetWindowPosition(mHandle, mPosition.x, mPosition.y);
			SDL_SetWindowSize(mHandle, mSpecs.width, mSpecs.height);
		}
	}

	void SDLWindow::SwapBuffers()
	{
		mGraphicsContext->SwapChain();
	}

	void ProcessEvents(SDL_Event& sdl_event, aio::SDLWindow* window)
	{
		switch (sdl_event.type)
		{
		case SDL_EVENT_WINDOW_RESIZED:
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		{
			if (sdl_event.window.windowID != SDL_GetWindowID(static_cast<SDLwindowHandler*>(window->GetHandle())))
				break;

			if (sdl_event.type == SDL_EVENT_WINDOW_RESIZED)
			{
				WindowResizeEvent event(sdl_event.window.data1, sdl_event.window.data2);
				window->GetSpecs().eventCallback(event);
				break;
			}

			if (sdl_event.window.windowID == SDL_GetWindowID(static_cast<SDLwindowHandler*>(window->GetHandle())))
			{
				WindowCloseEvent event;
				window->GetSpecs().eventCallback(event);
				break;
			}
		}
		case SDL_EVENT_KEY_DOWN:
		{
			KeyPressedEvent event(sdl_event.key.key, true);
			window->GetSpecs().eventCallback(event);
			size_t scancode = Keyboard::MapKeys[sdl_event.key.scancode];
			Input::GetKeyboard()->SetNewState(scancode, true);
			break;
		}
		case SDL_EVENT_KEY_UP:
		{
			KeyReleasedEvent event(sdl_event.key.key);
			window->GetSpecs().eventCallback(event);
			Input::GetKeyboard()->SetNewState(Keyboard::MapKeys[sdl_event.key.scancode], false);
			break;
		}
		case SDL_EVENT_TEXT_INPUT:
		{
			KeyTypedEvent event(sdl_event.key.key);
			window->GetSpecs().eventCallback(event);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			MouseButtonPressedEvent event(sdl_event.button.button);
			window->GetSpecs().eventCallback(event);
			Input::GetMouse()->SetNewState(Mouse::MapButtons[sdl_event.button.button], true);
			
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			MouseButtonReleasedEvent event(sdl_event.button.button);
			window->GetSpecs().eventCallback(event);
			Input::GetMouse()->SetNewState(Mouse::MapButtons[sdl_event.button.button], false);
			break;
		}
		case SDL_EVENT_MOUSE_MOTION:
		{
			MouseMovedEvent event(sdl_event.button.x, sdl_event.button.y);
			window->GetSpecs().eventCallback(event);
			break;
		}
		case SDL_EVENT_MOUSE_WHEEL:
		{
			MouseScrolledEvent event(sdl_event.wheel.x, sdl_event.wheel.y);
			window->GetSpecs().eventCallback(event);
			break;
		}
		default:
			Input::ProcessGamePadEvents(sdl_event);
			break;
		}
	}

}


