#pragma once
#include "Enums.h"
#include "Flags.h"
#include "AsmFunctions.h"
#include "AsmBuilder.hpp"
#include "Memory.hpp"
#include <ctime>
#include <memory>
#include <vector>
#include <type_traits>

using std::enable_if_t;
using std::is_base_of;

#pragma region definitions

#define STRING(str) str,sizeof(str)/sizeof(*str)
#define SETARG(asmfunction,index) *(int*)(asmfunction+index)
#define SETARGFLOAT(asmfunction,index) *(float*)(asmfunction+index)

#define PAGE_SIZE 1024
#define PVZ_BASE PVZ::Memory::ReadMemory<int>(0x6A9EC0)
#define PVZBASEADDRESS PVZ::Memory::ReadMemory<int>(PVZ_BASE + 0x768)

#define PROPERTY(type,getmethod,setmethod) type getmethod();void setmethod(type value);__declspec(property(get=getmethod,put=setmethod)) type
#define READONLY_PROPERTY(type,getmethod) type getmethod();__declspec(property(get=getmethod)) type
#define WRITEONLY_PROPERTY(type,setmethod) void setmethod(type value);__declspec(property(put=setmethod)) type

#define PROPERTY_BINDING(type,getmethod,getter,setmethod,setter) inline type getmethod(){return getter;};inline void setmethod(type value){setter;};__declspec(property(get=getmethod,put=setmethod)) type
#define READONLY_PROPERTY_BINDING(type,getmethod,getter) inline type getmethod(){return getter;};__declspec(property(get=getmethod)) type
#define WRITEONLY_PROPERTY_BINDING(type,setmethod,setter) inline void setmethod(type value){setter;};__declspec(property(put=setmethod)) type

#define INT_PROPERTY(propname,getmethod,setmethod,offset) PROPERTY_BINDING(int,getmethod,Memory::ReadMemory<int>(BaseAddress+offset),setmethod,Memory::WriteMemory<int>(BaseAddress+offset,value)) propname
#define INT_READONLY_PROPERTY(propname,getmethod,offset) READONLY_PROPERTY_BINDING(int,getmethod,Memory::ReadMemory<int>(BaseAddress+offset)) propname
#define T_PROPERTY(type,propname,getmethod,setmethod,offset) PROPERTY_BINDING(type,getmethod,Memory::ReadMemory<type>(BaseAddress+offset),setmethod,Memory::WriteMemory<type>(BaseAddress+offset,value)) propname
#define T_READONLY_PROPERTY(type,propname,getmethod,offset) READONLY_PROPERTY_BINDING(type,getmethod,Memory::ReadMemory<type>(BaseAddress+offset)) propname

#define INT_ARRAY_PROPERTY(getmethod,setmethod,offset) inline int getmethod(int index) \
	{ return Memory::ReadMemory<int>(BaseAddress+offset+index*4); } \
	inline void setmethod(int index, int value) \
	{ Memory::WriteMemory<int>(BaseAddress+offset+index*4, value); } \

#define T_ARRAY_PROPERTY(type,getmethod,setmethod,offset,size) inline type getmethod(int index) \
	{ return Memory::ReadMemory<type>(BaseAddress+offset+index*size); } \
	inline void setmethod(int index, type value) \
	{ Memory::WriteMemory<type>(BaseAddress+offset+index*size, value); } \

#define LOGICALINCLUDE(c,v) (c&v)==v

#define SPT std::shared_ptr
#define UPT std::unique_ptr
#define MKU std::make_unique
#define MKS std::make_shared

#pragma endregion

#define AP_ZOMBIESPEED		1
#define AP_ZOMBIECOLOR		2
#define AP_ZOMBIESCALE		4
#define AP_PLANTCOLOR		8
#define AP_PLANTSCALE		16

#define GPD_RIGHT			0
#define GPD_LEFT			1

#define MINGAM_ENABLE		2
#define MINGAM_DISABLE		-1

#define MINGAME_STARTED		1
#define MINGAME_NOTSTARTED	-1

#define APA_LOOP			1
#define APA_ONCE_DISAPPEAR	2
#define APA_ONCE_STOP		3

#define ID_INDEX(id) ((id) & 0x0000FFFF)
#define ID_RANK(id) ((id) & 0xFFFF0000)

#define HZC_COMMON 1
#define HZC_BALLOON_AIR 2
#define HZC_SNORKED_UNDER 4
#define HZC_NOT_GROUNDED 16
#define HZC_DYING 32
#define HZC_DIGGER_UNDER 64
#define HZC_HYPNOTIZED 128

#define INVALID_BASEADDRESS 0x400000

/// @brief 包含大部分用于控制 PVZ 内部对象的类和方法。
/// @note Only version 1.0.0.1051 is fully supported
namespace PVZ
{
	/// @brief 初始化 PVZ 命名空间，且不在程序内附加钩子。Memory::immediateExecute 会设置为 true 。
	/// @param pid 进程 id
	void InitPVZNoLock(DWORD pid);

	void InitPVZ(DWORD pid);
	void QuitPVZ();

#pragma region structs

	struct Color
	{
		int Red;
		int Green;
		int Blue;
		int Alpha;
	};

#pragma endregion

#pragma region classes	
	/// @brief 所有对应 PVZ 内部对象的类的基类。
	/// @attention 你不应该直接构造 BaseClass！
	class BaseClass
	{
	protected:
		/// @brief 对应对象的基地址
		int BaseAddress;
	public:
		BaseClass() : BaseAddress(INVALID_BASEADDRESS) {};
		BaseClass(int address) : BaseAddress(address){};
		/// @brief 返回基址
		/// @return 基址
		int GetBaseAddress() const
		{ return(this->BaseAddress); }
		/// @brief 对应的对象是否已经失效，或者构造不良。
		/// @return 是否已经失效或构造不良。
		const bool isValid()
		{ return(this->BaseAddress != INVALID_BASEADDRESS && this->BaseAddress != 0); }
	};

	class Rect
	{
	public:
		int X;
		int Y;
		int Width;
		int Height;
		// 判定坐标为 (X, Y)，半径为 radius 的圆与该矩阵是否有重叠部分。
		// 相切会视为有重叠部分。
		bool IsCircleOverlap(const int X, const int Y, const int radius);
	};
	// 取得两个矩形横向重叠部分的长度。
	// 若横向无重叠部分，返回两矩形横向间距的相反数。
	/// @param rect 计算重叠的另一个矩形。
	/// @return 矩形横向重叠的长度，或矩形横向间距的相反数。
	int GetXOverlap(const Rect& rect1, const Rect& rect2);

	typedef Rect CollisionBox;

	class PVZutil
	{
	public:
		READONLY_PROPERTY(const char*,				__get_Version)		Version;
		READONLY_PROPERTY(PVZVersion::PVZVersion,	__get_GameVersion)	GameVersion;
	};

	/// @brief 游戏程序主类（原 LawnApp）。
	class PVZApp : public BaseClass
	{
	public:
		PVZApp(DWORD address) : BaseClass(address) {};
		PROPERTY(double,						__get_MusicVolume,	__set_MusicVolume)			aMusicVolume;//range[0,1]
		PROPERTY(double,						__get_SoundFXVolume,__set_SoundFXVolume)		SoundFXVolume;//range[0,1]
		T_PROPERTY(DWORD,						PrimaryThreadId,	__get_PrimaryThreadId,		__set_PrimaryThreadId,		0x33C);
		T_PROPERTY(BOOLEAN,						Shutdown,			__get_Shutdown,				__set_Shutdown,				0x341);
		T_PROPERTY(HWND,						HWnd,				__get_HWnd,					__set_HWnd,					0x350);
		T_PROPERTY(PVZLevel::PVZLevel,			LevelId,			__get_LevelId,				__set_LevelId,				0x7F8);
		T_PROPERTY(PVZGameState::PVZGameState,	GameState,			__get_GameState,			__set_GameState,			0x7FC);
		T_PROPERTY(BOOLEAN,						FreePlantingCheat,	__get_FreePlantingCheat,	__set_FreePlantingCheat,	0x814);
		T_PROPERTY(BOOLEAN,						FullVersion,		__get_FullVersion,			__set_FullVersion,			0x8C0);
	};
	PVZApp GetPVZApp();

	class Image : public BaseClass
	{
	public:
		Image(int address) : BaseClass(address){};
	};

