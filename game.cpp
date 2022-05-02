#include "stdafx.h"
#include "game.h"
#include "sdk.h"
#include "globals.h"
#include "xor.hpp"
#include"UAV.h"
#include "gamepad.h"

#define M_PI   3.141592654f


struct Loot
{

	auto get_name() -> char*;

	auto is_valid() -> bool;

	auto get_position()->Vector3;

};

struct loot_definition_t
{
	char name[100];
	char text[50];
	bool* show = NULL;
};

auto Loot::get_name() -> char*
{
	auto ptr = *(uintptr_t*)(this + 0);
	if (is_bad_ptr(ptr))
		return NULL;

	return *(char**)(ptr);
};

auto Loot::is_valid() -> bool
{
	return *(char*)(this + 0xC8) == 1 && *(char*)(this + 0xC9) == 1;
};

auto Loot::get_position() -> Vector3
{
	return *(Vector3*)(this + 0x160);
};

auto get_loot_by_id(const uint32_t id) -> Loot*
{
	return (Loot*)(g_data::base + 0x1F08C710) + (static_cast<uint64_t>(id) * 0x250);

}

bool b_settings = true;
const loot_definition_t definitions[] =
{
	// check loot dump on the bottom for all items :)
	// a few examples are here:



		// Ammo
		{ "loot_ammo_pistol_smg", "SMG AMMO", &loot::smg_ammo },
		{ "ammo_box_ar", "AR AMMO", &loot::ar_ammo },
		{ "ammo_shotgun", "Shotgun AMMO", &loot::shotgun_ammo},
		{ "ammo_marksman_sniper", "SNIPER AMMO", &loot::sniper_ammo},
		/*{ "ammo_rocket", "ROCKET AMMO", &trr },*/

		{ "wm_weapon_root_s4_ar", "Assault Rifle", &loot::ar},
		{ "wm_weapon_root_s4_pi", "Pistol", &loot::pistol},
		{ "wm_weapon_root_s4_sm", "SMG", &loot::smg},
		{ "wm_weapon_root_s4_sn", "Sniper", &loot::sniper },
		{ "wm_weapon_root_s4_lm", "LMG", &loot::lmg },


};


CXBOXController GamePad1(1);
namespace g_game
{

	namespace fb
	{
		struct Vec2 { union { float v[2]; struct { float x; float y; }; }; };
		struct Vec3 { union { float v[3]; struct { float x; float y; float z; }; }; };
	struct Vec4 { public: union { float v[4]; struct { float x; float y; float z; float w; }; }; };
						static Vec4 asVec4V(float x, float y, float z, float w) { Vec4 out; out.x = x; out.y = y; out.z = z; out.w = w; return out; }
						struct Matrix4x4 { union { Vec4 v[4]; float m[4][4]; struct { Vec4 right; Vec4 up; Vec4 forward; Vec4 trans; }; }; };
						typedef Matrix4x4 LinearTransform;

						struct AxisAlignedBox
						{
							Vec4 min;
							Vec4 max;
						};

						struct LinearTransform_AABB
						{
						public:
							LinearTransform m_Transform;
							AxisAlignedBox m_Box;
							char _pad[0x200];
						};

	}

	namespace e_data
	{


		DWORD VisibleEnemyColor = 0xFFFFFF00;
		DWORD VisibleFriendlyColor = 0xFF9097FF;
		DWORD NotVisibleEnemyColor = 0xFFFF0000;
		DWORD NotVisibleFriendlyColor = 0xFF2E7600;

		sdk::refdef_t* refdef;
	}

	//Vector3 get_camera_location()
	//{
	//	return *(Vector3*)(*(uint64_t*)(g_data::base + view_port::camera_ptr) + view_port::camera_view);
	//}

	Vector3 get_camera_location()
	{
		float x = *(float*)(*(uint64_t*)(g_data::base + view_port::camera_ptr) + view_port::camera_view_x);
		float y = *(float*)(*(uint64_t*)(g_data::base + view_port::camera_ptr) + view_port::camera_view_y);
		float z = *(float*)(*(uint64_t*)(g_data::base + view_port::camera_ptr) + view_port::camera_view_z);

		return Vector3{ x, y, z };
	}



	//bool w2sTest(const Vector3& WorldPos, Vector2* ScreenPos)
	//{
	//	sdk::refdef_t* refdef = *(sdk::refdef_t**)sdk::get_refdef();

	//	if (is_bad_ptr(refdef))return false;

	//	Vector3 vLocal, vTrans;

	//	vLocal = WorldPos - refdef->ViewLocation;

	//	vTrans.x = vLocal.Dot(refdef->ViewAxis[1]);
	//	vTrans.y = vLocal.Dot(refdef->ViewAxis[2]);
	//	vTrans.z = vLocal.Dot(refdef->ViewAxis[0]);

	//	if (vTrans.z < 0.01f)
	//		return false;

	//	ScreenPos->x = ((refdef->Width / 2) * (1 - (vTrans.x / refdef->FovX / vTrans.z)));
	//	ScreenPos->y = ((refdef->Height / 2) * (1 - (vTrans.y / refdef->FovY / vTrans.z)));

	//	if (ScreenPos->x > refdef->Width || ScreenPos->y > refdef->Height || ScreenPos->x < 0 || ScreenPos->y < 0)
	//		return false;

	//	return true;
	//}



