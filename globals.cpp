#include "globals.h"
#include "sdk.h"

namespace globals
{
	bool b_visible_only;
	bool b_box;
	bool b_line;
	bool b_skeleton;
	bool b_names;
	bool b_distance;
	bool b_visible;
	bool b_fov;
	bool b_lock;
	bool b_crosshair;
	bool b_friendly;
	bool b_recoil;

	bool b_in_game;
	bool b_health;
	bool local_is_alive;
	bool b_rapid_fire;
	bool b_skip_knocked;
	bool b_prediction;
	bool b_aim_point;
	bool is_aiming;
	bool target_bone;
	bool b_aimbot;
	bool b_UAV;
	bool b_scale_Fov;
	bool third_person;
	bool gamepad;
	bool b_rainbow;
	bool b_unlock;
	bool b_tut;
	bool b_fog;
	//int game_mode_opt = 0;
	
	int bone_index = 1; //0 ~ 3
	int box_index = 0; //0 ~ 3
	//int lock_key = 1; //0 ~ 3
	int max_distance = 500; //50 ~ 1000
	int aim_smooth = 5; // 1 ~ 30
	int max_player_count = 0;
	int connecte_players = 0;
	int local_team_id;
	int fire_speed = 40;
	int call = 0;

	int player_index;
	float fov = 1.2f;
	Vector3 localpos1;
	Vector3 enemypos1;
	Vector3 enemypos2;
	Vector3 enemypos3;
	float f_fov_size = 90.0f; // 0 ~ 1000
	float aim_speed = 1.f;
	float bullet_speed = 2402.f; // 1 ~ 3000
	float bullet_gravity = 5.f;

	//float gravity = 1.f;
	const char* stance;
	const char* aim_lock_point[] = { "Helmet", "Head", "Neck", "Chest" };
	const char* box_types[] = { "2D Corner Box", "2D Box" };
	const char* theme_choose[] = { "Dark Mode","White Mode"};
	//const char* aim_lock_key[] = { "Left Button", "Right Button", "M4", "M5" };
	uintptr_t local_ptr;
	uintptr_t enemybase;
	sdk::refdef_t* refdefadd;

	int theme = 0;
}

namespace color
{
	ImColor Color{ 255,255,255,255};
	ImColor VisibleColorTeam{ 0.f, 0.f, 1.f, 1.f };
	ImColor NotVisibleColorTeam{ 0.f, 0.75f, 1.f, 1.f };
	ImColor VisibleColorEnemy{ 1.f, 0.f, 0.f, 1.f };
	ImColor NotVisibleColorEnemy{ 0.f,1.f,1.f,1.f };


}

namespace screenshot
{
	 bool visuals = true;
	 bool* pDrawEnabled = nullptr;
		uint32_t screenshot_counter = 0;
		uint32_t  bit_blt_log = 0;
	 const char* bit_blt_fail;
	 uintptr_t  bit_blt_anotherlog;

		uint32_t	GdiStretchBlt_log = 0;
	 const char* GdiStretchBlt_fail;
	 uintptr_t  GdiStretchBlt_anotherlog;

	uintptr_t	texture_copy_log = 0;




	 uintptr_t virtualqueryaddr = 0;
}


namespace loot
{
	ImColor lootcolor{ 0.f, 0.75f, 1.f, 1.f };
	bool name;
	bool distance;
	bool ar;
	bool smg;
	bool lmg;
	bool sniper;
	bool pistol;
	bool shotgun;

	bool ar_ammo;
	bool smg_ammo;
	bool sniper_ammo;
	bool shotgun_ammo;



}