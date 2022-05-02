#pragma once
#include "stdafx.h"
#include "vec.h"


#define ror

#define _PTR_MAX_VALUE ((PVOID)0x000F000000000000)
#define BYTEn(x, n)   (*((BYTE*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)

//auto padding
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define is_valid_ptr(p) ((uintptr_t)(p) <= 0x7FFFFFFEFFFF && (uintptr_t)(p) >= 0x1000) 
#define is_bad_ptr(p)	(!is_valid_ptr(p))

template <typename T>
bool IsValidPtr(PVOID ptr)
{
	if (is_bad_ptr(ptr))return false;
	else
		return true;

}


bool world(const Vector3& WorldPos, Vector2* ScreenPos);

class cg_t
{
public:
	char pad_0000[48]; //0x0000
	Vector3 vecOrig; //0x0030
	char pad_003C[416]; //0x003C
	Vector3 viewAngle; //0x01DC
	char pad_01E8[112]; //0x01E8
	int32_t N0000009A; //0x0258
	int32_t Health; //0x025C
	char pad_0260[160]; //0x0260
	int32_t N000000AF; //0x0300
	int32_t Uav; //0x0304
	char pad_0308[1408]; //0x0308
}; //Size: 0x0888
static_assert(sizeof(cg_t) == 0x888);

//[<ModernWarfare.exe>+1760F190]
class N0000004E
{
public:
	char pad_0000[1144]; //0x0000
	char N00000A88[16]; //0x0478
	char pad_0488[32]; //0x0488
	char N00000A89[16]; //0x04A8
	char pad_04B8[19416]; //0x04B8
}; //Size: 0x5090
static_assert(sizeof(N0000004E) == 0x5090);

#define game_mode 0x1E48D5A8
namespace player_info
{
	constexpr auto local_index = 0x6908;
	constexpr auto local_index_pos = 0x1FC;
	constexpr auto size = 0x60A0;
	constexpr auto valid = 0x58;
	constexpr auto team_id = 0x60; //39 81 ? ? ? ? 0F 84 ? ? ? ? B8 ? ? ? ?
	constexpr auto position_ptr = 0x860;
	constexpr auto stance = 0xC98;
	constexpr auto recoil_offset = 0x19B68;
	constexpr auto dead_1 = 0xA88;
	constexpr auto dead_2 = 0x644;
	constexpr auto loot_ptr = 0x1EA6A3F0;
	constexpr auto weapon_index = 0x648;
}

namespace bones
{
	constexpr auto bone_base_pos = 0x6C0C;
	constexpr auto size = 0x150;
	constexpr auto visible = 0x614B230;
	constexpr auto weapon_definitions = 0x1B30DE20;
	constexpr auto loot_ptr = 0x1EA6A3F0;
	constexpr auto distribute = 0x20FEEB48;
}

namespace view_port
{
	constexpr auto refdef_ptr = 0x1E912F00;
	constexpr auto camera_ptr = 0x1B263090;
	constexpr auto camera_view_x = 0xF8;
	constexpr auto camera_view_y = 0x108;
	constexpr auto camera_view_z = 0x118;
	constexpr auto camera_pos = 0x1D8;
	constexpr auto loot_ptr = 0x1EA6A3F0;
}

namespace client
{
	//constexpr auto info_ptr = 0x17E24638; //client_info
	//constexpr auto base_offset = 0x9ED78; //client_Base
	constexpr auto name_array = 0x1E91EC90;
	constexpr auto name_array_padding = 0x4C70;
	constexpr auto loot_ptr = 0x1E91EC90;
}

namespace direcX
{
//	constexpr uint32_t command_queue = 0x20B808D8;
}

//////////////////////////////////////////////////////////


namespace g_data
{
	extern HWND hWind;
	extern uintptr_t base;
	extern uintptr_t peb;
	extern uintptr_t visible_base;
	
	void init();
}

namespace sdk
{
	
	bool update_visible_addr(int i);
	Vector3 _get_pos(uintptr_t address);
	bool _is_visible(uintptr_t address);
	int _team_id(uintptr_t address);
	uintptr_t _get_player(int i);
	uint32_t _get_index(uintptr_t address);
	uint64_t get_client_info();
	uint64_t get_client_info_base();
	enum BONE_INDEX : unsigned long
	{

		BONE_POS_HELMET = 8,

		BONE_POS_HEAD = 7,
		BONE_POS_NECK = 6,
		BONE_POS_CHEST = 5,
		BONE_POS_MID = 4,
		BONE_POS_TUMMY = 3,
		BONE_POS_PELVIS = 2,

		BONE_POS_RIGHT_FOOT_1 = 21,
		BONE_POS_RIGHT_FOOT_2 = 22,
		BONE_POS_RIGHT_FOOT_3 = 23,
		BONE_POS_RIGHT_FOOT_4 = 24,

		BONE_POS_LEFT_FOOT_1 = 17,
		BONE_POS_LEFT_FOOT_2 = 18,
		BONE_POS_LEFT_FOOT_3 = 19,
		BONE_POS_LEFT_FOOT_4 = 20,

