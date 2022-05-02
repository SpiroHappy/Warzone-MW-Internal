#include "stdafx.h"
#include "Menu.h"
#include "imgui/imgui.h"
# include "globals.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"
#include "obfuscator.hpp"
#include "xor.hpp"
#include"memory.h"
#include "unlockAll.h"

#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))
bool b_menu_open = false;
bool b_debug_open = false;
bool boxcheck;


uintptr_t cbuff1;
uintptr_t cbuff2;

__int64 find_pattern(__int64 range_start, __int64 range_end, const char* pattern) {
	const char* pat = pattern;
	__int64 firstMatch = NULL;
	__int64 pCur = range_start;
	__int64 region_end;
	MEMORY_BASIC_INFORMATION mbi{};
	while (sizeof(mbi) == VirtualQuery((LPCVOID)pCur, &mbi, sizeof(mbi))) {
		if (pCur >= range_end - strlen(pattern))
			break;
		if (!(mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_READWRITE))) {
			pCur += mbi.RegionSize;
			continue;
		}
		region_end = pCur + mbi.RegionSize;
		while (pCur < region_end)
		{
			if (!*pat)
				return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
				if (!firstMatch)
					firstMatch = pCur;
				if (!pat[1] || !pat[2])
					return firstMatch;

				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
					pat += 3;
				else
					pat += 2;
			}
			else {
				if (firstMatch)
					pCur = firstMatch;
				pat = pattern;
				firstMatch = 0;
			}
			pCur++;
		}
	}
	return NULL;
}


