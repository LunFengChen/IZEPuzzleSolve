#pragma once
#include "DLLEvent.h"

// 卡槽卡牌更新事件。
// 时机上先于原版的更新。
/// @param 触发事件的卡槽卡牌。
/// @return 是否继续结算原版的过程。
class SeedPacketUpdateEvent : public DLLEvent
{
public:
	SeedPacketUpdateEvent();
};

SeedPacketUpdateEvent::SeedPacketUpdateEvent()
{
	int procAddress = PVZ::Memory::GetProcAddress("onSeedPacketUpdate");
	hookAddress = 0x487272;
	rawlen = 7;
	BYTE code[] =
	{
		PUSH_EDI,
		INVOKE(procAddress),
		ADD_ESP(4),

		TEST_AL_AL,
		JNZ(8),
		POPAD,
		MOV_ECX(0x487375),
		JMP_REG32(REG_ECX)
	};
	start(STRING(code));
}