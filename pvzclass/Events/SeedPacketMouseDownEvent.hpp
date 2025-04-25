﻿#pragma once
#include "DLLEvent.h"

/// @brief 卡槽卡牌被点选事件。
/// @note 时机上先于原版的指令。
/// @bug 该事件实际上根本就没有正常生效，似乎只是 PlantAddProjectileEvent 的复制品
/// @param 触发事件的卡槽卡牌。
/// @return 是否继续结算原版的指令。
class SeedPacketMouseDownEvent : public DLLEvent
{
public:
	SeedPacketMouseDownEvent();
};

SeedPacketMouseDownEvent::SeedPacketMouseDownEvent()
{
	int procAddress = PVZ::Memory::GetProcAddress("onPlantAddProjectile");
	hookAddress = 0x4672B5;
	rawlen = 6;
	BYTE code[] =
	{
		PUSH_EBP,
		INVOKE(procAddress),
		ADD_ESP(4),

		TEST_AL_AL,
		JNZ(8),
		POPAD,
		MOV_ECX(0x488EA6),
		JMP_REG32(REG_ECX)
	};
	start(STRING(code));
}