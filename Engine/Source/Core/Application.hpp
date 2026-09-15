#pragma once

#include "Layer.hpp"
#include "ImGui/ImGuiLayer.hpp"

#include "Window/Window.hpp"
#include "Renderer/Renderer.hpp"
#include "Events/Events.hpp"
#include "Input/Input.hpp"

namespace aio
{
	struct AppConfiguration
	{
		WindowConfiguration windowCfg;

		GraphicsAPI graphicsAPI = GraphicsAPI::OpenGL;
	};

	class Application
	{
	public:
		Application();
		Application(AppConfiguration& appCfg);
		~Application();

		virtual void Start() = 0;
		void Run();
		void Stop();

		static Application& Get();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline const Ref<Window>& GetAppWindow() const { return mAppWindow; }
		inline ImGuiLayer* GetImGuiLayer() { return mImGuiLayer; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseScrolled(MouseScrolledEvent& e);
	private:
		bool mRunning;
		AppConfiguration mappCfg;

		Ref<Window> mAppWindow;
		LayerStack mLayerStack;
		ImGuiLayer* mImGuiLayer;
	private:
	};
}
