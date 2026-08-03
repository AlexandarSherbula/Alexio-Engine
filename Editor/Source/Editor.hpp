#pragma once

#include "Alexio.hpp"

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

	bool mViewportFocused = false, mViewportHovered = false;
private:
	Ref<Framebuffer> framebuffer;
	FramebufferSpecification fbSpec;

	Ref<Scene> currentScene;
	Entity ent_primaryCamera;
	Entity ent_blueSquare;
	Entity ent_redSquare;
};