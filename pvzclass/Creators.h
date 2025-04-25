#pragma once
#include "PVZ.h"

/// @brief 包含创建多种 PVZ 内部对象的函数
namespace Creator
{

	inline short makeshort(byte b1, byte b2)
	{
		return (b2 << 8) + b1;
	}
	inline void xytorc(int* x, int* y)
	{
		int temp = *y;
		*y = (*x - 40) / 80;
		SceneType::SceneType scene = PVZ::Memory::ReadMemory<SceneType::SceneType>(PVZBASEADDRESS + 0x554C);
		bool sixroute = (scene == SceneType::Pool) || (scene == SceneType::Fog);
		*x = sixroute ? (temp - 80) / 85 : (temp - 80) / 100;
	}

#define PI 3.1415926f
#define CREATEREANIMATION INVOKE_DWORD_DWORD_DWORD(0x453C30,0,0,0)
#define CREATEZOMBIE INVOKE_BYTE_BYTE(0x42A0F0,0,0)
#define CREATEPLANT INVOKE_DWORD_BYTE_BYTE_BYTE(0x40D120,0,0,0,255)
#define CREATEPROJECTILE INVOKE_DWORD_DWORD_BYTE_BYTE_BYTE(0x40D620,0,0,0,0,0)
#define CREATECOIN INVOKE_DWORD_DWORD_BYTE_BYTE(0x40CB10,0,0,0,0)
#define RESETLAWNMOVER INVOKE_DWORD(0x40BC70,0)
#define CREATEGRDITEM INVOKE(0x41E1C0)
#define CREATEGRAVE INVOKE(0x426620)
#define CREATECRATER INVOKE_BYTE(0x408F80,0)
#define CREATELADDER INVOKE_BYTE(0x408F40,0)
#define CREATERAKE INVOKE_DWORD_BYTE_BYTE(0x40B9C0,0,0,0)
#define CHARSTRTOSTRING INVOKE(0x404450)
#define CREATECAPTION INVOKE(0x459010)
#define CREATEIMAGECAPTION INVOKE(0x40CA10)
#define CREATEPLANTEFFECT INVOKE_DWORD(0x4666A0,0)
#define CREATEEXPLOTION INVOKE_DWORD_BYTE_DWORD_DWORD_DWORD_BYTE_BYTE_BYTE(0x41D8A0,0,0,0,0,0,0,0,0)
#define _CREATEEFFECT INVOKE_DWORD_DWORD_DWORD_DWORD(0x518A70,0,0,0x61A80,0)
#define CREATESOUND INVOKE_BYTE(0x515020,0)
#define CREATEUPPERSOUND INVOKE_DWORD(0x4560C0,0)
#define FREEZEALL INVOKE(0x466420)
#define STOPSOUND INVOKE(0x515290)
#define CREATEIZOMBIEFORMATION INVOKE_DWORD(0x42A890,0)
#define CREATEVASEFORMATION INVOKE(0x4286F0)
#define CREATEPORTAL INVOKE(0x426FC0)
#define CLEARZOMBIEPREVIEW INVOKE(0x40DF70)
#define CREATEZOMBIEINLEVEL INVOKE(0x4092E0)

	/// @brief 在指定位置创建一个动画模型。
	/// @param type 动画类型
	/// @param x X 坐标
	/// @param y Y 坐标
	/// @param layer 图层大小
	/// @return 动画模型
	PVZ::Animation CreateReanimation(AnimationType::AnimationType type, float x, float y, int layer = 0x310000);

	/// @brief 在指定位置生成一个指定类型的僵尸。
	/// @note 僵王博士将会被强制生成在 0 行 0 列。
	/// @param type 僵尸类型
	/// @param row 行
	/// @param column 列
	/// @return 生成的僵尸
	PVZ::Zombie CreateZombie(ZombieType::ZombieType type, int row, byte column);

	/// @brief 在指定位置生成一个指定类型的植物。
	/// @param type 植物类型
	/// @param row 行
	/// @param column 列
	/// @param imitative 是否是模仿者生成的植物。若为 true，会产生一个模仿指定植物的模仿者。
	/// @return 生成的植物
	PVZ::Plant CreatePlant(SeedType::SeedType type, int row, byte column, BOOLEAN imitative = false);

	/// @brief 在指定位置生成一个指定类型的子弹。
	/// @note 索敌类型默认为 0x0B
	/// @param type 子弹类型
	/// @param row 行
	/// @param x X 坐标
	/// @return 生成的子弹
	PVZ::Projectile CreateProjectile(ProjectileType::ProjectileType type, byte row, int x);