	class Attachment;
	class AttachmentID;
	class Zombie;
	class Plant;
	class Projectile;
	class Coin;
	class Lawnmover;
	class Griditem;
	class MousePointer;
	class Caption;
	class CardSlot;
	class Challenge;
	class Lawn;
	class Icetrace;
	class Wave;
	class Widget : public BaseClass
	{
	public:
		Widget(int address) : BaseClass(address) {};
		INT_PROPERTY(ViewX,			__get_ViewX,		__set_ViewX,		0x30);
		INT_PROPERTY(ViewY,			__get_ViewY,		__set_ViewY,		0x34);
		INT_PROPERTY(ViewLength,	__get_ViewLength,	__set_ViewLength,	0x38);
		INT_PROPERTY(ViewHeight,	__get_ViewHeight,	__set_ViewHeight,	0x3C);
	};
	class Matrix3 : public BaseClass
	{
	public:
		Matrix3(DWORD address) : BaseClass(address) {};

		/// @brief 根据指定的平移坐标、旋转弧度和拉伸比例，设定矩阵每个项的数值。
		void ScaleRotateTransformMatrix(float x, float y, float rad, float ScaleX, float ScaleY);
	};
	class Board : public Widget
	{
	protected:
		/// @brief 所有 GetAll() 形式函数的原型，获取 获取 DataArray\<T\> 中的全体成员。
		/// @tparam T 成员类型
		/// @note T 必须具有 MemSize 静态常量，且类型为整数
		/// @tparam _Base_offset 基址的偏移量
		/// @tparam _Max_offset 最大数量的偏移量
		/// @tparam _T_Dead_offset 在 T 中，表示该成员已被移除变量的偏移量。此变量视为用 byte 存储。
		/// @return 装有全体 T 成员对象的 std::vector
		template<typename T, size_t _Base_offset, size_t _Max_offset, size_t _T_Dead_offset>
		std::vector<T> __prototype_GetAll()
		{
			std::vector<T> container;
			int maxnum = Memory::ReadMemory<int>(BaseAddress + _Max_offset);
			DWORD base_addr = Memory::ReadMemory<DWORD>(BaseAddress + _Base_offset);
			for (int i = 0; i < maxnum; i++)
			{
				if (!Memory::ReadMemory<byte>(base_addr + _T_Dead_offset + T::MemSize * i))
					container.push_back(T(i));
			}
			return container;
		}
	public:
		Board(int address) : Widget(address) {};
		PVZApp GetPVZApp();
		INT_READONLY_PROPERTY(ZombiesCount, __get_ZombiesCount, 0xA0);
		INT_READONLY_PROPERTY(PlantsCount, __get_PlantsCount, 0xBC);
		INT_READONLY_PROPERTY(ProjectilesCount, __get_ProjectilesCount, 0xD8);
		INT_READONLY_PROPERTY(CoinsCount, __get_CoinsCount, 0xF4);
		INT_READONLY_PROPERTY(LawnmoversCount, __get_LawnmoversCount, 0x110);
		INT_READONLY_PROPERTY(GriditemsCount, __get_GriditemsCount, 0x12C);
		T_PROPERTY(BOOLEAN, GamePaused, __get_GamePaused, __set_GamePaused, 0x164);

#pragma region fog

		int GetGridFog(int row, int column);
		T_PROPERTY(FLOAT, FogOffset, __get_FogOffset, __set_FogOffset, 0x5D0);
		INT_PROPERTY(FogBlownCountDown, __get_FogBlownCountDown, __set_FogBlownCountDown, 0x5D4);

#pragma endregion

		INT_PROPERTY(SunDropCountdown, __get_SunDropCountdown, __set_SunDropCountdown, 0x5538);
		INT_PROPERTY(SunDropCount, __get_SunDropCount, __set_SunDropCount, 0x553C);
		PROPERTY(SceneType::SceneType, __get_LevelScene, __set_LevelScene) LevelScene;
		INT_PROPERTY(AdventureLevel, __get_AdventureLevel, __set_AdventureLevel, 0x5550);
		INT_PROPERTY(Sun, __get_Sun, __set_Sun, 0x5560);
		PROPERTY(int, __get_WaveCount, __set_WaveCount) WaveCount;
		/*exclude preparing time*/
		INT_READONLY_PROPERTY(PlayingTime, __get_PlayingTime, 0x5568);
		/*include preparing time*/
		INT_READONLY_PROPERTY(PlayingTime2, __get_PlayingTime2, 0x556C);
		/*lose focus and recount*/
		INT_READONLY_PROPERTY(PlayingTime3, __get_PlayingTime3, 0x5570);
		INT_READONLY_PROPERTY(CurrentWave, __get_CurrentWave, 0x557C);
		INT_READONLY_PROPERTY(RefreshedWave, __get_RefreshedWave, 0x5580);
		INT_PROPERTY(FlashTip, __get_FlashTip, __set_FlashTip, 0x5584);
		/*Flash tips for novice tutorials*/
		INT_PROPERTY(RefreshHp, __get_RefreshHp, __set_RefreshHp, 0x5594);
		INT_READONLY_PROPERTY(CurrentWaveHp, __get_CurrentWaveHp, 0x5598);
		INT_PROPERTY(NextWaveCountdown, __get_NextWaveCountdown, __set_NextWaveCountdown, 0x559C);
		INT_READONLY_PROPERTY(NextWaveCountdownInitialValue, __get_NextWaveCountdownInitialValue, 0x55A0);
		INT_PROPERTY(HugeWaveCountdown, __get_HugeWaveCountdown, __set_HugeWaveCountdown, 0x55A4);
		T_PROPERTY(BOOLEAN, HaveShovel, __get_HaveShovel, __set_HaveShovel, 0x55F1);
		INT_PROPERTY(ShowMoneyCountdown, __get_ShowMoneyCountdown, __set_ShowMoneyCountdown, 0x55F4);
		T_PROPERTY(DebugModeType::DebugModeType, DebugMode, __get_DebugMode, __set_DebugMode, 0x55F8);
		INT_PROPERTY(LevelProcessBar, __get_LevelProcessBar, __set_LevelProcessBar, 0x5610);

		T_PROPERTY(BOOLEAN, Mustache, __get_Mustache, __set_Mustache, 0x5761);
		T_PROPERTY(BOOLEAN, Trickedout, __get_Trickedout, __set_Trickedout, 0x5762);
		T_PROPERTY(BOOLEAN, Future, __get_Future, __set_Future, 0x5763);
		T_PROPERTY(BOOLEAN, Pinata, __get_Pinata, __set_Pinata, 0x5764);
		T_PROPERTY(BOOLEAN, Dance, __get_Dance, __set_Dance, 0x5765);
		T_PROPERTY(BOOLEAN, Daisies, __get_Daisies, __set_Daisies, 0x5766);
		T_PROPERTY(BOOLEAN, Sukhbir, __get_Sukhbir, __set_Sukhbir, 0x5767);

		INT_READONLY_PROPERTY(EatenPlants, __get_EatenPlants, 0x5798);
		INT_READONLY_PROPERTY(ShoveledPlants, __get_ShoveledPlants, 0x579C);

		void GetZombieAllowed(ZombieType::ZombieType* ztypes);

		READONLY_PROPERTY_BINDING(
			BOOLEAN,
			__get_SixRoute,
			(LevelScene == SceneType::Pool) || (LevelScene == SceneType::Fog)) SixRoute;

#pragma region methods

