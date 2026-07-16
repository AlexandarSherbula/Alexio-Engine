#include "Sandbox.hpp"

#include <string>
#include <sstream>
#include <fstream>

using namespace aio;

Sandbox::Sandbox()
{
	
}

Sandbox::Sandbox(aio::AppSpecifications& appSpecs)
	: Application (appSpecs)
{
	PushLayer(new MainLayer());
}

Sandbox::~Sandbox()
{
}

void Sandbox::Start()
{
}

MainLayer::MainLayer()
	: Layer("Main")
{
}

void MainLayer::OnAttach()
{
	AIO_PROFILE_FUNCTION();

	TextureSpecification texSpec;
	Assets::Create<Texture>(texSpec, "awesomeface.png");
	Assets::Create<Texture>(texSpec, "AlexioLogo.png");

	//Assets::Create<Audio>("GreenHill1.ogg");
	//Assets::Get<Audio>("GreenHill1")->LoopInMiliseconds(17600.0f);
	//Assets::Get<Audio>("GreenHill1")->Play();
}

void MainLayer::OnUpdate()
{
	AIO_PROFILE_FUNCTION();

	if (Input::GetKeyboard()->IsPressed(F4))
		Application::Get().GetAppWindow()->SetFullScreen();

	Renderer::ClearColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));

	Renderer::DrawLinedQuad(Vector2(-0.3f, -0.9f), Vector2( 0.4f, 0.4f), Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	Renderer::DrawQuad(Vector2(-1.0f, -0.7f), Vector2(0.5f, 0.5f), Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	Renderer::DrawRotatedQuad(Vector2(1.0f, -0.8f), Vector2(0.5f, 0.5f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), -3.0f * AppTimer::GetElapsedTime());
	
	Renderer::DrawSprite(Assets::Get<Texture>("AlexioLogo"), Vector2(-0.5f, -0.5f), Vector2(1.0f, 1.0f));
	Renderer::DrawRotatedSprite(Assets::Get<Texture>("awesomeface"), Vector2( 0.3f, -0.8f), Vector2(0.5f, 0.5f), Vector4(1.0f), AppTimer::GetElapsedTime());
	
	Renderer::DrawCircle(Vector2(-1.3f, -0.5f), Vector4(1.0f, 0.5f, 0.0f, 1.0f), 0.25f);

	Renderer::DrawLinedTriangle(Vector2(-1.5f, 0.9f), Vector2(-0.5f, 0.9f), Vector2(-1.0f,  0.0f), Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	Renderer::DrawTriangle(Vector2( 1.5f, 0.9f), Vector2( 0.5f, 0.9f), Vector2( 1.0f,  0.0f), Vector4(1.0f, 1.0f, 0.0f, 1.0f));
}

void MainLayer::OnImGuiRender()
{

}

void MainLayer::OnEvent(aio::Event& event)
{
}