	bool WorldToScreen(const Vector3& WorldPos, Vector2* ScreenPos)
	{
		Vector3 ViewOrig = get_camera_location();
		auto refdef = e_data::refdef;
		Vector3 vLocal, vTransform;
		vLocal = WorldPos - ViewOrig;
		// get our dot products from viewAxis
		vTransform.x = vLocal.Dot(refdef->ViewAxis[1]);
		vTransform.y = vLocal.Dot(refdef->ViewAxis[2]);
		vTransform.z = vLocal.Dot(refdef->ViewAxis[0]);
		// make sure it is in front of us
		if (vTransform.z < 0.01f)
			return false;
		ScreenPos->x = ((refdef->Width / 2) * (1 - (vTransform.x / refdef->FovX / vTransform.z)));
		ScreenPos->y = ((refdef->Height / 2) * (1 - (vTransform.y / refdef->FovY / vTransform.z)));
		return true;
	}

	float get_distance(const Vector3& lhs, const Vector3& rhs)
	{
		const float distanceX = lhs.x - rhs.x;
		const float distanceY = lhs.y - rhs.y;
		const float distanceZ = lhs.z - rhs.z;
		return std::sqrt((distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ));
	}

	float xangle(const Vector3& LocalPos, const Vector3& WorldPos)
	{
		float dl = sqrt((WorldPos.x - LocalPos.x) * (WorldPos.x - LocalPos.x) + (WorldPos.y - LocalPos.y) * (WorldPos.y - LocalPos.y));

		if (dl == 0.0f)
			dl = 1.0f;

		float dl2 = abs(WorldPos.x - LocalPos.x);
		float teta = ((180.0f / M_PI) * acos(dl2 / dl));

		if (WorldPos.x < LocalPos.x)
			teta = 180.0f - teta;

		if (WorldPos.y < LocalPos.y)
			teta = teta * -1.0f;

		if (teta > 180.0f)
			teta = (360.0f - teta) * (-1.0f);

		if (teta < -180.0f)
			teta = (360.0f + teta);

		return teta;
	}

	bool world_to_screen_(Vector3 world_location, Vector2& out, Vector3 camera_pos, int screen_width, int screen_height, float fov_x, float fov_y, Vector3 matricies[3]) {
		auto local = world_location - camera_pos;
		Vector2 fov = { fov_x,fov_y };
		auto trans = Vector3{
			local.Dot(matricies[1]),
			local.Dot(matricies[2]),
			local.Dot(matricies[0])
		};

		if (trans.z < 0.01f) {
			return false;
		}

		out.x = ((float)screen_width / 2.0) * (1.0 - (trans.x / fov.x / trans.z));
		out.y = ((float)screen_height / 2.0) * (1.0 - (trans.y / fov.y / trans.z));

		if (out.x < 1 || out.y < 1 || (out.x > e_data::refdef->Width) || (out.y > e_data::refdef->Height)) {
			return false;
		}

		return true;
	}





	void rotation_point_alpha(float x, float y, float z, float alpha, Vector3* outVec3)
	{
		static HMODULE hD3dx9_43 = NULL;
		if (hD3dx9_43 == NULL)
			hD3dx9_43 = LoadLibraryA(xorstr("d3dx9_43.dll"));


		typedef fb::LinearTransform* (WINAPI* t_D3DXMatrixRotationY)(fb::LinearTransform* pOut, FLOAT Angle);
		static t_D3DXMatrixRotationY D3DXMatrixRotationY = NULL;
		if (D3DXMatrixRotationY == NULL)
			D3DXMatrixRotationY = (t_D3DXMatrixRotationY)GetProcAddress(hD3dx9_43, xorstr("D3DXMatrixRotationY"));

		typedef fb::Vec4* (WINAPI* t_D3DXVec3Transform)(fb::Vec4* pOut, CONST fb::Vec4* pV, CONST fb::LinearTransform* pM);
		static t_D3DXVec3Transform D3DXVec4Transform = NULL;
		if (D3DXVec4Transform == NULL)
			D3DXVec4Transform = (t_D3DXVec3Transform)GetProcAddress(hD3dx9_43, xorstr("D3DXVec4Transform"));

		Matrix4x4 rot1;
		Vector4 vec = { x, z, y, 1.0f };
		D3DXMatrixRotationY((fb::LinearTransform*)&rot1, alpha * M_PI / 180.0f);
		D3DXVec4Transform((fb::Vec4*)&vec, (const fb::Vec4*)&vec, (const  fb::LinearTransform*)&rot1);

		outVec3->x = vec.x;
		outVec3->y = vec.z;
		outVec3->z = vec.y;
	};

	bool w2s(const Vector3& WorldPos, Vector2* ScreenPos, Vector2* BoxSize)
	{
		auto ViewOrig = get_camera_location();

		float angleX = xangle(ViewOrig, WorldPos);

		Vector3 posl, posr;

		rotation_point_alpha(-16.0f, 0.0f, 65.0f, -angleX + 90.0f, &posl);
		rotation_point_alpha(16.0f, 0.0f, -5.0f, -angleX + 90.0f, &posr);

		Vector3 vposl, vposr;

		vposl.x = WorldPos.x + posl.x;
		vposl.y = WorldPos.y + posl.y;
		vposl.z = WorldPos.z + posl.z;

		vposr.x = WorldPos.x + posr.x;
		vposr.y = WorldPos.y + posr.y;
		vposr.z = WorldPos.z + posr.z;

		Vector2 screenPosl, screenPosr;

		if (!WorldToScreen(vposl, &screenPosl) || !WorldToScreen(vposr, &screenPosr))
		{
			return false;
		}

		BoxSize->x = abs(screenPosr.x - screenPosl.x);
		BoxSize->y = abs(screenPosl.y - screenPosr.y);

		ScreenPos->x = screenPosr.x - BoxSize->x;
		ScreenPos->y = screenPosl.y;

		return true;
	}

