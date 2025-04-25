#pragma once
#include "DLLEvent.h"

// 模仿者即将生成新植物的事件。
/// @param 触发事件的模仿者（注意：不是即将生成的新植物）。
/// @return 是否继续结算该事件。
class ImitaterPlantEvent : public DLLEvent
{
public:
	ImitaterPlantEvent();
};

ImitaterPlantEvent::ImitaterPlantEvent()
{
	int procAddress = PVZ::Memory::GetProcAddress("onImitaterPlant");
	hookAddress = 0x466B89;
	rawlen = 6;
	BYTE code[] = { PUSH_ESI, INVOKE(procAddress), ADD_ESP(4) };
	start(STRING(code));
}