		int GridToXPixel(int row, int column);
		int GridToYPixel(int row, int column);
		void Assault(int countdown = 1);
		void Bell(int countdown = 1);
		void Earthquake(int horizontalAmplitude = 2, int verticalAmplitude = 4, int duration = 20);
		void Lose();
		/// @brief 若当前可以承担 amount 点阳光的支出，则消耗 theAmount 阳光，
		//	否则触发阳光数量不足的的音效和闪红特效。
		/// @param amount 阳光消耗数值。
		/// @return 是否可以承担支出。
		bool TakeSunMoney(int amount);
		void Win();
		bool Save(const char* path, int pathlen);
		bool Load(const char* path, int pathlen);

#pragma endregion

#pragma region getmethod
		/// @brief 获取 DataArray\<Zombie\> 中的全体对象。
		/// @tparam T 成员的类型，必须为 Zombie 或它的派生类。
		/// @return 装有全体 Zombie （或者其派生类）对象的 std::vector
		template<typename T = Zombie, typename = enable_if_t<is_base_of<Zombie, T>::value>>
		std::vector<T> GetAllZombies()
		{
			return __prototype_GetAll<T, 0x90, 0x94, 0x0EC>();
		}
		/// @brief 获取 DataArray\<Plant\> 中的全体对象。
		/// @tparam T 成员值的类型，必须为 Plant 或它的派生类。
		/// @return 装有全体 Plant （或者其派生类）对象的 std::vector
		template<typename T = Plant, typename = enable_if_t<is_base_of<Plant, T>::value>>
		std::vector<T> GetAllPlants()
		{
			return __prototype_GetAll<T, 0x0AC, 0x0B0, 0x141>();
		}
		/// @brief 获取 DataArray\<Projectile\> 中的全体对象。
		/// @tparam T 成员值的类型，必须为 Projectile 或它的派生类。
		/// @return 装有全体 Projectile （或者其派生类）对象的 std::vector
		template<typename T = Projectile, typename = enable_if_t<is_base_of<Projectile, T>::value>>
		std::vector<T> GetAllProjectile()
		{
			return __prototype_GetAll<T, 0x0C8, 0x0CC, 0x50>();
		}
		/// @brief 获取 DataArray\<Coin\> 中的全体对象。
		/// @tparam T 成员值的类型，必须为 Coin 或它的派生类。
		/// @return 装有全体 Coin （或者其派生类）对象的 std::vector
		template<typename T = Coin, typename = enable_if_t<is_base_of<Coin, T>::value>>
		std::vector<T> GetAllCoins()
		{
			return __prototype_GetAll<T, 0x0E4, 0x0E8, 0x38>();
		}
		/// @brief 获取 DataArray\<LawnMower\> 中的全体对象。
		/// @tparam T 成员值的类型，必须为 LawnMower 或它的派生类。
		/// @return 装有全体 LawnMower （或者其派生类）对象的 std::vector
		template<typename T = LawnMower, typename = enable_if_t<is_base_of<LawnMower, T>::value>>
		std::vector<T> GetAllLawnmovers()
		{
			return __prototype_GetAll<T, 0x100, 0x104, 0x30>();
		}
		/// @brief 获取 DataArray\<Griditem\> 中的全体对象。
		/// @note 与其他 GetAll() 不同，此函数不依赖于 __prototype_GetAll() 。
		/// @tparam T 成员值的类型，必须为 Griditem 或它的派生类。
		/// @return 装有全体 Griditem （或者其派生类）对象的 std::vector
		template<typename T = Griditem, typename = enable_if_t<is_base_of<Griditem, T>::value>>
		std::vector<T> GetAllGriditems()
		{
			std::vector<T> griditems;
			int maxnum = Memory::ReadMemory<int>(BaseAddress + 0x120);
			DWORD base_addr = Memory::ReadMemory<DWORD>(BaseAddress + 0x11C);
			for (int i = 0; i < maxnum; i++)
			{
				if (!Memory::ReadMemory<byte>(base_addr + 0x20 + T::MemSize * i)
					&& (T::ItemType == 0 || Memory::ReadMemory<byte>(base_addr + 8 + T::MemSize * i) == T::ItemType))
						griditems.push_back(T(i));
			}
			return griditems;
		}
		Lawn GetLawn();
		Icetrace GetIcetrace();
		Wave GetWave(int index);
		MousePointer GetMousePointer();
		Caption GetCaption();
		CardSlot GetCardSlot();
		/// @brief 获取 Challenge 类型的成员。
		/// @tparam T 返回值的类型，必须为 Challenge 或它的派生类。
		/// @return Challenge （或者其派生类）成员对象 
		template<typename T = Challenge, typename = enable_if_t<is_base_of<Challenge, T>::value>>
		T GetChallenge()
		{
			return T(BaseAddress);
		}
		/// @brief 获取 Challenge 类型的成员。
		/// @attention 与 GetChallenge() 不同，此方法只能获得 Challenge 类型的对象。
		/// @return Challenge 成员对象 
		Challenge GetMiscellaneous();
#pragma endregion
	};
	//Do NOT construct this class directly!
	class GameObject : public BaseClass
	{
	public:
		GameObject() : BaseClass(0) {};
		PVZApp GetLawnApp()
		{ return(PVZ::PVZApp(Memory::ReadMemory<DWORD>(BaseAddress))); }
		PVZ::Board GetBoard()
		{
			return(PVZ::Board(Memory::ReadMemory<int>(BaseAddress + 4)));
		}
		INT_PROPERTY(ImageX, __get_ImageX, __set_ImageX, 8);
		INT_PROPERTY(ImageY, __get_ImageY, __set_ImageY, 0xC);
		INT_PROPERTY(Width, __get_Width, __set_Width, 0x10);
		INT_PROPERTY(Height, __get_Height, __set_Height, 0x14);
		T_PROPERTY(BOOLEAN, Visible, __get_Visible, __set_Visible, 0x18);
		INT_PROPERTY(Row, __get_Row, __set_Row, 0x1C);
		INT_PROPERTY(Layer, __get_Layer, __set_Layer, 0x20);
	};
	class TrackInstance;
	class AttachEffect : public BaseClass
	{
	public:
		AttachEffect(int address) : BaseClass(address) {};
		Matrix3 GetOffset();
	};
	class Animation : public BaseClass
	{
	public:
		Animation(int idoraddress);
		//support muiti-animprop(AP_XXXXXX)
		static void UnLock(int animprop);
		static void Lock();
		int GetBaseAddress();
		T_READONLY_PROPERTY(AnimationType::AnimationType, Type, __get_Type, 0);
		T_PROPERTY(FLOAT, CycleRate, __get_CycleRate, __set_CycleRate, 4);
		T_PROPERTY(FLOAT, Speed, __get_Speed, __set_Speed, 8);
		T_PROPERTY(BOOLEAN, NotExist, __get_NotExist, __set_NotExist, 0x14);
		INT_PROPERTY(StartFrame, __get_StartFrame, __set_StartFrame, 0x18);
		INT_PROPERTY(FrameCount, __get_FrameCount, __set_FrameCount, 0x1C);
		INT_PROPERTY(FrameBasePose, __get_FrameBasePose, __set_FrameBasePose, 0x20);
		T_PROPERTY(FLOAT, XScale, __get_XScale, __set_XScale, 0x24);
		T_PROPERTY(FLOAT, XSlant, __get_XSlant, __set_XSlant, 0x28);
		T_PROPERTY(FLOAT, XOffset, __get_XOffset, __set_XOffset, 0x2C);
		T_PROPERTY(FLOAT, YScale, __get_YScale, __set_YScale, 0x30);
		T_PROPERTY(FLOAT, YSlant, __get_YSlant, __set_YSlant, 0x34);
		T_PROPERTY(FLOAT, YOffset, __get_YOffset, __set_YOffset, 0x38);
		Color GetColor(); // 0x48-0x54，基础颜色
		void SetColor(Color color);
		TrackInstance GetTrackInstance(const char* trackName);
		INT_PROPERTY(CycleCount, __get_CycleCount, __set_CycleCount, 0x5C);
		Color GetAdditiveColor(); // 0x6C-0x78，加色模式
		void SetAdditiveColor(Color color);
		T_PROPERTY(BOOLEAN, DrawAdditiveColor, __get_DrawAdditiveColor, __set_DrawAdditiveColor, 0x7C);
		Color GetOverlayColor(); // 0x80-0x8C，覆盖模式
		void SetOverlayColor(Color color);
		T_PROPERTY(BOOLEAN, DrawOverlayColor, __get_DrawOverlayColor, __set_DrawOverlayColor, 0x90);
		T_PROPERTY(PaintState::PaintState, Paint, __get_Paint, __set_Paint, 0x98);
		INT_READONLY_PROPERTY(Id, __get_Id, 0x9C);
		READONLY_PROPERTY_BINDING(int, __get_Index, Id & 0xFFFF) Index;
		AttachEffect AttachTo(AttachmentID attachmentID, float OffsetX, float OffsetY);
		void Die();
		void Play(const char* TrackName, int blendType, int loopType, float rate);
		void AssignRenderGroupToPrefix(byte RenderGroup, const char* TrackName);
		/// @brief 设置指定动画轨道在绘制时的分组。通常情况下，分组为 -1 时表示隐藏该轨道。
		/// @param trackName 执行的动作轨道名称。
		/// @param renderGroup 分组大小。
		void AssignRenderGroupToTrack(const char* trackName, byte renderGroup);
		int FindTrackIndex(const char* trackName);

