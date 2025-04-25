#pragma once
/// @brief 三态，主要用于 Extensions.h 中函数的参数。
namespace ThreeState
{
	/// @brief 功能的启动状态
	enum ThreeState
	{
		/// @brief 禁用功能。任何情况下该特性都失效。
		Disable = -1,
		/// @brief 恢复默认情况。
		None,
		/// @brief 启用功能。任何情况下该特性都生效。
		Enable
	};

	extern const char* ToString(ThreeState mouset);

}