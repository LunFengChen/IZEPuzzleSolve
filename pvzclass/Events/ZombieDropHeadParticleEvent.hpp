#pragma once
#include "DLLEvent.h"

// ��ʬ��ͷ������Ч�������¼���
// �޷���ֵ
/// @param �����¼��Ľ�ʬ�������¼�������Ч����
class ZombieDropHeadParticleEvent : public DLLEvent
{
public:
	ZombieDropHeadParticleEvent();
};

ZombieDropHeadParticleEvent::ZombieDropHeadParticleEvent()
{
	int procAddress = PVZ::Memory::GetProcAddress("onZombieDropHeadParticle");
	hookAddress = 0x529D0E;
	rawlen = 6;
	BYTE code[] =
	{
		PUSH_EDI,
		PUSH_EBX,
		INVOKE(procAddress),
		ADD_ESP(8),
	};
	start(STRING(code));
}