		/// @brief 令动画部件执行 trackName 动作。
		/// @param trackName 执行的动作轨道名称。
		void SetFramesForLayer(const char* theTrackName);
		void SetImageOverride(const char* theTrackName, Image theImage);
	};
	class Attachment : public BaseClass
	{
	public:
		Attachment(int idoraddress);
		// TODO: check whether this function works properly.
		PVZ::Animation GetAnimation();
		INT_READONLY_PROPERTY(Id, __get_Id, 0x308);
	};
	class AttachmentID : public BaseClass
	{
	public:
		AttachmentID(DWORD address) : BaseClass(address) {};
	};
	class TrackInstance : public BaseClass
	{
	public:
		TrackInstance(int idoraddress);
		AttachmentID GetAttachmentID();
		Attachment GetAttachment();
	};
	class Lawn : public BaseClass
	{
	public:
		Lawn(int baseaddress);
		LawnType::LawnType GetGridType(int row, int column);
		void SetGridType(int row, int column, LawnType::LawnType type);
		RouteType::RouteType GetRouteType(int route);
		void SetRouteType(int route, RouteType::RouteType type);
		bool Plantable(int row, int column, SeedType::SeedType type);
	};
	class Icetrace : public BaseClass
	{
	public:
		Icetrace(int baseaddress);
		int GetX(int route);
		void SetX(int route, int x);
		int GetDisappearCountdown(int route);
		void SetDisappearCountdown(int route, int cs);
	};
	class Wave : public BaseClass
	{
	public:
		Wave(int baseaddress);
		READONLY_PROPERTY(int, __get_Count) Count;
		void GetAll(ZombieType::ZombieType* ztypes);
		void SetAll(ZombieType::ZombieType* ztypes, size_t length);
		ZombieType::ZombieType Get(int index);
		void Set(int index, ZombieType::ZombieType ztype);
		void Del(int index);
		void Add(ZombieType::ZombieType ztype);
		void AddAll(ZombieType::ZombieType* ztypes, int length);
	};
	// 鼠标对象(控制层面的鼠标)
	class Mouse : public BaseClass
	{
	public:
		Mouse(int baseaddress) : BaseClass(baseaddress) {};
		T_READONLY_PROPERTY(BOOLEAN, InGameArea, __get_InGameArea, 0xDC);
		INT_PROPERTY(X, __get_X, __set_X, 0xE0);
		INT_PROPERTY(Y, __get_Y, __set_Y, 0xE4);
		T_READONLY_PROPERTY(MouseClickState::MouseClickState, ClickState, __get_ClickState, 0xE8);
		void WMClick(short x, short y);
		void GameClick(int x, int y);
		void MoveTo(int x, int y);
	};
	class Zombie : public GameObject
	{
	public:
		Zombie(int indexoraddress);
		static const DWORD MemSize = 0x15C;
		/*调用该函数后，对应的 GetAll()、基类的构造函数都会失效。
		因此，请在派生类中调用这个函数，并且为派生类单独撰写新的构造函数和 GetAll() 。
		另外，调用该函数后，新生成的存档与原版存档不兼容，请注意清理。 */
		static void SetMemSize(int NewSize, int NewCount);

		struct AccessoriesType1
		{
			HelmType::HelmType Type;
			int Hp;
			int MaxHp;
		};
		struct AccessoriesType2
		{
			ShieldType::ShieldType Type;
			int Hp;
			int MaxHp;
		};

		T_PROPERTY(ZombieType::ZombieType, Type, __get_Type, __set_Type, 0x24);
		T_PROPERTY(ZombieState::ZombieState, State, __get_State, __set_State, 0x28);
		T_PROPERTY(FLOAT, X, __get_X, __set_X, 0x2C);
		T_PROPERTY(FLOAT, Y, __get_Y, __set_Y, 0x30);
		PROPERTY(FLOAT, __get_Speed, SetSpeed) Speed;
		T_READONLY_PROPERTY(BOOLEAN, ShowingTongue, __get_ShowingTongue, 0x50);
		T_READONLY_PROPERTY(BOOLEAN, Eating, __get_Eating, 0x51);
		void Light(int cs = 100);
		INT_READONLY_PROPERTY(ExistedTime, __get_ExistedTime, 0x60);
		INT_READONLY_PROPERTY(ZombieHeight, __get_ZombieHeight, 0x64);
		INT_PROPERTY(AttributeCountdown, __get_AttributeCountdown, __set_AttributeCountdown, 0x68);
		INT_PROPERTY(DisappearCountdown, __get_DisappearCountdown, __set_DisappearCountdown, 0x74);
		INT_PROPERTY(BungeeColumn, __get_BungeeColumn, __set_BungeeColumn, 0x80);
		T_PROPERTY(FLOAT, Height, __get_Height, __set_Height, 0x84);
		void GetCollision(CollisionBox* collbox);
		void SetCollision(CollisionBox* collbox);
		/// @brief 获取僵尸的基础攻击判定范围。
		/// @param collbox 攻击判定范围的存放位置。其中 X 和 Y 为相应坐标的偏移量。
		void GetAttackCollision(CollisionBox* collbox);
		/// @brief 设置僵尸的基础攻击判定范围。
		/// @param collbox 攻击判定范围的指针。其中 X 和 Y 为相应坐标的偏移量。
		void SetAttackCollision(CollisionBox* collbox);
		INT_PROPERTY(DecelerateCountdown, __get_DecelerateCountdown, __set_DecelerateCountdown, 0xAC);
		INT_PROPERTY(FixedCountdown, __get_FixedCountdown, __set_FixedCountdown, 0xB0);
		INT_PROPERTY(FrozenCountdown, __get_FrozenCountdown, __set_FrozenCountdown, 0xB4);
		T_PROPERTY(BOOLEAN, Hypnotized, __get_Hypnotized, __set_Hypnotized, 0xB8);
		T_PROPERTY(BOOLEAN, Blowaway, __get_Blowaway, __set_Blowaway, 0xB9);
		T_PROPERTY(BOOLEAN, NotDying, __get_NotDying, __set_NotDying, 0xBA);
		T_READONLY_PROPERTY(BOOLEAN, HandBroken, __get_HandBroken, 0xBB);
		T_PROPERTY(BOOLEAN, SthinHandOrYetiLeft, __get_SthinHandOrYetiLeft, __set_SthinHandOrYetiLeft, 0xBC);
		T_PROPERTY(BOOLEAN, InWater, __get_InWater, __set_InWater, 0xBD);
		T_PROPERTY(BOOLEAN, GarlicBited, __get_GarlicBited, __set_GarlicBited, 0xBF);
		T_PROPERTY(HelmType::HelmType, HelmType, __get_HelmType, __set_HelmType,		0x0C4);
		INT_PROPERTY(BodyHealth,		__get_BodyHealth,		__set_BodyHealth,		0x0C8);
		INT_PROPERTY(BodyMaxHealth,		__get_BodyMaxHealth,	__set_BodyMaxHealth,	0x0CC);
		INT_PROPERTY(HelmHealth,		__get_HelmHealth,		__set_HelmHealth,		0x0D0);
		INT_PROPERTY(HelmMaxHealth,		__get_HelmMaxHealth,	__set_HelmMaxHealth,	0x0D4);
		T_PROPERTY(ShieldType::ShieldType, ShieldType, __get_ShieldType, __set_ShieldType, 0x0D8);
		INT_PROPERTY(ShieldHealth,		__get_ShieldHealth,		__set_ShieldHealth,		0x0DC);
		INT_PROPERTY(ShieldMaxHealth,	__get_ShieldMaxHealth,	__set_ShieldMaxHealth,	0x0E0);
		INT_PROPERTY(FlyingHealth, __get_FlyingHealth, __set_FlyingHealth, 0x0E4);
		INT_PROPERTY(FlyingMaxHealth, __get_FlyingMaxHealth, __set_FlyingMaxHealth, 0xE8);
		T_PROPERTY(BOOLEAN, NotExist, __get_NotExist, __set_NotExist, 0xEC);
		PVZ::Animation GetAnimation();
		T_PROPERTY(FLOAT, Size, __get_Size, __set_Size, 0x11C);
		//临时变量
		INT_PROPERTY(Temp, __get_Temp, __set_Temp, 0x12C);
		Animation GetSpecialHeadAnimation();
		void SetSpecialHeadAnimation(Animation anim);
		INT_READONLY_PROPERTY(Id, __get_Id, 0x158);
		READONLY_PROPERTY_BINDING(int, __get_Index, Id & 0xFFFF) Index;
		void Hit(int damage, DamageFlags flags = DAMAGEF_NONE);
		// 忽略所有护甲只命中本体
		void HitBody(int damage, DamageFlags flags = DAMAGEF_NONE);
		void Blast();
		void Butter(int countdown);
		//减速僵尸，无法减速默认免疫减速的僵尸。
		void Decelerate(int countdown);
		//冻结僵尸，无法冻结默认免疫冻结的僵尸。
		//不造成冻结伤害，不影响减速时长。
		void Froze(int countdown);
		void Hypnotize();
		void Remove();
		void RemoveWithLoot();
		//animPlayArg(APA_XXXXXX)
		void SetAnimation(LPCSTR animName, byte animPlayArg);
		void EquipBucket(int shield = 1100);
		void EquipCone(int shield = 370);
		void ReanimShowPrefix(const char* trackPrefix, int renderGroup);

		bool canDecelerate();
		bool canFroze();
		/// @brief 判定僵尸是否能被指定索敌类型搜寻到。
		/// @param range 索敌类型。
		/// @param usepvzfunc 是否调用 pvz 内部函数。默认为 true。
		/// @return 是否能被搜寻到。
		bool EffectedBy(DamageRangeFlags range, bool usepvzfunc = true);

		/// @brief 获取僵尸的实际可攻击范围。
		/// @return 僵尸的实际攻击范围
		Rect GetActualAttackRect();
		/// @brief 获取僵尸的实际受击范围。
		/// @return 僵尸的实际受击范围
		Rect GetActualRect();

