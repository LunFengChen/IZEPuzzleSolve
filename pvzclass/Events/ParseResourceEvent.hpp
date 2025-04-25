#pragma once
#include "DLLEvent.h"

// 解析资源配置文件事件。
// 时机发生在 resources.xml 解析成功后。
// 无返回值
class ParseResourceEvent : public DLLEvent
{
public:
	ParseResourceEvent();
};

ParseResourceEvent::ParseResourceEvent()
{
	int procAddress = PVZ::Memory::GetProcAddress("onParseResource");
	hookAddress = 0x451A0D;
	rawlen = 7;
	BYTE code[] =
	{
		INVOKE(procAddress),
	};
	start(STRING(code));
}