#pragma once
#include<Windows.h>
#include"imgui/imgui.h"
#include"globals.h"
class UavOffset
{

public:
	uintptr_t UAV_Offset1 = 0x16F498F8;                  //  old one0x14740B40;
	uintptr_t UAV_Offset2 = UAV_Offset1 + 0x20;
	uintptr_t UAV_Offset3 = UAV_Offset1 + 0x18;
	uintptr_t UAV_Offset4 = UAV_Offset1 + 0x1C;
	uintptr_t UAV_Offset5 = UAV_Offset1 + 0x38;
	uintptr_t UAV_Offset6 = UAV_Offset1 + 0x3C;
};




void UAV()
{

	UavOffset offset;
	BYTE ON1[]{ 0x01, 0x00, 0x00, 0x00 };
	BYTE ON2[]{ 0x01, 0x00, 0x00, 0x00 };
	BYTE ON3[]{ 0x4f, 0xbb, 0x55, 0x51 };
	BYTE ON4[]{ 0xf6, 0x6d, 0xae, 0x79 };
	BYTE ON5[]{ 0x4f, 0xbb, 0x55, 0x51 };
	BYTE ON6[]{ 0xf6, 0x6d, 0xae, 0x79 };


	BYTE OFF1[]{ 0x00, 0x00, 0x00, 0x00 };
	BYTE OFF2[]{ 0x00, 0x00, 0x00, 0x00 };
	BYTE OFF3[]{ 0x00, 0x00, 0x00, 0x00 };
	BYTE OFF4[]{ 0x00, 0x00, 0x00, 0x00 };
	BYTE OFF5[]{ 0x00, 0x00, 0x00, 0x00 };
	BYTE OFF6[]{ 0x00, 0x00, 0x00, 0x00 };

	auto base = GetModuleHandleA(NULL);
	

		if (globals::b_UAV)
		{
			*(BYTE**)(base + offset.UAV_Offset1) = ON1;
			*(BYTE**)(base + offset.UAV_Offset2) = ON2;
			*(BYTE**)(base + offset.UAV_Offset3) = ON3;
			*(BYTE**)(base + offset.UAV_Offset4) = ON4;
			*(BYTE**)(base + offset.UAV_Offset5) = ON5;
			*(BYTE**)(base + offset.UAV_Offset6) = ON6;
		}
		else
		{
			*(BYTE**)(base + offset.UAV_Offset1) = OFF1;
			*(BYTE**)(base + offset.UAV_Offset2) = OFF2;
			*(BYTE**)(base + offset.UAV_Offset3) = OFF3;
			*(BYTE**)(base + offset.UAV_Offset4) = OFF4;
			*(BYTE**)(base + offset.UAV_Offset5) = OFF5;
			*(BYTE**)(base + offset.UAV_Offset6) = OFF6;
		}




	
}