		/// @brief 设置是否显示铁门僵尸的手臂。
		/// @param shown 是否显示，默认为 true
		void ShowDoorArms(bool shown = true);

		/// @deprecated
		void GetBodyHp(int* hp, int* maxhp);
		/// @deprecated
		void SetBodyHp(int hp, int maxhp);
		/// @deprecated
		AccessoriesType1 GetAccessoriesType1();
		/// @deprecated
		void SetAccessoriesType1(AccessoriesType1 acctype1);
		/// @deprecated
		AccessoriesType2 GetAccessoriesType2();
		/// @deprecated
		void SetAccessoriesType2(AccessoriesType2 acctype2);
	};
	class Projectile : public GameObject
	{
	public:
		static const DWORD MemSize = 0x94;
		Projectile(int indexoraddress);
		T_PROPERTY(FLOAT, X, __get_X, __set_X, 0x30);
		T_PROPERTY(FLOAT, Y, __get_Y, __set_Y, 0x34);
		T_PROPERTY(FLOAT, Height, __get_Height, __set_Height, 0x38);
		T_PROPERTY(FLOAT, XSpeed, __get_XSpeed, __set_XSpeed, 0x3C);
		T_PROPERTY(FLOAT, YSpeed, __get_YSpeed, __set_YSpeed, 0x40);
		T_PROPERTY(FLOAT, HeightSpeed, __get_HeightSpeed, __set_HeightSpeed, 0x44);
		T_PROPERTY(FLOAT, HeightAcceleration, __get_HeightAcceleration, __set_HeightAcceleration, 0x48);
		T_PROPERTY(FLOAT, ShadowY, __get_ShadowY, __ShadowY, 0x4C);
		T_PROPERTY(BOOLEAN, NotExist, __get_NotExist, __set_NotExist, 0x50);
		T_PROPERTY(MotionType::MotionType, Motion, __get_Motion, __set_Motion, 0x58);
		T_PROPERTY(ProjectileType::ProjectileType, Type, __get_Type, __set_Type, 0x5C);
		INT_READONLY_PROPERTY(ExistedTime, __get_ExistedTime, 0x60);
		T_PROPERTY(FLOAT, RotationAngle, __get_RotationAngle, __set_RotationAngle, 0x68);
		T_PROPERTY(FLOAT, RotationSpeed, __get_RotationSpeed, __set_RotationSpeed, 0x6C);
		INT_PROPERTY(DamageAbility, __get_DamageAbility, __set_DamageAbility, 0x74);
		INT_PROPERTY(TracktargetId, __get_TracktargetId, __set_TracktargetId, 0x88);
		INT_READONLY_PROPERTY(Id, __get_Id, 0x90);
		READONLY_PROPERTY_BINDING(int, __get_Index, Id & 0xFFFF) Index;
		void OnFire();
		void Remove();
	};
	class Plant : public GameObject
	{
	public:
		static const int MemSize = 0x14C;
		Plant(int indexoraddress);
		/*调用该函数后，对应的 GetAll()、基类的构造函数都会失效。
		因此，请在派生类中调用这个函数，并且为派生类单独撰写新的构造函数和 GetAll() 。
		另外，调用该函数后，新生成的存档与原版存档不兼容，请注意清理。 */
		static void SetMemSize(int NewSize, int NewCount);
		T_PROPERTY(SeedType::SeedType, Type, __get_Type, __set_Type, 0x24);
		INT_PROPERTY(Column, __get_Column, __set_Column, 0x28);
		T_PROPERTY(PlantState::PlantState, State, __get_State, __set_State, 0x3C);
		INT_PROPERTY(Hp, __get_Hp, __set_Hp, 0x40);
		INT_PROPERTY(MaxHp, __get_MaxHp, __set_MaxHp, 0x44);
		T_PROPERTY(BOOLEAN, Aggressive, __get_Aggressive, __set_Aggressive, 0x48);
		INT_PROPERTY(BloverDisappearCountdown, __get_BloverDisappearCountdown, __set_BloverDisappearCountdown, 0x4C);
		INT_PROPERTY(EffectiveCountdown, __get_EffectiveCountdown, __set_EffectiveCountdown, 0x50);
		INT_PROPERTY(AttributeCountdown, __get_AttributeCountdown, __set_AttributeCountdown, 0x54);
		INT_PROPERTY(ShootOrProductCountdown, __get_ShootOrProductCountdown, __set_ShootOrProductCountdown, 0x58);
		INT_PROPERTY(ShootOrProductInterval, __get_ShootOrProductInterval, __set_ShootOrProductInterval, 0x5C);
		INT_PROPERTY(mTargetX, __get_mTargetX, __set_mTargetX, 0x88);
		INT_PROPERTY(mTargetY, __get_mTargetY, __set_mTargetY, 0x8C);
		INT_PROPERTY(ShootingCountdown, __get_ShootingCountdown, __set_ShootingCountdown, 0x90);
		PVZ::Animation GetAnimationPart1();
		PVZ::Animation GetAnimationPart2();
		PVZ::Animation GetAnimationPart3();
		PVZ::Animation GetAnimationPart4();
		PVZ::Animation GetAnimationEyeBlink();
		PVZ::Animation GetAnimationPotatoGlow();
		PVZ::Animation GetAnimationSleep();
		void Light(int cs = 100);
		void Flash(int cs = 100);
		T_PROPERTY(FLOAT, ImageXOffset, __get_ImageXOffset, __set_ImageXOffset, 0xC0);
		T_PROPERTY(FLOAT, ImageYOffset, __get_ImageYOffset, __set_ImageYOffset, 0xC4);
		T_PROPERTY(DWORD, mTargetZombieID, __get_mTargetZombieID, __set_mTargetZombieID, 0x12C);
		INT_PROPERTY(mWakeUpCounter, __get_mWakeUpCounter, __set_mWakeUpCounter, 0x130);
		INT_PROPERTY(mOnBungee, __get_mOnBungee, __set_mOnBungee, 0x134);
		T_PROPERTY(BOOLEAN, NotExist, __get_NotExist, __set_NotExist, 0x141);
		T_PROPERTY(BOOLEAN, Squash, __get_Squash, __set_Squash, 0x142);
		T_READONLY_PROPERTY(BOOLEAN, Sleeping, __get_Sleeping, 0x143);
		void SetSleeping(bool sleeping);
		INT_READONLY_PROPERTY(Id, __get_Id, 0x148);
		READONLY_PROPERTY_BINDING(int, __get_Index, Id & 0xFFFF) Index;
		void CreateEffect();
		// 将植物定身为纸板（同 IZ），会让土豆地雷直接出土。
		void SetStatic();
		void Smash();
		int CalcLayer();
		void MoveTo(int row, int column);
		void Remove();
		PVZ::Projectile Shoot(int targetid = -1);
		PVZ::Projectile Shoot(MotionType::MotionType motiontype = MotionType::None, int targetid = -1, bool special = false);
		//animPlayArg(APA_XXXXXX)
		void SetAnimation(LPCSTR animName, byte animPlayArg, int imagespeed);
		class MagnetItem
		{
			int BaseAddress;
		public:
			MagnetItem(int address);
			T_PROPERTY(FLOAT, X, __get_X, __set_X, 0);
			T_PROPERTY(FLOAT, Y, __get_Y, __set_Y, 4);
			T_PROPERTY(FLOAT, DestOffsetX, __get_DestOffsetX, __set_DestOffsetX, 8);
			T_PROPERTY(FLOAT, DestOffsetY, __get_DestOffsetY, __set_DestOffsetY, 0xC);
			T_PROPERTY(MagnetItemType::MagnetItemType, Type, __get_Type, __set_Type, 0x10);
		};
		MagnetItem GetMagnetItem(int num);
	};
	class GardenPlant : public BaseClass
	{
	public:
		GardenPlant(int address);
		T_PROPERTY(SeedType::SeedType, Type, __get_Type, __set_Type, 8);
		T_PROPERTY(GardenScene::GardenScene, Location, __get_Location, __set_Location, 0xC);
		INT_PROPERTY(Column, __get_Column, __set_Column, 0x10);
		INT_PROPERTY(Row, __get_Row, __set_Row, 0x14);
		//GPD_LEFT or GPD_RIGTHT
		INT_PROPERTY(Direction, __get_Direction, __set_Direction, 0x18);
		T_PROPERTY(std::time_t, LastWateredTime, __get_LastWateredTime, __set_LastWateredTime, 0x20);
		INT_PROPERTY(Colour, __get_Colour, __set_Colour, 0x28);
		T_PROPERTY(GardenPlantState::GardenPlantState, State, __get_State, __set_State, 0x2C);
		INT_PROPERTY(WateredCount, __get_WateredCount, __set_WateredCount, 0x30);
		INT_PROPERTY(LevelUpWaterNeed, __get_LevelUpWaterNeed, __set_LevelUpWaterNeed, 0x34);
		T_PROPERTY(GardenPlantNeed::GardenPlantNeed, LevelUpNeed, __get_LevelUpNeed, __set_LevelUpNeed, 0x38);
		T_PROPERTY(std::time_t, LastGoldTime, __get_LastGoldTime, __set_LastGoldTime, 0x40);
		T_PROPERTY(std::time_t, LastGrowthTime, __get_LastGrowthTime, __set_LastGrowthTime, 0x48);
	};
	class Coin : public GameObject //Item
	{
	public:
		static const DWORD MemSize = 0x0D8;
		Coin(int indexoraddress);
		INT_READONLY_PROPERTY(ImageXVariation, __get_ImageXVariation, 8);
		INT_READONLY_PROPERTY(ImageYVariation, __get_ImageYVariation, 0xC);
		void GetCollision(CollisionBox* collbox);
		void SetCollision(CollisionBox* collbox);
		T_PROPERTY(FLOAT, X, __get_X, __set_X, 0x24);
		T_PROPERTY(FLOAT, Y, __get_Y, __set_Y, 0x28);
		T_PROPERTY(FLOAT, Size, __get_Size, __set_Size, 0x34);
		T_PROPERTY(BOOLEAN, NotExist, __get_NotExist, __set_NotExist, 0x38);
		INT_READONLY_PROPERTY(ExistedTime, __get_ExistedTime, 0x4C);
		T_PROPERTY(BOOLEAN, Collected, __get_Collected, __set_Collected, 0x50);
		INT_PROPERTY(DisappearCounter, __get_DisappearCounter, __set_DisappearCounter, 0x54);
		T_PROPERTY(CoinType::CoinType, Type, __get_Type, __set_Type, 0x58);
		T_PROPERTY(CoinMotionType::CoinMotionType, Motion, __get_Motion, __set_Motion, 0x5C);
		PVZ::Attachment GetAttachment();
		T_PROPERTY(SeedType::SeedType, ContentCard, __get_ContentCard, __set_ContentCard, 0x68);
		PVZ::GardenPlant GetGardenPlant();
		T_PROPERTY(BOOLEAN, HasHalo, __get_HasHalo, __set_HasHalo, 0xC8);
		INT_READONLY_PROPERTY(Id, __get_Id, 0xD0);
		READONLY_PROPERTY_BINDING(int, __get_Index, Id & 0xFFFF) Index;
		void Collect();
		void Die();
	};
	class Lawnmover : public BaseClass
	{
	public:
		static const DWORD MemSize = 0x48;
		Lawnmover(int indexoraddress);
		int GetBaseAddress();
		INT_PROPERTY(X, __get_X, __set_X, 8);
		INT_PROPERTY(Y, __get_Y, __set_Y, 0xC);
		INT_PROPERTY(Layer, __get_Layer, __set_Layer, 0x10);
		INT_PROPERTY(Row, __get_Row, __set_Row, 0x14);
		PVZ::Animation GetAnimation();
		T_PROPERTY(LawnmoverState::LawnmoverState, State, __get_State, __set_State, 0x2C);
		T_PROPERTY(BOOLEAN, NotExist, __get_NotExist, __set_NotExist, 0x30);
		T_PROPERTY(BOOLEAN, Visible, __get_Visible, __set_Visible, 0x31);
		T_PROPERTY(LawnmoverType::LawnmoverType, Type, __get_Type, __set_Type, 0x34);
		T_PROPERTY(FLOAT, YOffset, __get_YOffset, __set_YOffset, 0x38);
		INT_READONLY_PROPERTY(Id, __get_Id, 0x44);
		READONLY_PROPERTY_BINDING(int, __get_Index, Id & 0xFFFF) Index;
		void Die();
	};
	using LawnMower = Lawnmover;
	class Griditem : public BaseClass
	{
	public:
		static const DWORD MemSize = 0x0EC;
		static const GriditemType::GriditemType ItemType = GriditemType::None;
		Griditem(int indexoraddress);
		PVZ::Board GetBoard();
		T_PROPERTY(GriditemType::GriditemType, Type, __get_Type, __set_Type, 0x8);
		T_PROPERTY(GriditemState::GriditemState, State, __get_State, __set_State, 0xC);
		INT_PROPERTY(Column, __get_Column, __set_Column, 0x10);
		INT_PROPERTY(Row, __get_Row, __set_Row, 0x14);
		INT_PROPERTY(Layer, __get_Layer, __set_Layer, 0x1C);
		T_PROPERTY(BOOLEAN, NotExist, __get_NotExist, __set_NotExist, 0x20);
		INT_READONLY_PROPERTY(Id, __get_Id, 0xE8);
		READONLY_PROPERTY_BINDING(int, __get_Index, Id & 0xFFFF) Index;
		void Remove();
	};
	class Grave :public PVZ::Griditem
	{
	public:
		static const GriditemType::GriditemType ItemType = GriditemType::Grave;
		Grave(int indexoraddress) :Griditem(indexoraddress) {};
		INT_PROPERTY(AppearedValue, __get_AppearedValue, __set_AppearedValue, 0x18);
	};
	class Crater :public PVZ::Griditem
	{
	public:
		static const GriditemType::GriditemType ItemType = GriditemType::Crater;
		Crater(int indexoraddress) :Griditem(indexoraddress) {};
		INT_PROPERTY(DisappearCountdown, __get_DisappearCountdown, __set_DisappearCountdown, 0x18);
	};
	class AquariumBrain :public PVZ::Griditem
	{
	public:
		static const GriditemType::GriditemType ItemType = GriditemType::AquariumBrain;
		AquariumBrain(int indexoraddress) :Griditem(indexoraddress) {};
		T_PROPERTY(FLOAT, X, __get_X, __set_X, 0x24);
		T_PROPERTY(FLOAT, Y, __get_Y, __set_Y, 0x28);
	};
	class Snail :public PVZ::Griditem
	{
	public:
		static const GriditemType::GriditemType ItemType = GriditemType::Snail;
		Snail(int indexoraddress) :Griditem(indexoraddress) {};
		T_PROPERTY(FLOAT, X, __get_X, __set_X, 0x24);
		T_PROPERTY(FLOAT, Y, __get_Y, __set_Y, 0x28);
		T_PROPERTY(FLOAT, TargetX, __get_TargetX, __set_TargetX, 0x2C);
		T_PROPERTY(FLOAT, TargetY, __get_TargetY, __set_TargetY, 0x30);
	};
	class Vase :public PVZ::Griditem
	{
	public:
		static const GriditemType::GriditemType ItemType = GriditemType::Vase;
		Vase(int indexoraddress) :Griditem(indexoraddress) {};
		T_PROPERTY(VaseSkin::VaseSkin, Skin, __get_Skin, __set_Skin, 0xC);
		T_PROPERTY(ZombieType::ZombieType, ContentZombie, __get_ContentZombie, __set_ContentZombie, 0x3C);
		T_PROPERTY(SeedType::SeedType, ContentPlant, __get_ContentPlant, __set_ContentPlant, 0x40);
		T_PROPERTY(VaseContent::VaseContent, Content, __get_Content, __set_Content, 0x44);
		T_READONLY_PROPERTY(BOOLEAN, MouseEnter, __get_MouseEnter, 0x48);
		INT_PROPERTY(TransparentCountDown, __get_TransparentCountDown, __set_TransparentCountDown, 0x4C);
		INT_PROPERTY(Sun, __get_Sun, __set_Sun, 0x50);
		void Open();
	};
	class IZBrain :public PVZ::Griditem
	{
	public:
		static const GriditemType::GriditemType ItemType = GriditemType::IZBrain;
		IZBrain(int indexoraddress) :Griditem(indexoraddress) {};
		INT_PROPERTY(Hp, __get_Hp, __set_Hp, 0x18);
		T_PROPERTY(FLOAT, X, __get_X, __set_X, 0x24);
		T_PROPERTY(FLOAT, Y, __get_Y, __set_Y, 0x28);
	};
	// 在非传送门关卡中只有画面效果不会触发传送
	// 提供几个方法用于模拟传送门关卡中的传送效果
	class Portal :public PVZ::Griditem
	{
	public:
		Portal(int indexoraddress) :Griditem(indexoraddress) {};
		void Close();
		// 判定僵尸是否进入这个传送门
		bool isZombieIn(std::shared_ptr<PVZ::Zombie> zombie);
		// 获取僵尸从这个传送门出来时的X坐标
		int getZombieOutX();
		// 判定子弹是否进入这个传送门
		bool isProjectileIn(std::shared_ptr<PVZ::Projectile> projectile);
		// 获取子弹从这个传送门射出时的X坐标
		int getProjectileOutX();
	};
	class CirclePortal : public Portal
	{
	public:
		static const GriditemType::GriditemType ItemType = GriditemType::PortalBlue;
		CirclePortal(int indexoraddress) : Portal(indexoraddress) {};
	};
	class SquarePortal : public Portal
	{
	public:
		static const GriditemType::GriditemType ItemType = GriditemType::PortalYellow;
		SquarePortal(int indexoraddress) : Portal(indexoraddress) {};
	};
	class MousePointer : public GameObject//+138
	{
	public:
		MousePointer(int address);
		INT_PROPERTY(CardIndex, __get_CardIndex, __set_CardIndex, 0x24);
		T_PROPERTY(SeedType::SeedType, ContentCard, __get_ContentCard, __set_ContentCard, 0x28);
		T_PROPERTY(SeedType::SeedType, ContentCardImitative, __get_ContentCardImitative, __set_ContentCardImitative, 0x2C);
		T_PROPERTY(MouseType::MouseType, Type, __get_Type, __set_Type, 0x30);
		INT_PROPERTY(CoinID, __get_CoinID, __set_CoinID, 0x34);
		INT_PROPERTY(GlovePlantID, __get_GlovePlantID, __set_GlovePlantID, 0x38);
		INT_PROPERTY(ImitativePlantID, __get_ImitativePlantID, __set_ImitativePlantID, 0x3C);
		INT_PROPERTY(CobCannonID, __get_CobCannonID, __set_CobCannonID, 0x40);
		INT_PROPERTY(HammerDownCount, __get_HammerDownCount, __set_HammerDownCount, 0x44);
		PVZ::Animation GetAnimation();
		READONLY_PROPERTY_BINDING(int, __get_Row, Memory::ReadPointer(PVZBASEADDRESS + 0x13C, 0x28)) Row;
		READONLY_PROPERTY_BINDING(int, __get_Column, Memory::ReadPointer(PVZBASEADDRESS + 0x13C, 0x24)) Column;
	};
	class Caption : public BaseClass
	{
	public:
		Caption(int address);
		int GetBaseAddress();
		void GetText(char str[]);//str[0x80]
		void SetText(const char str[]);//str[0x80]
		INT_PROPERTY(DisappearCountdown, __get_DisappearCountdown, __set_DisappearCountdown, 0x88);
		T_PROPERTY(CaptionStyle::CaptionStyle, Style, __get_CaptionStyle, __set_CaptionStyle, 0x8C);
	};
	using Advice = Caption;
	class CardSlot : public BaseClass
	{
	public:
		CardSlot(int address);
		int GetBaseAddress();
		INT_PROPERTY(X, __get_X, __set_X, 8);
		INT_PROPERTY(Y, __get_Y, __set_Y, 0xC);
		INT_PROPERTY(CollisionLength, __get_CollisionLength, __set_CollisionLength, 0x10);
		T_PROPERTY(BOOLEAN, Visible, __get_Visible, __set_Visible, 0x18);
		PROPERTY(int, __get_CardsCount, SetCardsCount) CardsCount;
		class SeedCard : BaseClass
		{
		public:
			SeedCard(int address);
			int GetBaseAddress();
			INT_PROPERTY(X, __get_X, __set_X, 0x8);
			INT_PROPERTY(Y, __get_Y, __set_Y, 0xC);
			void GetCollision(CollisionBox* collbox);
			void SetCollision(CollisionBox* collbox);
			T_PROPERTY(BOOLEAN, Visible, __get_Visible, __set_Visible, 0x18);
			INT_PROPERTY(CoolDown, __get_CoolDown, __set_CoolDown, 0x24); // 已冷却时间，从0开始
			INT_PROPERTY(CoolDownInterval, __get_CoolDownInterval, __set_CoolDownInterval, 0x28);
			INT_READONLY_PROPERTY(Index, __get_Index, 0x2C);
			INT_PROPERTY(XInConveyorBelt, __get_XInConveyorBelt, __set_XInConveyorBelt, 0x30);
			T_PROPERTY(SeedType::SeedType, ContentCard, __get_ContentCard, __set_ContentCard, 0x34);
			T_PROPERTY(SeedType::SeedType, ContentCardImitative, __get_ContentCardImitative, __set_ContentCardImitative, 0x38);
			INT_PROPERTY(SlotCountdown, __get_SlotCountdown, __set_SlotCountdown, 0x3C);
			T_PROPERTY(SeedType::SeedType, SlotType, __get_SlotType, __set_SlotType, 0x40);
			T_PROPERTY(FLOAT, SlotPosition, __get_SlotPosition, __set_SlotPosition, 0x44);
			T_PROPERTY(BOOLEAN, Enable, __get_Enable, __set_Enable, 0x48); // 该卡槽是否可点击
			T_PROPERTY(BOOLEAN, Active, __get_Active, __set_Active, 0x49); // 该卡槽是否正在CD
			INT_PROPERTY(UsageCount, __get_UsageCount, __set_UsageCount, 0x4C);
			// 该卡槽进入CD，持续时间为-1则为该卡槽的默认时间
			void EnterCoolDown(int duration = -1);
			void SetPacketType(SeedType::SeedType type, SeedType::SeedType imitater_type = SeedType::None);
		};
		PVZ::CardSlot::SeedCard GetCard(int index);
		using SeedPacket = SeedCard;
	};
	using SeedBank = CardSlot;
	class Challenge : public BaseClass
	{
	public:
		Challenge(int address);
		static const int MemSize = 0x0BC;
		/*请在派生类中调用这个函数。
		另外，调用该函数后，新生成的存档与原版存档不兼容，请注意清理。
		@return 是否修改成功。
		*/
		static bool SetMemSize(int NewSize);

