#include "Editor.hpp"
#include "Scripts/CameraController.hpp"

#include <string>
#include <sstream>
#include <fstream>

#include <nfd.h>

static ImGuizmo::OPERATION currentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE currentGizmoMode(ImGuizmo::LOCAL);
static bool isEditorView = true;

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

    fbSpec.width = Application::Get().GetAppWindow()->GetSpecs().width;
    fbSpec.height = Application::Get().GetAppWindow()->GetSpecs().height;
    fbSpec.Attachments.TextureSpecifications = { TextureFormat::RGBA , TextureFormat::RED32UI };

    mEditorCamera = EditorCamera(static_cast<float>(fbSpec.width / fbSpec.height));
    framebuffer = Framebuffer::Create(fbSpec);

    mSceneHierarchyPanel.SetContext(currentScene);

    mSceneView = SceneView::Editor;
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

    if (isEditorView)
        mSceneView = SceneView::Editor;
    else
        mSceneView = SceneView::Runtime;

    switch (mSceneView)
    {
        case SceneView::Editor:
        {
            if (ViewportHovered && ViewportFocused)
                mEditorCamera.OnMove();

            mEditorCamera.OnUpdate();

            currentScene->DrawEntities();

            break;
        }
        case SceneView::Runtime:
        {
            currentScene->OnUpdate();
            break;
        }
    }

    framebuffer->Unbind();
}

void EditorLayer::OnImGuiRender()
{
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDockingTransparentPayload = true;

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoDocking;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("EditorDockSpace", nullptr, flags);
    ImGui::PopStyleVar(3);
    {
        ImGuiID dockspaceID = ImGui::GetID("EditorDockSpaceID");
        ImGui::DockSpace(dockspaceID, ImVec2(0, 0), ImGuiDockNodeFlags_None);
    
        ImGui::BeginMenuBar();
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                {
                    NewScene();
                }

                if (ImGui::MenuItem("Load Scene", "Ctrl+O"))
                {
                    LoadScene();
                }

                if (ImGui::MenuItem("Save Scene", "Ctrl+Shift+S"))
                {
                    SaveScene();
                }

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
            ImGui::Checkbox("Editor View", &isEditorView);
            ImGui::Unindent();
        }
        ImGui::End();
    
        ImGui::Begin("Viewport");
        {
            ViewportFocused = ImGui::IsWindowFocused();
            ViewportHovered = ImGui::IsWindowHovered();
            Application::Get().GetImGuiLayer()->BlockEvents(ImGui::GetIO().WantTextInput);

            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

            ImVec2 uv0 = Renderer::CheckAPI() == GraphicsAPI::OpenGL ? ImVec2(0, 1) : ImVec2(0, 0); // Top-left UV coordinate
            ImVec2 uv1 = Renderer::CheckAPI() == GraphicsAPI::OpenGL ? ImVec2(1, 0) : ImVec2(1, 1); // Bottom-right UV coordinate
            ImGui::Image(framebuffer->GetColorAttachmentID(), ImVec2(mViewportSize.x, mViewportSize.y), uv0, uv1);

            ImVec2 viewportPos = ImGui::GetWindowPos();
            ImVec2 contentMin = ImGui::GetWindowContentRegionMin();

            float viewportX = viewportPos.x + contentMin.x;
            float viewportY = viewportPos.y + contentMin.y;

            float mouseX = Input::GetMouse()->GetPosition().x;
            float mouseY = Input::GetMouse()->GetPosition().y;

            float localX = mouseX - viewportX;
            float localY = mouseY - viewportY;

            // Flip Y because OpenGL framebuffer origin is bottom-left
            if (Renderer::CheckAPI() == GraphicsAPI::OpenGL)
                localY = mViewportSize.y - localY;

            framebuffer->ReadPixel({ localX, localY });

            if (mSceneView == SceneView::Editor)
            {
                Entity selectedEntity = mSceneHierarchyPanel.SelectedEntity;
                if (selectedEntity)
                {
                    ImGuizmo::SetOrthographic(true);
                    ImGuizmo::SetDrawlist();

                    ImVec2 windowPos = ImGui::GetWindowPos();
                    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
                    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

                    float x = windowPos.x + contentMin.x;
                    float y = windowPos.y + contentMin.y;
                    float w = contentMax.x - contentMin.x;
                    float h = contentMax.y - contentMin.y;

                    ImGuizmo::SetRect(x, y, w, h);

                    if (Input::GetKeyboard()->IsPressed(T))
                        currentGizmoOperation = ImGuizmo::TRANSLATE;
                    if (Input::GetKeyboard()->IsPressed(E))
                        currentGizmoOperation = ImGuizmo::ROTATE;
                    if (Input::GetKeyboard()->IsPressed(R))
                        currentGizmoOperation = ImGuizmo::SCALE;

                    bool snap = Input::GetKeyboard()->IsHeld(L_CTRL);
                    float snapValue = 0.5f;
                    if (currentGizmoOperation == ImGuizmo::OPERATION::ROTATE)
                        snapValue = 45.0f;

                    auto& entityTC = selectedEntity.GetComponent<TransformComponent>();
                    Mat4x4 entityTransform = entityTC.GetTransform();
                    ImGuizmo::Manipulate(glm::value_ptr(mEditorCamera.GetView()), glm::value_ptr(mEditorCamera.GetProjection()), currentGizmoOperation,
                        currentGizmoMode, glm::value_ptr(entityTransform), nullptr, snap ? &snapValue : nullptr);

                    if (ImGuizmo::IsUsing())
                    {
                        Vector3 translation, rotation, scale;

                        DecomposeTransform(entityTransform, translation, rotation, scale);

                        Vector3 deltaRotation = rotation - entityTC.Rotation;

                        entityTC.Position = translation;
                        entityTC.Rotation += deltaRotation;
                        entityTC.Scale = scale;
                    }
                }
            }

        }
        ImGui::End();

        mSceneHierarchyPanel.OnImGuiRender();

    }
    ImGui::End();
}

