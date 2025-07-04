﻿#include <cstring>
#include "../PVZ.h"

PVZ::Animation::Animation(int idoraddress)
{
	if (idoraddress > 1024)
		BaseAddress = idoraddress;
	else
		BaseAddress = Memory::ReadPointer(0x6A9EC0, 0x820, 8, 0) + idoraddress * 0xA0;
}

void PVZ::Animation::UnLock(int animprop)
{
	if (LOGICALINCLUDE(animprop, AP_ZOMBIESPEED))
		Memory::WriteMemory<LONGLONG>(0x52EFF0, -8029759185035983678);
	if (LOGICALINCLUDE(animprop, AP_ZOMBIECOLOR))
		Memory::WriteMemory<int>(0x52D3EA, -1869598485);
	if (LOGICALINCLUDE(animprop, AP_ZOMBIESCALE))
		Memory::WriteMemory<byte>(0x52C57E, 0x39);
	if (LOGICALINCLUDE(animprop, AP_PLANTCOLOR))
		Memory::WriteMemory<byte>(0x4636B4, 0x6B);
	if (LOGICALINCLUDE(animprop, AP_PLANTSCALE))
		Memory::WriteMemory<int>(0x463E1E, -1869607701);
}

void PVZ::Animation::Lock()
{
	Memory::WriteMemory<LONGLONG>(0x52EFF0, 1205015873675);
	Memory::WriteMemory<int>(0x52D3EA, 807683211);
	Memory::WriteMemory<byte>(0x52C57E, 0x14);
	Memory::WriteMemory<byte>(0x4636B4, 0x2B);
	Memory::WriteMemory<int>(0x463E1E, 405030105);
}

int PVZ::Animation::GetBaseAddress()
{
	return BaseAddress;
}

PVZ::Color PVZ::Animation::GetColor()
{
	Color color;
	color.Red = Memory::ReadMemory<int>(BaseAddress + 0x48);
	color.Green = Memory::ReadMemory<int>(BaseAddress + 0x4C);
	color.Blue = Memory::ReadMemory<int>(BaseAddress + 0x50);
	color.Alpha = Memory::ReadMemory<int>(BaseAddress + 0x54);
	return color;
}

void PVZ::Animation::SetColor(Color color)
{
	Memory::WriteMemory<int>(BaseAddress + 0x48, color.Red);
	Memory::WriteMemory<int>(BaseAddress + 0x4C, color.Green);
	Memory::WriteMemory<int>(BaseAddress + 0x50, color.Blue);
	Memory::WriteMemory<int>(BaseAddress + 0x54, color.Alpha);
}

PVZ::Color PVZ::Animation::GetAdditiveColor()
{
	Color color;
	color.Red = Memory::ReadMemory<int>(BaseAddress + 0x6C);
	color.Green = Memory::ReadMemory<int>(BaseAddress + 0x70);
	color.Blue = Memory::ReadMemory<int>(BaseAddress + 0x74);
	color.Alpha = Memory::ReadMemory<int>(BaseAddress + 0x78);
	return color;
}

void PVZ::Animation::SetAdditiveColor(Color color)
{
	Memory::WriteMemory<int>(BaseAddress + 0x6C, color.Red);
	Memory::WriteMemory<int>(BaseAddress + 0x70, color.Green);
	Memory::WriteMemory<int>(BaseAddress + 0x74, color.Blue);
	Memory::WriteMemory<int>(BaseAddress + 0x78, color.Alpha);
}

PVZ::Color PVZ::Animation::GetOverlayColor()
{
	Color color;
	color.Red = Memory::ReadMemory<int>(BaseAddress + 0x80);
	color.Green = Memory::ReadMemory<int>(BaseAddress + 0x84);
	color.Blue = Memory::ReadMemory<int>(BaseAddress + 0x88);
	color.Alpha = Memory::ReadMemory<int>(BaseAddress + 0x8C);
	return color;
}

void PVZ::Animation::SetOverlayColor(Color color)
{
	Memory::WriteMemory<int>(BaseAddress + 0x80, color.Red);
	Memory::WriteMemory<int>(BaseAddress + 0x84, color.Green);
	Memory::WriteMemory<int>(BaseAddress + 0x88, color.Blue);
	Memory::WriteMemory<int>(BaseAddress + 0x8C, color.Alpha);
}

PVZ::TrackInstance PVZ::Animation::GetTrackInstance(const char* trackName)
{
	int address = PVZ::Memory::ReadMemory<int>(BaseAddress + 0x58);
	return TrackInstance(this->FindTrackIndex(trackName) * 0x60 + address);
}

