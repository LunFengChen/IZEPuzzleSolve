#pragma once
#include "DLLEvent.h"

// 提取资源事件。
// 时机发生在 resources.xml 解析成功后。
/// @param 触发事件的 ResourceManager，读取的资源组名称（char* 形式）
/// @return 若为正数，则读取成功；若为 0，则读取失败；若为负数，结算原版过程。
class ExtractResourceEvent : public DLLEvent
{
public:
	ExtractResourceEvent();
};

ExtractResourceEvent::ExtractResourceEvent()
{
	int procAddress = PVZ::Memory::GetProcAddress("onExtractResource");
	hookAddress = 0x474700;
	rawlen = 5;
	BYTE code[] =
	{
		PUSH_PTR_ESP_ADD_V(0x28),
		PUSH_PTR_ESP_ADD_V(0x28),
		INVOKE(procAddress),
		ADD_ESP(8),

		TEST_EUX_EVX(REG_EAX, REG_EAX),
		JS(17),
		JE(9),

		POPAD,
		MOV_EAX(1),
		RETN(8),

		POPAD,
		XOR_EUX_EVX(REG_EAX, REG_EAX),
		RETN(8),
	};
	start(STRING(code));
}