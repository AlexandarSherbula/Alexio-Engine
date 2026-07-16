#include "aio_pch.hpp"
#include "Application.hpp"
#include "Renderer/Renderer.hpp"

#include "Utils/Timer.hpp"
#include "Renderer/Camera.hpp"

#include "Utils/FileReading.hpp"

namespace aio
{
	static Application* sInstance = nullptr;

	Ref<Camera> Application::sMainCamera = nullptr;

	Application::Application()
	{
	}

	Application::Application(AppSpecifications& appSpecs)
	{
		AIO_PROFILE_FUNCTION();
		Log::Init();

		sInstance = this;

		mRunning = true;

		mAppSpecs = appSpecs;

		SET_API(mAppSpecs.graphicsAPI);

		mAppSpecs.windowSpecs.eventCallback = AIO_BIND_EVENT_FN(Application::OnEvent);

		mAppWindow = Window::Create(mAppSpecs.windowSpecs);
		Input::Init();

		Renderer::Init();

		sMainCamera = CreateRef<Camera>(static_cast<float>(mAppSpecs.windowSpecs.width) / static_cast<float>(mAppSpecs.windowSpecs.height));

		imguiLayer = new ImGuiLayer();
		PushOverlay(imguiLayer);
	}

	Application::~Application()
	{		
		sInstance = nullptr;
	}

	void Application::Run()
	{
		AIO_PROFILE_FUNCTION();
		AppTimer::Start();
		while (mRunning)
		{
			AppTimer::Update();

			mAppWindow->PollEvents();
			Input::Scan();

			imguiLayer->Begin();

			sMainCamera->OnUpdate(AppTimer::DeltaTime());

			for (Layer* layer : mLayerStack)
				layer->OnUpdate();

			Renderer::Flush();

			for (Layer* layer : mLayerStack)
				layer->OnImGuiRender();

			mAppWindow->SwapBuffers();
		}

		Renderer::End();
		Input::Close();
	}

	void Application::Stop()
	{
		if (mRunning)
			mRunning = false;
	}

	Application& Application::Get()
	{
		AIO_ASSERT(sInstance, "Instance hasn't been made");
		return *sInstance;
	}

	void Application::PushLayer(Layer* layer)
	{
		mLayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		mLayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(AIO_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(AIO_BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<MouseMovedEvent>(AIO_BIND_EVENT_FN(Application::OnMouseMoved));
		dispatcher.Dispatch<MouseScrolledEvent>(AIO_BIND_EVENT_FN(Application::OnMouseScrolled));

		if (sMainCamera != nullptr)
			sMainCamera->OnEvent(e);

		for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); ++it)
		{
			(*it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		Renderer::OnWindowResize(e);
		return true;
	}

	bool Application::OnMouseMoved(MouseMovedEvent& e)
	{		
		Input::GetMouse()->SetPosition(Vector2(e.GetX(), e.GetY()));
		return true;
	}

	bool Application::OnMouseScrolled(MouseScrolledEvent& e)
	{
		Input::GetMouse()->NewMouseWheelState(Vector2(e.GetXOffset(), e.GetYOffset()));
		return true;
	}
}


