#include <iostream>
#include <unordered_map>



#include "../pvzclass/ProcessOpener.h"
#include "../pvzclass/PVZ.h"
#include "../pvzclass/Extensions.h"



std::unordered_map<PlantType::PlantType, char> plant_symbol_map = {
	{PlantType::Peashooter,    '1'},  // [1] 单发/豌豆
	{PlantType::Sunflower,     'h'},  // [h] 小向
	{PlantType::Wallnut,       'o'},  // [o] 坚果
	{PlantType::PotatoMine,    't'},  // [t] 地雷/土豆
	{PlantType::SnowPea,       'b'},  // [b] 冰豆/寒冰/雪花/雪豆
	{PlantType::Chomper,       'z'},  // [z] 大嘴
	{PlantType::Repeater,      '2'},  // [2] 双发
	{PlantType::Puffshroom,    'p'},  // [p] 小喷
	{PlantType::Fumeshroom,    'd'},  // [d] 大喷
	{PlantType::Scaredyshroom, 'x'},  // [x] 胆小
	{PlantType::Squash,        'w'},  // [w] 倭瓜/窝瓜
	{PlantType::Threepeater,   '3'},  // [3] 三线
	{PlantType::Spickweed,     '_'},  // [_] 地刺/棘草
	{PlantType::Torchwood,     'j'},  // [j] 火树/火炬/树桩
	{PlantType::SplitPea,      'l'},  // [l] 分裂/裂荚/双向
	{PlantType::Starfruit,     '5'},  // [5] 杨桃/星星
	{PlantType::Magnetshroom,  'c'},  // [c] 磁铁
	{PlantType::Kernelpult,     'y'},  // [y] 玉米/黄油
	{PlantType::UmbrellaLeaf,      's'}   // [s] 伞叶/叶子
};


struct BitmapData {
	int width;
	int height;
	HBITMAP hBitmap;
	BYTE* pixels;
};


class GameControler {
private:
	/// @brief 使用izcV1.5.10.exe的算血公式 对每行的5植物进行平均算血
	/// @param  
	void calc_avg_hp() {
	}




public:
	PVZ::Board board = PVZ::Board(0);
	PVZ::PVZApp pvz = PVZ::PVZApp(0);
	DWORD pid;

	GameControler() {
		this->find_game();
	}

	~GameControler() {
		// 退出PVZ
		PVZ::QuitPVZ();
	}

	/// @brief 寻找游戏
	bool find_game() {
		this->pid = ProcessOpener::Open();
		if (!this->pid) return false; // 没有找到进程
		PVZ::InitPVZ(this->pid);
		EnableBackgroundRunning(true);

		this->pvz = PVZ::GetPVZApp();
		if (!this->pvz.GetBaseAddress()) {
			PVZ::QuitPVZ();
			return false;
		}
		this->board = PVZ::GetBoard();
		if (!this->board.GetBaseAddress()) {
			PVZ::QuitPVZ();
			return false;
		}
		return true;
	}


	/// @brief 重新寻找游戏
	bool refind_pvz() {
		DWORD pid = ProcessOpener::Open();
		if (pid && this->pid == pid) {
			if (!this->board.GetBaseAddress() || !this->pvz.GetBaseAddress()) {
				this->board = PVZ::GetBoard();
				this->pvz = PVZ::GetPVZApp();
			}
			return true;
		}

		PVZ::QuitPVZ();
		this->pid = pid;
		EnableBackgroundRunning(true); // 启用pvz后台运行
		PVZ::InitPVZ(this->pid);
		this->board = PVZ::GetBoard();
		if (!this->board.GetBaseAddress()) {
			PVZ::QuitPVZ();
			return false;
		}
		this->pvz = PVZ::GetPVZApp();
		if (!this->pvz.GetBaseAddress()) {
			PVZ::QuitPVZ();
			return false;
		}
		return true;
	}


	/// @brief 检测是否在ize模式
	bool is_ize_mode() {
		if(this->pvz.GetBaseAddress() && this->pvz.LevelId == 70) return true;
		return false;
	}


	/// @brief 获取当前某行的izt植物类型
	/// @param row: 行号
	std::string get_plants_row(int row) {
		std::string res(5, '.'); // 每行固定 5 列，初始化为.
		auto plants = this->board.GetAllPlants();
		for (auto& plant : plants) {
			if (plant.Row == row && plant.Column >= 0 && plant.Column < 5) {
				res[plant.Column] = get_plant_izt(plant.Type);
			}
		}
		return res;
	}

