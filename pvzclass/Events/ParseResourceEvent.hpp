#pragma once
#include "DLLEvent.h"

// ������Դ�����ļ��¼���
// ʱ�������� resources.xml �����ɹ���
// �޷���ֵ
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