#pragma once

#include "Core/Layer.hpp"
#include "Renderer/GraphicsContext.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace aio
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;
		void OnEvent(Event& e) override;

		void SetFonts();
		void SetDarkThemeColors();
		void BlockEvents(bool block) { mBlockEvents = block; }

		void Begin();
	private:
		bool mBlockEvents = true;

		Ref<GraphicsContext> mGraphicsContext;
	};
}