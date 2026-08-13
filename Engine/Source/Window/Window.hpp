#pragma once

#include "Utils/Utilities.hpp"
#include "Utils/Math.hpp"
#include "Events/Events.hpp"
#include "Input/Input.hpp"
#include "Renderer/GraphicsContext.hpp"

#include <string>
#include <functional>

namespace aio
{
	using EventCallbackFn = std::function<void(Event&)>;

	struct WindowSpecifications
	{
		const char* title = "Alexio Engine";
		int32_t width = 1280;
		int32_t height = 720;
		int32_t pixelWidth = 1;
		int32_t pixelHeight = 1;
		bool vSync = true;
		bool isFullScreen = false;
		bool startMaximized = false;

		EventCallbackFn eventCallback;
	};

	class Window
	{
	public:
		virtual ~Window() = 0;
		virtual void Init() = 0;

		virtual void PollEvents() = 0;
		virtual void Update() = 0;

		virtual void* GetHandle() const = 0;

		virtual void SwapBuffers() = 0;
		
		virtual void SetFullScreen() = 0;
		virtual void PixelResize(uint32_t pixelSize) = 0;

		WindowSpecifications& GetSpecs() { return mSpecs; }
		const Ref<GraphicsContext>& GetContext() const { return mGraphicsContext; }
		inline bool IsFullScreen() const { return mSpecs.isFullScreen; }

		static Scope<Window> Create(const WindowSpecifications& windowSpec);
	protected:
		aio::Vector2i mProjectionSize;
		WindowSpecifications mSpecs;
		Ref<GraphicsContext> mGraphicsContext;
	};
}