void EditorLayer::OnEvent(Event& event)
{
    if (ViewportHovered && ViewportFocused)
        mEditorCamera.OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(AIO_BIND_EVENT_FN(EditorLayer::OnKeyPressedEvent));
}

bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& e)
{
    bool control = Input::GetKeyboard()->IsHeld(KeyCode::L_CTRL) || Input::GetKeyboard()->IsHeld(KeyCode::R_CTRL);
    bool shift   = Input::GetKeyboard()->IsHeld(KeyCode::L_SHIFT) || Input::GetKeyboard()->IsHeld(KeyCode::R_SHIFT);

    switch (e.GetKeyCode())
    {
        case KeyCode::N:
        {
            if (control)
                NewScene();
            return true;
        }
        case KeyCode::O:
        {
            if (control)
                LoadScene();
            return true;
        }
        case KeyCode::S:
        {
            if (control && shift)
                SaveScene();
            return true;
        }
        case KeyCode::DEL:
        {
            mSceneHierarchyPanel.EntityDeleted = true;
            return true;
        }
        case KeyCode::F2:
        {
            mSceneHierarchyPanel.RenamingEntity = true;
            return true;
        }
    };

    return false;
}

void EditorLayer::NewScene()
{
    currentScene = CreateRef<Scene>();
    mSceneHierarchyPanel.SetContext(currentScene);
}

void EditorLayer::LoadScene()
{
    std::filesystem::path sceneFilePath = FileDialog::Open("yaml");
    if (!sceneFilePath.empty())
    {
        NewScene();

        SceneSerializer serializer(currentScene);
        serializer.Deserialize(sceneFilePath);
    }
}

void EditorLayer::SaveScene()
{
    std::filesystem::path sceneFilePath = FileDialog::Save("yaml");
    if (!sceneFilePath.empty())
    {
        SceneSerializer serializer(currentScene);
        serializer.Serialize(sceneFilePath);
    }
}