		T_READONLY_PROPERTY(BOOLEAN, DragingPlant, __get_DragingPlant, 8);
		INT_READONLY_PROPERTY(DragingX, __get_DragingX, 0xC);
		INT_READONLY_PROPERTY(DragingY, __get_DragingY, 0x10);
		BOOLEAN HaveCrater(int row, int column);
		void SetCrater(int row, int column, BOOLEAN b);
		T_PROPERTY(BOOLEAN, UpgradedRepeater, __get_UpgradedRepeater, __set_UpgradedRepeater, 0x4A);
		T_PROPERTY(BOOLEAN, UpgradedFumeshroom, __get_UpgradedFumeshroom, __set_UpgradedFumeshroom, 0x4B);
		T_PROPERTY(BOOLEAN, UpgradedTallnut, __get_UpgradedTallnut, __set_UpgradedTallnut, 0x4C);
		INT_PROPERTY(BeghouledMatchesThisMove, __get_BeghouledMatchesThisMove, __set_BeghouledMatchesThisMove, 0x50);
		T_PROPERTY(ChallengeState::ChallengeState, State, __get_State, __set_State, 0x54);
		INT_PROPERTY(AttributeCountdown, __get_AttributeCountdown, __set_AttributeCountdown, 0x58);
		INT_PROPERTY(ConveyorCountdown, __get_ConveyorCountdown, __set_ConveyorCountdown, 0x5C);
		INT_PROPERTY(LevelProcess, __get_LevelProcess, __set_LevelProcess, 0x60);
		T_PROPERTY(BOOLEAN, ShowBowlingLine, __get_ShowBowlingLine, __set_ShowBowlingLine, 0x64);
		T_PROPERTY(SeedType::SeedType, ConveyorLastCard, __get_ConveyorLastCard, __set_ConveyorLastCard, 0x68);
		INT_PROPERTY(Round, __get_Round, __set_Round, 0x6C);
		INT_PROPERTY(SlotMachineRollCount, __get_SlotMachineRollCount, __set_SlotMachineRollCount, 0x70);
		INT_PROPERTY(ChallengeGridX, __get_ChallengeGridX, __set_ChallengeGridX, 0x0A8);
		INT_PROPERTY(ChallengeGridY, __get_ChallengeGridY, __set_ChallengeGridY, 0x0AC);
		INT_READONLY_PROPERTY(VaseCount, __get_VaseCount, 0x0B0);
		INT_PROPERTY(RainCounter, __get_RainCounter, __set_RainCounter, 0x0B4);
		INT_READONLY_PROPERTY(TreeOfWisdomTalkIndex, __get_TreeOfWisdomTalkIndex, 0x0B8);

