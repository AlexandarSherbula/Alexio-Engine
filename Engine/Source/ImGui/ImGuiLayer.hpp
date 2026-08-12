#pragma once

#include "Core/Layer.hpp"
#include "Renderer/GraphicsContext.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <filesystem>

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
		void BlockEvents(bool block) { mBlockEvents = block; }

		void Begin();
	private:
		ImFont* AddFontFromFileTTF(const std::filesystem::path& filepath, float size_pixels = 0.0f, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);
	private:
		bool mBlockEvents = true;

		Ref<GraphicsContext> mGraphicsContext;
	};
}