#pragma once
#include "DLLEvent.h"

// ��ʬʧȥ��������¼���
/// @param �����¼��Ľ�ʬ
/// @return �Ƿ��������ԭ��Ĵ�����̡���Ϊ���񡱣���������Щ���̡�
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