		void IZSquishBrain(IZBrain brain);
	};
	using Miscellaneous = Challenge;
	class SaveData : public BaseClass
	{
	public:
		SaveData(int baseaddress) : BaseClass(baseaddress) {};
		void GetPVZUserName(char str[]);//str[12]
		INT_READONLY_PROPERTY(UserSwitchCount, __get_UserSwitchCount, 0x1C);
		INT_READONLY_PROPERTY(UserIndex, __get_UserIndex, 0x20);
		INT_PROPERTY(AdventureLevel, __get_AdventureLevel, __set_AdventureLevel, 0x24);
		INT_PROPERTY(Money, __get_Money, __set_Money, 0x28);
		INT_PROPERTY(AdventureFinishCount, __get_AdventureFinishCount, __set_AdventureFinishCount, 0x2C);
		// 获取当前用户是否购买了指定物品，或者指定物品的剩余数量。
		int GetPurchase(StoreItem::StoreItem item);
		// 设定当前用户是否购买了指定物品，或者指定物品的剩余数量。
		void SetPurchase(StoreItem::StoreItem item, int val);
		// 获取当前用户通过某一关卡的次数，或者最高波数的通关记录。
		int GetChallengeRecord(PVZLevel::PVZLevel mode);
		// 设定当前用户通过某一关卡的次数，或者最高波数的通关记录。
		void SetChallengeRecord(PVZLevel::PVZLevel mode, int val);
		INT_PROPERTY(TreeHight, __get_TreeHight, __set_TreeHight, 0xF4);
		BOOLEAN HavePurpleCard(SeedType::SeedType purplecard);
		T_PROPERTY(BOOLEAN, HaveImitater, __get_HaveImitater, __set_HaveImitater, 0x1E0);
		T_PROPERTY(BOOLEAN, HaveGoldenWatering, __get_HaveGoldenWatering, __set_HaveGoldenWatering, 0x1F4);
		INT_PROPERTY(Fertilizer, __get_Fertilizer, __set_Fertilizer, 0x1F8);//-1000
		INT_PROPERTY(BugSpray, __get_BugSpray, __set_BugSpray, 0x1FC);//-1000
		T_PROPERTY(BOOLEAN, HaveMusicBox, __get_HaveMusicBox, __set_HaveMusicBox, 0x200);
		T_PROPERTY(BOOLEAN, HaveGardeningGlove, __get_HaveGardeningGlove, __set_HaveGardeningGlove, 0x204);
		T_PROPERTY(BOOLEAN, HaveMushroomGarden, __get_HaveMushroomGarden, __set_HaveMushroomGarden, 0x208);
		T_PROPERTY(BOOLEAN, HaveWheelBarrow, __get_HaveWheelBarrow, __set_HaveWheelBarrow, 0x20C);
		T_PROPERTY(BOOLEAN, HaveSnail, __get_HaveSnail, __set_HaveSnail, 0x210);
		INT_PROPERTY(CardSlotNum, __get_CardSlotNum, __set_CardSlotNum, 0x214);
		T_PROPERTY(BOOLEAN, HavePoolCleaner, __get_HavePoolCleaner, __set_HavePoolCleaner, 0x218);
		T_PROPERTY(BOOLEAN, HaveRoofCleaner, __get_HaveRoofCleaner, __set_HaveRoofCleaner, 0x21C);
		INT_PROPERTY(LeftRakeCount, __get_LeftRakeCount, __set_LeftRakeCount, 0x220);
		T_PROPERTY(BOOLEAN, HaveAquarium, __get_HaveAquarium, __set_HaveAquarium, 0x224);
		INT_PROPERTY(Chocolate, __get_Chocolate, __set_Chocolate, 0x228);//-1000
		T_PROPERTY(BOOLEAN, HaveTreeOfWisdom, __get_HaveTreeOfWisdom, __set_HaveTreeOfWisdom, 0x22C);
		INT_PROPERTY(TreeFood, __get_TreeFood, __set_TreeFood, 0x230);//-1000
		T_PROPERTY(BOOLEAN, HaveWallnutFirstAid, __get_HaveWallnutFirstAid, __set_HaveWallnutFirstAid, 0x234);
		INT_READONLY_PROPERTY(GardenPlantCount, __get_GardenPlantCount, 0x350);
		class GardenPlant : public PVZ::GardenPlant
		{
		public:
			GardenPlant(int address) : PVZ::GardenPlant(address) {};
		};
		GardenPlant GetGardenPlant(int index);
	};
	class Music : public BaseClass
	{
	public:
		Music(int address) : BaseClass(address) {};
		PROPERTY(MusicType::MusicType, __get_Type, __set_Type) Type;
		//MINGAM_ENABLE or MINGAM_DISABLE
		INT_PROPERTY(INGAMEable, __get_INGAMEable, __set_INGAMEable, 0x10);
		//MINGAME_STARTED or MINGAME_NOTSTARTED
		INT_PROPERTY(INGAMEStart, __get_INGAMEStart, __set_INGAMEStart, 0x18);
		INT_READONLY_PROPERTY(Tempo, __get_Tempo, 0x1C);
		INT_READONLY_PROPERTY(Ticks_Row, __get_Ticks_Row, 0x20);
		T_PROPERTY(INGAMEState::INGAMEState, State, __get_State, __set_State, 0x24);
		INT_PROPERTY(AttributeCountdown, __get_AttributeCountdown, __set_AttributeCountdown, 0x28);
		T_PROPERTY(INGAMEEffect::INGAMEEffect, INGAMEEffect, __get_INGAMEEffect, __set_INGAMEEffect, 0x2C);
	};
	//if anyone want a class for calling functions in bass.dll to totally control the music in game,just tell me

