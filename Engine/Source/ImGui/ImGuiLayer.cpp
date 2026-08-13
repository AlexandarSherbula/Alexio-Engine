#include "aio_pch.hpp"

#include "ImGuiLayer.hpp"

#include "Core/Application.hpp"
#include "Window/SDL_Window.hpp"
#include "Utils/FileUtils.hpp"

#include <imgui.h>

namespace aio
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGui")
	{
	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows


		static std::filesystem::path iniFP = ASSETS_DIRECTORY / "Config" / "EditorLayout.ini";
		if (std::filesystem::exists(iniFP))
		{
			std::string iniFP_str = iniFP.string();
			io.IniFilename = iniFP_str.c_str();

			// Load previous layout
			ImGui::LoadIniSettingsFromDisk(io.IniFilename);
		}
		
		
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;		
		style.GrabRounding = 1.0f;
		style.FrameRounding = 2.0f;
		style.FrameBorderSize = 1.0f;
		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.WindowPadding = ImVec2(8, 8);
		style.ItemSpacing = ImVec2(6, 4);

		//SetFonts();
		io.FontDefault = AddFontFromFileTTF(ASSETS_DIRECTORY / "fonts" / "Roboto" / "static" / "Roboto-Regular.ttf", 18.0f);
		AddFontFromFileTTF(ASSETS_DIRECTORY / "fonts" / "Roboto" / "static" / "Roboto-Bold.ttf", 18.0f);

		SetDarkThemeColors();

		Application& app = Application::Get();
		Window* window = app.GetAppWindow().get();

		mGraphicsContext = window->GetContext();
		mGraphicsContext->ImGuiBackendInit();
	}

	void ImGuiLayer::Begin()
	{
		mGraphicsContext->ImGuiBackendBegin();
	}

	ImFont* ImGuiLayer::AddFontFromFileTTF(const std::filesystem::path& filepath, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges)
	{
		AIO_ASSERT(std::filesystem::exists(filepath), "Cannot get name from filepath " + filepath.string());

		ImGuiIO& io = ImGui::GetIO();

		return io.Fonts->AddFontFromFileTTF(filepath.string().c_str(), size_pixels, font_cfg, glyph_ranges);
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui::SaveIniSettingsToDisk(ImGui::GetIO().IniFilename);
		mGraphicsContext->ImGuiBackendShutDown();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		mGraphicsContext->ImGuiBackendUpdate();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (mBlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();

			e.Handled |= e.IsInCategory(EventCategoryMouse) && io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) && io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;

		// Metallic base
		colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
		
		// Headers
		colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.28f, 1.0f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.35f, 0.40f, 1.0f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.40f, 0.45f, 1.0f);
		
		// Buttons
		colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.25f, 1.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.32f, 0.32f, 0.35f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.45f, 1.0f);
		
		// Frame BG
		colors[ImGuiCol_FrameBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		colors[ImGuiCol_FrameBgActive]  = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);

		// Borders
		colors[ImGuiCol_Border] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
		
		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		
		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Metallic accent (Unreal blue)
		colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.45f, 0.90f, 1.0f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.20f, 0.45f, 0.90f, 1.0f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.55f, 1.00f, 1.0f);
	}
}