#pragma once

#include "Layer.hpp"
#include "ImGui/ImGuiLayer.hpp"

#include "Window/Window.hpp"
#include "Renderer/Camera.hpp"
#include "Events/Events.hpp"
#include "Input/Input.hpp"

namespace aio
{
	struct AppSpecifications
	{
		const char* title;
		uint32_t width;
		uint32_t height;
		bool vSync = true;
	};

	class Application
	{
	public:
		Application();
		Application(AppSpecifications& appSpecs);
		~Application();

		virtual void Start() = 0;
		void Run();
		void Stop();

		static Application& Get();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline const Ref<Window>& GetAppWindow() const { return mAppWindow; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseScrolled(MouseScrolledEvent& e);
	private:
		bool mRunning;
		AppSpecifications mAppSpecs;

		Ref<Window> mAppWindow;
		LayerStack mLayerStack;
		ImGuiLayer* imguiLayer;
	private:
		static Ref<Camera> sMainCamera;
	};
}
