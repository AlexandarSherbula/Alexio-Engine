#pragma once

#include "Alexio.hpp"

#include "Panels/SceneHierarchyPanel.hpp"

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

	Vector2 mViewportSize = { 0.0f, 0.0f };

	bool ViewportFocused = false, ViewportHovered = false;
private:
	FramebufferSpecification fbSpec;
	Ref<Framebuffer> framebuffer;

	Entity mSelectedEntity;
	SceneHierarchyPanel mSceneHierarchyPanel;
	Ref<Scene> currentScene;
};