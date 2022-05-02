#include "stdafx.h"
#include "sdk.h"
#include "xor.hpp"
#include "lazyimporter.h"
#include "memory.h"
#include <map>
#include "defs.h"
#include "globals.h"


namespace process
{
	HWND hwnd;

	BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
	{
		DWORD dwPid = 0;
		GetWindowThreadProcessId(hWnd, &dwPid);
		if (dwPid == lParam)
		{
			hwnd = hWnd;
			return FALSE;
		}
		return TRUE;
	}

	HWND get_process_window()
	{
		if (hwnd)
			return hwnd;

		EnumWindows(EnumWindowCallBack, GetCurrentProcessId());

		if (hwnd == NULL)
			Exit();

		return hwnd;
	}
}

namespace g_data
{
	uintptr_t base;
	uintptr_t peb;
	HWND hWind;
	uintptr_t visible_base;


	void init()
	{
		base = (uintptr_t)(iat(GetModuleHandleA).get()("ModernWarfare.exe"));

		hWind = process::get_process_window();

		peb = __readgsqword(0x60);
	}
}

namespace sdk
{
	const DWORD nTickTime = 64;//64 ms
	bool bUpdateTick = false;
	std::map<DWORD, velocityInfo_t> velocityMap;
	uint64_t current_visible_offset;
	uint64_t last_visible_offset;
	uint64_t cached_visible_base;

	uintptr_t _get_player(int i)
	{
		auto cl_info_base = get_client_info_base();

		if (is_bad_ptr(cl_info_base))return 0;
		
		
			auto base_address = *(uintptr_t*)(cl_info_base);
			if (is_bad_ptr(base_address))return 0;

				return sdk::get_client_info_base() + (i * player_info::size);

	}
	bool in_game()
	{
		auto gameMode = *(int*)(g_data::base + game_mode);
		return  gameMode > 4;
	}

	int get_game_mode()
	{
		return *(int*)(g_data::base + game_mode + 0x4);
	}

	int get_max_player_count()
	{
		return *(int*)(g_data::base + game_mode);
	}

	Vector3 _get_pos(uintptr_t address)
	{
		auto local_pos_ptr = *(uintptr_t*)((uintptr_t)address + player_info::position_ptr);

		if (local_pos_ptr)
		{
			return *(Vector3*)(local_pos_ptr + 0x40);
		}
		return Vector3{};
	}

	uint32_t _get_index(uintptr_t address)
	{
		auto cl_info_base = get_client_info_base();

		if (is_bad_ptr(cl_info_base))return 0;

		return ((uintptr_t)address - cl_info_base) / player_info::size;
	
		
	}

	int _team_id(uintptr_t address)    {

		return *(int*)((uintptr_t)address + player_info::team_id);
	}

	int decrypt_visible_flag(int i, QWORD valid_list)
	{
		auto ptr = valid_list + ((i + i * 8) * 8) + 0xA83; //80 BF ? ? ? ? ? 74 20 80 BF ? ? ? ? ? 74 17
		DWORD dw1 = (*(DWORD*)(ptr + 4) ^ (DWORD)ptr);
		DWORD dw2 = ((dw1 + 2) * dw1);
		BYTE dec_visible_flag = *(BYTE*)(ptr) ^ BYTE1(dw2) ^ (BYTE)dw2;

		return (int)dec_visible_flag;
	}


	bool _is_visible(uintptr_t address)
	{
		if (IsValidPtr<uintptr_t>(&g_data::visible_base))
		{
			uint64_t VisibleList = *(uint64_t*)(g_data::visible_base + 0x108);
			if (is_bad_ptr( VisibleList))
				return false;

			uint64_t rdx = VisibleList + (_get_index(address) * 9 + 0x14E) * 8;
			if (is_bad_ptr(rdx))
				return false;

			DWORD VisibleFlags = (rdx + 0x10) ^ (*(DWORD*)(rdx + 0x14));
			if (is_bad_ptr(VisibleFlags))
				return false;

			DWORD v511 = VisibleFlags * (VisibleFlags + 2);
			if (!v511)
				return false;

			BYTE VisibleFlags1 = *(DWORD*)(rdx + 0x10) ^ v511 ^ BYTE1(v511);
			if (VisibleFlags1 == 3) {
				return true;
			}
		}
		return false;
	}




