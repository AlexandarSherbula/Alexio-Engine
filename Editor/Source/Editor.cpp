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
    sceneCamera = CreateRef<Camera>(static_cast<float>(Application::Get().GetAppWindow()->GetSpecs().width) / static_cast<float>(Application::Get().GetAppWindow()->GetSpecs().height));
}

void EditorLayer::OnAttach()
{
	AIO_PROFILE_FUNCTION();

	TextureSpecification texSpec;
	Assets::Create<Texture>(texSpec, "awesomeface.png");
	Assets::Create<Texture>(texSpec, "AlexioLogo.png");

    fbSpec.width = Application::Get().GetAppWindow()->GetSpecs().width;
    fbSpec.height = Application::Get().GetAppWindow()->GetSpecs().height;

    framebuffer = Framebuffer::Create(fbSpec);
}

void EditorLayer::OnUpdate()
{
	AIO_PROFILE_FUNCTION();

    if (mViewportSize.x > 0.0f && mViewportSize.y > 0.0f && // zero sized framebuffer is invalid
        (fbSpec.width != mViewportSize.x || fbSpec.height != mViewportSize.y))
    {
        framebuffer->Resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
        //sceneCamera->UpdateProjection(mViewportSize.x / mViewportSize.y);
    }

    framebuffer->Bind();
    framebuffer->ClearColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
    sceneCamera->OnUpdate(AppTimer::DeltaTime());
    
    Renderer::DrawLinedQuad(Vector2(-0.3f, -0.9f), Vector2(0.4f, 0.4f), Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    Renderer::DrawQuad(Vector2(-1.0f, -0.7f), Vector2(0.5f, 0.5f), Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    Renderer::DrawRotatedQuad(Vector2(1.0f, -0.8f), Vector2(0.5f, 0.5f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), -3.0f * AppTimer::GetElapsedTime());
    
    Renderer::DrawSprite(Assets::Get<Texture>("AlexioLogo"), Vector2(-0.5f, -0.5f), Vector2(1.0f, 1.0f));
    Renderer::DrawRotatedSprite(Assets::Get<Texture>("awesomeface"), Vector2(0.3f, -0.8f), Vector2(0.5f, 0.5f), Vector4(1.0f), AppTimer::GetElapsedTime());
    
    Renderer::DrawCircle(Vector2(-1.3f, -0.5f), Vector4(1.0f, 0.5f, 0.0f, 1.0f), 0.25f);
    
    Renderer::DrawLinedTriangle(Vector2(-1.5f, 0.9f), Vector2(-0.5f, 0.9f), Vector2(-1.0f, 0.0f), Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    Renderer::DrawTriangle(Vector2(1.5f, 0.9f), Vector2(0.5f, 0.9f), Vector2(1.0f, 0.0f), Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    
    
    // Developer Note
    // With Batch Rendering system, Renderer::Draw functions are just adding rendering input rather than making actual drawcall. 
    // As a result, Renderer::Flush, which does make drawcall has to be called before framebuffer is unbind
    Renderer::Flush();
    
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
    
    if (ImGui::Begin("DockSpace", &dockspace_open, window_flags))
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
    
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit")) 
                    Application::Get().Stop();
                ImGui::EndMenu();
            }
    
        }
        ImGui::EndMenuBar();
    
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        if (ImGui::Begin("Viewport"))
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
        ImGui::PopStyleVar();
        ImGui::End();
    
        if (ImGui::Begin("Inspector"))
        {

        }
        ImGui::End();
    }
    ImGui::End();
}

void EditorLayer::OnEvent(aio::Event& event)
{
}
