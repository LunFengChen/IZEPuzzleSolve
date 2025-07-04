﻿#include "..\PVZ.h"

PVZ::Challenge::Challenge(int address)
{
	BaseAddress = Memory::ReadMemory<int>(address + 0x160);
}

namespace PVZ
{
	bool Challenge::SetMemSize(int NewSize)
	{
		if(NewSize < Challenge::MemSize)
			return(false);
		Memory::WriteMemory<int>(0x00408232, NewSize);
		Memory::WriteMemory<int>(0x00481C39, NewSize);

		byte __asm__inject[]
		{
			MOV_PTR_EUX_ADD_V_EVX(REG_EDI, REG_EBX, 0x50),
			0x8D, 0x87, INUMBER(0x0AC),
			PUSHDWORD(NewSize - 0x0AC),
			PUSH_EBX,
			PUSH_EAX,
			CALL(0x00206E0E),
			ADD_ESP(0x0C),
			NOP,
			NOP,
			NOP
		};
		Memory::WriteArray(0x0041F1FD, STRING(__asm__inject));
		return(true);
	}
}

BOOLEAN PVZ::Challenge::HaveCrater(int row, int column)
{
	if (row >= 0 && row < 6 && column >= 0 && column < 9)
		return Memory::ReadMemory<byte>(BaseAddress + 0x14 + 6 * column + row);
	return false;
}

void PVZ::Challenge::SetCrater(int row, int column, BOOLEAN b)
{
	if (row >= 0 && row < 6 && column >= 0 && column < 9)
		Memory::WriteMemory<byte>(BaseAddress + 0x14 + 6 * column + row, b);
}

byte __asm__IZSquishBrain[]
{
	MOV_EUX(REG_EAX, 0),
	PUSHDWORD(0),
	INVOKE(0x42BA30),
	RET
};

void PVZ::Challenge::IZSquishBrain(IZBrain brain)
{
	SETARG(__asm__IZSquishBrain, 1) = brain.GetBaseAddress();
	SETARG(__asm__IZSquishBrain, 6) = this->GetBaseAddress();
	PVZ::Memory::Execute(STRING(__asm__IZSquishBrain));
}