	bool head_to_screen(Vector3 pos, Vector2* pos_out, int stance)
	{
		Vector2 w2s_head;

		pos.z += 58;
		if (!WorldToScreen(pos, &w2s_head))
		{
			return false;
		}
		else if (stance == sdk::CROUNCH)
		{
			pos.z -= 20;
			if (!WorldToScreen(pos, &w2s_head))
				return false;
		}
		else if (stance == sdk::KNOCKED)
		{
			pos.z -= 28;
			if (!WorldToScreen(pos, &w2s_head))
				return false;
		}
		else if (stance == sdk::PRONE)
		{
			pos.z -= 50;
			if (!WorldToScreen(pos, &w2s_head))
				return false;
		}

		pos_out->x = w2s_head.x;
		pos_out->y = w2s_head.y;

		return true;
	}

	bool bones_to_screen(Vector3* BonePosArray, Vector2* ScreenPosArray, const long Count)
	{
		for (long i = 0; i < Count; ++i)
		{
			if (!WorldToScreen(BonePosArray[i], &ScreenPosArray[i]))
				return false;
		}
		return true;
	}

	bool is_valid_bone_new(Vector3* bone, const long Count, Vector3 origin)
	{
		for (long i = 0; i < Count; ++i)
		{
			if (bone[i].distance_to(bone[i + 1]) > 38 && origin.distance_to(bone[i]) > 500)
				return false;
		}
		return true;
	}

	bool is_valid_bone(Vector2 screenPos, float x, float y, float w, float h)
	{
		constexpr long size = 30;

		float r = x + w + size;
		float b = y + h;

		x -= size;

		if (screenPos.x < x || screenPos.x > r || screenPos.y < y || screenPos.y > b)
			return false;

		return true;
	}

	void main_loop(ImFont* font);

	void ui_header()
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin("A", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
	}

