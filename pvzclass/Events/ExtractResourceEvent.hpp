#pragma once
#include "DLLEvent.h"

// ��ȡ��Դ�¼���
// ʱ�������� resources.xml �����ɹ���
/// @param �����¼��� ResourceManager����ȡ����Դ�����ƣ�char* ��ʽ��
/// @return ��Ϊ���������ȡ�ɹ�����Ϊ 0�����ȡʧ�ܣ���Ϊ����������ԭ����̡�
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