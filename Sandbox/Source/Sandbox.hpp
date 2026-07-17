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
};