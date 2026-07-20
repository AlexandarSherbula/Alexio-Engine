#pragma once

#include "Alexio.hpp"

class Editor : public aio::Application
{
public:
	Editor();
	Editor(aio::AppSpecifications& appSpecs);
	~Editor();

	void Start() override;
};

class EditorLayer : public aio::Layer
{
public:
	EditorLayer();

	void OnAttach() override;
	void OnUpdate() override;
	void OnImGuiRender() override;
	void OnEvent(aio::Event& event) override;

	aio::Vector2 mViewportSize = { 0.0f, 0.0f };

	bool mViewportFocused = false, mViewportHovered = false;
private:
	aio::Ref<aio::Framebuffer> framebuffer;
	aio::FramebufferSpecification fbSpec;

	aio::Ref<aio::Camera> sceneCamera;
};