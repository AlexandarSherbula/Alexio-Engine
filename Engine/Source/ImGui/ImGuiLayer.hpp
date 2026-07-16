#pragma once

#include "Core/Layer.hpp"
#include "Renderer/GraphicsContext.hpp"

#include <imgui.h>

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

		void SetDarkThemeColors();

		void Begin();
		void End();
	private:
		bool mBlockEvents = true;

		Ref<GraphicsContext> mGraphicsContext;
	};
}