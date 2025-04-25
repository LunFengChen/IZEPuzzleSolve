#pragma once
#include "DLLEvent.h"

// 僵尸掉头的粒子效果处理事件。
// 无返回值
/// @param 触发事件的僵尸、触发事件的粒子效果。
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