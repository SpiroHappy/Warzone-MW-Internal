#pragma once
#include"imgui/imgui.h"
#include "imgui_draw.h"











static void DarkMode(type::tESP esp_function, ImFont* font)
{
	ImGuiStyle& style = ImGui::GetStyle();
	const float bor_size = style.WindowBorderSize;
	style.WindowBorderSize = 0.0f;

	esp_function(font);
}

static void White(type::tESP esp_function,ImFont* font)
{
	ImGuiStyle& style = ImGui::GetStyle();
	const float bor_size = style.WindowBorderSize;
	style.WindowBorderSize = 0.0f;

	esp_function(font);
}

static void normal(type::tESP esp_function, ImFont* font)
{
	ImGuiStyle& style = ImGui::GetStyle();
	const float bor_size = style.WindowBorderSize;
	style.WindowBorderSize = 0.0f;

	esp_function(font);
}