#pragma once
#include "stdafx.h"
#include "imgui/imgui.h"

struct FrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	UINT64                  FenceValue;
};

namespace type
{
	typedef void(CALLBACK* tImguiStyle)();
	typedef void(CALLBACK* tESP)(ImFont*);
}

namespace imgui
{
	void reset_imgui_request();
	bool get_is_need_reset_imgui();

	ImFont* start(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue, type::tImguiStyle SetStyleFunction);
	bool IsReady();
	void clear();
	ImFont* add_font(const char* font_path, float font_size);
	void imgui_frame_header();
	void imgui_no_border(type::tESP esp_function, ImFont* font);
	void imgui_frame_end();
}