#pragma once
#include "DLLEvent.h"

// 植物初始化之后的事件。
// 无返回值
/// @param 依次为：触发事件的植物。
class PlantInitAfterEvent : public DLLEvent
{
public:
	PlantInitAfterEvent();
};

PlantInitAfterEvent::PlantInitAfterEvent()
{
	int procAddress = PVZ::Memory::GetProcAddress("onPlantInitAfter");
	hookAddress = 0x45E7AF;
	rawlen = 7;
	BYTE code[] =
	{
		PUSH_PTR_ESP_ADD_V(0x28),
		INVOKE(procAddress),
		ADD_ESP(4),
	};
	start(STRING(code));
}