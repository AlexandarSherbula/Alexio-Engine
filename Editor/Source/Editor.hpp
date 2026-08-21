#pragma once

#include "Alexio.hpp"
#include "Renderer/EditorCamera.hpp"

#include "Panels/SceneHierarchyPanel.hpp"
#include "ImGuizmo.h"

using namespace aio;


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
	Vector2 mViewportSize = { 0.0f, 0.0f };
	FramebufferSpecification fbSpec;
	Ref<Framebuffer> framebuffer;

	SceneHierarchyPanel mSceneHierarchyPanel;
	Ref<Scene> currentScene;
	Ref<EditorCamera> mEditorCamera;

	enum class SceneState
	{
		Editor,
		Runtime
	};

	SceneState mSceneState;

};