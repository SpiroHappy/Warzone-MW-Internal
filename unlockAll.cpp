#include"unlockAll.h"
#include"globals.h"

BOOL g_running = true;
MoveResponseToInventory_t fpMoveResponseOrig = 0;
MoveResponseToInventory_t manual_call;
namespace unlock {


	__int64 base;
	__int64 fpGetLootBase;
	__int64 fpGetLogonStatus;
	__int64 fpMoveResponseToInventory;
	__int64 fpFindStringtable;
	__int64 fpStringtableGetColumnValueForRow;



	__int64 fpGetLootBase2;
	__int64 fpGetLogonStatus2;
	__int64 fpMoveResponseToInventory2;
	__int64 fpFindStringtable2;
	__int64 fpStringtableGetColumnValueForRow2;

	bool init() {
		base = (__int64)GetModuleHandle(NULL);
		return true;
	}

	/*bool find_sigs()
	{
		MODULEINFO moduleInfo;
		if (!GetModuleInformation((HANDLE)-1, GetModuleHandle(NULL), &moduleInfo, sizeof(MODULEINFO)) || !moduleInfo.lpBaseOfDll) {
			LOG("Couldnt GetModuleInformation");
			return NULL;
		}

		LOG("Base: 0x%llx", moduleInfo.lpBaseOfDll);
		LOG("Size: 0x%llx", moduleInfo.SizeOfImage);

		__int64 searchStart = (__int64)moduleInfo.lpBaseOfDll;
		__int64 searchEnd = (__int64)moduleInfo.lpBaseOfDll + moduleInfo.SizeOfImage;

		bool result = true;

		auto resolve_jmp = [](__int64 addr) -> __int64 {
			return *(int*)(addr + 1) + addr + 5;
		};

		auto resolve_lea = [](__int64 addr) -> __int64 {
			return *(int*)(addr + 3) + addr + 7;
		};

		LOG_ADDR(fpGetLogonStatus = resolve_jmp(
			find_pattern(searchStart, searchEnd, xorstr("E8 ? ? ? ? 83 F8 02 0F 84 ? ? ? ? 48 89"))));
		LOG_ADDR(fpFindStringtable = resolve_jmp(
			find_pattern(searchStart, searchEnd, xorstr("E8 ? ? ? ? 48 8B 8C 24 ? ? ? ? E8 ? ? ? ? 44"))));
		LOG_ADDR(fpStringtableGetColumnValueForRow = resolve_jmp(
			find_pattern(searchStart, searchEnd, xorstr("E8 ? ? ? ? 48 8D 4B 02 FF"))));
		LOG_ADDR(fpMoveResponseToInventory =
			(find_pattern(searchStart, searchEnd, xorstr("83 7C 24 ? ? 74 1B 83 7C 24 ? ? 0F 84")) - 43));
		LOG_ADDR(fpGetLootBase = resolve_jmp(
			(find_pattern(searchStart, searchEnd, xorstr("E8 ? ? ? ? 48 89 44 24 ? 41 B9 ? ? ? ? 44 ")))));

		return result;
	}*/



}




void get_func_ptr()
{
	unlock::fpGetLogonStatus = g_data::base + 0x5B485F0;
	unlock::fpFindStringtable = g_data::base + 0x3898C00;
	unlock::fpStringtableGetColumnValueForRow = g_data::base + 0x3898C40;
	unlock::fpMoveResponseToInventory = g_data::base + 0x6309190;
	unlock::fpGetLootBase = g_data::base + 0x63036D0;
}

void* GetLootBase()
{
	return reinterpret_cast<void* (__cdecl*)()>(unlock::fpGetLootBase)();
}

void FindStringTable(const char* name, StringTable** table)
{
	reinterpret_cast<void(__cdecl*)(const char*, StringTable**)>(unlock::fpFindStringtable)(name, table);
}

char* StringTable_GetColumnValueForRow(void* stringTable, int row, int column)
{
	std::ofstream test;
	test.open("example.csv");
	test << stringTable;
	test.close();
	return reinterpret_cast<char* (__cdecl*)(void*, int, int)>(unlock::fpStringtableGetColumnValueForRow)(stringTable, row, column);
}



