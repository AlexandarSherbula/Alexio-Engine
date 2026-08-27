#pragma once

#include "Alexio.hpp"
#include "Renderer/EditorCamera.hpp"

#include "Panels/SceneHierarchyPanel.hpp"
#include "ImGuizmo.h"

using namespace aio;

struct PanelSpecification
{
	Vector2 position = {0.0f, 0.0f};
	Vector2 size = { 0.0f, 0.0f };
	Vector2 regionMin = { 0.0f, 0.0f };
	Vector2 regionMax = { 0.0f, 0.0f };
};


class Editor : public Application
{
public:
	Editor();
	Editor(AppSpecifications& appSpecs);
	~Editor();

	void Start() override;
};

class EditorLayer : public Layer
{
public:
	EditorLayer();

	void OnAttach() override;
	void OnUpdate() override;
	void OnImGuiRender() override;
	void OnEvent(Event& event) override;
	bool OnKeyPressedEvent(KeyPressedEvent& event);
private:
	void NewScene();
	void LoadScene();
	void SaveScene();
public:
	bool ViewportFocused = false, ViewportHovered = false;
private:
	PanelSpecification mViewportSpec;
	FramebufferSpecification fbSpec;
	Ref<Framebuffer> framebuffer;

	SceneHierarchyPanel mSceneHierarchyPanel;
	Ref<Scene> currentScene;
	EditorCamera mEditorCamera;

	enum class SceneView
	{
		Editor,
		Runtime
	};

	SceneView mSceneView;

};