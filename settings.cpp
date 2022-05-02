
#include"settings.h"
#include "xor.hpp"

BOOL WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName) {
	char lpString[1024];
	sprintf(lpString, "%d", nInteger);
	return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}
BOOL WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float nInteger, LPCSTR lpFileName) {
	char lpString[1024];
	sprintf(lpString, "%f", nInteger);
	return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}
float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
{
	char szData[32];

	GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);

	return (float)atof(szData);
}


//examples
void Save_Settings(std::string fileName) {
	char file_path[MAX_PATH];
	sprintf_s(file_path, xorstr("C:\\yourname\\config\\%s%s"), fileName.c_str(), xorstr(".ini"));

	WritePrivateProfileInt("esp", "thickness", esp::thickness, file_path);
	WritePrivateProfileInt("esp", "line_choose", esp::line_choose, file_path);
	WritePrivateProfileInt("esp", "box_index", esp::box_index, file_path);
	WritePrivateProfileInt("esp", "b_uav", esp::b_uav, file_path);
	WritePrivateProfileInt("esp", "enable_auav", esp::enable_auav, file_path);
	WritePrivateProfileInt("esp", "b_box", esp::b_box, file_path);
	WritePrivateProfileInt("esp", "visible only", esp::b_visible_only, file_path);
	WritePrivateProfileInt("esp", "b_crosshair", esp::b_crosshair, file_path);
	WritePrivateProfileInt("esp", "b_distance", esp::b_distance, file_path);
	WritePrivateProfileInt("esp", "b_fov", esp::b_fov, file_path);
	WritePrivateProfileInt("esp", "b_friendly", esp::b_friendly, file_path);
	WritePrivateProfileInt("esp", "b_health", esp::b_health, file_path);
	WritePrivateProfileInt("esp", "b_line", esp::b_line, file_path);
	WritePrivateProfileInt("esp", "b_names", esp::b_names, file_path);
	WritePrivateProfileInt("esp", "b_radar", esp::b_radar, file_path);
	WritePrivateProfileInt("esp", "b_skeleton", esp::b_skeleton, file_path);
	WritePrivateProfileInt("esp", "b_visible", esp::b_visible, file_path);
	WritePrivateProfileInt("esp", "i_fov_size", esp::fov_size, file_path);
	WritePrivateProfileInt("esp", "i_max_distance", esp::max_distance, file_path);
	WritePrivateProfileInt("esp", "i_radar_zoom", esp::radar_zoom, file_path);
	WritePrivateProfileInt("aimbot", "b_smooth", aimbot::b_smooth, file_path);
	WritePrivateProfileInt("aimbot", "b_lock", aimbot::b_lock, file_path);
	WritePrivateProfileInt("aimbot", "b_prediction", aimbot::b_prediction, file_path);
	WritePrivateProfileInt("aimbot", "b_recoil", aimbot::b_recoil, file_path);
	WritePrivateProfileInt("aimbot", "b_skip_knocked", aimbot::b_skip_knocked, file_path);
	WritePrivateProfileInt("aimbot", "b_target_bone", aimbot::b_target_bone, file_path);
	WritePrivateProfileFloat("aimbot", "f_bullet_speed", aimbot::f_bullet_speed, file_path);
	WritePrivateProfileFloat("aimbot", "f_min_closest", aimbot::f_min_closest, file_path);
	WritePrivateProfileFloat("aimbot", "f_smooth", aimbot::f_smooth, file_path);
	WritePrivateProfileFloat("aimbot", "f_speed", aimbot::f_speed, file_path);

}