PVZ::AttachEffect PVZ::Animation::AttachTo(PVZ::AttachmentID attachmentID, float OffsetX, float OffsetY)
{
	SETARG(__asm__Reanimation__AttachTo, 1) = BaseAddress;
	SETARG(__asm__Reanimation__AttachTo, 6) = attachmentID.GetBaseAddress();
	SETARGFLOAT(__asm__Reanimation__AttachTo, 11) = OffsetY;
	SETARGFLOAT(__asm__Reanimation__AttachTo, 17) = OffsetX;
	SETARG(__asm__Reanimation__AttachTo, 39) = PVZ::Memory::Variable;
	return(PVZ::AttachEffect(PVZ::Memory::Execute(STRING(__asm__Reanimation__AttachTo))));
}

void PVZ::Animation::Die()
{
	SETARG(__asm__Reanimation__Die, 1) = BaseAddress;
	Memory::Execute(STRING(__asm__Reanimation__Die));
}

void PVZ::Animation::Play(const char* trackName, int blendType, int loopType, float rate)
{
	PVZ::Memory::WriteArray<const char>(PVZ::Memory::Variable + 100, trackName, std::strlen(trackName) + 1);
	SETARGFLOAT(__asm__Reanimation__Play, 1) = rate;
	SETARG(__asm__Reanimation__Play, 7) = blendType;
	SETARG(__asm__Reanimation__Play, 12) = BaseAddress;
	SETARG(__asm__Reanimation__Play, 17) = loopType;
	SETARG(__asm__Reanimation__Play, 22) = PVZ::Memory::Variable + 100;
	PVZ::Memory::Execute(STRING(__asm__Reanimation__Play));
}

void PVZ::Animation::AssignRenderGroupToPrefix(byte RenderGroup, const char* trackName)
{
	PVZ::Memory::WriteArray<const char>(PVZ::Memory::Variable + 100, trackName, std::strlen(trackName) + 1);
	__asm__Reanimation__AssignGroupToPrefix[1] = RenderGroup;
	SETARG(__asm__Reanimation__AssignGroupToPrefix, 3) = PVZ::Memory::Variable + 100;
	SETARG(__asm__Reanimation__AssignGroupToPrefix, 8) = this->BaseAddress;
	PVZ::Memory::Execute(STRING(__asm__Reanimation__AssignGroupToPrefix));
}

AsmBuilder AssignRenderGroupToTrack_builder = AsmBuilder();
void PVZ::Animation::AssignRenderGroupToTrack(const char* trackName, byte renderGroup)
{
	PVZ::Memory::WriteArray<const char>(PVZ::Memory::Variable + 100, trackName, std::strlen(trackName) + 1);
	AssignRenderGroupToTrack_builder.clear()
		.push(renderGroup)
		.push_imm32(PVZ::Memory::Variable + 100)
		.push_imm32(this->GetBaseAddress())
		.invoke(0x473A40)
		.ret();

	PVZ::Memory::Execute(AssignRenderGroupToTrack_builder);
}

int PVZ::Animation::FindTrackIndex(const char* trackName)
{
	PVZ::Memory::WriteArray<const char>(PVZ::Memory::Variable + 100, trackName, std::strlen(trackName) + 1);
	SETARG(__asm__Reanimation__FindTrackIndex, 1) = BaseAddress;
	SETARG(__asm__Reanimation__FindTrackIndex, 6) = PVZ::Memory::Variable + 100;
	SETARG(__asm__Reanimation__FindTrackIndex, 24) = PVZ::Memory::Variable;
	return(PVZ::Memory::Execute(STRING(__asm__Reanimation__FindTrackIndex)));
}

byte __asm__Reanimation_SetFramesForLayer[]
{
	PUSHDWORD(0),
	MOV_ECX(0),
	INVOKE(0x473280),
	RET
};

void PVZ::Animation::SetFramesForLayer(const char* theTrackName)
{
	PVZ::Memory::WriteArray<const char>(PVZ::Memory::Variable + 100, theTrackName, std::strlen(theTrackName) + 1);
	SETARG(__asm__Reanimation_SetFramesForLayer, 1) = PVZ::Memory::Variable + 100;
	SETARG(__asm__Reanimation_SetFramesForLayer, 6) = this->BaseAddress;
	PVZ::Memory::Execute(STRING(__asm__Reanimation_SetFramesForLayer));
}

byte __asm__Reanimation_SetImageOverride[]
{
	PUSHDWORD(0),
	MOV_EAX(0),
	MOV_ECX(0),
	INVOKE(0x473490),
	RET
};

void PVZ::Animation::SetImageOverride(const char* theTrackName, Image theImage)
{
	PVZ::Memory::WriteArray<const char>(PVZ::Memory::Variable + 100, theTrackName, std::strlen(theTrackName) + 1);
	SETARG(__asm__Reanimation_SetImageOverride, 1) = theImage.GetBaseAddress();
	SETARG(__asm__Reanimation_SetImageOverride, 6) = PVZ::Memory::Variable + 100;
	SETARG(__asm__Reanimation_SetImageOverride, 11) = this->GetBaseAddress();
	PVZ::Memory::Execute(STRING(__asm__Reanimation_SetImageOverride));
}