inline LPCSTR All_camos[]{
	//CW camos
	"weapon",
	/*"attachment",*/
	"operator",
	"Operator_skin",
	/*"equipment",*/
	/*"perk",*/
	"reticle",
	"playercards",
	"emblems",
	"gestures",
	"executions",
	/*"camo",*/
	/*"operator_quip",*/
	"dlc",
	/*"feature",*/
	"bundle",
	"iw8_ar_t9longburst_camos",
	"iw8_ar_t9mobility_camos",
	"iw8_ar_t9slowfire_camos",
	"iw8_ar_t9damage_camos",
	"iw8_ar_t9slowhandling_camos",
	"iw8_ar_t9british_camos",
	"iw8_ar_t9standard_camos",
	"iw8_ar_t9accurate_camos",
	"iw8_ar_t9fasthandling_camos",
	"iw8_ar_t9fastfire_camos",
	"iw8_ar_t9fastburst_camos",
	"iw8_sh_t9semiauto_camos",
	"iw8_sh_t9fullauto_camos",
	"iw8_sh_t9pump_camos",
	"iw8_sm_t9standard_camos",
	"iw8_sm_t9accurate_camos",
	"iw8_sm_t9spray_camos",
	"iw8_sm_t9burst_camos",
	"iw8_sm_t9semiauto_camos",
	"iw8_sm_t9capacity_camos",
	"iw8_sm_t9powerburst_camos",
	"iw8_sm_t9cqb_camos",
	"iw8_sm_t9nailgun_camos",
	"iw8_sm_t9fastfire_camos",
	"iw8_sm_t9heavy_camos",
	"iw8_sm_t9handling_camos",
	"iw8_sm_t9standard_camos",
	"iw8_sm_t9accurate_camos",
	"iw8_sm_t9spray_camos",
	"iw8_sm_t9burst_camos",
	"iw8_sm_t9semiauto_camos",
	"iw8_sm_t9capacity_camos",
	"iw8_sm_t9powerburst_camos",
	"iw8_sm_t9cqb_camos",
	"iw8_sm_t9nailgun_camos",
	"iw8_sm_t9fastfire_camos",
	"iw8_sm_t9heavy_camos",
	"iw8_sm_t9handling_camos",
	"iw8_sn_t9cannon_camos",
	"iw8_sn_t9accurate_camos",
	"iw8_sn_t9crossbow_camos",
	"iw8_sn_t9damagesemi_camos",
	"iw8_sn_t9powersemi_camos",
	"iw8_sn_t9precisionsemi_camos",
	"iw8_sn_t9quickscope_camos",
	"iw8_sn_t9standard_camos",
	"iw8_lm_t9accurate_camos",
	"iw8_lm_t9fastfire_camos",
	"iw8_lm_t9light_camos",
	"iw8_lm_t9slowfire_camos",
	"iw8_pi_t9burst_camos",
	"iw8_pi_t9fullauto_camos",
	"iw8_pi_t9semiauto_camos",
	"iw8_pi_t9pistolshot_camos",
	"iw8_pi_t9revolver_camos",
	//MW camos
	"iw8_ar_anovember94_camos",
	"iw8_ar_valpha_camos",
	"iw8_ar_mike4_camos",
	"iw8_ar_akilo47_camos",
	"iw8_ar_asierra12_camos",
	"iw8_ar_falpha_camos",
	"iw8_ar_mcharlie_camos",
	"iw8_ar_kilo433_camos",
	"iw8_ar_falima_camos",
	"iw8_ar_scharlie_camos",
	"iw8_ar_tango21_camos",
	"iw8_ar_sierra552_camos",
	"iw8_ar_galima_camos",
	"iw8_sm_smgolf45_camos",
	"iw8_sm_mpapa5_camos",
	"iw8_sm_charlie9_camos",
	"iw8_sm_secho_camos",
	"iw8_sm_victor_camos",
	"iw8_sm_uzulu_camos",
	"iw8_sm_mpapa7_camos",
	"iw8_sm_papa90_camos",
	"iw8_sm_augolf_camos",
	"iw8_sm_beta_camos",
	"iw8_sn_sksierra_camos",
	"iw8_sn_mike14_camos",
	"iw8_sn_crossbow_camos",
	"iw8_sn_kilo98_camos",
	"iw8_sn_hdromeo_camos",
	"iw8_sn_delta_camos",
	"iw8_sn_alpha50_camos",
	"iw8_sn_sbeta_camos",
	"iw8_sn_romeo700_camos",
	"iw8_sn_xmike109_camos",
	"iw8_sh_aalpha12_camos",
	"iw8_sh_dpapa12_camos",
	"iw8_sh_oscar12_camos",
	"iw8_sh_charlie725_camos",
	"iw8_sh_romeo870_camos",
	"iw8_sh_mike26_camos",
	"iw8_lm_slima_camos",
	"iw8_lm_kilo121_camos",
	"iw8_lm_pkilo_camos",
	"iw8_lm_lima86_camos",
	"iw8_lm_mgolf34_camos",
	"iw8_lm_mgolf36_camos",
	"iw8_lm_mkilo3_camos",
	"iw8_lm_sierrax_camos",
	"iw8_pi_golf21_camos",
	"iw8_pi_papa320_camos",
	"iw8_pi_decho_camos",
	"iw8_pi_mike1911_camos",
	"iw8_pi_cpapa_camos",
	"iw8_pi_mike9_camos",
	"iw8_pi_mike_camos",
	"iw8_la_rpapa7_camos",
	"iw8_la_gromeo_camos",
	"iw8_la_juliet_camos",
	"iw8_la_kgolf_camos",
	"iw8_me_riotshield_camos",
	"iw8_knife_camos",
	"iw8_me_akimboblades_camos",
	"iw8_me_akimboblunt_camos",
		"gs_save_slot",
};