	/// @brief 为部分函数建立基础。
	void AsmInit();

	/// @brief 消除 AsmInit() 产生的效应。
	void AsmReset();

	/// @brief 快速创建一个子弹。
	/// @attention 你需要先调用一次 AsmInit() 后才能使用这个函数。
	/// @see AsmInit()
	/// @param type 子弹类型
	/// @param x	X坐标
	PVZ::Projectile CreateProjectile(ProjectileType::ProjectileType type, int x, int y, float angle, float speed);

	/// @brief 在指定位置生成一个指定类型的物品。
	/// @param type 物品类型
	/// @param x X 坐标
	/// @param y Y 坐标
	/// @param motion 物品的位移类型
	/// @return 生成的物品
	PVZ::Coin CreateCoin(CoinType::CoinType type, int x, int y, CoinMotionType::CoinMotionType motion);

	/// @brief 移除场上已有的除草机，然后初始化本关的除草机。
	void ResetLawnmover();

	/// @brief 生成一个空的场地物件
	/// @note 此物件的各项属性都需要自行设定。
	/// @return 生成的场地物件
	PVZ::Griditem CreateGriditem();

	/// @brief 在指定位置生成一个墓碑
	/// @note 此函数没有返回值。
	/// @todo 尝试实现一个可捕获返回值的版本。
	/// @param row 行
	/// @param column 列
	void CreateGrave(int row, int column);

	/// @brief 在指定位置生成一个弹坑
	/// @param row 行
	/// @param column 列
	/// @param duration 持续时间
	/// @return 生成的弹坑
	PVZ::Crater CreateCrater(int row, int column, int duration);

	/// @brief 在指定位置生成一个梯子
	/// @note 返回值为 PVZ::Griditem 类型，而不是梯子类型
	/// @param row 行
	/// @param column 列
	/// @return 生成的梯子
	PVZ::Griditem CreateLadder(int row, byte column);

	PVZ::Vase CreateVase(int row, int column, VaseContent::VaseContent content, VaseSkin::VaseSkin skin = VaseSkin::VaseSkinUnknow, ZombieType::ZombieType zombie = ZombieType::Zombie, SeedType::SeedType plant = SeedType::Peashooter, int sun = 0);

	PVZ::IZBrain CreateIZBrain(int row, int column = 0);

	PVZ::Portal CreatePortal(int row, int column, int isYellow = 0);

	/// @brief 待创建罐子的详细信息
	struct VaseCreateInfo
	{
		int row;
		int column;
		VaseContent::VaseContent content;
		VaseSkin::VaseSkin skin;
		ZombieType::ZombieType zombie;
		SeedType::SeedType plant;
		int sun;
	};

	PVZ::Vase CreateVase(VaseCreateInfo vaseinfo);

	void CreateRake(byte row, byte column);

	void CreateCaption(const char* str, int length, CaptionStyle::CaptionStyle style, int duration = 500);

	void CreateImageCaption(const char* str, int length);

	void CreatePlantEffect(PlantEffectType::PlantEffectType type, int x, int y);

	void CreateExplosion(int x, int y, int radius, BOOLEAN cinder = true, byte bound = 5, BOOLEAN fromjackzombie = false);

	void CreateEffect(EffectType::EffectType type, float x, float y);

	//部分音效是持续的，需要用StopSound手动停止音效持续播放
	void CreateSound(int soundid);

	void CreateLowerSound(LowerSoundType::LowerSoundType sound);

	void CreateUpperSound(UpperSoundType::UpperSoundType sound);

	/// @brief 将第一个植物视为寒冰菇，然后令其冻结全场僵尸。
	/// @attention 当前关卡必须至少存在过一个植物，此函数才有效。
	void FrozeAll();

	void StopSound(int soundid);

	void CreateIZombieFormation(PVZLevel::PVZLevel izlevel);

	void CreateVaseFormation(PVZLevel::PVZLevel vblevel);

	/// @attention 你需要先调用一次 AsmInit() 后才能使用这个函数。
	/// @see AsmInit()
	void __CreatePortal();

	/// @attention 你需要先调用一次 AsmInit() 后才能使用这个函数。
	/// @see AsmInit()
	void CreatePortal(int yellow1Row, int yellow1Column, int yellow2Row, int yellow2Column, int blue1Row, int blue1Column, int blue2Row, int blue2Column);

	void __ClearZombiePreview();

	//生成出怪列表
	//wave = 0 means defalt and Non-intervention
	void CreateZombieInLevel(ZombieType::ZombieType* ztypes, int length, int wave = 0);

}