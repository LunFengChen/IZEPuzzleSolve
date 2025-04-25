﻿#include "../PVZ.h"

PVZ::Attachment::Attachment(int idoraddress)
{
	if (idoraddress > 1024)
		BaseAddress = idoraddress;
	else
		BaseAddress = Memory::ReadPointer(0x6A9EC0, 0x820, 0xC, 0) + idoraddress * 0x30C;
}

PVZ::Animation PVZ::Attachment::GetAnimation()
{
	int ID = Memory::ReadMemory<int>(BaseAddress);
	return (Animation(ID & 0x00FFFF));
}
