#pragma once
#include "DLLEvent.h"

// 僵尸失去盾类防具事件。
/// @param 触发事件的僵尸
/// @return 是否继续结算原版的处理过程。若为“否”，则跳过这些过程。
class ZombieDetachShieldEvent : public DLLEvent
{
public:
	ZombieDetachShieldEvent();
};

ZombieDetachShieldEvent::ZombieDetachShieldEvent()
{
	int procAddress = PVZ::Memory::GetProcAddress("onZombieDetachShield");
	hookAddress = 0x5330E0;
	rawlen = 5;
	BYTE code[] =
	{
		PUSH_EAX,
		INVOKE(procAddress),
		ADD_ESP(4),

		TEST_AL_AL,
		JNZ(2),
		POPAD,
		RET
	};
	start(STRING(code));
}
