#pragma once
#include "stdafx.h"
#include "lazyimporter.h"
#include <type_traits>

#define in_range(x, a, b) (x >= a && x <= b)
#define get_bits(x) (in_range((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xA): (in_range(x, '0', '9') ? x - '0': 0))
#define get_byte(x) (get_bits(x[0]) << 4 | get_bits(x[1]))

struct EnumData 
{
	DWORD dwProcessId;
	HWND hWnd;
};

namespace memory 
{
	inline uintptr_t occurence(const char* module, const char* pattern) 
	{
		MODULEINFO mod;
		iat(K32GetModuleInformation).get()(iat(GetCurrentProcess).get()(), iat(GetModuleHandleA).get()(module), &mod, sizeof(MODULEINFO));
		uintptr_t start = (uintptr_t)mod.lpBaseOfDll;
		uintptr_t end = (uintptr_t)mod.lpBaseOfDll + (uintptr_t)mod.SizeOfImage;
		uintptr_t match = (uintptr_t)nullptr;
		const char* current = pattern;

		for (uintptr_t pCur = start; pCur < end; pCur++) {

			if (!*current)
				return match;

			if (*(PBYTE)current == ('\?') || *(BYTE*)pCur == get_byte(current)) {
				if (!match)
					match = pCur;

				if (!current[2])
					return match;

				if (*(PWORD)current == ('\?\?') || *(PBYTE)current != ('\?'))
					current += 3;
				else
					current += 2;
			}
			else {
				current = pattern;
				match = 0;
			}
		}
		return (uintptr_t)nullptr;
	}

	inline uintptr_t dereference(uintptr_t address, unsigned int offset)
	{
		if (address == 0)
			return (uintptr_t)nullptr;

		if (sizeof(uintptr_t) == 8)
			return address + (int)((*(int*)(address + offset) + offset) + sizeof(int));

		return *(uintptr_t*)(address + offset);
	}
}