	class ZenGarden : public BaseClass
	{
	public:
		ZenGarden(int address);
		PVZ::Board GetBoard();
		T_PROPERTY(GardenScene::GardenScene, GardenType, __get_GardenType, __set_GardenType, 0x8);
		bool IsFull(bool consider_items);
		Snail GetSnail();
	};

	class PlantDefinition : public BaseClass
	{
	public:
		PlantDefinition(SeedType::SeedType type);
		T_READONLY_PROPERTY(SeedType::SeedType, Type, __get_Type, 0);
		T_READONLY_PROPERTY(AnimationType::AnimationType, AnimType, __get_AnimType, 8);
		INT_READONLY_PROPERTY(PacketID, __get_PacketID, 0xC);
		INT_PROPERTY(Cost, __get_Cost, __set_Cost, 0x10);
		INT_PROPERTY(Cooldown, __get_Cooldown, __set_Cooldown, 0x14);
		INT_PROPERTY(IsShooter, __get_IsShooter, __set_IsShooter, 0x18);
		INT_PROPERTY(AttackCooldown, __get_AttackCooldown, __set_AttackCooldown, 0x1C);
	};

	class ZombieDefinition : public BaseClass
	{
	public:
		ZombieDefinition(ZombieType::ZombieType type);
		T_READONLY_PROPERTY(ZombieType::ZombieType, Type, __get_Type, 0);
		T_READONLY_PROPERTY(AnimationType::AnimationType, AnimType, __get_AnimType, 4);
		INT_PROPERTY(Value, __get_Value, __set_Value, 8);
		INT_PROPERTY(StartingLevel, __get_StartingLevel, __set_StartingLevel, 0xC);
		INT_PROPERTY(FirstAllowedWave, __get_FirstAllowedWave, __set_FirstAllowedWave, 0x10);
		INT_PROPERTY(PickWeight, __get_PickWeight, __set_PickWeight, 0x14);
	};

	class ProjectileDefinition : public BaseClass
	{
	public:
		ProjectileDefinition(ProjectileType::ProjectileType type);
		T_READONLY_PROPERTY(ProjectileType::ProjectileType, Type, __get_Type, 0);
		INT_READONLY_PROPERTY(ImageRow, __get_ImageRow, 4);
		INT_PROPERTY(Damage, __get_Damage, __set_Damage, 8);
	};

	class ChallengeDefinition : public BaseClass
	{
	public:
		ChallengeDefinition(PVZLevel::PVZLevel mode);
		T_READONLY_PROPERTY(PVZLevel::PVZLevel, Mode, __get_Mode, 0);
		INT_PROPERTY(IconIndex, __get_IconIndex, __set_IconIndex, 4);
		INT_PROPERTY(Page, __get_Page, __set_Page, 8);
		INT_PROPERTY(Row, __get_Row, __set_Row, 0x0C);
		INT_PROPERTY(Column, __get_Column, __set_Column, 0x10);
		INT_PROPERTY(NamePTR, __get_NamePTR, __set_NamePTR, 0x14);
	};

#pragma endregion

#pragma region methods

	void InitImages();
	Mouse GetMouse();
	//若 BaseAddress 为 0，返回空指针
	Board GetBoard();
	SaveData GetSaveData();
	Music GetMusic();
	ZenGarden GetZenGarden();
	PlantDefinition GetPlantDefinition(SeedType::SeedType type);
	ZombieDefinition GetZombieDefinition(ZombieType::ZombieType type);
	ProjectileDefinition GetProjectileDefinition(ProjectileType::ProjectileType type);
	ChallengeDefinition GetChallengeDefinition(PVZLevel::PVZLevel mode);

#pragma endregion

#pragma region Images

	class Resource
	{
	public:
		static Image* IMAGE_BLANK;
	};

#pragma endregion
};