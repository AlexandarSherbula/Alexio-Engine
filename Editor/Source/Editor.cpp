#include "Editor.hpp"
#include "Scripts/CameraController.hpp"

#include <string>
#include <sstream>
#include <fstream>

Editor::Editor()
{
	
}

Editor::Editor(AppSpecifications& appSpecs)
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

    currentScene = CreateRef<Scene>();

    ent_blueSquare = currentScene->CreateEntity("Blue Square");
    ent_blueSquare.AddComponent<SpriteComponent>(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    ent_blueSquare.GetComponent<TransformComponent>().Position = { -5.0f, 0.0f, 0.0f };
    ent_blueSquare.GetComponent<TransformComponent>().Scale = { 2.0f, 2.0f, 1.0f };

    ent_redSquare = currentScene->CreateEntity("Red Square");
    ent_redSquare.AddComponent<SpriteComponent>(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    ent_redSquare.GetComponent<TransformComponent>().Position = { 5.0f, 0.0f, 0.0f };
    ent_redSquare.GetComponent<TransformComponent>().Scale = { 2.0f, 2.0f, 1.0f };

    ent_primaryCamera = currentScene->CreateEntity("Primary Camera");
    CameraComponent& cam = ent_primaryCamera.AddComponent<CameraComponent>();
    cam.Primary = true;

    ent_primaryCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();

    fbSpec.width = Application::Get().GetAppWindow()->GetSpecs().width;
    fbSpec.height = Application::Get().GetAppWindow()->GetSpecs().height;

    framebuffer = Framebuffer::Create(fbSpec);

    mSceneHierarchyPanel.SetContext(currentScene);
}

void EditorLayer::OnUpdate()
{
	AIO_PROFILE_FUNCTION();

    if (mViewportSize.x > 0.0f && mViewportSize.y > 0.0f && // zero sized framebuffer is invalid
        (fbSpec.width != mViewportSize.x || fbSpec.height != mViewportSize.y))
    {
        framebuffer->Resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
        currentScene->OnViewportResize(mViewportSize.x, mViewportSize.y);
    }

    framebuffer->Bind();
    framebuffer->ClearColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
    
    currentScene->OnUpdate();

    framebuffer->Unbind();
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
    
    ImGui::Begin("DockSpace", &dockspace_open, window_flags);
    {
        if (!opt_padding)
            ImGui::PopStyleVar();
    
        if (opt_fullscreen)
            ImGui::PopStyleVar(2);
    
        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("DockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        else
        {
            AIO_LOG_ERROR("Docking isn't enabled");
        }
    
        ImGui::BeginMenuBar();
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit")) 
                    Application::Get().Stop();
                ImGui::EndMenu();
            }
    
        }
        ImGui::EndMenuBar();

        const char* apiName = "";
        CHECK_API(apiName = "OpenGL 4.5", apiName = "DirectX 11");

        ImGui::Begin("App Info");
        {
            ImGui::Text("Graphics API: %s", apiName);
            ImGui::Text("");
            ImGui::Text("Application Time: %.2f", AppTimer::GetElapsedTime());
            ImGui::Text("");
            ImGui::Text("Application framerate:");
            ImGui::Indent();
            ImGui::Text("%.3f ms / frame", 1000.0f / ImGui::GetIO().Framerate);
            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            ImGui::Unindent();
            ImGui::Text("");
            ImGui::Text("Rendering Stats:");
            ImGui::Indent();
            ImGui::Text("Lines: %d", Renderer::Stats.Lines);
            ImGui::Text("DrawLine: %d", Renderer::Stats.DrawLine);
            ImGui::Text("");
            ImGui::Text("Quads: %d", Renderer::Stats.Quads);
            ImGui::Text("DrawQuad: %d", Renderer::Stats.DrawQuad);
            ImGui::Text("");
            ImGui::Text("Circles: %d", Renderer::Stats.Circles);
            ImGui::Text("DrawCircle: %d", Renderer::Stats.DrawCircle);
            ImGui::Text("");
            ImGui::Unindent();

        }
        ImGui::End();
    
        ImGui::Begin("Viewport");
        {
            mViewportFocused = ImGui::IsWindowFocused();
            mViewportHovered = ImGui::IsWindowHovered();
            Application::Get().GetImGuiLayer()->BlockEvents(!mViewportFocused);
    
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
    
            ImVec2 uv0 = Renderer::CheckAPI() == GraphicsAPI::OpenGL ? ImVec2(0, 1) : ImVec2(0, 0); // Top-left UV coordinate
            ImVec2 uv1 = Renderer::CheckAPI() == GraphicsAPI::OpenGL ? ImVec2(1, 0) : ImVec2(1, 1); // Bottom-right UV coordinate
            ImGui::Image(framebuffer->GetColorAttachmentID(), ImVec2(mViewportSize.x, mViewportSize.y), uv0, uv1);
    
        }
        ImGui::End();

        mSceneHierarchyPanel.OnImGuiRender();
    }
    ImGui::End();
}

void EditorLayer::OnEvent(Event& event)
{
}
