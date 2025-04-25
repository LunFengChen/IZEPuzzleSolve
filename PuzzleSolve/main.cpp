#include <iostream>
#include <unordered_map>



#include "../pvzclass/ProcessOpener.h"
#include "../pvzclass/PVZ.h"
#include "../pvzclass/Extensions.h"



std::unordered_map<PlantType::PlantType, char> plant_symbol_map = {
	{PlantType::Peashooter,    '1'},  // [1] ����/�㶹
	{PlantType::Sunflower,     'h'},  // [h] С��
	{PlantType::Wallnut,       'o'},  // [o] ���
	{PlantType::PotatoMine,    't'},  // [t] ����/����
	{PlantType::SnowPea,       'b'},  // [b] ����/����/ѩ��/ѩ��
	{PlantType::Chomper,       'z'},  // [z] ����
	{PlantType::Repeater,      '2'},  // [2] ˫��
	{PlantType::Puffshroom,    'p'},  // [p] С��
	{PlantType::Fumeshroom,    'd'},  // [d] ����
	{PlantType::Scaredyshroom, 'x'},  // [x] ��С
	{PlantType::Squash,        'w'},  // [w] ����/�ѹ�
	{PlantType::Threepeater,   '3'},  // [3] ����
	{PlantType::Spickweed,     '_'},  // [_] �ش�/����
	{PlantType::Torchwood,     'j'},  // [j] ����/���/��׮
	{PlantType::SplitPea,      'l'},  // [l] ����/�Ѽ�/˫��
	{PlantType::Starfruit,     '5'},  // [5] ����/����
	{PlantType::Magnetshroom,  'c'},  // [c] ����
	{PlantType::Kernelpult,     'y'},  // [y] ����/����
	{PlantType::UmbrellaLeaf,      's'}   // [s] ɡҶ/Ҷ��
};


struct BitmapData {
	int width;
	int height;
	HBITMAP hBitmap;
	BYTE* pixels;
};


