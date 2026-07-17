#include "Editor.hpp"

#include <string>
#include <sstream>
#include <fstream>

using namespace aio;

Editor::Editor()
{
	
}

Editor::Editor(aio::AppSpecifications& appSpecs)
	: Application (appSpecs)
{
	PushLayer(new EditorLayer());
}

Editor::~Editor()
{
}

void Editor::Start()
{
}

EditorLayer::EditorLayer()
	: Layer("Editor")
{
}

void EditorLayer::OnAttach()
{
	AIO_PROFILE_FUNCTION();

	TextureSpecification texSpec;
	Assets::Create<Texture>(texSpec, "awesomeface.png");
	Assets::Create<Texture>(texSpec, "AlexioLogo.png");
}

static bool show_window = true;
ImGuiWindowFlags window_flags;

void EditorLayer::OnUpdate()
{
	AIO_PROFILE_FUNCTION();

	if (Input::GetKeyboard()->IsPressed(F4))
		Application::Get().GetAppWindow()->SetFullScreen();
	
    Renderer::ClearColor();
}



void EditorLayer::OnImGuiRender()
{
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static bool dockspace_open = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit")) Application::Get().Stop();
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void EditorLayer::OnEvent(aio::Event& event)
{
}