	void ui_end()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->PushClipRectFullScreen();
		ImGui::End();
		ImGui::PopStyleColor();
	}

	void init(ImFont* font)
	{
		ui_header();
		main_loop(font);
		ui_end();
	}

	namespace g_draw
	{
		void draw_line(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness)
		{
			auto window = ImGui::GetBackgroundDrawList();;
			/*float a = (color >> 24) & 0xff;
			float r = (color >> 16) & 0xff;
			float g = (color >> 8) & 0xff;
			float b = (color) & 0xff;*/
			window->AddLine(from, to, color, thickness);

		}



		void draw_box(const float x, const float y, const float width, const float height, const uint32_t color, float thickness)
		{
			draw_line(ImVec2(x, y), ImVec2(x + width, y), color, thickness);
			draw_line(ImVec2(x, y), ImVec2(x, y + height), color, thickness);
			draw_line(ImVec2(x, y + height), ImVec2(x + width, y + height), color, thickness);
			draw_line(ImVec2(x + width, y), ImVec2(x + width, y + height), color, thickness);
		}

		void draw_corned_box(const Vector2& rect, const Vector2& size, uint32_t color, float thickness)
		{
			float cornerSize = size.x / 4;
			const float lineW = (size.x / 5);
			const float lineH = (size.y / 6);
			const float lineT = 1;



			//outline
			draw_line(ImVec2(rect.x - lineT, rect.y - lineT), ImVec2(rect.x + lineW, rect.y - lineT), color, thickness); //top left
			draw_line(ImVec2(rect.x - lineT, rect.y - lineT), ImVec2(rect.x - lineT, rect.y + lineH), color, thickness);

			draw_line(ImVec2(rect.x - lineT, rect.y + size.y - lineH), ImVec2(rect.x - lineT, rect.y + size.y + lineT), color, thickness); //bot left
			draw_line(ImVec2(rect.x - lineT, rect.y + size.y + lineT), ImVec2(rect.x + lineW, rect.y + size.y + lineT), color, thickness);
			draw_line(ImVec2(rect.x + size.x - lineW, rect.y - lineT), ImVec2(rect.x + size.x + lineT, rect.y - lineT), color, thickness); // top right
			draw_line(ImVec2(rect.x + size.x + lineT, rect.y - lineT), ImVec2(rect.x + size.x + lineT, rect.y + lineH), color, thickness);
			draw_line(ImVec2(rect.x + size.x + lineT, rect.y + size.y - lineH), ImVec2(rect.x + size.x + lineT, rect.y + size.y + lineT), color, thickness); // bot right
			draw_line(ImVec2(rect.x + size.x - lineW, rect.y + size.y + lineT), ImVec2(rect.x + size.x + lineT, rect.y + size.y + lineT), color, thickness);

			//inline
			draw_line(ImVec2(rect.x, rect.y), ImVec2(rect.x, rect.y + lineH), color, thickness);//top left
			draw_line(ImVec2(rect.x, rect.y), ImVec2(rect.x + lineW, rect.y), color, thickness);
			draw_line(ImVec2(rect.x + size.x - lineW, rect.y), ImVec2(rect.x + size.x, rect.y), color, thickness); //top right
			draw_line(ImVec2(rect.x + size.x, rect.y), ImVec2(rect.x + size.x, rect.y + lineH), color, thickness);
			draw_line(ImVec2(rect.x, rect.y + size.y - lineH), ImVec2(rect.x, rect.y + size.y), color, thickness); //bot left
			draw_line(ImVec2(rect.x, rect.y + size.y), ImVec2(rect.x + lineW, rect.y + size.y), color, thickness);
			draw_line(ImVec2(rect.x + size.x - lineW, rect.y + size.y), ImVec2(rect.x + size.x, rect.y + size.y), color, thickness);//bot right
			draw_line(ImVec2(rect.x + size.x, rect.y + size.y - lineH), ImVec2(rect.x + size.x, rect.y + size.y), color, thickness);

		}

		void fill_rectangle(const float x, const float y, const float width, const float hight, const uint32_t color)
		{
			const float end_y = y + hight;
			for (float curr_y = y; curr_y < end_y; ++curr_y)
			{
				draw_line(ImVec2(x, curr_y), ImVec2(x + width, curr_y), color, 1.5f);
			}
		}




		void draw_circle(const ImVec2& position, float radius, uint32_t color, float thickness)
		{
			constexpr double M_PI1 = 3.14159265358979323846;
			float step = (float)M_PI1 * 2.0f / thickness;
			for (float a = 0; a < (M_PI * 2.0f); a += step)
			{
				draw_line(
					ImVec2(radius * cosf(a) + position.x, radius * sinf(a) + position.y),
					ImVec2(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y),
					color,
					1.5f
				);
			}
		}

		void draw_sketch_edge_text(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, uint32_t color, bool center, uint32_t EdgeColor = 0xFF000000)
		{
			constexpr float fStrokeVal1 = 1.0f;
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			float Edge_a = (EdgeColor >> 24) & 0xff;
			float Edge_r = (EdgeColor >> 16) & 0xff;
			float Edge_g = (EdgeColor >> 8) & 0xff;
			float Edge_b = (EdgeColor) & 0xff;
			std::stringstream steam(text);
			std::string line;
			float y = 0.0f;
			int i = 0;
			while (std::getline(steam, line))
			{
				ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());
				if (center)
				{
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), color, line.c_str());
				}
				else
				{
					window->DrawList->AddText(pFont, size, ImVec2((pos.x) - fStrokeVal1, (pos.y + textSize.y * i)), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x) + fStrokeVal1, (pos.y + textSize.y * i)), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), color, line.c_str());
				}
				y = pos.y + textSize.y * (i + 1);
				i++;
			}
		}



		void draw_crosshair()
		{
			constexpr long crosshair_size = 15.0f;

			ImVec2 center = ImVec2(e_data::refdef->Width / 2, e_data::refdef->Height / 2);

			g_draw::draw_line(ImVec2((center.x), (center.y) - crosshair_size), ImVec2((center.x), (center.y) + crosshair_size), 0xFFFFFFFF, 1.5f);
			g_draw::draw_line(ImVec2((center.x) - crosshair_size, (center.y)), ImVec2((center.x) + crosshair_size, (center.y)), 0xFFFFFFFF, 1.5f);
		}

		void draw_fov(const float aimbot_fov)
		{
			ImVec2 center = ImVec2(e_data::refdef->Width / 2, e_data::refdef->Height / 2);

			g_draw::draw_circle(center, aimbot_fov, 0xFFFFFF, 100.0f);
		}

		void draw_bones(Vector2* bones_screenPos, long count, DWORD color)
		{
			long last_count = count - 1;
			for (long i = 0; i < last_count; ++i)
				g_draw::draw_line(ImVec2(bones_screenPos[i].x, bones_screenPos[i].y), ImVec2(bones_screenPos[i + 1].x, bones_screenPos[i + 1].y), color, 1.8f);
		}



		void draw_bones(unsigned long i, Vector3 origin, DWORD color)
		{
			Vector3 header_to_bladder[6], right_foot_to_bladder[5], left_foot_to_bladder[5], right_hand[5], left_hand[5];
			Vector2 screen_header_to_bladder[6], screen_right_foot_to_bladder[5], screen_left_foot_to_bladder[5], screen_right_hand[5], screen_left_hand[5]; screen_left_hand[5];

			if (sdk::get_bone_by_player_index(i, sdk::BONE_POS_HEAD, &header_to_bladder[0]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_NECK, &header_to_bladder[1]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_CHEST, &header_to_bladder[2]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_MID, &header_to_bladder[3]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_TUMMY, &header_to_bladder[4]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_PELVIS, &header_to_bladder[5]) &&

				sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_1, &right_foot_to_bladder[1]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_2, &right_foot_to_bladder[2]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_3, &right_foot_to_bladder[3]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_4, &right_foot_to_bladder[4]) &&

				sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_1, &left_foot_to_bladder[1]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_2, &left_foot_to_bladder[2]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_3, &left_foot_to_bladder[3]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_4, &left_foot_to_bladder[4]) &&

				sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_1, &right_hand[1]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_2, &right_hand[2]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_3, &right_hand[3]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_4, &right_hand[4]) &&

				sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_1, &left_hand[1]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_2, &left_hand[2]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_3, &left_hand[3]) &&
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_4, &left_hand[4]))
			{
				right_foot_to_bladder[0] = header_to_bladder[5];
				left_foot_to_bladder[0] = header_to_bladder[5];
				right_hand[0] = header_to_bladder[3];
				left_hand[0] = header_to_bladder[3];


				if (!is_valid_bone_new(header_to_bladder, 6, origin) ||
					!is_valid_bone_new(right_foot_to_bladder, 5, origin) ||
					!is_valid_bone_new(left_foot_to_bladder, 5, origin) ||
					!is_valid_bone_new(right_hand, 5, origin) ||
					!is_valid_bone_new(left_hand, 5, origin))
				{
					return;
				}

				if (!bones_to_screen(header_to_bladder, screen_header_to_bladder, 6))
					return;

				if (!bones_to_screen(right_foot_to_bladder, screen_right_foot_to_bladder, 5))
					return;

				if (!bones_to_screen(left_foot_to_bladder, screen_left_foot_to_bladder, 5))
					return;

				if (!bones_to_screen(right_hand, screen_right_hand, 5))
					return;

				if (!bones_to_screen(left_hand, screen_left_hand, 5))
					return;



				draw_bones(screen_header_to_bladder, 6, color);
				draw_bones(screen_right_foot_to_bladder, 5, color);
				draw_bones(screen_left_foot_to_bladder, 5, color);
				draw_bones(screen_right_hand, 5, color);
				draw_bones(screen_left_hand, 5, color);
			}
		}

		void draw_health(int i_health, Vector3 pos)
		{
			Vector2 bottom, top;

			if (!WorldToScreen(pos, &bottom))
				return;

			pos.z += 60;
			if (!WorldToScreen(pos, &top))
				return;

			top.y -= 5;
			auto height = top.y - bottom.y;
			auto width = height / 2.f;
			auto x = top.x - width / 1.8f;
			auto y = top.y;

			auto bar_width = max(0, min(127, i_health)) * (bottom.y - top.y) / 127.f;
			auto health = max(0, min(127, i_health));
			uint32_t color_health = ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 255, 255));

			fill_rectangle(x, y, 4, 127 * (bottom.y - top.y) / 127.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.000f, 0.000f, 0.000f, 1.000f)));
			fill_rectangle(x + 1, y + 1, 2, bar_width - 2, color_health);

		}
	}

	namespace aim_lock
	{
		constexpr float init_center_dist = 10000.0f;
		float min_center_dist = init_center_dist;
		Vector2 lock_position;

		__forceinline void reset_lock()
		{
			min_center_dist = init_center_dist;
		}

		__forceinline bool is_key_down(int vk_key)
		{
			return ((GetAsyncKeyState(vk_key) & 0x8000) ? 1 : 0);
		}

		__forceinline unsigned long get_bone_opt()
		{
			switch (globals::bone_index)
			{
			case 0:
				return sdk::BONE_INDEX::BONE_POS_HELMET;
			case 1:
				return sdk::BONE_INDEX::BONE_POS_HEAD;
			case 2:
				return sdk::BONE_INDEX::BONE_POS_NECK;
			case 3:
				return sdk::BONE_INDEX::BONE_POS_CHEST;
			};
			return sdk::BONE_INDEX::BONE_POS_HEAD;
		}

		bool is_aiming()
		{
			if (GamePad1.IsConnected())
			{
				if (GamePad1.checkButtonPress(VK_PAD_LTRIGGER) ||
					GamePad1.checkButtonPress(VK_PAD_LTHUMB_PRESS) && GamePad1.GetState().Gamepad.bRightTrigger & VK_PAD_RTRIGGER)
				{
					return true;
				}
			}
			return false;
		}

		//rapid fire
		//void rapid_fire()
		//{
		//	if (!globals::b_rapid_fire && !globals::b_in_game)
		//		return;

		//	if (is_key_down(VK_LBUTTON))
		//	{
		//		if (isDown)
		//		{
		//			isDown = false;
		//			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		//		}
		//		if (clock() - fire_timer > globals::fire_speed)
		//		{
		//			isDown = true;
		//			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		//			fire_timer = clock();
		//		}
		//	}
		//}

		//whit S_Input
		/*void get_bone_in_fov(int i, float vdistance)
		{
			Vector3 current_bone;
			POINT bone_screen_pos;

			if (g_sdk::get_bones_by_index(i, g_sdk::BONE_POS_HEAD, &current_bone))
			{
				auto current_pos = g_sdk::get_predict_pos(i, vdistance, current_bone, globals::bullet_speed);

				if (w2s(current_pos, &bone_screen_pos))
				{
					const float x = bone_screen_pos.x - (e_data::refdef->Width / 2);
					const float y = bone_screen_pos.y - (e_data::refdef->Height / 2);
					float center_dis = sqrt(pow(y, 2) + pow(x, 2));

					if (center_dis < min_center_dist && center_dis <= globals::f_fov_size)
					{
						min_center_dist = center_dis;
						g_lock_position.x = x;
						g_lock_position.y = y;
						globals::b_has_target = true;
					}
					globals::b_has_target = false;
				}
			}
		}*/

		/*void get_target_in_fov(POINT bone_pos)
		{
			const float x = bone_pos.x - (e_data::refdef->Width / 2);
			const float y = bone_pos.y - (e_data::refdef->Height / 2);
			float center_dis = sqrt(pow(y, 2) + pow(x, 2));

			if (center_dis < min_center_dist && center_dis <= globals::f_fov_size)
			{
				min_center_dist = center_dis;
				lock_position.x = x;
				lock_position.y = y;
			}
		}*/

		//lock Sinput
		/*void lock_on_target()
		{
			if (is_key_down(VK_RBUTTON) && is_key_down(VK_LBUTTON) ||
				is_key_down(VK_RBUTTON) && is_key_down(VK_LSHIFT)  ||
				is_key_down(VK_LSHIFT) && is_key_down(VK_LBUTTON))
			{
				if (min_center_dist != init_center_dist)
				{
					INPUT Input;
					Input.type = INPUT_MOUSE;
					Input.mi.dwFlags = MOUSEEVENTF_MOVE;
					Input.mi.dx = (long)(lock_position.x * (globals::aim_speed / globals::aim_smooth));
					Input.mi.dy = (long)(lock_position.y * (globals::aim_speed / globals::aim_smooth));

					if (min_center_dist != 0)
					{
						globals::is_aiming = true;
						SendInput(1, &Input, sizeof(INPUT));
					}
					globals::is_aiming = false;
				}
			}
		}*/

		//whit m_move
		void get_target_in_fov(Vector2 target_pos)
		{
			const float x = target_pos.x - (e_data::refdef->Width / 2);
			const float y = target_pos.y - (e_data::refdef->Height / 2);
			float center_dis = sqrt(pow(y, 2) + pow(x, 2));

			if (center_dis < min_center_dist && center_dis <= globals::f_fov_size)
			{
				min_center_dist = center_dis;
				lock_position = target_pos;
			}
		}

		void lock_on_target()
		{
			if (is_key_down(VK_RBUTTON) || is_aiming())


			{
				if (min_center_dist != init_center_dist)
				{
					float target_x = 0;
					float target_y = 0;
					auto center_x = g_game::e_data::refdef->Width / 2;
					auto center_y = g_game::e_data::refdef->Height / 2;

					if (lock_position.x != 0)
					{
						if (lock_position.x > center_x)
						{
							target_x = -(center_x - lock_position.x);
							target_x /= globals::aim_smooth;
							if (target_x + center_x > center_x * 2)
								target_x = 0;
						}
						if (lock_position.x < center_x)
						{
							target_x = lock_position.x - center_x;
							target_x /= globals::aim_smooth;
							if (target_x + center_x < 0)
								target_x = 0;
						}
					}
					if (lock_position.y != 0)
					{
						if (lock_position.y > center_y)
						{
							target_y = -(center_y - lock_position.y);
							target_y /= globals::aim_smooth;
							if (target_y + center_y > center_y * 2)
								target_y = 0;
						}
						if (lock_position.y < center_y)
						{
							target_y = lock_position.y - center_y;
							target_y /= globals::aim_smooth;
							if (target_y + center_y < 0)
								target_y = 0;
						}
					}
					if (target_x != 0 && target_y != 0)
						mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(target_x), static_cast<DWORD>(target_y), NULL, NULL);
				}
			}
			else
			{
				globals::is_aiming = false;
			}
		}
	}

	int get_local_index() {
		auto local_index = *reinterpret_cast<uintptr_t*>(sdk::get_client_info() + player_info::local_index);
		if (is_bad_ptr(local_index)) return 0;
		auto ret = *reinterpret_cast<int*>(local_index + 0x1FC);
		if (is_bad_ptr(ret)) return 0;
		return ret;
	}
	uintptr_t get_local()
	{
		auto addr = sdk::get_client_info_base() + (get_local_index() * player_info::size);
		if (is_bad_ptr(addr)) return 0;
		return addr;
	}

	uintptr_t get_entity(int index)
	{
		auto ret = sdk::get_client_info_base() + (index * player_info::size);
		if (is_bad_ptr(ret)) return 0;
		return ret;
	}
	Vector3 adjustments(Vector3 position, int localStance, int playerStance)
	{
		position.z -= 25.0f;

		switch (playerStance)
		{
		case sdk::STANCE::CROUNCH:
			position.z -= 9.0f;
			break;
		case sdk::STANCE::PRONE:
		case sdk::STANCE::KNOCKED:
			position.z -= 22.0f;
			break;
		}

		switch (localStance)
		{
		case sdk::STANCE::CROUNCH:
			position.z += 17.5f;
			break;
		case sdk::STANCE::PRONE:
		case sdk::STANCE::KNOCKED:
			position.z += 45.0f;
			break;
		}

		return position;
	}
	const char* stance_as_string(int stance)
	{
		switch (stance)
		{
		case sdk::STANCE::STAND: return "Standing";
		case sdk::STANCE::CROUNCH: return "Crounch";
		case sdk::STANCE::PRONE: return "Prone";
		case sdk::STANCE::KNOCKED: return "knocked";
		case 4: return "Unknown";
		default:
			break;
		}
	}




	Vector2 size(float distance)
	{
		// As the distance increases, the size of width and height decreases
		// This is "Inverse proportion"
		// C = k / D
		// C = Width or Height
		// D = Distance
		// k = C * D

		// At a distance of 272, width is 140 and height is 280
		float pDistance = 272.0f;
		float pWidth = 140.0f;
		float pHeight = 280.0f;

		float kWidth = pDistance * pWidth; // 272 * 140
		float kHeight = pDistance * pHeight; // 272 * 280

		return Vector2(kWidth / distance, kHeight / distance);
	}


	int get_player_health(int i)
	{
		uint64_t bgs = *(uint64_t*)(g_data::base + client::name_array);

		if (bgs)
		{
			sdk::name_t* clientInfo_ptr = (sdk::name_t*)(bgs + client::name_array_padding + (i * 0xD0));
			int health = clientInfo_ptr->health;

			return health;
		}
		return 0;
	}



	bool _is_dead(uintptr_t base) {
		auto dead1 = *reinterpret_cast<bool*>((uintptr_t)base + player_info::dead_1);
		auto dead2 = *reinterpret_cast<bool*>((uintptr_t)base + player_info::dead_2);
		return dead1 || dead2;
	}

	bool _is_valid(uintptr_t base) {

		return *reinterpret_cast<bool*>((uintptr_t)base + player_info::valid);
	}

	int _stance(uintptr_t base) {

		return *reinterpret_cast<int*>((uintptr_t)base + player_info::stance);
	}
	DWORD color = 0x0;
	int called = 0;

	Vector3 Subtract(Vector3 src, Vector3 dst)
	{
		Vector3 diff;
		diff.x = src.x - dst.x;
		diff.y = src.y - dst.y;
		diff.z = src.z - dst.z;
		return diff;
	}

	float DotProduct(Vector3 src, Vector3 dst)
	{
		return src.x * dst.x + src.y * dst.y + src.z * dst.z;
	}




	void main_loop(ImFont* font)
	{
		GamePad1.GetState();
		//if (!screenshot::visuals) return;
		globals::b_in_game = sdk::in_game();
		globals::max_player_count = sdk::get_max_player_count();
		//als::connecte_players = sdk::get_client_count();
		globals::gamepad = GamePad1.IsConnected();

		if (!globals::b_in_game)
		{
			e_data::refdef = nullptr;
			sdk::clear_map();

			globals::call = 0;
			called = 0;
		}

		sdk::start_tick();

		if (e_data::refdef == nullptr)
			e_data::refdef = sdk::get_refdef();

		if (e_data::refdef == nullptr) return;
		globals::refdefadd = e_data::refdef;

		if (globals::b_crosshair && screenshot::visuals)
			g_draw::draw_crosshair();

		if (globals::b_fov)
			g_draw::draw_fov(globals::f_fov_size);




		if (!globals::is_aiming)
			aim_lock::reset_lock();



		/*uintptr_t local_player = get_local();
		globals::local_ptr = local_player;
		globals::localpos1 = sdk::_get_pos(local_player);*/

		sdk::player_t local_player = sdk::get_local_player();

		globals::local_ptr = local_player.address;
		if (is_bad_ptr(globals::local_ptr))return;

		globals::player_index = local_player.get_index();
		DWORD local_team_id = local_player.team_id();
		/*	if (!local_player->is_valid() || !globals::local_is_alive) return;*/

		//auto is_dead_local = _is_dead(local_player);
		//
		//globals::local_is_alive = !is_dead_local;
		//auto local_stance = _stance(local_player);
		//globals::stance = stance_as_string(local_stance);
		if (globals::b_recoil) sdk::no_recoil();

		if (globals::b_visible_only || globals::b_visible)
		{
			g_data::visible_base = sdk::get_visible_base();
			if (is_bad_ptr(g_data::visible_base)) return;
		}

		//globals::call = called++;

		for (int i = 0; i < globals::max_player_count; ++i)
		{
			if (local_player.get_index() == i)continue;
			sdk::player_t player_info = sdk::get_player(i);




			//auto localplayer_index = sdk::_get_index(local_player);



			if (!player_info.address)/*|| i != player_info->get_index()*/
				continue;



			bool is_friendly = player_info.team_id() == local_team_id;

			if (is_friendly && !globals::b_friendly) continue;

			/*	sdk::update_visible_addr(i);*/

			auto is_visible = player_info.is_visible();
			auto is_valid = player_info.is_valid();
			auto is_dead = player_info.is_dead();
			if (!is_valid || is_dead)continue;
			float fdistance = 0;
			float display_string = 0;

			/*auto local_pos = sdk::get_camera_pos();*/


			fdistance = get_distance(local_player.get_pos(), player_info.get_pos()) / 40;

			if (fdistance > globals::max_distance)
				continue;



			Vector2 ScreenPos, BoxSize;
			if (globals::b_visible && is_visible && !globals::b_rainbow)
				color = is_friendly ? color::VisibleColorTeam : color::VisibleColorEnemy;
			else
				color = is_friendly ? color::NotVisibleColorTeam : color::NotVisibleColorEnemy;

			if (globals::b_rainbow)
			{

			}

			if (globals::b_visible_only && !is_visible)continue;





			Vector3 FeetBone, HeadBone;


			/*
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_MID, &FeetBone);
				sdk::get_bone_by_player_index(i, sdk::BONE_POS_HELMET, &HeadBone);

				if (!WorldToScreen(FeetBone, &ScreenPos) || !WorldToScreen(HeadBone, &ScreenPos))continue;*/




			if (!w2s(player_info.get_pos(), &ScreenPos, &BoxSize))continue;
			//if (!w2stest(FeetBone, local_player.get_pos(), e_data::refdef->Width, e_data::refdef->Height,Vector2(e_data::refdef->FovX, e_data::refdef->FovY), e_data::refdef->ViewAxis, &ScreenPos))continue;
			//if (!WorldToScreen(FeetBone,&ScreenPos))continue;
		/*	Vector2 screensize = size(fdistance);

			ScreenPos.x -= (screensize.x / 2);
			ScreenPos.y -= (screensize.y / 2);*/


			for (auto j = 0; j < 1000; j++)
			{


				float display_string2 = 0;
				Loot* loot = get_loot_by_id(j);

				if (!loot->is_valid())
					continue;

				auto name = loot->get_name();

				if (!strlen(name))
					continue;

				auto pos = loot->get_position();


				Vector2 ScreenPos2, BoxSize2;
				if (!w2s(pos, &ScreenPos2, &BoxSize2))continue;

				ImColor loot_color =loot::lootcolor;   //use your setting for loot color




				for (auto k = 0; k < IM_ARRAYSIZE(definitions); k++)
				{
					auto* loot_def = &definitions[k];

					if (name && strstr(name, loot_def->name))
					{

						if (loot::name)//your settings
						{
							if (!*loot_def->show)
								continue;

							g_draw::draw_sketch_edge_text(font, loot_def->text, ImVec2(
								ScreenPos2.x + (BoxSize2.x / 2),
								ScreenPos2.y + BoxSize2.y + 5 + display_string2),
								18.0f,
								loot_color,
								true
							);

						}

						if (loot::distance)//your settings
						{
							std::string distance_string("[ ");
							distance_string += std::to_string(fdistance);
							std::string::size_type end = distance_string.find('.');
							if (end != std::string::npos && (end + 2) <= (distance_string.length() - 1))
								distance_string.erase(distance_string.begin() + end + 2, distance_string.end());
							distance_string += "m ]";
							g_draw::draw_sketch_edge_text(font, distance_string, ImVec2(
								ScreenPos2.x + (BoxSize2.x / 2),
								ScreenPos2.y + BoxSize2.y + 5 + 20),
								18.0f,
								loot_color,
								true
							);
						}


					}

				}
			}


			if (globals::b_box && screenshot::visuals)
			{

				switch (globals::box_index)
				{
				case 0:g_draw::draw_corned_box(ScreenPos, BoxSize, color, 1.f); break;
				case 1:g_draw::draw_box(ScreenPos.x, ScreenPos.y, BoxSize.x, BoxSize.y, color, 1.f); break;
				default:
					break;
				}

			}

			if (globals::b_line && screenshot::visuals)
			{
				ImVec2 origin;
				origin.x = e_data::refdef->Width / 2;
				origin.y = e_data::refdef->Height / 2;

				g_draw::draw_line(origin, ImVec2(ScreenPos.x, ScreenPos.y), color, 1.f);
			}

			if (globals::b_names && screenshot::visuals)
			{
				std::string name = sdk::get_player_name(i);
				g_draw::draw_sketch_edge_text(font, name, ImVec2(
					ScreenPos.x + (BoxSize.x / 2),
					ScreenPos.y + BoxSize.y + 5 + display_string),
					18.0f,
					color, false);

				display_string += 15;
			}

			if (globals::b_health && screenshot::visuals)
			{
				g_draw::draw_health(get_player_health(i), player_info.get_pos());
			}

			if (globals::b_distance && screenshot::visuals)
			{
				std::string distance_string("[ ");
				distance_string += std::to_string(fdistance);
				std::string::size_type end = distance_string.find('.');
				if (end != std::string::npos && (end + 2) <= (distance_string.length() - 1))
					distance_string.erase(distance_string.begin() + end + 2, distance_string.end());
				distance_string += "m ]";
				g_draw::draw_sketch_edge_text(font, distance_string, ImVec2(
					ScreenPos.x + (BoxSize.x / 2),
					ScreenPos.y + BoxSize.y + 5 + display_string),
					18.0f,
					color, true);
			}

			if (globals::b_skeleton && screenshot::visuals)
			{

				g_draw::draw_bones(i, player_info.get_pos(), color);

			}





			if (globals::b_lock)
			{
				Vector3 current_bone;
				Vector2 bone_screen_pos;

				sdk::update_vel_map(i, player_info.get_pos());
				sdk::update_vel_map(local_index(), local_player.get_pos());

				if (globals::target_bone)
				{
					if (!sdk::get_bone_by_player_index(i, g_game::aim_lock::get_bone_opt(), &current_bone))
						continue;

					if (globals::b_prediction /*&& fdistance > 4*/)
					{

						current_bone = sdk::get_prediction(i, local_player.get_pos(), current_bone);
					}

					if (!WorldToScreen(current_bone, &bone_screen_pos))
						continue;

					if (globals::b_aim_point)
						g_draw::draw_circle(ImVec2(bone_screen_pos.x, bone_screen_pos.y), 5, color, 100.0f);


					if (player_info.get_stance() == sdk::KNOCKED && globals::b_skip_knocked)
						continue;

					if (globals::b_visible && !player_info.is_visible())
						continue;

					g_game::aim_lock::get_target_in_fov(bone_screen_pos);
				}
				else
				{
					current_bone = player_info.get_pos();

					if (globals::b_prediction /*&& fdistance > 4*/)
					{
						//current_bone = sdk::prediction_solver(local_player.get_pos(), player.get_pos(), i, globals::bullet_speed);
						current_bone = sdk::get_prediction(i, local_player.get_pos(), current_bone);
					}

					if (!head_to_screen(current_bone, &bone_screen_pos, player_info.get_stance()))
						continue;

					if (globals::b_aim_point)
						g_draw::draw_circle(ImVec2(bone_screen_pos.x, bone_screen_pos.y), 5, color, 100.0f);

					if (player_info.get_stance() == sdk::KNOCKED && globals::b_skip_knocked)
						continue;

					if (globals::b_visible && !player_info.is_visible())
						continue;

					g_game::aim_lock::get_target_in_fov(bone_screen_pos);
				}
			}
		}

		aim_lock::lock_on_target();


	}
}




bool world(const Vector3& WorldPos, Vector2* ScreenPos)
{
	g_game::WorldToScreen(WorldPos, ScreenPos);
}