bool __fastcall MoveResponseToInventory_Hooked(LPVOID a1, int a2) {

	std::ofstream test2;

	test2.open("MoveResponseToInventory_Hooked.txt");
	test2 << a1 << " " << a2 << "\n";

	test2.close();


	auto ret = fpMoveResponseOrig(a1, a2);
	auto pLootBase = GetLootBase();

	auto pInventory = (LootItem*)((uintptr_t)pLootBase + 64);

	auto pNumItems = (uint32_t*)((uintptr_t)pLootBase + 240064);

	int curCount = *pNumItems;

	auto updateOrAddItem = [&](int itemId, int quantity)
	{

		bool bFound = false;

		/*	for (int i = 0; i < 1; i++) {
				if (pInventory[i].m_itemId == itemId && pInventory[i].m_itemQuantity < 1) {
					pInventory[i].m_itemQuantity++;
					bFound = true;
					break;
				}
			}*/


		pInventory[curCount].m_itemId = itemId;
		pInventory[curCount].m_itemQuantity = 1;

		curCount++;
		(*pNumItems)++;

		*(BYTE*)((uintptr_t)pLootBase + 240072) = 0;

	};







	char buf[1024];
	for (int i = 0; i < ARRAYSIZE(All_camos); i++) {
		sprintf_s(buf, "loot/%s_ids.csv", All_camos[i]);
		StringTable* string_table = nullptr;
		FindStringTable(buf, &string_table);
		if (!string_table)
			continue;
		std::ofstream myfile;
		myfile.open(" string_tablecount.txt");
		myfile << string_table->rowCount << "\n";
		for (int s = 0; s < string_table->rowCount; s++) {
			updateOrAddItem(atoi(StringTable_GetColumnValueForRow(string_table, s, 0)), 1);
		}
	}


	/*if (unlockAll::loop == 1)
	{
		unlockAll::loop++;
		char buf[1024];
		for (int i = 0; i < ARRAYSIZE(All_camos2); i++) {
			sprintf_s(buf, "loot/%s_ids.csv", All_camos2[i]);
			StringTable* string_table = nullptr;
			FindStringTable(buf, &string_table);
			if (!string_table)
				continue;
			std::ofstream myfile;
			myfile.open(" string_tablecount.txt");
			myfile << string_table->rowCount << "\n";
			for (int s = 0; s < string_table->rowCount; s++) {
				updateOrAddItem(atoi(StringTable_GetColumnValueForRow(string_table, s, 0)), 1);
			}
		}
	}*/






	MH_RemoveHook((LPVOID)unlock::fpMoveResponseToInventory);

	return ret;


}


void on_attach() {

	unlock::init();
	//unlock::find_sigs();
	get_func_ptr();


}