﻿#pragma once
#include "../PVZ.h"

namespace PVZ
{
	class BaseClass;
	/// @brief 粒子系统。
	class TodParticleSystem : public BaseClass
	{
	public:
		TodParticleSystem(DWORD indexoraddress);
		/// @brief 是否已被移除。
		T_PROPERTY(BOOLEAN, Dead, __get_Dead, __set_Dead, 0x1C);
		/// @brief 移除此粒子系统。
		void Die();
		/// @brief 将粒子系统移至指定位置。
		/// @param X 新位置 X 坐标
		/// @param Y 新位置 Y 坐标
		void MoveTo(float X, float Y);
		/// @brief 重载粒子系统的颜色。
		/// @param emitter_name 重载的粒子系统的发射器名称。若为空指针，则重载所有发射器。
		/// @param color 重载的颜色
		void OverrideColor(const char* emitter_name, Color& color);
		/// @brief 更改粒子系统内所有发射器中粒子是否高亮。
		/// @param isEnable 是否启用高亮
		void OverrideExtraAdditiveDraw(bool isEnable);
		/// @brief 重载粒子系统内所有发射器中粒子的图像。
		/// @param image 重载的图像
		void OverrideImage(Image image);
		/// @brief 重载粒子系统内所有发射器中粒子的大小。
		/// @param scale 重载的大小倍数
		void OverrideScale(float scale);
	};
	/// @brief 获取所有粒子系统。
	/// @return 包含所有粒子系统的 vector。
	std::vector<TodParticleSystem> GetAllParticleSystem();
	/// @brief 创建一个粒子系统。
	/// @param X 粒子系统的 X 坐标
	/// @param Y 粒子系统的 Y 坐标
	/// @param render_order 粒子系统的绘制顺序，数值大的会覆盖数值小的
	/// @param type 粒子系统类型
	/// @return 创建的粒子系统
	PVZ::TodParticleSystem CreateParticleSystem(float X, float Y, int render_order, EffectType::EffectType type);
}