		BONE_POS_LEFT_HAND_1 = 13,
		BONE_POS_LEFT_HAND_2 = 14,
		BONE_POS_LEFT_HAND_3 = 15,
		BONE_POS_LEFT_HAND_4 = 16,

		BONE_POS_RIGHT_HAND_1 = 9,
		BONE_POS_RIGHT_HAND_2 = 10,
		BONE_POS_RIGHT_HAND_3 = 11,
		BONE_POS_RIGHT_HAND_4 = 12
	};

	enum TYPE_TAG
	{
		ET_GENERAL = 0x0,
		ET_PLAYER = 0x1,
		ET_PLAYER_CORPSE = 0x2,
		ET_ITEM = 0x3,
		ET_MISSILE = 0x4,
		ET_INVISIBLE = 0x5,
		ET_SCRIPTMOVER = 0x6,
		ET_SOUND_BLEND = 0x7,
		ET_FX = 0x8,
		ET_LOOP_FX = 0x9,
		ET_PRIMARY_LIGHT = 0xA,
		ET_TURRET = 0xB,
		ET_HELICOPTER = 0xC,
		ET_PLANE = 0xD,
		ET_VEHICLE = 0xE,
		ET_VEHICLE_COLLMAP = 0xF,
		ET_VEHICLE_CORPSE = 0x10,
		ET_VEHICLE_SPAWNER = 0x11,
		ET_AGENT = 0x12,
		ET_AGENT_CORPSE = 0x13,
		ET_EVENTS = 0x14,
	};

	enum STANCE : int
	{
		STAND = 0,
		CROUNCH = 1,
		PRONE = 2,
		KNOCKED = 3
	};

	class name_t {
	public:
		uint32_t idx;
		char name[0x24];
		uint8_t unk1[0x64];
		uint32_t health;
	};

	struct ref_def_view {
		Vector2 tan_half_fov;
		char pad[0xC];
		Vector3 axis[3];
	};

	class refdef_t {
	public:
		char pad_0000[8]; //0x0000
		__int32 Width; //0x0008
		__int32 Height; //0x000C
		float FovX; //0x0010
		float FovY; //0x0014
		float Unk; //0x0018
		char pad_001C[8]; //0x001C
		Vector3 ViewAxis[3]; //0x0024
	};

	//class refdef_t
	//{
	//public:
	//	char pad_0000[8]; //0x0000
	//	__int32 Width; //0x0008
	//	__int32 Height; //0x000C
	//	float FovX; //0x0010
	//	float FovY; //0x0014
	//	float Unk; //0x0018
	//	char pad_001C[8]; //0x001C
	//	Vector3 ViewAxis[3]; //0x0024
	//	char pad_0048[52]; //0x0048
	//	Vector3 ViewLocationDelayed0; //0x007C
	//	Vector3 ViewLocationDelayed1; //0x0088
	//	char pad_0094[2808]; //0x0094
	//	Vector3 ViewMatrixTest[3]; //0x0B8C
	//	Vector3 ViewLocation; //0x0BB0
	//};


	class Result
	{
	public:
		bool hasResult;
		float a;
		float b;
	};

	struct velocityInfo_t
	{
		Vector3 lastPos;
		Vector3 delta;
	};

	class player_t
	{
	public:
		player_t(uintptr_t address) {
			this->address = address;
		}
		uintptr_t address{};
		uint32_t get_index();

		bool is_valid();

		bool is_visible();

		bool is_dead();

		int team_id();

		int get_stance();

		Vector3 get_pos();

		float get_rotation();
	};

	bool in_game();
	int get_game_mode();
	int get_max_player_count();
	name_t* get_name_ptr(int i);
	refdef_t* get_refdef();
	Vector3 get_camera_pos();
	player_t get_player(int i);
	player_t get_local_player();
	int get_local_index_num();
	std::string get_player_name(int i);
	bool get_bone_by_player_index(int i, int bone_index, Vector3* Out_bone_pos);
	void no_recoil();
	int get_player_health(int i);
	void start_tick();
	void update_vel_map(int index, Vector3 vPos);
	void clear_map();
	Vector3 get_speed(int index);
	Vector3 prediction_solver(Vector3 local_pos, Vector3 position, int index, float bullet_speed);
	uintptr_t get_visible_base();
	Vector3 get_prediction(int index, Vector3 source, Vector3 destination);
	/*int get_client_count();*/
}

namespace g_radar {

	void draw_entity(sdk::player_t local_entity, sdk::player_t entity, bool IsFriendly, bool IsAlive, DWORD color);
	void show_radar_background();
}


namespace g_game
{
	namespace g_draw
	{
		inline void draw_line(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness);
		inline void draw_box(const float x, const float y, const float width, const float height, const uint32_t color, float thickness);
		inline void draw_corned_box(const Vector2& rect, const Vector2& size, uint32_t color, float thickness);
		inline void fill_rectangle(const float x, const float y, const float width, const float hight, const uint32_t color);
	}
}
inline int local_index()
{
	uint64_t decryptedPtr = sdk::get_client_info();

	if (is_valid_ptr(decryptedPtr))
	{
		auto local_index = *(uintptr_t*)(decryptedPtr + player_info::local_index);
		return *(int*)(local_index + player_info::local_index_pos);
	}
	return 0;
}