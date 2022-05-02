#include "stdafx.h"
//#include "types.h"
#include "minhook/MinHook.h"
#include "sdk.h"
#include<fstream>
#include "xor.hpp"
#define _PRINT_DEBUG

struct StringTable {
	char* name;
	int columnCount;
	int rowCount;
};

struct LootItem {
	int m_itemId;
	int m_itemQuantity;
};
extern BOOL g_running;

extern std::once_flag g_flag;

using DWGetLogonStatus_t = int (*)(int);

using  MoveResponseToInventory_t = bool(__fastcall*)(LPVOID, int);





extern MoveResponseToInventory_t fpMoveResponseOrig;
extern MoveResponseToInventory_t manual_call;






namespace unlock {

	extern __int64 base;
	extern __int64 fpGetLootBase;
	extern __int64 fpGetLogonStatus;
	extern __int64 fpMoveResponseToInventory;
	extern __int64 fpFindStringtable;
	extern __int64 fpStringtableGetColumnValueForRow;


	extern __int64 fpGetLootBase2;
	extern __int64 fpGetLogonStatus2;
	extern __int64 fpMoveResponseToInventory2;
	extern __int64 fpFindStringtable2;
	extern __int64 fpStringtableGetColumnValueForRow2;

	extern bool init();
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

void get_func_ptr();



static void* GetLootBase();


static void FindStringTable(const char* name, StringTable** table);

static char* StringTable_GetColumnValueForRow(void* stringTable, int row, int column);
static void SaveLootTable(StringTable* loot_master);

extern bool __fastcall MoveResponseToInventory_Hooked(LPVOID a1, int a2);

extern void on_attach();