//examples
void Load_Settings(std::string fileName)
{
	char file_path[MAX_PATH];
	sprintf_s(file_path, xorstr("C:\\yourname\\config\\%s%s"), fileName.c_str(), xorstr(".ini"));
	esp::thickness = GetPrivateProfileIntA("esp", "thickness", esp::thickness, file_path);
	misc::font_index = GetPrivateProfileIntA("misc", "font_index", misc::font_index, file_path);
	esp::line_choose = GetPrivateProfileIntA("esp", "line_choose", esp::line_choose, file_path);
	esp::box_index = GetPrivateProfileIntA("esp", "box_index", esp::box_index, file_path);
	esp::b_uav = GetPrivateProfileIntA("esp", "b_uav", esp::b_uav, file_path);
	esp::enable_auav = GetPrivateProfileIntA("esp", "enable_auav", esp::enable_auav, file_path);
	esp::b_visible_only = GetPrivateProfileIntA("esp", "visible only", esp::b_visible_only, file_path);
	esp::b_box = GetPrivateProfileIntA("esp", "b_box", esp::b_box, file_path);
	esp::b_crosshair = GetPrivateProfileIntA("esp", "b_crosshair", esp::b_crosshair, file_path);
	esp::b_distance = GetPrivateProfileIntA("esp", "b_distance", esp::b_distance, file_path);
	esp::b_fov = GetPrivateProfileIntA("esp", "b_fov", esp::b_fov, file_path);
	esp::b_friendly = GetPrivateProfileIntA("esp", "b_friendly", esp::b_friendly, file_path);
	esp::b_health = GetPrivateProfileIntA("esp", "b_health", esp::b_health, file_path);
	esp::b_line = GetPrivateProfileIntA("esp", "b_line", esp::b_line, file_path);
	esp::b_names = GetPrivateProfileIntA("esp", "b_names", esp::b_names, file_path);
	esp::b_radar = GetPrivateProfileIntA("esp", "b_radar", esp::b_radar, file_path);
	esp::b_skeleton = GetPrivateProfileIntA("esp", "b_skeleton", esp::b_skeleton, file_path);
	esp::b_visible = GetPrivateProfileIntA("esp", "b_visible", esp::b_visible, file_path);
	esp::fov_size = GetPrivateProfileIntA("esp", "i_fov_size", esp::fov_size, file_path);
	esp::max_distance = GetPrivateProfileIntA("esp", "i_max_distance", esp::max_distance, file_path);
	esp::radar_zoom = GetPrivateProfileIntA("esp", "i_radar_zoom", esp::radar_zoom, file_path);
	aimbot::b_smooth = GetPrivateProfileIntA("aimbot", "b_smooth", aimbot::b_smooth, file_path);
	aimbot::b_lock = GetPrivateProfileIntA("aimbot", "b_lock", aimbot::b_lock, file_path);
	aimbot::b_prediction = GetPrivateProfileIntA("aimbot", "b_prediction", aimbot::b_prediction, file_path);
	aimbot::b_recoil = GetPrivateProfileIntA("aimbot", "b_recoil", aimbot::b_recoil, file_path);
	aimbot::b_skip_knocked = GetPrivateProfileIntA("aimbot", "b_skip_knocked", aimbot::b_skip_knocked, file_path);
	aimbot::b_target_bone = GetPrivateProfileIntA("aimbot", "b_target_bone", aimbot::b_target_bone, file_path);
	aimbot::f_bullet_speed = GetPrivateProfileFloat("aimbot", "f_bullet_speed", aimbot::f_bullet_speed, file_path);
	aimbot::f_min_closest = GetPrivateProfileFloat("aimbot", "f_min_closest", aimbot::f_min_closest, file_path);
	aimbot::f_smooth = GetPrivateProfileFloat("aimbot", "f_smooth", aimbot::f_smooth, file_path);



	g_menu::str_config_name = fileName;
};

std::vector<std::string> GetList() {
	std::vector<std::string> configs;
	WIN32_FIND_DATA ffd;
	LPCSTR directory = xorstr("C:\\yourname\\config\\*");
	auto hFind = FindFirstFile(directory, &ffd);
	while (FindNextFile(hFind, &ffd))
	{
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			std::string file_name = ffd.cFileName;
			if (file_name.size() < 4) // .cfg
				continue;
			std::string end = file_name;
			end.erase(end.begin(), end.end() - 4);
			if (end != xorstr(".ini"))
				continue;
			file_name.erase(file_name.end() - 4, file_name.end());
			configs.push_back(file_name);
		}
	}
	return configs;
}

void Auto_Load() {
	if (!GetList().empty()) {
		Load_Settings(GetList().at(0));
	}
	else {
		CreateDirectoryA(xorstr("C:\\yourname"), NULL);
		CreateDirectoryA(xorstr("C:\\yourname\\config"), NULL);
	}
}

}
