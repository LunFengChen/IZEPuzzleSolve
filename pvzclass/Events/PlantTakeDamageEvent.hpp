#pragma once
#include "DLLEvent.h"

// 植物受到啃食伤害事件。
// 此事件不考虑 IZ 模式下的双倍伤害。若要手动避免，请另行结算后取消伤害，
/// @param 依次为：植物地址、僵尸地址、GameObjectType::OBJECT_TYPE_NONE、伤害数值（非引用）
/// @return 调整后的伤害数值，负数会取消伤害。
class PlantTakeEatDamageEvent : public DLLEvent
{
public:
	PlantTakeEatDamageEvent();
	PlantTakeEatDamageEvent(const char* name);
};

PlantTakeEatDamageEvent::PlantTakeEatDamageEvent()
{
	PlantTakeEatDamageEvent::PlantTakeEatDamageEvent("onPlantTakeEatDamage");
}

PlantTakeEatDamageEvent::PlantTakeEatDamageEvent(const char* name)
{
	DWORD procAddress = PVZ::Memory::GetProcAddress(name);
	hookAddress = 0x52FCF0;
	rawlen = 7;
	BYTE code[] =
	{
		PUSH(4),
		PUSHDWORD(GameObjectType::OBJECT_TYPE_NONE),
		PUSH_EBP,
		PUSH_ESI,
		INVOKE(procAddress),
		ADD_ESP(16),
		TEST_EUX_EVX(REG_EAX, REG_EAX),
		JNS(8),

		POPAD,
		MOV_ECX(0x52FD2E),
		JMP_REG32(REG_ECX),

		0xF7, 0xD8,
		ADD_PTR_EUX_ADD_V_EVX(REG_ESI, 0x40, REG_EAX),
		ADD_PTR_EUX_ADD_V_V(REG_ESI, 0x40, 4),
	};
	start(STRING(code));
}

// 植物受到子弹伤害事件。
/// @param 依次为：植物地址、子弹地址、子弹的 GameObjectType、伤害数值（非引用）
/// @return 调整后的伤害数值，负数会取消伤害。
class PlantTakeProjectileDamageEvent : public DLLEvent
{
public:
	PlantTakeProjectileDamageEvent();
	PlantTakeProjectileDamageEvent(const char* name);
};

PlantTakeProjectileDamageEvent::PlantTakeProjectileDamageEvent()
{
	PlantTakeProjectileDamageEvent::PlantTakeProjectileDamageEvent("onPlantTakeProjectileDamage");
}

PlantTakeProjectileDamageEvent::PlantTakeProjectileDamageEvent(const char* name)
{
	DWORD procAddress = PVZ::Memory::GetProcAddress(name);
	hookAddress = 0x46CFEB;
	rawlen = 6;
	BYTE code[] =
	{
		PUSH_EAX,

		PUSH_EDX,
		PUSHDWORD(GameObjectType::OBJECT_TYPE_PROJECTILE),
		PUSH_EBP,
		PUSH_EAX,
		INVOKE(procAddress),

		ADD_ESP(16),
		TEST_EUX_EVX(REG_EAX, REG_EAX),
		POP_EUX(REG_ECX),
		JS(5),

		0xF7, 0xD8,
		ADD_PTR_EUX_ADD_V_EVX(REG_ECX, 0x40, REG_EAX),

		POPAD,
		MOV_ECX(0x46CFFE),
		JMP_REG32(REG_ECX),
	};
	start(STRING(code));
}

// 钢地刺因车辆、碾压等受伤事件。
/// @param 依次为：植物地址、0、GameObjectType::None、伤害数值（非引用）
/// @return 调整后的伤害数值，负数会取消伤害。
class SpikeRockTakeDamageEvent : public DLLEvent
{
public:
	SpikeRockTakeDamageEvent();
	SpikeRockTakeDamageEvent(const char* name);
};

SpikeRockTakeDamageEvent::SpikeRockTakeDamageEvent()
{
	SpikeRockTakeDamageEvent::SpikeRockTakeDamageEvent("onSpikeRockTakeDamage");
}

SpikeRockTakeDamageEvent::SpikeRockTakeDamageEvent(const char* name)
{
	DWORD procAddress = PVZ::Memory::GetProcAddress(name);
	hookAddress = 0x45EC63;
	rawlen = 7;
	BYTE code[] =
	{
		PUSH(50),
		PUSHDWORD(GameObjectType::OBJECT_TYPE_PROJECTILE),
		PUSH(0),
		PUSH_EAX,
		INVOKE(procAddress),
		ADD_ESP(16),
		TEST_EUX_EVX(REG_EAX, REG_EAX),
		JNS(8),

		POPAD,
		MOV_ECX(0x46CFFE),
		JMP_REG32(REG_ECX),

		0xF7, 0xD8,
		ADD_PTR_EUX_ADD_V_EVX(REG_ESI, 0x40, REG_EAX),

		POPAD,
		MOV_EUX_PTR_EVX_ADD(REG_EAX, REG_ESI, 0x40),
		MOV_ECX(0x45EC6A),
		JMP_REG32(REG_ECX)
	};
	start(STRING(code));
}

// 植物受伤事件。
// 复合事件。
// 此事件不考虑 IZ 模式下的双倍伤害。若要手动避免，请另行结算后取消伤害，
/// @param 依次为：植物地址、伤害来源地址（可能为空）、伤害来源的 GameObjectType、伤害数值（非引用）
/// @return 调整后的伤害数值，负数会取消伤害。
class PlantTakeDamageEvent
{
private:
	PlantTakeEatDamageEvent* pted_event;
	PlantTakeProjectileDamageEvent* ptej_event;
	SpikeRockTakeDamageEvent* srtd_event;
public:
	PlantTakeDamageEvent()
	{
		pted_event = new PlantTakeEatDamageEvent("onPlantTakeDamage");
		ptej_event = new PlantTakeProjectileDamageEvent("onPlantTakeDamage");
		srtd_event = new SpikeRockTakeDamageEvent("onPlantTakeDamage");
	}
	void end()
	{
		pted_event->end();
		ptej_event->end();
		srtd_event->end();
	}
};