	uint64_t get_client_info()
	{
		auto baseModuleAddr = g_data::base;
		auto Peb = __readgsqword(0x60);
		uint64_t rax = baseModuleAddr, rbx = baseModuleAddr, rcx = baseModuleAddr, rdx = baseModuleAddr, rdi = baseModuleAddr, rsi = baseModuleAddr, r8 = baseModuleAddr, r9 = baseModuleAddr, r10 = baseModuleAddr, r11 = baseModuleAddr, r12 = baseModuleAddr, r13 = baseModuleAddr, r14 = baseModuleAddr, r15 = baseModuleAddr;
		rbx = *(uint64_t*)(baseModuleAddr + 0x1E910518);
		if (!rbx)
			return rbx;
		rdx = Peb;              //mov rdx, gs:[rax]
		rax = 0;                //and rax, 0xFFFFFFFFC0000000
		rax = _rotl64(rax, 0x10);               //rol rax, 0x10
		rax ^= *(uint64_t*)(baseModuleAddr + 0x71510ED);                //xor rax, [0x0000000004ED7491]
		rax = _byteswap_uint64(rax);            //bswap rax
		rbx *= *(uint64_t*)(rax + 0xb);                 //imul rbx, [rax+0x0B]
		rbx += rdx;             //add rbx, rdx
		rax = rbx;              //mov rax, rbx
		rcx = rbx;              //mov rcx, rbx
		rax >>= 0xB;            //shr rax, 0x0B
		rbx = rdx;              //mov rbx, rdx
		rcx ^= rax;             //xor rcx, rax
		rax = rcx;              //mov rax, rcx
		rax >>= 0x16;           //shr rax, 0x16
		rcx ^= rax;             //xor rcx, rax
		rax = baseModuleAddr + 0x5FE0;          //lea rax, [0xFFFFFFFFFDD8C358]
		rbx *= rax;             //imul rbx, rax
		rax = rcx;              //mov rax, rcx
		rax >>= 0x2C;           //shr rax, 0x2C
		rbx ^= rax;             //xor rbx, rax
		rax = 0xC4138E51387F1EA1;               //mov rax, 0xC4138E51387F1EA1
		rbx ^= rcx;             //xor rbx, rcx
		rbx *= rax;             //imul rbx, rax
		rax = 0x42D230AEBD9F3922;               //mov rax, 0x42D230AEBD9F3922
		rbx ^= rax;             //xor rbx, rax
		return rbx;
	}
	uint64_t get_client_info_base()
	{
		auto baseModuleAddr = g_data::base;
		auto Peb = __readgsqword(0x60);
		uint64_t rax = baseModuleAddr, rbx = baseModuleAddr, rcx = baseModuleAddr, rdx = baseModuleAddr, rdi = baseModuleAddr, rsi = baseModuleAddr, r8 = baseModuleAddr, r9 = baseModuleAddr, r10 = baseModuleAddr, r11 = baseModuleAddr, r12 = baseModuleAddr, r13 = baseModuleAddr, r14 = baseModuleAddr, r15 = baseModuleAddr;
		rax = *(uint64_t*)(get_client_info() + 0xae308);
		if (!rax)
			return rax;
		rdi = Peb;              //mov rdi, gs:[rcx]
		rcx = rdi;              //mov rcx, rdi
		rcx = _rotl64(rcx, 0x22);               //rol rcx, 0x22
		rcx &= 0xF;

		switch (rcx) {
		case 0:
		{
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD862D9]
			r10 = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov r10, [0x0000000004ED7392]
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x2;            //shr rcx, 0x02
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x4;            //shr rcx, 0x04
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x8;            //shr rcx, 0x08
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x10;           //shr rcx, 0x10
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x20;           //shr rcx, 0x20
			rax ^= rcx;             //xor rax, rcx
			rcx = 0x3614DC28B2B4B103;               //mov rcx, 0x3614DC28B2B4B103
			rax *= rcx;             //imul rax, rcx
			rcx = baseModuleAddr + 0x6965;          //lea rcx, [0xFFFFFFFFFDD8C9C8]
			rax += rdi;             //add rax, rdi
			rax += rcx;             //add rax, rcx
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r10;             //xor rcx, r10
			rcx = ~rcx;             //not rcx
			rax *= *(uint64_t*)(rcx + 0x9);                 //imul rax, [rcx+0x09]
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x26;           //shr rcx, 0x26
			rax ^= rcx;             //xor rax, rcx
			rcx = 0xC0E9608E29382049;               //mov rcx, 0xC0E9608E29382049
			rax *= rcx;             //imul rax, rcx
			rax += rbx;             //add rax, rbx
			rcx = 0x8620C22020E040C1;               //mov rcx, 0x8620C22020E040C1
			rax ^= rcx;             //xor rax, rcx
			return rax;
		}
		case 1:
		{
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD85D06]
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151139);          //mov r9, [0x0000000004ED6DDE]
			rax += rdi;             //add rax, rdi
			rcx = 0xCB4523AB3671CB5B;               //mov rcx, 0xCB4523AB3671CB5B
			rax *= rcx;             //imul rax, rcx
			rax += rbx;             //add rax, rbx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x26;           //shr rcx, 0x26
			rax ^= rcx;             //xor rax, rcx
			rcx = 0x5CC5BC52843E5AF;                //mov rcx, 0x5CC5BC52843E5AF
			rax *= rcx;             //imul rax, rcx
			r11 = 0x4B971A0A30E5AC0;                //mov r11, 0x4B971A0A30E5AC0
			rax += r11;             //add rax, r11
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r9;              //xor rcx, r9
			rcx = ~rcx;             //not rcx
			rax *= *(uint64_t*)(rcx + 0x9);                 //imul rax, [rcx+0x09]
			rcx = rdi;              //mov rcx, rdi
			rcx = ~rcx;             //not rcx
			rax += rcx;             //add rax, rcx
			rax -= rbx;             //sub rax, rbx
			rax -= 0x2BD942E4;              //sub rax, 0x2BD942E4
			return rax;
		}
		case 2:
		{
			r10 = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov r10, [0x0000000004ED6A75]
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD8592A]
			rdx = baseModuleAddr + 0x42E99BA4;              //lea rdx, [0x0000000040C1F477]
			rcx = rdi;              //mov rcx, rdi
			rcx = ~rcx;             //not rcx
			rcx ^= rax;             //xor rcx, rax
			rax = baseModuleAddr + 0x1BCD6F2F;              //lea rax, [0x0000000019A5C511]
			rcx ^= rax;             //xor rcx, rax
			rax = rdi;              //mov rax, rdi
			rax ^= rdx;             //xor rax, rdx
			rcx += rax;             //add rcx, rax
			rax = rdi + 0xffffffffffff6769;                 //lea rax, [rdi-0x9897]
			rcx -= rbx;             //sub rcx, rbx
			rax += rcx;             //add rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x20;           //shr rcx, 0x20
			rax ^= rcx;             //xor rax, rcx
			rcx = 0xA8C9167DC25FE0F;                //mov rcx, 0xA8C9167DC25FE0F
			rax *= rcx;             //imul rax, rcx
			rcx = 0x415794CE48E83B12;               //mov rcx, 0x415794CE48E83B12
			rax += rcx;             //add rax, rcx
			r11 = 0x38D8DF6FD893701F;               //mov r11, 0x38D8DF6FD893701F
			rax += r11;             //add rax, r11
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r10;             //xor rcx, r10
			rcx = ~rcx;             //not rcx
			rax *= *(uint64_t*)(rcx + 0x9);                 //imul rax, [rcx+0x09]
			return rax;
		}
		case 3:
		{
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD85506]
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151139);          //mov r9, [0x0000000004ED65DA]
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x1C;           //shr rcx, 0x1C
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x38;           //shr rcx, 0x38
			rax ^= rcx;             //xor rax, rcx
			rax += rbx;             //add rax, rbx
			rcx = rbx + 0x72d5b242;                 //lea rcx, [rbx+0x72D5B242]
			rcx += rdi;             //add rcx, rdi
			rax ^= rcx;             //xor rax, rcx
			rax ^= rdi;             //xor rax, rdi
			rcx = 0x4730EC87E12C22C;                //mov rcx, 0x4730EC87E12C22C
			rax ^= rcx;             //xor rax, rcx
			rcx = 0x32B4C91E2D57D988;               //mov rcx, 0x32B4C91E2D57D988
			rax += rcx;             //add rax, rcx
			rcx = 0xFB6F374D513999ED;               //mov rcx, 0xFB6F374D513999ED
			rax *= rcx;             //imul rax, rcx
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r9;              //xor rcx, r9
			rcx = ~rcx;             //not rcx
			rax *= *(uint64_t*)(rcx + 0x9);                 //imul rax, [rcx+0x09]
			return rax;
		}
		case 4:
		{
			r10 = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov r10, [0x0000000004ED612C]
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD84FEC]
			rax += rbx;             //add rax, rbx
			rcx = 0xE8E1551BD670F715;               //mov rcx, 0xE8E1551BD670F715
			rax *= rcx;             //imul rax, rcx
			rcx = 0x1DAD297E91C0E8AE;               //mov rcx, 0x1DAD297E91C0E8AE
			rax -= rcx;             //sub rax, rcx
			rcx = rdi;              //mov rcx, rdi
			rcx -= rbx;             //sub rcx, rbx
			rcx += 0xFFFFFFFF81A79BB7;              //add rcx, 0xFFFFFFFF81A79BB7
			rax += rcx;             //add rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x25;           //shr rcx, 0x25
			rax ^= rcx;             //xor rax, rcx
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r10;             //xor rcx, r10
			rcx = ~rcx;             //not rcx
			rax *= *(uint64_t*)(rcx + 0x9);                 //imul rax, [rcx+0x09]
			rcx = 0x87FB17E76E4325C5;               //mov rcx, 0x87FB17E76E4325C5
			rax += rcx;             //add rax, rcx
			rax += rbx;             //add rax, rbx
			return rax;
		}
		case 5:
		{
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD84AFC]
			rdx = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov rdx, [0x0000000004ED5BD9]
			rcx = 0x43A6F57288B545E2;               //mov rcx, 0x43A6F57288B545E2
			rax += rcx;             //add rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x16;           //shr rcx, 0x16
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x2C;           //shr rcx, 0x2C
			rax ^= rcx;             //xor rax, rcx
			rax -= rdi;             //sub rax, rdi
			rcx = baseModuleAddr + 0x415C4D63;              //lea rcx, [0x000000003F349464]
			rcx -= rdi;             //sub rcx, rdi
			rax ^= rcx;             //xor rax, rcx
			rcx = 0xDAA0ABF4F2176CF9;               //mov rcx, 0xDAA0ABF4F2176CF9
			rax *= rcx;             //imul rax, rcx
			rax -= rbx;             //sub rax, rbx
			uintptr_t RSP_0xFFFFFFFFFFFFFF80;
			RSP_0xFFFFFFFFFFFFFF80 = 0x5660AE0D8FE019D3;            //mov rcx, 0x5660AE0D8FE019D3 : RBP+0xFFFFFFFFFFFFFF80
			rax += RSP_0xFFFFFFFFFFFFFF80;          //add rax, [rbp-0x80]
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= rdx;             //xor rcx, rdx
			rcx = ~rcx;             //not rcx
			rax *= *(uint64_t*)(rcx + 0x9);                 //imul rax, [rcx+0x09]
			return rax;
		}
		case 6:
		{
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD846CC]
			r10 = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov r10, [0x0000000004ED57A5]
			rax -= rbx;             //sub rax, rbx
			uintptr_t RSP_0x68;
			RSP_0x68 = 0xD581FA5F208D6FBB;          //mov rcx, 0xD581FA5F208D6FBB : RSP+0x68
			rax ^= RSP_0x68;                //xor rax, [rsp+0x68]
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r10;             //xor rcx, r10
			rcx = ~rcx;             //not rcx
			rax *= *(uint64_t*)(rcx + 0x9);                 //imul rax, [rcx+0x09]
			rcx = 0x5098219BA5D3A99F;               //mov rcx, 0x5098219BA5D3A99F
			rax *= rcx;             //imul rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x1E;           //shr rcx, 0x1E
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x3C;           //shr rcx, 0x3C
			rax ^= rcx;             //xor rax, rcx
			rcx = 0x3A60D00674385E4B;               //mov rcx, 0x3A60D00674385E4B
			rax ^= rcx;             //xor rax, rcx
			rcx = rdi;              //mov rcx, rdi
			rcx = ~rcx;             //not rcx
			uintptr_t RSP_0xFFFFFFFFFFFFFF80;
			RSP_0xFFFFFFFFFFFFFF80 = baseModuleAddr + 0x7946FC98;           //lea rcx, [0x00000000771F4379] : RBP+0xFFFFFFFFFFFFFF80
			rcx += RSP_0xFFFFFFFFFFFFFF80;          //add rcx, [rbp-0x80]
			rax ^= rcx;             //xor rax, rcx
			rax += rdi;             //add rax, rdi
			return rax;
		}
		case 7:
		{
			r10 = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov r10, [0x0000000004ED537F]
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD8423F]
			rax -= rbx;             //sub rax, rbx
			rcx = 0xCF11CC43A518562C;               //mov rcx, 0xCF11CC43A518562C
			rax ^= rcx;             //xor rax, rcx
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r10;             //xor rcx, r10
			rcx = ~rcx;             //not rcx
			rax *= *(uint64_t*)(rcx + 0x9);                 //imul rax, [rcx+0x09]
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x23;           //shr rcx, 0x23
			rax ^= rcx;             //xor rax, rcx
			rcx = rbx + 0x4444;             //lea rcx, [rbx+0x4444]
			rcx += rdi;             //add rcx, rdi
			rax ^= rcx;             //xor rax, rcx
			rcx = 0x7E57D567755F9808;               //mov rcx, 0x7E57D567755F9808
			rax ^= rcx;             //xor rax, rcx
			rcx = 0xA0F53D7EAD386BD7;               //mov rcx, 0xA0F53D7EAD386BD7
			rax *= rcx;             //imul rax, rcx
			rcx = rbx + 0x7ead;             //lea rcx, [rbx+0x7EAD]
			rcx += rdi;             //add rcx, rdi
			rax ^= rcx;             //xor rax, rcx
			return rax;
		}
		case 8:
		{
			r10 = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov r10, [0x0000000004ED4EDE]
			rcx = rax;              //mov rcx, rax
			rcx >>= 0xA;            //shr rcx, 0x0A
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x14;           //shr rcx, 0x14
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x28;           //shr rcx, 0x28
			rax ^= rcx;             //xor rax, rcx
			rcx = baseModuleAddr + 0xEF19;          //lea rcx, [0xFFFFFFFFFDD92A16]
			rdx = rdi;              //mov rdx, rdi
			rax += rcx;             //add rax, rcx
			rdx = ~rdx;             //not rdx
			rax += rdx;             //add rax, rdx
			rdx = baseModuleAddr + 0x8630;          //lea rdx, [0xFFFFFFFFFDD8C334]
			rcx = 0xE3E5140ADA297923;               //mov rcx, 0xE3E5140ADA297923
			rax ^= rcx;             //xor rax, rcx
			rcx = rdi;              //mov rcx, rdi
			rcx = ~rcx;             //not rcx
			rcx *= rdx;             //imul rcx, rdx
			rax += rcx;             //add rax, rcx
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r10;             //xor rcx, r10
			rcx = ~rcx;             //not rcx
			rcx = *(uint64_t*)(rcx + 0x9);          //mov rcx, [rcx+0x09]
			uintptr_t RSP_0xFFFFFFFFFFFFFF80;
			RSP_0xFFFFFFFFFFFFFF80 = 0x4731D26EBE5B963F;            //mov rcx, 0x4731D26EBE5B963F : RBP+0xFFFFFFFFFFFFFF80
			rcx *= RSP_0xFFFFFFFFFFFFFF80;          //imul rcx, [rbp-0x80]
			rax *= rcx;             //imul rax, rcx
			rcx = 0x6F73924E5D00230B;               //mov rcx, 0x6F73924E5D00230B
			rax -= rcx;             //sub rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x5;            //shr rcx, 0x05
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0xA;            //shr rcx, 0x0A
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x14;           //shr rcx, 0x14
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x28;           //shr rcx, 0x28
			rax ^= rcx;             //xor rax, rcx
			return rax;
		}
		case 9:
		{
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD837C8]
			r11 = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov r11, [0x0000000004ED48AF]
			rcx = rax;              //mov rcx, rax
			rcx >>= 0xB;            //shr rcx, 0x0B
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x16;           //shr rcx, 0x16
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x2C;           //shr rcx, 0x2C
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x2;            //shr rcx, 0x02
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x4;            //shr rcx, 0x04
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x8;            //shr rcx, 0x08
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x10;           //shr rcx, 0x10
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x20;           //shr rcx, 0x20
			rax ^= rcx;             //xor rax, rcx
			rdx = rax;              //mov rdx, rax
			rdx >>= 0x23;           //shr rdx, 0x23
			rdx ^= rax;             //xor rdx, rax
			r8 = 0;                 //and r8, 0xFFFFFFFFC0000000
			r8 = _rotl64(r8, 0x10);                 //rol r8, 0x10
			r8 ^= r11;              //xor r8, r11
			rax = 0xE068C563D16D8F05;               //mov rax, 0xE068C563D16D8F05
			rdx *= rax;             //imul rdx, rax
			rax = 0xE22A467A21B263F1;               //mov rax, 0xE22A467A21B263F1
			r8 = ~r8;               //not r8
			rcx = rbx;              //mov rcx, rbx
			rcx *= rax;             //imul rcx, rax
			rax = 0xA794251B31804DF7;               //mov rax, 0xA794251B31804DF7
			rax += rdx;             //add rax, rdx
			rax += rcx;             //add rax, rcx
			rax *= *(uint64_t*)(r8 + 0x9);          //imul rax, [r8+0x09]
			return rax;
		}
		case 10:
		{
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD83351]
			r11 = baseModuleAddr + 0x21BE859E;              //lea r11, [0x000000001F96B8D7]
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151139);          //mov r9, [0x0000000004ED441F]
			rcx = rax;              //mov rcx, rax
			rcx >>= 0xA;            //shr rcx, 0x0A
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x14;           //shr rcx, 0x14
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x28;           //shr rcx, 0x28
			rax ^= rcx;             //xor rax, rcx
			rax -= rbx;             //sub rax, rbx
			rax -= rdi;             //sub rax, rdi
			rcx = 0x6A3FC3C778863DF1;               //mov rcx, 0x6A3FC3C778863DF1
			rax *= rcx;             //imul rax, rcx
			rcx = r11;              //mov rcx, r11
			rcx = ~rcx;             //not rcx
			rcx ^= rdi;             //xor rcx, rdi
			rax -= rcx;             //sub rax, rcx
			rcx = 0xF950B1E674466089;               //mov rcx, 0xF950B1E674466089
			rax ^= rcx;             //xor rax, rcx
			rax -= rbx;             //sub rax, rbx
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r9;              //xor rcx, r9
			rcx = ~rcx;             //not rcx
			rax *= *(uint64_t*)(rcx + 0x9);                 //imul rax, [rcx+0x09]
			return rax;
		}
		case 11:
		{
			rbx = baseModuleAddr + 0xECD7;          //lea rbx, [0xFFFFFFFFFDD91B73]
			r10 = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov r10, [0x0000000004ED3F73]
			rcx = rbx;              //mov rcx, rbx
			rcx ^= rdi;             //xor rcx, rdi
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x27;           //shr rcx, 0x27
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x18;           //shr rcx, 0x18
			rax ^= rcx;             //xor rax, rcx
			rdx = 0;                //and rdx, 0xFFFFFFFFC0000000
			rcx = rax;              //mov rcx, rax
			rdx = _rotl64(rdx, 0x10);               //rol rdx, 0x10
			rdx ^= r10;             //xor rdx, r10
			rcx >>= 0x30;           //shr rcx, 0x30
			rax ^= rcx;             //xor rax, rcx
			rdx = ~rdx;             //not rdx
			rax *= *(uint64_t*)(rdx + 0x9);                 //imul rax, [rdx+0x09]
			rcx = 0x60C7EC28E7999765;               //mov rcx, 0x60C7EC28E7999765
			rax *= rcx;             //imul rax, rcx
			rcx = 0x7376CF3E6CF9900C;               //mov rcx, 0x7376CF3E6CF9900C
			rax += rcx;             //add rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x4;            //shr rcx, 0x04
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x8;            //shr rcx, 0x08
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x10;           //shr rcx, 0x10
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x20;           //shr rcx, 0x20
			rax ^= rcx;             //xor rax, rcx
			rcx = 0xF4A897B8F6F79DBD;               //mov rcx, 0xF4A897B8F6F79DBD
			rax *= rcx;             //imul rax, rcx
			return rax;
		}
		case 12:
		{
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD829D3]
			r10 = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov r10, [0x0000000004ED3AA3]
			rcx = baseModuleAddr + 0x407F;          //lea rcx, [0xFFFFFFFFFDD86588]
			rcx = ~rcx;             //not rcx
			rcx += rdi;             //add rcx, rdi
			rax += rcx;             //add rax, rcx
			rcx = 0xD34834AD3FF6BC4;                //mov rcx, 0xD34834AD3FF6BC4
			rax ^= rcx;             //xor rax, rcx
			rcx = 0x564A48D6BFC062D7;               //mov rcx, 0x564A48D6BFC062D7
			rax *= rcx;             //imul rax, rcx
			rcx = 0x33DA61190ABBC492;               //mov rcx, 0x33DA61190ABBC492
			rax -= rcx;             //sub rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x1F;           //shr rcx, 0x1F
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x3E;           //shr rcx, 0x3E
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0xC;            //shr rcx, 0x0C
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x18;           //shr rcx, 0x18
			rax ^= rcx;             //xor rax, rcx
			rdx = 0;                //and rdx, 0xFFFFFFFFC0000000
			rdx = _rotl64(rdx, 0x10);               //rol rdx, 0x10
			rdx ^= r10;             //xor rdx, r10
			rcx = rax;              //mov rcx, rax
			rdx = ~rdx;             //not rdx
			rcx >>= 0x30;           //shr rcx, 0x30
			rax ^= rcx;             //xor rax, rcx
			rax *= *(uint64_t*)(rdx + 0x9);                 //imul rax, [rdx+0x09]
			rax -= rbx;             //sub rax, rbx
			return rax;
		}
		case 13:
		{
			r11 = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov r11, [0x0000000004ED35D2]
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD82492]
			rdx = 0xA7EF47892BCE6574;               //mov rdx, 0xA7EF47892BCE6574
			rax ^= rbx;             //xor rax, rbx
			rcx = baseModuleAddr + 0x2C31;          //lea rcx, [0xFFFFFFFFFDD84FC2]
			rcx = ~rcx;             //not rcx
			rcx ^= rdi;             //xor rcx, rdi
			rcx += rdx;             //add rcx, rdx
			rax += rcx;             //add rax, rcx
			rcx = 0x118F0BC41931911C;               //mov rcx, 0x118F0BC41931911C
			rax += rcx;             //add rax, rcx
			rdx = rax;              //mov rdx, rax
			rdx >>= 0x27;           //shr rdx, 0x27
			rdx ^= rax;             //xor rdx, rax
			rax = rdi;              //mov rax, rdi
			rax = ~rax;             //not rax
			rax += rdx;             //add rax, rdx
			rax -= rbx;             //sub rax, rbx
			rcx = 0x512A06E5A89C89E3;               //mov rcx, 0x512A06E5A89C89E3
			r8 = 0;                 //and r8, 0xFFFFFFFFC0000000
			rax *= rcx;             //imul rax, rcx
			r8 = _rotl64(r8, 0x10);                 //rol r8, 0x10
			rcx = 0x2F5211956108A689;               //mov rcx, 0x2F5211956108A689
			r8 ^= r11;              //xor r8, r11
			rax -= rcx;             //sub rax, rcx
			r8 = ~r8;               //not r8
			rax *= *(uint64_t*)(r8 + 0x9);          //imul rax, [r8+0x09]
			return rax;
		}
		case 14:
		{
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151139);          //mov r9, [0x0000000004ED3237]
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x6;            //shr rcx, 0x06
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0xC;            //shr rcx, 0x0C
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x18;           //shr rcx, 0x18
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x30;           //shr rcx, 0x30
			rax ^= rcx;             //xor rax, rcx
			rcx = 0x8F69920EB2B46367;               //mov rcx, 0x8F69920EB2B46367
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x3;            //shr rcx, 0x03
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x6;            //shr rcx, 0x06
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0xC;            //shr rcx, 0x0C
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x18;           //shr rcx, 0x18
			rax ^= rcx;             //xor rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x30;           //shr rcx, 0x30
			rax ^= rcx;             //xor rax, rcx
			rcx = 0x35833DAFED12173F;               //mov rcx, 0x35833DAFED12173F
			rax += rcx;             //add rax, rcx
			rax -= rdi;             //sub rax, rdi
			rcx = 0xB0007B38500FE4E7;               //mov rcx, 0xB0007B38500FE4E7
			rax *= rcx;             //imul rax, rcx
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r9;              //xor rcx, r9
			rcx = ~rcx;             //not rcx
			rax *= *(uint64_t*)(rcx + 0x9);                 //imul rax, [rcx+0x09]
			rax -= rdi;             //sub rax, rdi
			return rax;
		}
		case 15:
		{
			r10 = *(uint64_t*)(baseModuleAddr + 0x7151139);                 //mov r10, [0x0000000004ED2D5A]
			rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDD81C1A]
			r12 = baseModuleAddr + 0x7481;          //lea r12, [0xFFFFFFFFFDD89083]
			rcx = rdi;              //mov rcx, rdi
			rcx = ~rcx;             //not rcx
			uintptr_t RSP_0xFFFFFFFFFFFFFFB8;
			RSP_0xFFFFFFFFFFFFFFB8 = baseModuleAddr + 0x1610;               //lea rcx, [0xFFFFFFFFFDD831EE] : RBP+0xFFFFFFFFFFFFFFB8
			rcx ^= RSP_0xFFFFFFFFFFFFFFB8;          //xor rcx, [rbp-0x48]
			rax -= rcx;             //sub rax, rcx
			rcx = rax;              //mov rcx, rax
			rcx >>= 0x21;           //shr rcx, 0x21
			rax ^= rcx;             //xor rax, rcx
			rax ^= rbx;             //xor rax, rbx
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r10;             //xor rcx, r10
			rcx = ~rcx;             //not rcx
			rax *= *(uint64_t*)(rcx + 0x9);                 //imul rax, [rcx+0x09]
			rcx = 0x1878E588DC30511;                //mov rcx, 0x1878E588DC30511
			rax *= rcx;             //imul rax, rcx
			rcx = 0x1BDFAD63E7B0499E;               //mov rcx, 0x1BDFAD63E7B0499E
			rax += rcx;             //add rax, rcx
			rcx = 0xD2F688CE4CF2546B;               //mov rcx, 0xD2F688CE4CF2546B
			rax ^= rcx;             //xor rax, rcx
			rcx = rdi + 0x1;                //lea rcx, [rdi+0x01]
			rcx *= r12;             //imul rcx, r12
			rax += rcx;             //add rax, rcx
			return rax;
		}
		}

	}
	uint64_t get_bone_ptr()
	{
		auto baseModuleAddr = g_data::base;
		auto Peb = __readgsqword(0x60);
		uint64_t rax = baseModuleAddr, rbx = baseModuleAddr, rcx = baseModuleAddr, rdx = baseModuleAddr, rdi = baseModuleAddr, rsi = baseModuleAddr, r8 = baseModuleAddr, r9 = baseModuleAddr, r10 = baseModuleAddr, r11 = baseModuleAddr, r12 = baseModuleAddr, r13 = baseModuleAddr, r14 = baseModuleAddr, r15 = baseModuleAddr;
		rdx = *(uint64_t*)(baseModuleAddr + 0x1BA9BFE8);
		if (!rdx)
			return rdx;
		r10 = Peb;           //mov r10, gs:[rax]
		rax = r10;              //mov rax, r10
		rax >>= 0x13;           //shr rax, 0x13
		rax &= 0xF;

		switch (rax) {
		case 0:
		{
			r15 = baseModuleAddr + 0x7CEA;          //lea r15, [0xFFFFFFFFFDC958D7]
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC8DBD5]
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDEDCA]
			rdx ^= r10;             //xor rdx, r10
			rcx = r10;              //mov rcx, r10
			rcx = ~rcx;             //not rcx
			rax = r15;              //mov rax, r15
			rax = ~rax;             //not rax
			rdx += rax;             //add rdx, rax
			rdx += rcx;             //add rdx, rcx
			rdx -= r10;             //sub rdx, r10
			rax = 0;                //and rax, 0xFFFFFFFFC0000000
			rax = _rotl64(rax, 0x10);               //rol rax, 0x10
			rax ^= r9;              //xor rax, r9
			rax = ~rax;             //not rax
			rdx *= *(uint64_t*)(rax + 0xd);                 //imul rdx, [rax+0x0D]
			rax = rdx;              //mov rax, rdx
			rax >>= 0x23;           //shr rax, 0x23
			rdx ^= rax;             //xor rdx, rax
			rax = 0xC8F0AF33880E5BCF;               //mov rax, 0xC8F0AF33880E5BCF
			rdx *= rax;             //imul rdx, rax
			rax = r11 + 0x11b0;             //lea rax, [r11+0x11B0]
			rax += r10;             //add rax, r10
			rdx ^= rax;             //xor rdx, rax
			rax = r10;              //mov rax, r10
			rax -= r11;             //sub rax, r11
			rax += 0xFFFFFFFFB40A59F8;              //add rax, 0xFFFFFFFFB40A59F8
			rdx += rax;             //add rdx, rax
			return rdx;
		}
		case 1:
		{
			//failed to translate: pop rdx
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDE964]
			r15 = baseModuleAddr + 0x1851D8C4;              //lea r15, [0x00000000161AAFDD]
			r12 = baseModuleAddr + 0x6D105AE1;              //lea r12, [0x000000006AD931EE]
			r13 = baseModuleAddr + 0x2E0A0DA9;              //lea r13, [0x000000002BD2E4AA]
			rax = rdx;              //mov rax, rdx
			rax >>= 0x22;           //shr rax, 0x22
			rdx ^= rax;             //xor rdx, rax
			rax = 0xFC68A9112A83A459;               //mov rax, 0xFC68A9112A83A459
			rdx *= rax;             //imul rdx, rax
			rax = r10;              //mov rax, r10
			rax *= r15;             //imul rax, r15
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x12;           //shr rax, 0x12
			rdx ^= rax;             //xor rdx, rax
			rcx = rdx;              //mov rcx, rdx
			rcx >>= 0x24;           //shr rcx, 0x24
			rdx ^= rcx;             //xor rdx, rcx
			rax = r10;              //mov rax, r10
			rax *= r12;             //imul rax, r12
			rdx -= rax;             //sub rdx, rax
			rcx = r13;              //mov rcx, r13
			rcx = ~rcx;             //not rcx
			//failed to translate: inc rcx
			rcx += r10;             //add rcx, r10
			rax = 0;                //and rax, 0xFFFFFFFFC0000000
			rax = _rotl64(rax, 0x10);               //rol rax, 0x10
			rax ^= r9;              //xor rax, r9
			rax = ~rax;             //not rax
			rdx *= *(uint64_t*)(rax + 0xd);                 //imul rdx, [rax+0x0D]
			rdx ^= rcx;             //xor rdx, rcx
			rax = rdx;              //mov rax, rdx
			rax >>= 0xF;            //shr rax, 0x0F
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1E;           //shr rax, 0x1E
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x3C;           //shr rax, 0x3C
			rdx ^= rax;             //xor rdx, rax
			return rdx;
		}
		case 2:
		{
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC8D143]
			r12 = baseModuleAddr + 0x12A7D395;              //lea r12, [0x000000001070A4CC]
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDE307]
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r9;              //xor rcx, r9
			rax = r11 + 0xa045;             //lea rax, [r11+0xA045]
			rcx = ~rcx;             //not rcx
			rax += r10;             //add rax, r10
			rdx ^= rax;             //xor rdx, rax
			rdx *= *(uint64_t*)(rcx + 0xd);                 //imul rdx, [rcx+0x0D]
			rdx ^= r11;             //xor rdx, r11
			rax = r12;              //mov rax, r12
			rax = ~rax;             //not rax
			rax ^= r10;             //xor rax, r10
			rdx += rax;             //add rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x25;           //shr rax, 0x25
			rdx ^= rax;             //xor rdx, rax
			rax = 0xBE0966A117A4CCA0;               //mov rax, 0xBE0966A117A4CCA0
			rdx ^= rax;             //xor rdx, rax
			rax = 0xF3CDB9AE04A5DE9B;               //mov rax, 0xF3CDB9AE04A5DE9B
			rdx *= rax;             //imul rdx, rax
			rax = 0x3CFDFB48A269239A;               //mov rax, 0x3CFDFB48A269239A
			rdx += rax;             //add rdx, rax
			return rdx;
		}
		case 3:
		{
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC8CC1F]
			r15 = baseModuleAddr + 0x4D7964CE;              //lea r15, [0x000000004B4230E1]
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDDE23]
			rdx ^= r10;             //xor rdx, r10
			rax = 0xE43F2E4C7BB83B4B;               //mov rax, 0xE43F2E4C7BB83B4B
			rdx *= rax;             //imul rdx, rax
			rax = r10;              //mov rax, r10
			rax ^= r15;             //xor rax, r15
			rdx -= rax;             //sub rdx, rax
			rax = r11 + 0x6c89d3e6;                 //lea rax, [r11+0x6C89D3E6]
			rax += r10;             //add rax, r10
			rdx += rax;             //add rdx, rax
			rdx -= r10;             //sub rdx, r10
			rax = rdx;              //mov rax, rdx
			rax >>= 0x9;            //shr rax, 0x09
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x12;           //shr rax, 0x12
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x24;           //shr rax, 0x24
			rdx ^= rax;             //xor rdx, rax
			rax = 0xAB9EA13EFAD12F7;                //mov rax, 0xAB9EA13EFAD12F7
			rdx -= rax;             //sub rdx, rax
			rax = 0;                //and rax, 0xFFFFFFFFC0000000
			rax = _rotl64(rax, 0x10);               //rol rax, 0x10
			rax ^= r9;              //xor rax, r9
			rax = ~rax;             //not rax
			rdx *= *(uint64_t*)(rax + 0xd);                 //imul rdx, [rax+0x0D]
			return rdx;
		}
		case 4:
		{
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDD91E]
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC8C6D8]
			rax = rdx;              //mov rax, rdx
			rax >>= 0x20;           //shr rax, 0x20
			rdx ^= rax;             //xor rdx, rax
			rdx -= r11;             //sub rdx, r11
			rax = 0x676EAABFE95D694B;               //mov rax, 0x676EAABFE95D694B
			rdx *= rax;             //imul rdx, rax
			rax = 0x70068A9FE4DF2257;               //mov rax, 0x70068A9FE4DF2257
			rdx -= rax;             //sub rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1;            //shr rax, 0x01
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x2;            //shr rax, 0x02
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x4;            //shr rax, 0x04
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x8;            //shr rax, 0x08
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rax >>= 0x10;           //shr rax, 0x10
			rdx ^= rax;             //xor rdx, rax
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rcx ^= r9;              //xor rcx, r9
			rcx = ~rcx;             //not rcx
			rax = rdx;              //mov rax, rdx
			rax >>= 0x20;           //shr rax, 0x20
			rdx ^= rax;             //xor rdx, rax
			rdx *= *(uint64_t*)(rcx + 0xd);                 //imul rdx, [rcx+0x0D]
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1C;           //shr rax, 0x1C
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x38;           //shr rax, 0x38
			rdx ^= rax;             //xor rdx, rax
			rax = 0xE9BBCE7C879EE545;               //mov rax, 0xE9BBCE7C879EE545
			rdx *= rax;             //imul rdx, rax
			return rdx;
		}
		case 5:
		{
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC8C088]
			r8 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r8, [0x0000000004DDD259]
			rax = 0;                //and rax, 0xFFFFFFFFC0000000
			rax = _rotl64(rax, 0x10);               //rol rax, 0x10
			rax ^= r8;              //xor rax, r8
			rax = ~rax;             //not rax
			rdx *= *(uint64_t*)(rax + 0xd);                 //imul rdx, [rax+0x0D]
			rax = 0xD607BDC612D39625;               //mov rax, 0xD607BDC612D39625
			rdx += rax;             //add rdx, rax
			rdx += r11;             //add rdx, r11
			rax = 0x59993DD6A38BDC5C;               //mov rax, 0x59993DD6A38BDC5C
			rdx -= rax;             //sub rdx, rax
			rax = 0x30F0D6E93C3440EB;               //mov rax, 0x30F0D6E93C3440EB
			rdx *= rax;             //imul rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1F;           //shr rax, 0x1F
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x3E;           //shr rax, 0x3E
			rdx ^= rax;             //xor rdx, rax
			rdx += 0xFFFFFFFFB8E61505;              //add rdx, 0xFFFFFFFFB8E61505
			return rdx;
		}
		case 6:
		{
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDCE11]
			r15 = baseModuleAddr + 0x74AE;          //lea r15, [0xFFFFFFFFFDC93074]
			rcx = r10;              //mov rcx, r10
			rcx = ~rcx;             //not rcx
			rax = baseModuleAddr + 0x2CD0;          //lea rax, [0xFFFFFFFFFDC8E6E0]
			rcx *= rax;             //imul rcx, rax
			rax = r10;              //mov rax, r10
			rax *= r15;             //imul rax, r15
			rdx += rax;             //add rdx, rax
			rdx += rcx;             //add rdx, rcx
			rax = rdx;              //mov rax, rdx
			rax >>= 0xF;            //shr rax, 0x0F
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1E;           //shr rax, 0x1E
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x3C;           //shr rax, 0x3C
			rdx ^= rax;             //xor rdx, rax
			rax = 0;                //and rax, 0xFFFFFFFFC0000000
			rax = _rotl64(rax, 0x10);               //rol rax, 0x10
			rax ^= r9;              //xor rax, r9
			rax = ~rax;             //not rax
			rax = *(uint64_t*)(rax + 0xd);          //mov rax, [rax+0x0D]
			uintptr_t RSP_0x48;
			RSP_0x48 = 0x89242E575753D875;          //mov rax, 0x89242E575753D875 : RSP+0x48
			rax *= RSP_0x48;                //imul rax, [rsp+0x48]
			rdx *= rax;             //imul rdx, rax
			rax = 0x2BF716BB2C1B20C9;               //mov rax, 0x2BF716BB2C1B20C9
			rdx += rax;             //add rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1E;           //shr rax, 0x1E
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x3C;           //shr rax, 0x3C
			rdx ^= rax;             //xor rdx, rax
			rax = 0xCB141FA9883685A1;               //mov rax, 0xCB141FA9883685A1
			rdx ^= rax;             //xor rdx, rax
			return rdx;
		}
		case 7:
		{
			//failed to translate: pop rdx
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC8B6F3]
			r8 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r8, [0x0000000004DDC8BC]
			rdx -= r11;             //sub rdx, r11
			rax = 0xA04B31EA35C0CF51;               //mov rax, 0xA04B31EA35C0CF51
			rdx *= rax;             //imul rdx, rax
			rdx -= r10;             //sub rdx, r10
			rax = rdx;              //mov rax, rdx
			rax >>= 0x19;           //shr rax, 0x19
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x32;           //shr rax, 0x32
			rdx ^= rax;             //xor rdx, rax
			rax = 0;                //and rax, 0xFFFFFFFFC0000000
			rax = _rotl64(rax, 0x10);               //rol rax, 0x10
			rax ^= r8;              //xor rax, r8
			rax = ~rax;             //not rax
			rdx *= *(uint64_t*)(rax + 0xd);                 //imul rdx, [rax+0x0D]
			rax = 0x55179C9DC00EDC49;               //mov rax, 0x55179C9DC00EDC49
			rdx += rax;             //add rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x6;            //shr rax, 0x06
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0xC;            //shr rax, 0x0C
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x18;           //shr rax, 0x18
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x30;           //shr rax, 0x30
			rdx ^= rax;             //xor rdx, rax
			rdx += r10;             //add rdx, r10
			return rdx;
		}
		case 8:
		{
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDC3C8]
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC8B182]
			r15 = baseModuleAddr + 0xB99;           //lea r15, [0xFFFFFFFFFDC8BD0F]
			rax = 0x832FED9D5B1D7075;               //mov rax, 0x832FED9D5B1D7075
			rax -= r11;             //sub rax, r11
			rdx += rax;             //add rdx, rax
			rcx = r11 + 0x77c85755;                 //lea rcx, [r11+0x77C85755]
			rcx += r10;             //add rcx, r10
			rcx ^= rdx;             //xor rcx, rdx
			rdx = r15;              //mov rdx, r15
			rdx = ~rdx;             //not rdx
			rdx += rcx;             //add rdx, rcx
			rax = rdx;              //mov rax, rdx
			rax >>= 0x11;           //shr rax, 0x11
			rdx ^= rax;             //xor rdx, rax
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rax = rdx;              //mov rax, rdx
			rcx ^= r9;              //xor rcx, r9
			rax >>= 0x22;           //shr rax, 0x22
			rcx = ~rcx;             //not rcx
			rdx ^= rax;             //xor rdx, rax
			rdx *= *(uint64_t*)(rcx + 0xd);                 //imul rdx, [rcx+0x0D]
			rax = 0x94960D6A6B8B9A0F;               //mov rax, 0x94960D6A6B8B9A0F
			rdx *= rax;             //imul rdx, rax
			return rdx;
		}
		case 9:
		{
			//failed to translate: pop rdx
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC8ABFF]
			r15 = baseModuleAddr + 0x68F8CDC1;              //lea r15, [0x0000000066C179B4]
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDBDE7]
			rax = r10;              //mov rax, r10
			rax *= r15;             //imul rax, r15
			rdx -= rax;             //sub rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x26;           //shr rax, 0x26
			rdx ^= rax;             //xor rdx, rax
			rax = 0;                //and rax, 0xFFFFFFFFC0000000
			rax = _rotl64(rax, 0x10);               //rol rax, 0x10
			rax ^= r9;              //xor rax, r9
			rax = ~rax;             //not rax
			rdx *= *(uint64_t*)(rax + 0xd);                 //imul rdx, [rax+0x0D]
			rdx -= r11;             //sub rdx, r11
			rax = 0x5B8DFD67533270DD;               //mov rax, 0x5B8DFD67533270DD
			rdx *= rax;             //imul rdx, rax
			rax = 0xC42C2755C7AF16C;                //mov rax, 0xC42C2755C7AF16C
			rdx += rax;             //add rdx, rax
			rdx -= r11;             //sub rdx, r11
			return rdx;
		}
		case 10:
		{
			//failed to translate: pop rdx
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC8A780]
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDB938]
			rax = 0;                //and rax, 0xFFFFFFFFC0000000
			rax = _rotl64(rax, 0x10);               //rol rax, 0x10
			rax ^= r9;              //xor rax, r9
			rax = ~rax;             //not rax
			rdx *= *(uint64_t*)(rax + 0xd);                 //imul rdx, [rax+0x0D]
			rax = r10;              //mov rax, r10
			rax -= r11;             //sub rax, r11
			rax -= 0xCD08;          //sub rax, 0xCD08
			rdx ^= rax;             //xor rdx, rax
			rdx -= r11;             //sub rdx, r11
			rax = r11 + 0x192d16d1;                 //lea rax, [r11+0x192D16D1]
			rax += r10;             //add rax, r10
			rdx += rax;             //add rdx, rax
			rax = 0x625A9722FE921DBC;               //mov rax, 0x625A9722FE921DBC
			rdx += rax;             //add rdx, rax
			rax = 0x2161E46FBF3812E;                //mov rax, 0x2161E46FBF3812E
			rdx ^= rax;             //xor rdx, rax
			rax = 0x2BF8C3AD0316BAAD;               //mov rax, 0x2BF8C3AD0316BAAD
			rdx *= rax;             //imul rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1F;           //shr rax, 0x1F
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x3E;           //shr rax, 0x3E
			rdx ^= rax;             //xor rdx, rax
			return rdx;
		}
		case 11:
		{
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDB49F]
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC8A259]
			r15 = baseModuleAddr + 0x163B;          //lea r15, [0xFFFFFFFFFDC8B888]
			r12 = baseModuleAddr + 0xEC1;           //lea r12, [0xFFFFFFFFFDC8B102]
			rax = r10;              //mov rax, r10
			rax *= r15;             //imul rax, r15
			rdx -= rax;             //sub rdx, rax
			rdx ^= r10;             //xor rdx, r10
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rax = 0x8388EB1FCB0491EB;               //mov rax, 0x8388EB1FCB0491EB
			rcx ^= r9;              //xor rcx, r9
			rcx = ~rcx;             //not rcx
			rdx *= *(uint64_t*)(rcx + 0xd);                 //imul rdx, [rcx+0x0D]
			rdx *= rax;             //imul rdx, rax
			rdx ^= r11;             //xor rdx, r11
			rax = rdx;              //mov rax, rdx
			rax >>= 0xD;            //shr rax, 0x0D
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1A;           //shr rax, 0x1A
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x34;           //shr rax, 0x34
			rdx ^= rax;             //xor rdx, rax
			rax = 0x18F32DD6893941E0;               //mov rax, 0x18F32DD6893941E0
			rdx -= rax;             //sub rdx, rax
			rax = r10;              //mov rax, r10
			rax = ~rax;             //not rax
			rax *= r12;             //imul rax, r12
			rdx += rax;             //add rdx, rax
			return rdx;
		}
		case 12:
		{
			r12 = baseModuleAddr + 0x2A8B306D;              //lea r12, [0x000000002853CE17]
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDAF99]
			rcx = baseModuleAddr + 0x8A7A;          //lea rcx, [0xFFFFFFFFFDC92669]
			rcx = ~rcx;             //not rcx
			rcx -= r10;             //sub rcx, r10
			rax = rdx;              //mov rax, rdx
			rax >>= 0x24;           //shr rax, 0x24
			rcx ^= rax;             //xor rcx, rax
			rdx ^= rcx;             //xor rdx, rcx
			rdx -= r10;             //sub rdx, r10
			rax = r12;              //mov rax, r12
			rax = ~rax;             //not rax
			rax ^= r10;             //xor rax, r10
			rdx += rax;             //add rdx, rax
			rax = 0x1EA3A776413528D8;               //mov rax, 0x1EA3A776413528D8
			rdx -= rax;             //sub rdx, rax
			rax = 0;                //and rax, 0xFFFFFFFFC0000000
			rax = _rotl64(rax, 0x10);               //rol rax, 0x10
			rax ^= r9;              //xor rax, r9
			rax = ~rax;             //not rax
			rax = *(uint64_t*)(rax + 0xd);          //mov rax, [rax+0x0D]
			uintptr_t RSP_0xFFFFFFFFFFFFFF88;
			RSP_0xFFFFFFFFFFFFFF88 = 0x9D07C699A1C792E5;            //mov rax, 0x9D07C699A1C792E5 : RBP+0xFFFFFFFFFFFFFF88
			rax *= RSP_0xFFFFFFFFFFFFFF88;          //imul rax, [rbp-0x78]
			rdx *= rax;             //imul rdx, rax
			rax = 0x73DF8F3C344FCC30;               //mov rax, 0x73DF8F3C344FCC30
			rdx += rax;             //add rdx, rax
			return rdx;
		}
		case 13:
		{
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC89934]
			r8 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r8, [0x0000000004DDAB26]
			rax = rdx;              //mov rax, rdx
			rax >>= 0x26;           //shr rax, 0x26
			rdx ^= rax;             //xor rdx, rax
			rdx -= r11;             //sub rdx, r11
			rax = 0;                //and rax, 0xFFFFFFFFC0000000
			rax = _rotl64(rax, 0x10);               //rol rax, 0x10
			rax ^= r8;              //xor rax, r8
			rax = ~rax;             //not rax
			rdx *= *(uint64_t*)(rax + 0xd);                 //imul rdx, [rax+0x0D]
			rax = rdx;              //mov rax, rdx
			rax >>= 0x28;           //shr rax, 0x28
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0xE;            //shr rax, 0x0E
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1C;           //shr rax, 0x1C
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x38;           //shr rax, 0x38
			rax ^= r10;             //xor rax, r10
			rdx ^= rax;             //xor rdx, rax
			rax = 0x5FE9A694DE9C42A5;               //mov rax, 0x5FE9A694DE9C42A5
			rdx *= rax;             //imul rdx, rax
			rax = 0x73ED4F0A7D63206F;               //mov rax, 0x73ED4F0A7D63206F
			rdx ^= rax;             //xor rdx, rax
			return rdx;
		}
		case 14:
		{
			r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDC894B3]
			r12 = baseModuleAddr + 0xC28C;          //lea r12, [0xFFFFFFFFFDC95733]
			r9 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r9, [0x0000000004DDA6A4]
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1C;           //shr rax, 0x1C
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x38;           //shr rax, 0x38
			rdx ^= rax;             //xor rdx, rax
			rax = r10;              //mov rax, r10
			rax = ~rax;             //not rax
			rax *= r12;             //imul rax, r12
			rdx ^= rax;             //xor rdx, rax
			rax = 0x676B51DF73D302E0;               //mov rax, 0x676B51DF73D302E0
			rdx ^= rax;             //xor rdx, rax
			rax = 0x20594CEA4FE3424E;               //mov rax, 0x20594CEA4FE3424E
			rdx ^= rax;             //xor rdx, rax
			rax = 0x5B09186274EBFA65;               //mov rax, 0x5B09186274EBFA65
			rdx *= rax;             //imul rdx, rax
			rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
			rax = r10;              //mov rax, r10
			rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
			rax -= r11;             //sub rax, r11
			rcx ^= r9;              //xor rcx, r9
			rax += 0xFFFFFFFFEC26E147;              //add rax, 0xFFFFFFFFEC26E147
			rdx += rax;             //add rdx, rax
			rcx = ~rcx;             //not rcx
			rdx *= *(uint64_t*)(rcx + 0xd);                 //imul rdx, [rcx+0x0D]
			rdx ^= r11;             //xor rdx, r11
			return rdx;
		}
		case 15:
		{
			r8 = *(uint64_t*)(baseModuleAddr + 0x7151233);          //mov r8, [0x0000000004DDA242]
			r14 = baseModuleAddr + 0x2573EEF8;              //lea r14, [0x00000000233C7EF4]
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1A;           //shr rax, 0x1A
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x34;           //shr rax, 0x34
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x1C;           //shr rax, 0x1C
			rdx ^= rax;             //xor rdx, rax
			rax = rdx;              //mov rax, rdx
			rax >>= 0x38;           //shr rax, 0x38
			rdx ^= rax;             //xor rdx, rax
			rax = r14;              //mov rax, r14
			rax = ~rax;             //not rax
			rax ^= r10;             //xor rax, r10
			rdx ^= rax;             //xor rdx, rax
			rax = 0x7A820CD3664BEAC7;               //mov rax, 0x7A820CD3664BEAC7
			rdx -= rax;             //sub rdx, rax
			r11 = 0xB379995FC7472809;               //mov r11, 0xB379995FC7472809
			rdx += r11;             //add rdx, r11
			rax = 0;                //and rax, 0xFFFFFFFFC0000000
			rax = _rotl64(rax, 0x10);               //rol rax, 0x10
			rax ^= r8;              //xor rax, r8
			rax = ~rax;             //not rax
			rdx *= *(uint64_t*)(rax + 0xd);                 //imul rdx, [rax+0x0D]
			rdx -= r10;             //sub rdx, r10
			rax = 0x8BB69F998A62572B;               //mov rax, 0x8BB69F998A62572B
			rdx *= rax;             //imul rdx, rax
			return rdx;
		}
		}

	}
	uint16_t get_bone_index(uint32_t bone_index)
	{
		auto baseModuleAddr = g_data::base;
		auto Peb = __readgsqword(0x60);
		uint64_t rax = baseModuleAddr, rbx = baseModuleAddr, rcx = baseModuleAddr, rdx = baseModuleAddr, rdi = baseModuleAddr, rsi = baseModuleAddr, r8 = baseModuleAddr, r9 = baseModuleAddr, r10 = baseModuleAddr, r11 = baseModuleAddr, r12 = baseModuleAddr, r13 = baseModuleAddr, r14 = baseModuleAddr, r15 = baseModuleAddr;
		rbx = bone_index;
		rcx = rbx * 0x13C8;
		rax = 0x68F3B9FF6FB0E041;               //mov rax, 0x68F3B9FF6FB0E041
		rbx = baseModuleAddr;           //lea rbx, [0xFFFFFFFFFDE1625D]
		rax = _umul128(rax, rcx, (uintptr_t*)&rdx);             //mul rcx
		r10 = 0x8AD8F2FBA9386823;               //mov r10, 0x8AD8F2FBA9386823
		r9 = 0xCD4C7EABC2B2FAF;                 //mov r9, 0xCD4C7EABC2B2FAF
		rdx >>= 0xC;            //shr rdx, 0x0C
		rax = rdx * 0x2707;             //imul rax, rdx, 0x2707
		rcx -= rax;             //sub rcx, rax
		rax = 0x995B4C828493BD95;               //mov rax, 0x995B4C828493BD95
		r8 = rcx * 0x2707;              //imul r8, rcx, 0x2707
		rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
		rdx >>= 0xD;            //shr rdx, 0x0D
		rax = rdx * 0x356B;             //imul rax, rdx, 0x356B
		r8 -= rax;              //sub r8, rax
		rax = 0xFC0FC0FC0FC0FC1;                //mov rax, 0xFC0FC0FC0FC0FC1
		rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
		rax = r10;              //mov rax, r10
		rdx >>= 0x2;            //shr rdx, 0x02
		rcx = rdx * 0x41;               //imul rcx, rdx, 0x41
		rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
		rdx >>= 0x7;            //shr rdx, 0x07
		rcx += rdx;             //add rcx, rdx
		rax = rcx * 0x1D8;              //imul rax, rcx, 0x1D8
		rcx = r8 * 0x1DA;               //imul rcx, r8, 0x1DA
		rcx -= rax;             //sub rcx, rax
		rax = *(uint16_t*)(rcx + rbx * 1 + 0x716A0C0);          //movzx eax, word ptr [rcx+rbx*1+0x716A0C0]
		rcx = rax * 0x13C8;             //imul rcx, rax, 0x13C8
		rax = r9;               //mov rax, r9
		rax = _umul128(rax, rcx, (uintptr_t*)&rdx);             //mul rcx
		rax = rcx;              //mov rax, rcx
		rax -= rdx;             //sub rax, rdx
		rax >>= 0x1;            //shr rax, 0x01
		rax += rdx;             //add rax, rdx
		rax >>= 0xC;            //shr rax, 0x0C
		rax = rax * 0x1E79;             //imul rax, rax, 0x1E79
		rcx -= rax;             //sub rcx, rax
		rax = r9;               //mov rax, r9
		r8 = rcx * 0x2F53;              //imul r8, rcx, 0x2F53
		rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
		rax = r8;               //mov rax, r8
		rax -= rdx;             //sub rax, rdx
		rax >>= 0x1;            //shr rax, 0x01
		rax += rdx;             //add rax, rdx
		rax >>= 0xC;            //shr rax, 0x0C
		rax = rax * 0x1E79;             //imul rax, rax, 0x1E79
		r8 -= rax;              //sub r8, rax
		rax = 0x446F86562D9FAEE5;               //mov rax, 0x446F86562D9FAEE5
		rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
		rcx = r8;               //mov rcx, r8
		rax = r10;              //mov rax, r10
		rcx -= rdx;             //sub rcx, rdx
		rcx >>= 0x1;            //shr rcx, 0x01
		rcx += rdx;             //add rcx, rdx
		rcx >>= 0x7;            //shr rcx, 0x07
		rcx = rcx * 0xCA;               //imul rcx, rcx, 0xCA
		rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
		rdx >>= 0x5;            //shr rdx, 0x05
		rcx += rdx;             //add rcx, rdx
		rax = rcx * 0x76;               //imul rax, rcx, 0x76
		rcx = r8 * 0x78;                //imul rcx, r8, 0x78
		rcx -= rax;             //sub rcx, rax
		rsi = *(uint16_t*)(rcx + rbx * 1 + 0x71718A0);          //movsx esi, word ptr [rcx+rbx*1+0x71718A0]
		return rsi;
	}

	player_t get_player(int i)
	{
		uint64_t decryptedPtr = get_client_info();

		if (is_valid_ptr (decryptedPtr))
		{
			uint64_t client_info = get_client_info_base();

			if (is_valid_ptr(client_info))
			{
				return player_t(client_info + (i * player_info::size));
			}
		}
		return player_t(NULL);
	}

	//player_t player_t
	
	//player_t get_local_player()
	//{
	//	auto addr = sdk::get_client_info_base() + (get_local_index() * player_info::size);
	//	if (is_bad_ptr(addr)) return 0;
	//	return addr;


	//}

	player_t get_local_player()
	{
		uint64_t decryptedPtr = get_client_info();

		if (is_bad_ptr(decryptedPtr))return player_t(NULL);

			auto local_index = *(uintptr_t*)(decryptedPtr + player_info::local_index);
			if (is_bad_ptr(local_index))return player_t(NULL);
			auto index = *(int*)(local_index + player_info::local_index_pos);
			return get_player(index);
		
		
	}

	name_t* get_name_ptr(int i)
	{
		uint64_t bgs = *(uint64_t*)(g_data::base + client::name_array);

		if (bgs)
		{
			name_t* pClientInfo = (name_t*)(bgs + client::name_array_padding + ((i + i * 8) << 4));

			if (is_bad_ptr(pClientInfo))return 0;
			else
			return pClientInfo;
			
		}
		return 0;
	}

	refdef_t* get_refdef()
	{
		uint32_t crypt_0 = *(uint32_t*)(g_data::base + view_port::refdef_ptr);
		uint32_t crypt_1 = *(uint32_t*)(g_data::base + view_port::refdef_ptr + 0x4);
		uint32_t crypt_2 = *(uint32_t*)(g_data::base + view_port::refdef_ptr + 0x8);
		// lower 32 bits
		uint32_t entry_1 = (uint32_t)(g_data::base + view_port::refdef_ptr);
		uint32_t entry_2 = (uint32_t)(g_data::base + view_port::refdef_ptr + 0x4);
		// decryption
		uint32_t _low = entry_1 ^ crypt_2;
		uint32_t _high = entry_2 ^ crypt_2;
		uint32_t low_bit = crypt_0 ^ _low * (_low + 2);
		uint32_t high_bit = crypt_1 ^ _high * (_high + 2);
		auto ret = (refdef_t*)(((QWORD)high_bit << 32) + low_bit);
		if (is_bad_ptr(ret)) return 0;
		else
			return ret;
	}

	Vector3 get_camera_pos()
	{
		Vector3 pos = Vector3{};

		auto camera_ptr = *(uint64_t*)(g_data::base + view_port::camera_ptr);

		if (is_bad_ptr(camera_ptr))return pos;
		
		
		pos = *(Vector3*)(camera_ptr + view_port::camera_pos);
		if (pos.IsZero())return {};
		else
		return pos;
	}

	std::string get_player_name(int i)
	{
		uint64_t bgs = *(uint64_t*)(g_data::base + client::name_array);

		if (is_bad_ptr(bgs))return NULL;


		if (bgs)
		{
			name_t* clientInfo_ptr = (name_t*)(bgs + client::name_array_padding + (i * 0xD0));
			if (is_bad_ptr(clientInfo_ptr))return NULL;

			int length = strlen(clientInfo_ptr->name);
			for (int j = 0; j < length; ++j)
			{
				char ch = clientInfo_ptr->name[j];
				bool is_english = ch >= 0 && ch <= 127;
				if (!is_english)
					return xorstr("Player");
			}
			return clientInfo_ptr->name;
		}
		return xorstr("Player");
	}

	
    bool bones_to_screen(Vector3* BonePosArray, Vector2* ScreenPosArray, const long Count)
    {
        for (long i = 0; i < Count; ++i)
        {
            if (!world(BonePosArray[i], &ScreenPosArray[i]))
                return false;
        }
        return true;
    }



	bool get_bone_by_player_index(int i, int bone_id, Vector3* Out_bone_pos)
	{
		uint64_t decrypted_ptr = get_bone_ptr();

		if (is_bad_ptr(decrypted_ptr))return false;
		
			unsigned short index = get_bone_index(i);
			if (index != 0)
			{
				uint64_t bone_ptr = *(uint64_t*)(decrypted_ptr + (index * bones::size) + 0xC0);

				if (is_bad_ptr(bone_ptr))return false;
				
					Vector3 bone_pos = *(Vector3*)(bone_ptr + (bone_id * 0x20) + 0x10);

					if (bone_pos.IsZero())return false;

					uint64_t client_info = get_client_info();

					if (is_bad_ptr(client_info))return false;

					
					
						Vector3 BasePos = *(Vector3*)(client_info + bones::bone_base_pos);

						if (BasePos.IsZero())return false;

						bone_pos.x += BasePos.x;
						bone_pos.y += BasePos.y;
						bone_pos.z += BasePos.z;

						*Out_bone_pos = bone_pos;
						return true;
					
				
			}
		
		return false;
	}

	

	void no_recoil()
	{
		uint64_t characterInfo_ptr = get_client_info();

		if (characterInfo_ptr)
		{
			// up, down
			uint64_t r12 = characterInfo_ptr;
			r12 += player_info::recoil_offset;
			uint64_t rsi = r12 + 0x4;
			DWORD edx = *(uint64_t*)(r12 + 0xC);
			DWORD ecx = (DWORD)r12;
			ecx ^= edx;
			DWORD eax = (DWORD)((uint64_t)ecx + 0x2);
			eax *= ecx;
			ecx = (DWORD)rsi;
			ecx ^= edx;
			DWORD udZero = eax;
			//left, right
			eax = (DWORD)((uint64_t)ecx + 0x2);
			eax *= ecx;
			DWORD lrZero = eax;
			*(DWORD*)(r12) = udZero;
			*(DWORD*)(rsi) = lrZero;

		}
	}

	int get_player_health(int i)
	{
		uint64_t bgs = *(uint64_t*)(g_data::base + client::name_array);

		if (bgs)
		{
			name_t* pClientInfo = (name_t*)(bgs + client::name_array_padding  +(i * 0xD0));

			if (pClientInfo)
			{
				return pClientInfo->health;
			}
		}
		return 0;
	}

	void start_tick()
	{
		static DWORD lastTick = 0;
		DWORD t = GetTickCount();
		bUpdateTick = lastTick < t;

		if (bUpdateTick)
			lastTick = t + nTickTime;
	}

	void update_vel_map(int index, Vector3 vPos)
	{
		if (!bUpdateTick)
			return;

		velocityMap[index].delta = vPos - velocityMap[index].lastPos;
		velocityMap[index].lastPos = vPos;
	}

	void clear_map()
	{
		if (!velocityMap.empty()) { velocityMap.clear(); }
	}

	Vector3 get_speed(int index)
	{
		return velocityMap[index].delta;
	}

	Vector3 get_prediction(int index, Vector3 source, Vector3 destination)
	{
		auto local_velocity = get_speed(local_index());
		auto target_velocity = get_speed(index);

		const auto distance = source.distance_to(destination);
		const auto travel_time = distance / globals::bullet_speed;
		auto pred_destination = destination + (target_velocity - local_velocity) * travel_time;
		/*position.x += travel_time * final_speed.x;
		position.y += travel_time * final_speed.y;
		position.z += 0.5 * globals::bullet_gravity * travel_time * travel_time;
		return position;*/

		pred_destination.z += 0.5f * std::fabsf(globals::bullet_gravity) * travel_time;

		return pred_destination;
	}

	/*int get_client_count()
	{
		auto cl_info = get_client_info();
		if (cl_info)
		{
			auto client_ptr = *(uint64_t*)(cl_info + player_info::local_index);
			if (client_ptr)
			{
				return *(unsigned int*)(client_ptr + 0x1C);
			}
		}

		return 0;
	}*/

	Result MidnightSolver(float a, float b, float c)
	{
		Result res;

		double subsquare = b * b - 4 * a * c;

		if (subsquare < 0)
		{
			res.hasResult = false;
			return res;
		}
		else
		{
			res.hasResult = true,
			res.a = (float)((-b + sqrt(subsquare)) / (2 * a));
			res.b = (float)((-b - sqrt(subsquare)) / (2 * a));
		}
		return res;
	}

	Vector3 prediction_solver(Vector3 local_pos, Vector3 position, int index, float bullet_speed)
	{
		Vector3 aimPosition = Vector3().Zero();
		auto target_speed = get_speed(index);

		local_pos -= position; 

		float a = (target_speed.x * target_speed.x) + (target_speed.y * target_speed.y) + (target_speed.z * target_speed.z) - ((bullet_speed * bullet_speed) * 100);
		float b = (-2 * local_pos.x * target_speed.x) + (-2 * local_pos.y * target_speed.y) + (-2 * local_pos.z * target_speed.z);
		float c = (local_pos.x * local_pos.x) + (local_pos.y * local_pos.y) + (local_pos.z * local_pos.z);

		local_pos += position; 

		Result r = MidnightSolver(a, b, c);

		if (r.a >= 0 && !(r.b >= 0 && r.b < r.a))
		{
			aimPosition = position + target_speed * r.a;
		}
		else if (r.b >= 0)
		{
			aimPosition = position + target_speed * r.b;
		}

		return aimPosition;
	
	}

	uintptr_t get_visible_base()
	{
		for (int32_t j = 4000; j >= 0; --j)
		{
			uintptr_t n_index = (j + (j << 2)) << 0x6;
			uintptr_t vis_base = *(uintptr_t*)(g_data::base + bones::distribute);

			if (!vis_base)
				continue;

			uintptr_t vis_base_ptr = vis_base + n_index;
			uintptr_t cmp_function = *(uintptr_t*)(vis_base_ptr + 0x90);

			if (!cmp_function)
				continue;

			uintptr_t about_visible = g_data::base + bones::visible;
			if (cmp_function == about_visible)
			{
				return vis_base_ptr;
			}
		}
		return NULL;
	}

	bool update_visible_addr(int i)
	{
		auto vis_base_ptr = get_visible_base();

		if (vis_base_ptr)
		{
			uint64_t visible_head_offset = *(uint64_t*)(vis_base_ptr + 0x108);

			if (visible_head_offset)
			{
				auto visible_flag = decrypt_visible_flag(i, visible_head_offset);

				if (visible_flag == 3)
				{
					current_visible_offset = vis_base_ptr;
					return true;
				}
			}
		}
		return false;
	}

	

	// player class methods
	bool player_t::is_valid() {
		if (is_bad_ptr(address))return 0;

		return *(bool*)((uintptr_t)address + player_info::valid);
	}

	bool player_t::is_dead() {
		if (is_bad_ptr(address))return 0;

		auto dead1 = *(bool*)((uintptr_t)address + player_info::dead_1);
		auto dead2 = *(bool*)((uintptr_t)address + player_info::dead_2);
		return dead1 || dead2;
	}

	int player_t::team_id() {

		if (is_bad_ptr(address))return 0;
		return *(int*)((uintptr_t)address + player_info::team_id);
	}

	int player_t::get_stance() {
		
		if (is_bad_ptr(address))return 4;
		auto ret = *(int*)((uintptr_t)address + player_info::stance);
	

		return ret;
	}


	float player_t::get_rotation()
	{
		if (is_bad_ptr(address))return 0;
		auto local_pos_ptr = *(uintptr_t*)((uintptr_t)address + player_info::position_ptr);

		if (is_bad_ptr(local_pos_ptr))return 0;

		auto rotation = *(float*)(local_pos_ptr + 0x58);

		if (rotation < 0)
			rotation = 360.0f - (rotation * -1);

		rotation += 90.0f;

		if (rotation >= 360.0f)
			rotation = rotation - 360.0f;

		return rotation;
	}

	Vector3 player_t::get_pos() 
	{
		if (is_bad_ptr(address))return {};
		auto local_pos_ptr = *(uintptr_t*)((uintptr_t)address + player_info::position_ptr);

		if (is_bad_ptr(local_pos_ptr))return{};
		else
			return *(Vector3*)(local_pos_ptr + 0x40);
		return Vector3{}; 


	}

	uint32_t player_t::get_index()
	{
		if (is_bad_ptr(this->address))return 0;

		auto cl_info_base = get_client_info_base();
		if (is_bad_ptr(cl_info_base))return 0;
		
		
	return ((uintptr_t)this->address - cl_info_base) / player_info::size;
		
	
	}

	bool player_t::is_visible()
	{
		if (is_bad_ptr(g_data::visible_base))return false;

		if (is_bad_ptr(this->address))return false;
		
			uint64_t VisibleList =*(uint64_t*)(g_data::visible_base + 0x108);
			if (is_bad_ptr(VisibleList))
				return false;

			uint64_t rdx = VisibleList + (player_t::get_index() * 9 + 0x14E) * 8;
			if (is_bad_ptr(rdx))
				return false;

			DWORD VisibleFlags = (rdx + 0x10) ^ *(DWORD*)(rdx + 0x14);
			if (!VisibleFlags)
				return false;

			DWORD v511 = VisibleFlags * (VisibleFlags + 2);
			if (!v511)
				return false;

			BYTE VisibleFlags1 = *(DWORD*)(rdx + 0x10) ^ v511 ^ BYTE1(v511);
			if (VisibleFlags1 == 3) {
				return true;
			}
		
		return false;
	}
	



}

