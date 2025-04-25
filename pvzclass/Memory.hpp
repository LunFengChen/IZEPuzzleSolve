#pragma once

namespace PVZ
{
#pragma region Memory Class

	class Memory
	{
	public:
		/*	000-100存放创建子弹的函数
			100-200存放字符串或者PlantEffect的伪造植物对象
			300-400存放__autocollect_set
			400-500存放__asm__Plant_memset
			500-600存放Execute的同步代码 */
		static int Variable;
		static HANDLE hProcess;
		static DWORD processId;
		// 主线程（第一个线程），在初始化EventHandler后赋值
		static HANDLE hThread;
		static DWORD mainThreadId;
		static HWND mainwindowhandle;
		// 如果为true，则不会等待PVZ进程，立即执行远程代码
		static bool immediateExecute;
		// 如果为true，则在当前线程执行代码，在dll中设置为true
		static bool localExecute;
		static int DLLAddress;
		template <class T>
		inline static T ReadMemory(DWORD address)
		{
			if (localExecute)
			{
				T* buffer = (T*)address;
				return *buffer;
			}
			else
			{
				T buffer = (T)NULL;
				ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
				return buffer;
			}
		};
		template <class T>
		inline static BOOL WriteMemory(DWORD address, T value)
		{
			if (localExecute)
			{
				AllAccess(address);
				T* buffer = (T*)address;
				*buffer = value;
				return true;
			}
			else
			{
				return WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(T), NULL);
			}
		};
		template <class T>
		inline static BOOL ReadArray(DWORD address, T* result, size_t length)
		{
			if (localExecute)
			{
				memcpy(result, (const void*)address, length);
				return true;
			}
			else
			{
				return ReadProcessMemory(hProcess, (LPCVOID)address, (LPVOID)result, length, NULL);
			}
		};
		template <class T>
		inline static BOOL WriteArray(DWORD address, T* value, size_t length)
		{
			if (localExecute)
			{
				AllAccess(address);
				memcpy((void*)address, value, length);
				return true;
			}
			else
			{
				return WriteProcessMemory(hProcess, (LPVOID)address, value, length, NULL);
			}
		};
		static int ReadPointer(int baseaddress, int offset);
		static int ReadPointer(int baseaddress, int offset, int offset1);
		static int ReadPointer(int baseaddress, int offset, int offset1, int offset2);
		static BOOL AllAccess(int address);
		static int AllocMemory(int pages = 1, int size = 0);
		static void CreateThread(int address);
		static void FreeMemory(int address);
		static int Execute(byte asmcode[], int lengrh);
		static int Execute(AsmBuilder& builder);
		static bool InjectDll(const char* dllname);
		static int GetProcAddress(const char* procname);
		static int InvokeDllProc(const char* procname);
		static void WaitPVZ(); // 等待PVZ到达更新前
		static void ResumePVZ(); // 恢复PVZ
	};

#pragma endregion
}