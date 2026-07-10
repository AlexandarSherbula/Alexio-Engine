#pragma once

#include "Alexio.hpp"

class Sandbox : public aio::Application
{
public:
	Sandbox();
	Sandbox(aio::AppSpecifications& appSpecs);
	~Sandbox();

	void Start() override;
};

class MainLayer : public aio::Layer
{
public:
	MainLayer();

	void OnAttach() override;
	void OnUpdate() override;
	void OnImGuiRender() override;
	void OnEvent(aio::Event& event) override;
public:
	aio::Ref<aio::VertexBuffer> vb;
	aio::Ref<aio::IndexBuffer> ib;
	aio::Ref<aio::VertexInput> vi;
	aio::Ref<aio::Shader> shader;
	aio::Ref<aio::Texture> texture;
	aio::Ref<aio::Audio> audio;
};