	/// @brief 获取植物的ize表达
	char get_plant_izt(PlantType::PlantType pt) {
		auto it = plant_symbol_map.find(pt);  // 避免直接使用 operator[] 防止意外插入新键
		return (it != plant_symbol_map.end()) ? it->second : '.';
	}


	/**
	 * @brief 截取窗口指定区域并保存为BMP文件
	 * @param hwnd 目标窗口句柄
	 * @param x 区域左上角X坐标（相对于窗口客户区）
	 * @param y 区域左上角Y坐标（相对于窗口客户区）
	 * @param width 截取区域宽度（像素）
	 * @param height 截取区域高度（像素）
	 * @param filename 输出文件名（UTF-8编码路径）
	 * @return 成功返回true，失败返回false
	 *
	 * @details
	 * - 坐标系说明：
	 *   ┌──────────────────────┐
	 *   │(0,0) 窗口客户区左上角					   │
	 *   │											   │
	 *   │      width								   │
	 *   │    ←────→							   │
	 *   │x,y ┌──────┐ height				   │
	 *   │    │			 │ ↓					   │
	 *   │    └──────┘						   │
	 *   └──────────────────────┘
	 *
	 * - 文件保存为24位色深的BMP格式
	 * - 需要调用者拥有文件写入权限
	 */
	bool CaptureAndSave(HWND hwnd, int x, int y, int width, int height, const char* filename) {
		if (!hwnd || width <= 0 || height <= 0) {
			std::cout << "please check 1)hwnd 2)width 3)height" << std::endl;
			return false;
		}
		// 获取窗口设备上下文
		HDC hdcWindow = GetWindowDC(hwnd);
		HDC hdcMem = CreateCompatibleDC(hdcWindow);

		// 创建兼容位图
		HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
		SelectObject(hdcMem, hBitmap);

		// 执行截图
		BitBlt(hdcMem, 0, 0, width, height, hdcWindow, x, y, SRCCOPY);

		// 保存为 BMP 文件
		BITMAPFILEHEADER bmfHeader = { 0 };
		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = width;
		bi.biHeight = height;
		bi.biPlanes = 1;
		bi.biBitCount = 24; // 24位色深
		bi.biCompression = BI_RGB;

		DWORD dwBmpSize = ((width * 3 + 3) & ~3) * height; // 计算像素数据大小
		char* pixels = new char[dwBmpSize];
		GetDIBits(hdcMem, hBitmap, 0, height, pixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

		// 写入文件
		HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			delete[] pixels;
			return false;
		}

		DWORD dwSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
		bmfHeader.bfType = 0x4D42; // "BM"
		bmfHeader.bfSize = dwSize;
		bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		DWORD bytesWritten;
		WriteFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &bytesWritten, NULL);
		WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &bytesWritten, NULL);
		WriteFile(hFile, pixels, dwBmpSize, &bytesWritten, NULL);

		// 清理资源
		CloseHandle(hFile);
		delete[] pixels;
		DeleteObject(hBitmap);
		DeleteDC(hdcMem);
		ReleaseDC(hwnd, hdcWindow);

		return true;
	}



	// 加载 BMP 文件到内存
	BitmapData LoadBitmapFromFile(const char* filename) {
		BitmapData data = { 0 };

		// 加载位图文件
		HBITMAP hBitmap = (HBITMAP)LoadImageA(
			NULL, filename, IMAGE_BITMAP, 0, 0,
			LR_LOADFROMFILE | LR_CREATEDIBSECTION
		);

		if (!hBitmap) {
			printf("[错误] 无法加载文件: %s (错误码:%d)\n", filename, GetLastError());
			return data;
		}

		// 获取位图信息
		BITMAP bm;
		GetObject(hBitmap, sizeof(BITMAP), &bm);
		data.width = bm.bmWidth;
		data.height = bm.bmHeight;
		data.hBitmap = hBitmap;

		// 提取像素数据 (24位色深)
		HDC hdc = CreateCompatibleDC(NULL);
		SelectObject(hdc, hBitmap);

		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = data.width;
		bi.biHeight = -data.height; // 负值表示从上到下
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = BI_RGB;

		DWORD dwSize = data.width * data.height * 3;
		data.pixels = new BYTE[dwSize];

		if (!GetDIBits(hdc, hBitmap, 0, data.height, data.pixels,
			(BITMAPINFO*)&bi, DIB_RGB_COLORS)) {
			printf("[警告] %s 像素数据读取失败\n", filename);
		}

		DeleteDC(hdc);
		return data;
	}

	// 横向拼接函数
	bool StitchImagesHorizontally(const std::vector<BitmapData>& images, const char* output) {
		if (images.empty()) return false;

		// 计算总尺寸
		int totalWidth = 0;
		int maxHeight = 0;
		for (const auto& img : images) {
			totalWidth += img.width;
			if (img.height > maxHeight) maxHeight = img.height;
		}

		// 创建目标位图
		HDC hdc = GetDC(NULL);
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hDest = CreateCompatibleBitmap(hdc, totalWidth, maxHeight);
		SelectObject(hMemDC, hDest);

		// 绘制所有图片
		int xOffset = 0;
		for (const auto& img : images) {
			HDC hImgDC = CreateCompatibleDC(hdc);
			SelectObject(hImgDC, img.hBitmap);
			BitBlt(hMemDC, xOffset, 0, img.width, img.height,
				hImgDC, 0, 0, SRCCOPY);
			xOffset += img.width;
			DeleteDC(hImgDC);
		}

		// 保存拼接结果
		BITMAPFILEHEADER bmf = { 0 };
		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = totalWidth;
		bi.biHeight = -maxHeight; // 关键修复：负高度值
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = BI_RGB;

		// 计算对齐后的行字节数
		DWORD lineSize = (totalWidth * 3 + 3) & ~3;
		DWORD pixelDataSize = lineSize * maxHeight;
		BYTE* pixels = new BYTE[pixelDataSize]{ 0 };

		GetDIBits(hMemDC, hDest, 0, maxHeight, pixels,
			(BITMAPINFO*)&bi, DIB_RGB_COLORS);

		// 写入文件
		HANDLE hFile = CreateFileA(output, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			printf("[错误] 无法创建输出文件: %s\n", output);
			return false;
		}

		DWORD headerSize = sizeof(bmf) + sizeof(bi);
		bmf.bfType = 0x4D42;
		bmf.bfSize = headerSize + pixelDataSize;
		bmf.bfOffBits = sizeof(bmf) + sizeof(bi);

		DWORD written;
		WriteFile(hFile, &bmf, sizeof(bmf), &written, NULL);
		WriteFile(hFile, &bi, sizeof(bi), &written, NULL);
		WriteFile(hFile, pixels, pixelDataSize, &written, NULL);

		// 清理资源
		CloseHandle(hFile);
		delete[] pixels;
		DeleteObject(hDest);
		DeleteDC(hMemDC);
		ReleaseDC(NULL, hdc);

		return true;
	}


	// 竖向拼接函数
	bool StitchImagesVertically(const std::vector<BitmapData>& images, const char* output) {
		if (images.empty()) return false;

		// 计算总尺寸
		int totalHeight = 0;
		int maxWidth = 0;
		for (const auto& img : images) {
			totalHeight += img.height;
			if (img.width > maxWidth) maxWidth = img.width;
		}

		// 创建目标位图
		HDC hdc = GetDC(NULL);
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hDest = CreateCompatibleBitmap(hdc, maxWidth, totalHeight);
		SelectObject(hMemDC, hDest);

		// 绘制所有图片
		int yOffset = 0;
		for (const auto& img : images) {
			HDC hImgDC = CreateCompatibleDC(hdc);
			SelectObject(hImgDC, img.hBitmap);
			BitBlt(hMemDC, 0, yOffset, img.width, img.height,
				hImgDC, 0, 0, SRCCOPY);
			yOffset += img.height;
			DeleteDC(hImgDC);
		}

		// 保存拼接结果
		BITMAPFILEHEADER bmf = { 0 };
		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = maxWidth;
		bi.biHeight = -totalHeight; // 负值表示从上到下
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = BI_RGB;

		// 计算对齐后的行字节数
		DWORD lineSize = (maxWidth * 3 + 3) & ~3;
		DWORD pixelDataSize = lineSize * totalHeight;
		BYTE* pixels = new BYTE[pixelDataSize]{ 0 };

		GetDIBits(hMemDC, hDest, 0, totalHeight, pixels,
			(BITMAPINFO*)&bi, DIB_RGB_COLORS);

		// 写入文件
		HANDLE hFile = CreateFileA(output, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			printf("[错误] 无法创建输出文件: %s\n", output);
			return false;
		}

		DWORD headerSize = sizeof(bmf) + sizeof(bi);
		bmf.bfType = 0x4D42;
		bmf.bfSize = headerSize + pixelDataSize;
		bmf.bfOffBits = sizeof(bmf) + sizeof(bi);

		DWORD written;
		WriteFile(hFile, &bmf, sizeof(bmf), &written, NULL);
		WriteFile(hFile, &bi, sizeof(bi), &written, NULL);
		WriteFile(hFile, pixels, pixelDataSize, &written, NULL);

		// 清理资源
		CloseHandle(hFile);
		delete[] pixels;
		DeleteObject(hDest);
		DeleteDC(hMemDC);
		ReleaseDC(NULL, hdc);

		return true;
	}

	bool StitchImagesGrid(const std::vector<std::string>& filePaths, const char* output) {
		if (filePaths.size() != 25) {
			std::cout << "[错误] 需要正好传入 25 个 BMP 文件路径！" << std::endl;
			return false;
		}

		// 加载所有图片
		std::vector<BitmapData> images;
		GameControler gameControler;
		for (const auto& path : filePaths) {
			BitmapData data = gameControler.LoadBitmapFromFile(path.c_str());
			if (!data.hBitmap) {
				std::cout << "[错误] 无法加载图片: " << path << std::endl;
				return false;
			}
			images.push_back(data);
		}

		// 假设所有图片大小一致，获取单个图片的宽高
		int singleWidth = images[0].width;
		int singleHeight = images[0].height;

		// 计算网格总宽高
		int totalWidth = singleWidth * 5;
		int totalHeight = singleHeight * 5;

		// 创建目标位图
		HDC hdc = GetDC(NULL);
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hDest = CreateCompatibleBitmap(hdc, totalWidth, totalHeight);
		SelectObject(hMemDC, hDest);

		// 绘制所有图片到网格
		for (int row = 0; row < 5; ++row) {
			for (int col = 0; col < 5; ++col) {
				int index = row * 5 + col;
				HDC hImgDC = CreateCompatibleDC(hdc);
				SelectObject(hImgDC, images[index].hBitmap);
				BitBlt(hMemDC, col * singleWidth, row * singleHeight, singleWidth, singleHeight,
					hImgDC, 0, 0, SRCCOPY);
				DeleteDC(hImgDC);
			}
		}

		// 保存拼接结果
		BITMAPFILEHEADER bmf = { 0 };
		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = totalWidth;
		bi.biHeight = -totalHeight; // 负值表示从上到下
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = BI_RGB;

		// 计算对齐后的行字节数
		DWORD lineSize = (totalWidth * 3 + 3) & ~3;
		DWORD pixelDataSize = lineSize * totalHeight;
		BYTE* pixels = new BYTE[pixelDataSize]{ 0 };

		GetDIBits(hMemDC, hDest, 0, totalHeight, pixels,
			(BITMAPINFO*)&bi, DIB_RGB_COLORS);

		// 写入文件
		HANDLE hFile = CreateFileA(output, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			std::cout << "[错误] 无法创建输出文件: " << output << std::endl;
			return false;
		}

		DWORD headerSize = sizeof(bmf) + sizeof(bi);
		bmf.bfType = 0x4D42;
		bmf.bfSize = headerSize + pixelDataSize;
		bmf.bfOffBits = sizeof(bmf) + sizeof(bi);

		DWORD written;
		WriteFile(hFile, &bmf, sizeof(bmf), &written, NULL);
		WriteFile(hFile, &bi, sizeof(bi), &written, NULL);
		WriteFile(hFile, pixels, pixelDataSize, &written, NULL);

		// 清理资源
		CloseHandle(hFile);
		delete[] pixels;
		DeleteObject(hDest);
		DeleteDC(hMemDC);
		ReleaseDC(NULL, hdc);

		// 释放加载的图片资源
		for (auto& img : images) {
			DeleteObject(img.hBitmap);
			delete[] img.pixels;
		}

		return true;
	}
	
};

int main() {
	GameControler game_controler;
	while (!game_controler.is_ize_mode()) { Sleep(10); };
	std::cout << "已经进入ize!" << std::endl;

	// 准备 25 个 BMP 文件路径
	std::vector<std::string> filePaths = {};
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			std::string filename = "./imgs/" + std::to_string(i) + "_" + std::to_string(j) + ".bmp";
			filePaths.push_back(filename);
		}
	}
	game_controler.StitchImagesGrid(filePaths, "combine.bmp");

	return 0;
}