bool init_once = true;
char input[30];
bool Unlock_once = true;
namespace g_menu
{
	void menu()
	{

		if (GetAsyncKeyState(VK_INSERT) & 0x1)
		{
			b_menu_open = !b_menu_open;
		}

		if (init_once)
		{
			//init_buffer();
			init_once = false;
		}


		if (b_menu_open && screenshot::visuals)
		{
			ImGuiStyle* style = &ImGui::GetStyle();
			ImGui::Begin(xorstr("MENU"), &b_menu_open, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
			ImGui::SetWindowSize(ImVec2(560, 560), ImGuiCond_Always);
			style->WindowTitleAlign = ImVec2(0.5, 0.5);
			style->WindowPadding = ImVec2(15, 15);
			style->WindowRounding = 5.0f;
			style->FramePadding = ImVec2(5, 5);
			style->FrameRounding = 4.0f;
			style->ItemSpacing = ImVec2(12, 8);
			style->ItemInnerSpacing = ImVec2(8, 6);
			style->IndentSpacing = 25.0f;
			style->ScrollbarSize = 15.0f;
			style->ScrollbarRounding = 9.0f;
			style->GrabMinSize = 5.0f;
			style->GrabRounding = 3.0f;
			style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
			style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
			style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
			style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
			style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
			style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
			style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
			style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
			style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
			style->Colors[ImGuiCol_TitleBg] = ImColor(142, 30, 255, 100);
			style->Colors[ImGuiCol_TitleBgActive] = ImColor(142, 30, 255, 100);
			style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(142, 30, 255, 100);
			/*style->FramePadding = ImVec2(8, 6);
			style->Colors[ImGuiCol_WindowBg] = ImColor(69, 69, 69, 69);
			style->Colors[ImGuiCol_TitleBg] = ImColor(142, 30, 255, 100);
			style->Colors[ImGuiCol_TitleBgActive] = ImColor(142, 30, 255, 100);
			style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(142, 30, 255, 100);
			style->Colors[ImGuiCol_FrameBg] = ImColor(142, 30, 255, 100);
			style->Colors[ImGuiCol_FrameBgActive] = ImColor(106, 22, 191, 100);
			style->Colors[ImGuiCol_FrameBgHovered] = ImColor(199, 143, 255, 100);
			style->Colors[ImGuiCol_Header] = ImColor(199, 143, 255, 100);
			style->Colors[ImGuiCol_Header] = ImColor(106, 22, 191, 100);
			style->Colors[ImGuiCol_ButtonHovered] = ImColor(199, 143, 255, 100);*/
			ImGui::Text(xorstr("On GamePad:"), globals::gamepad ? "yes" : "no");
			ImGui::Text(xorstr("In Game: %s"), globals::b_in_game ? "yes" : "no");
			ImGui::Text(xorstr("Menu Key: INSERT"));
			//ImGui::Text(xorstr("refdef: %x"), globals::refdefadd);
			/*	ImGui::Text(xorstr("esp loop called: %d"), globals::call);
				ImGui::Text(xorstr("total players: %d"), globals::max_player_count);
				ImGui::Text(xorstr("local player: 0x%x"), globals::local_ptr);
				ImGui::Text(xorstr("local player alive: %s"), globals::local_is_alive ? "yes" : "no");
				*/

				/*ImGui::Text(xorstr("stance: %s"), globals::stance);
				ImGui::Text(xorstr("BitBlt hook: %s"), screenshot::bit_blt_fail);
				ImGui::Text(xorstr("BitBlt hook addr: %x"), screenshot::bit_blt_anotherlog);
				ImGui::Text(xorstr("BitBlt log: %d"), screenshot::bit_blt_log);
				ImGui::Text(xorstr("NtGdiStretchBlt hook: %s"), screenshot::GdiStretchBlt_fail);
				ImGui::Text(xorstr("NtGdiStretchBlt hook addr: %x"), screenshot::GdiStretchBlt_anotherlog);
				ImGui::Text(xorstr("NtGdiStretchBlt log: %d"), screenshot::GdiStretchBlt_log);
				ImGui::Text(xorstr("texture_copy hook addr: %x"), screenshot::texture_copy_log);*/
			ImGui::Text(xorstr("Fiz + Aoc Are Cute"));
			/*ImGui::Text(xorstr("virtquery hook addr:  0x%x"), screenshot::virtualqueryaddr);*/
			ImGui::Separator();

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			style->Colors[ImGuiCol_Tab] = ImColor(142, 30, 255, 100);
			style->Colors[ImGuiCol_TabHovered] = ImColor(106, 22, 191, 100);
			style->Colors[ImGuiCol_TabActive] = ImColor(142, 30, 255, 100);
			style->Colors[ImGuiCol_CheckMark] = ImColor(106, 22, 191, 100);
			ImGui::BeginTabBar("##tab1"); //beginTabBar needs to be called before BeginTabItem
			if (ImGui::BeginTabItem("ESP"))
			{
				style->Colors[ImGuiCol_SliderGrab] = ImColor(142, 30, 255, 100);
				style->Colors[ImGuiCol_SliderGrabActive] = ImColor(142, 30, 255, 100);
				style->Colors[ImGuiCol_Border] = ImColor(142, 30, 255, 100);
				ImGui::Checkbox(xorstr("Color Visibility"), &globals::b_visible);
				ImGui::Checkbox(xorstr("Show Only Visible"), &globals::b_visible_only);
				ImGui::Checkbox(xorstr("Show Box"), &globals::b_box);
				ImGui::SameLine();
				ImGui::Combo("##", &globals::box_index, globals::box_types, 2);

				ImGui::Checkbox(xorstr("Show HealthBar"), &globals::b_health);
				ImGui::Checkbox(xorstr("Show Line"), &globals::b_line);
				ImGui::Checkbox(xorstr("Show Bones "), &globals::b_skeleton);
				ImGui::Checkbox(xorstr("Show Aim Point"), &globals::b_aim_point);
				ImGui::Checkbox(xorstr("Show Names"), &globals::b_names);
				ImGui::Checkbox(xorstr("Show Distance"), &globals::b_distance);
				ImGui::Checkbox(xorstr("Show Team"), &globals::b_friendly);
				ImGui::SliderInt(xorstr("##MAXDISTANCE"), &globals::max_distance, 0, 1000, xorstr("ESP Distance: %d"));






				ImGui::EndTabItem();
			}

			/// <summary>
			/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/// </summary>
			if (ImGui::BeginTabItem("Loot"))
			{

				ImGui::Checkbox("Show Name ", &loot::name);
				ImGui::Checkbox("Show Distance ", &loot::distance);
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();
				ImGui::Checkbox("Assault Rifle ", &loot::ar);
				ImGui::Checkbox("LMG ", &loot::lmg);
				ImGui::Checkbox("SMG ", &loot::smg);
				ImGui::Checkbox("Shotgun ", &loot::shotgun);
				ImGui::Checkbox("Pistol ", &loot::pistol);
				ImGui::Checkbox("Assault Rifle Ammo ", &loot::ar_ammo);
				ImGui::Checkbox("Pistol/Smg Ammo ", &loot::smg_ammo);
				ImGui::Checkbox("Shotgun Ammo ", &loot::shotgun_ammo);
				ImGui::Checkbox("Sniper Ammo ", &loot::sniper_ammo);


				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Aimbot"))
			{

				ImGui::Checkbox(xorstr("Skip Knocked"), &globals::b_skip_knocked);
				ImGui::Checkbox(xorstr("Aim Lock"), &globals::b_lock);
				ImGui::Checkbox(xorstr("Prediction"), &globals::b_prediction);
				ImGui::SliderInt(xorstr("##LOCKSMOOTH"), &globals::aim_smooth, 1, 30, xorstr("Lock Smooth: %d"));
				//ImGui::SliderFloat(xorstr("##BULLETSPEED"), &globals::bullet_speed, 1, 5000, xorstr("Bulled Speed: %.1f"));
				ImGui::Checkbox(xorstr("Show FOV"), &globals::b_fov);
				if (globals::b_fov)
				{
					ImGui::SliderFloat(xorstr("##LOCKFOV"), &globals::f_fov_size, 10.f, 800.f, xorstr("FOV Size: %0.0f"));
				}

				ImGui::Checkbox(xorstr("Use Bones"), &globals::target_bone);
				if (globals::target_bone)
				{
					ImGui::Combo(xorstr("Lock Bone"), &globals::bone_index, globals::aim_lock_point, 4);
				}

				ImGui::EndTabItem();
			}

			/// <summary>
			/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/// </summary>

			if (ImGui::BeginTabItem("Misc"))
			{
				ImGui::Checkbox(xorstr("Show Crosshair"), &globals::b_crosshair);
				ImGui::Checkbox(xorstr("Infinite UAV"), &globals::b_UAV);
				ImGui::Checkbox(xorstr("No Recoil"), &globals::b_recoil);
				//	ImGui::Combo("Theme", &globals::theme, globals::theme_choose, 2);
				//	ImGui::Spacing();

					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//	ImGui::Text("Command: ");
				//	ImGui::SameLine();

				//	if (ImGui::InputText("##", input, sizeof(input) + 1, ImGuiInputTextFlags_EnterReturnsTrue ))
				//	{
				//																												//// Just for testing Commands
				//		std::string convert_to_string = std::string(input);
				//		CBuff(convert_to_string);
				//	}

					//
					/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				/*ImGui::Checkbox("Third Person Mode", &globals::third_person);
				if (globals::third_person)
				{
					CBuff("NOSLRNTRKL 1"); //Third Person Mode ON
				}
				else
				{
					CBuff("NOSLRNTRKL 0"); //Third Person Mode OFF
				}

				ImGui::Checkbox("FOV Scale", &globals::b_scale_Fov);
				if (globals::b_scale_Fov)
				{
					ImGui::SliderFloat("##Fov", &globals::fov, 1.2f, 2.0f, "Fov Scale: %f");

					std::string fov = "NSSLSNKPN " + std::to_string(globals::fov); // FOV SCALE

					CBuff(fov);
				}
				ImGui::Checkbox("Skip Warzone Tutorial", &globals::b_tut);
				if (globals::b_tut)
				{
					CBuff("LSPSKLPNQT 1");
				}
				else
					CBuff("LSPSKLPNQT 0");

				ImGui::Checkbox("Semi Unlock All", &globals::b_unlock);
				if (globals::b_unlock)
				{
					CBuff("set OLKMKMTKRO 1;");
				}
				else
					CBuff("set OLKMKMTKRO 0;");

				ImGui::Checkbox("Clear Fog", &globals::b_fog);
				if (globals::b_fog)
				{
					CBuff("set LSPSKLPNQT 1;");
				}
				else
					CBuff("set LSPSKLPNQT 0;");*/
				ImGui::EndTabItem();

			}
		}




		/// <summary>
		/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// </summary>

		if (ImGui::BeginTabItem("Colour"))
		{

			ImGui::Checkbox("RGB Mode", &globals::b_rainbow);
			ImGui::Text("Visible Team Color");
			ImGui::ColorEdit4("##esp Color1", (float*)&color::VisibleColorTeam);
			ImGui::Spacing();
			ImGui::Text("Not Visible Team Color");
			ImGui::ColorEdit4("##esp Color2", (float*)&color::NotVisibleColorTeam);
			ImGui::Spacing();
			ImGui::Text("Visible Enemy Color");
			ImGui::ColorEdit4("##esp Color3", (float*)&color::VisibleColorEnemy);
			ImGui::Spacing();
			ImGui::Text("Not Visible Enemy Color");
			ImGui::ColorEdit4("##esp Color4", (float*)&color::NotVisibleColorEnemy);
			ImGui::Text("loot Color");
			ImGui::ColorEdit4("##esp lootcolor", (float*)&loot::lootcolor);
			ImGui::EndTabItem(); //color tab end;

		}




		ImGui::EndTabBar(); // needs to be called at the end when using tabs
		ImGui::End();
	}

}