class GameControler {
private:
	/// @brief ʹ��izcV1.5.10.exe����Ѫ��ʽ ��ÿ�е�5ֲ�����ƽ����Ѫ
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
		// �˳�PVZ
		PVZ::QuitPVZ();
	}

	/// @brief Ѱ����Ϸ
	bool find_game() {
		this->pid = ProcessOpener::Open();
		if (!this->pid) return false; // û���ҵ�����
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


	/// @brief ����Ѱ����Ϸ
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
		EnableBackgroundRunning(true); // ����pvz��̨����
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


	/// @brief ����Ƿ���izeģʽ
	bool is_ize_mode() {
		if(this->pvz.GetBaseAddress() && this->pvz.LevelId == 70) return true;
		return false;
	}


	/// @brief ��ȡ��ǰĳ�е�iztֲ������
	/// @param row: �к�
	std::string get_plants_row(int row) {
		std::string res(5, '.'); // ÿ�й̶� 5 �У���ʼ��Ϊ.
		auto plants = this->board.GetAllPlants();
		for (auto& plant : plants) {
			if (plant.Row == row && plant.Column >= 0 && plant.Column < 5) {
				res[plant.Column] = get_plant_izt(plant.Type);
			}
		}
		return res;
	}

	/// @brief ��ȡֲ���ize���
	char get_plant_izt(PlantType::PlantType pt) {
		auto it = plant_symbol_map.find(pt);  // ����ֱ��ʹ�� operator[] ��ֹ��������¼�
		return (it != plant_symbol_map.end()) ? it->second : '.';
	}


	/**
	 * @brief ��ȡ����ָ�����򲢱���ΪBMP�ļ�
	 * @param hwnd Ŀ�괰�ھ��
	 * @param x �������Ͻ�X���꣨����ڴ��ڿͻ�����
	 * @param y �������Ͻ�Y���꣨����ڴ��ڿͻ�����
	 * @param width ��ȡ�����ȣ����أ�
	 * @param height ��ȡ����߶ȣ����أ�
	 * @param filename ����ļ�����UTF-8����·����
	 * @return �ɹ�����true��ʧ�ܷ���false
	 *
	 * @details
	 * - ����ϵ˵����
	 *   ������������������������������������������������
	 *   ��(0,0) ���ڿͻ������Ͻ�					   ��
	 *   ��											   ��
	 *   ��      width								   ��
	 *   ��    ������������							   ��
	 *   ��x,y ���������������� height				   ��
	 *   ��    ��			 �� ��					   ��
	 *   ��    ����������������						   ��
	 *   ������������������������������������������������
	 *
	 * - �ļ�����Ϊ24λɫ���BMP��ʽ
	 * - ��Ҫ������ӵ���ļ�д��Ȩ��
	 */
	bool CaptureAndSave(HWND hwnd, int x, int y, int width, int height, const char* filename) {
		if (!hwnd || width <= 0 || height <= 0) {
			std::cout << "please check 1)hwnd 2)width 3)height" << std::endl;
			return false;
		}
		// ��ȡ�����豸������
		HDC hdcWindow = GetWindowDC(hwnd);
		HDC hdcMem = CreateCompatibleDC(hdcWindow);

		// ��������λͼ
		HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
		SelectObject(hdcMem, hBitmap);

		// ִ�н�ͼ
		BitBlt(hdcMem, 0, 0, width, height, hdcWindow, x, y, SRCCOPY);

		// ����Ϊ BMP �ļ�
		BITMAPFILEHEADER bmfHeader = { 0 };
		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = width;
		bi.biHeight = height;
		bi.biPlanes = 1;
		bi.biBitCount = 24; // 24λɫ��
		bi.biCompression = BI_RGB;

		DWORD dwBmpSize = ((width * 3 + 3) & ~3) * height; // �����������ݴ�С
		char* pixels = new char[dwBmpSize];
		GetDIBits(hdcMem, hBitmap, 0, height, pixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

		// д���ļ�
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

		// ������Դ
		CloseHandle(hFile);
		delete[] pixels;
		DeleteObject(hBitmap);
		DeleteDC(hdcMem);
		ReleaseDC(hwnd, hdcWindow);

		return true;
	}



	// ���� BMP �ļ����ڴ�
	BitmapData LoadBitmapFromFile(const char* filename) {
		BitmapData data = { 0 };

		// ����λͼ�ļ�
		HBITMAP hBitmap = (HBITMAP)LoadImageA(
			NULL, filename, IMAGE_BITMAP, 0, 0,
			LR_LOADFROMFILE | LR_CREATEDIBSECTION
		);

		if (!hBitmap) {
			printf("[����] �޷������ļ�: %s (������:%d)\n", filename, GetLastError());
			return data;
		}

		// ��ȡλͼ��Ϣ
		BITMAP bm;
		GetObject(hBitmap, sizeof(BITMAP), &bm);
		data.width = bm.bmWidth;
		data.height = bm.bmHeight;
		data.hBitmap = hBitmap;

		// ��ȡ�������� (24λɫ��)
		HDC hdc = CreateCompatibleDC(NULL);
		SelectObject(hdc, hBitmap);

		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = data.width;
		bi.biHeight = -data.height; // ��ֵ��ʾ���ϵ���
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = BI_RGB;

		DWORD dwSize = data.width * data.height * 3;
		data.pixels = new BYTE[dwSize];

		if (!GetDIBits(hdc, hBitmap, 0, data.height, data.pixels,
			(BITMAPINFO*)&bi, DIB_RGB_COLORS)) {
			printf("[����] %s �������ݶ�ȡʧ��\n", filename);
		}

		DeleteDC(hdc);
		return data;
	}

	// ����ƴ�Ӻ���
	bool StitchImagesHorizontally(const std::vector<BitmapData>& images, const char* output) {
		if (images.empty()) return false;

		// �����ܳߴ�
		int totalWidth = 0;
		int maxHeight = 0;
		for (const auto& img : images) {
			totalWidth += img.width;
			if (img.height > maxHeight) maxHeight = img.height;
		}

		// ����Ŀ��λͼ
		HDC hdc = GetDC(NULL);
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hDest = CreateCompatibleBitmap(hdc, totalWidth, maxHeight);
		SelectObject(hMemDC, hDest);

		// ��������ͼƬ
		int xOffset = 0;
		for (const auto& img : images) {
			HDC hImgDC = CreateCompatibleDC(hdc);
			SelectObject(hImgDC, img.hBitmap);
			BitBlt(hMemDC, xOffset, 0, img.width, img.height,
				hImgDC, 0, 0, SRCCOPY);
			xOffset += img.width;
			DeleteDC(hImgDC);
		}

		// ����ƴ�ӽ��
		BITMAPFILEHEADER bmf = { 0 };
		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = totalWidth;
		bi.biHeight = -maxHeight; // �ؼ��޸������߶�ֵ
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = BI_RGB;

		// ������������ֽ���
		DWORD lineSize = (totalWidth * 3 + 3) & ~3;
		DWORD pixelDataSize = lineSize * maxHeight;
		BYTE* pixels = new BYTE[pixelDataSize]{ 0 };

		GetDIBits(hMemDC, hDest, 0, maxHeight, pixels,
			(BITMAPINFO*)&bi, DIB_RGB_COLORS);

		// д���ļ�
		HANDLE hFile = CreateFileA(output, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			printf("[����] �޷���������ļ�: %s\n", output);
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

		// ������Դ
		CloseHandle(hFile);
		delete[] pixels;
		DeleteObject(hDest);
		DeleteDC(hMemDC);
		ReleaseDC(NULL, hdc);

		return true;
	}


	// ����ƴ�Ӻ���
	bool StitchImagesVertically(const std::vector<BitmapData>& images, const char* output) {
		if (images.empty()) return false;

		// �����ܳߴ�
		int totalHeight = 0;
		int maxWidth = 0;
		for (const auto& img : images) {
			totalHeight += img.height;
			if (img.width > maxWidth) maxWidth = img.width;
		}

		// ����Ŀ��λͼ
		HDC hdc = GetDC(NULL);
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hDest = CreateCompatibleBitmap(hdc, maxWidth, totalHeight);
		SelectObject(hMemDC, hDest);

		// ��������ͼƬ
		int yOffset = 0;
		for (const auto& img : images) {
			HDC hImgDC = CreateCompatibleDC(hdc);
			SelectObject(hImgDC, img.hBitmap);
			BitBlt(hMemDC, 0, yOffset, img.width, img.height,
				hImgDC, 0, 0, SRCCOPY);
			yOffset += img.height;
			DeleteDC(hImgDC);
		}

		// ����ƴ�ӽ��
		BITMAPFILEHEADER bmf = { 0 };
		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = maxWidth;
		bi.biHeight = -totalHeight; // ��ֵ��ʾ���ϵ���
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = BI_RGB;

		// ������������ֽ���
		DWORD lineSize = (maxWidth * 3 + 3) & ~3;
		DWORD pixelDataSize = lineSize * totalHeight;
		BYTE* pixels = new BYTE[pixelDataSize]{ 0 };

		GetDIBits(hMemDC, hDest, 0, totalHeight, pixels,
			(BITMAPINFO*)&bi, DIB_RGB_COLORS);

		// д���ļ�
		HANDLE hFile = CreateFileA(output, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			printf("[����] �޷���������ļ�: %s\n", output);
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

		// ������Դ
		CloseHandle(hFile);
		delete[] pixels;
		DeleteObject(hDest);
		DeleteDC(hMemDC);
		ReleaseDC(NULL, hdc);

		return true;
	}

	bool StitchImagesGrid(const std::vector<std::string>& filePaths, const char* output) {
		if (filePaths.size() != 25) {
			std::cout << "[����] ��Ҫ���ô��� 25 �� BMP �ļ�·����" << std::endl;
			return false;
		}

		// ��������ͼƬ
		std::vector<BitmapData> images;
		GameControler gameControler;
		for (const auto& path : filePaths) {
			BitmapData data = gameControler.LoadBitmapFromFile(path.c_str());
			if (!data.hBitmap) {
				std::cout << "[����] �޷�����ͼƬ: " << path << std::endl;
				return false;
			}
			images.push_back(data);
		}

		// ��������ͼƬ��Сһ�£���ȡ����ͼƬ�Ŀ��
		int singleWidth = images[0].width;
		int singleHeight = images[0].height;

		// ���������ܿ��
		int totalWidth = singleWidth * 5;
		int totalHeight = singleHeight * 5;

		// ����Ŀ��λͼ
		HDC hdc = GetDC(NULL);
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hDest = CreateCompatibleBitmap(hdc, totalWidth, totalHeight);
		SelectObject(hMemDC, hDest);

		// ��������ͼƬ������
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

		// ����ƴ�ӽ��
		BITMAPFILEHEADER bmf = { 0 };
		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = totalWidth;
		bi.biHeight = -totalHeight; // ��ֵ��ʾ���ϵ���
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = BI_RGB;

		// ������������ֽ���
		DWORD lineSize = (totalWidth * 3 + 3) & ~3;
		DWORD pixelDataSize = lineSize * totalHeight;
		BYTE* pixels = new BYTE[pixelDataSize]{ 0 };

		GetDIBits(hMemDC, hDest, 0, totalHeight, pixels,
			(BITMAPINFO*)&bi, DIB_RGB_COLORS);

		// д���ļ�
		HANDLE hFile = CreateFileA(output, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			std::cout << "[����] �޷���������ļ�: " << output << std::endl;
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

		// ������Դ
		CloseHandle(hFile);
		delete[] pixels;
		DeleteObject(hDest);
		DeleteDC(hMemDC);
		ReleaseDC(NULL, hdc);

		// �ͷż��ص�ͼƬ��Դ
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
	std::cout << "�Ѿ�����ize!" << std::endl;

	// ׼�� 25 �� BMP �ļ�·��
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