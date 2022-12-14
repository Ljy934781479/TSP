#include"../_ClassHpp/readBmp.hpp"
#include"..\_ClassHpp\struct.hpp"
#include"../myDefine.h"

CReadBmp::CReadBmp()
{
}

CReadBmp::~CReadBmp()
{

}

bool CReadBmp::lineto(myPoint& a, myPoint& b)
{
	short width = abs(a.x - b.x);
	short hight = abs(a.y - b.y);
	double k = double(hight) / width;//斜率k
	/*首先保证能画出连续的不间断的*/
	short prey = a.y;
	int dx = (a.x > b.x ? -1 : 1);
	COLOR col = COL_BLACK;
	int dw = 1;
	int dy = a.y > b.y ? -1 : 1;
	for (int x = a.x + dx; (x < b.x + 1 && dx == 1) || (x > b.x-1 && dx == -1)
		; x += dx,dw++)
	{//根据斜率计算Y位置
		int dh = dw * k;
		short cury = a.y + dy *dh;
		//垂直做线,保持连续
		for (int y = prey; (y < cury + 1 && dy == 1) || (y > cury && dy == -1)
			; y += dy)
			setPixel(col, x, y);
		if(cury== prey)
			setPixel(col, x, prey);
		prey = cury;
		if (float(dw) / width > 0.3)
			col = COL_LINETO;
	}
	return false;
}

bool CReadBmp::isClosePix(int x1, int y1, int x2, int y2)
{
	if (abs(x1 - x2) == 1 || abs(y1 - y2) == 1)
		return true;
	return false;
}

bool CReadBmp::readFile(const wchar_t* pFileName)
{
	FILE* file;
	long dpixeladd;
	if (_Buffer) {
		free(_Buffer);
		_Buffer = NULL;
	}
	if (_wfopen_s(&file, pFileName, L"rb")) 
		return 0;
	fread(&_bfh, sizeof(BITMAPFILEHEADER), 1, file);
	if (_bfh.bfType != 0x4D42) {
		fclose(file);
		return 0;
	}
	fread(&_bih, sizeof(BITMAPINFOHEADER), 1, file);

	dpixeladd = _bih.biBitCount / 8;
	_LineByteWidth = _bih.biWidth * (dpixeladd);
	if ((_LineByteWidth % 4) != 0)
		_LineByteWidth += 4 - (_LineByteWidth % 4);

	if ((_Buffer = (BYTE*)malloc(sizeof(BYTE) * _LineByteWidth * _bih.biHeight)) != NULL)
	{
		fread(_Buffer, _LineByteWidth * _bih.biHeight, 1, file);
		fclose(file);
		_midx = _bih.biWidth / 2;
		_midy = _bih.biHeight / 2;
		return 1;
	}
	fclose(file);
	return 0;
}

int CReadBmp::saveGray(const char* path, RECT* rc)
{
	int l = 0, t = 0, w = _bih.biWidth, h = _bih.biHeight;
	if (rc)
	{
		l = rc->left;
		w = rc->right - rc->left+1;
		t = rc->top;
		h = rc->bottom - rc->top+1;
	}
	FILE* BinFile;
	BITMAPFILEHEADER FileHeader;
	BITMAPINFOHEADER BmpHeader;
	int i, extend;
	bool Suc = true;
	BYTE p[4];
	BYTE* ex = NULL;
	extend = (w + 3) / 4 * 4 - w;

	if (extend)
	{
		ex = (BYTE*)malloc(extend);
		memset(ex, 0, extend);
	}
	// Open File
	if (fopen_s(&BinFile,path, "w+b"))
		return false;
	//参数填法见结构链接
	FileHeader.bfType = ((WORD)('M' << 8) | 'B');
	FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4L;//2个头结构后加调色板
	FileHeader.bfSize = FileHeader.bfOffBits + (w + extend) * h;
	FileHeader.bfReserved1 = 0;
	FileHeader.bfReserved2 = 0;
	if (fwrite((void*)&FileHeader, 1, sizeof(FileHeader), BinFile) != sizeof(FileHeader)) Suc = false;
	// Fill the ImgHeader
	BmpHeader.biSize = 40;
	BmpHeader.biWidth = w;
	BmpHeader.biHeight = h;
	BmpHeader.biPlanes = 1;
	BmpHeader.biBitCount = 8;
	BmpHeader.biCompression = 0;
	BmpHeader.biSizeImage = 0;
	BmpHeader.biXPelsPerMeter = 0;
	BmpHeader.biYPelsPerMeter = 0;
	BmpHeader.biClrUsed = 0;
	BmpHeader.biClrImportant = 0;
	if (fwrite((void*)&BmpHeader, 1, sizeof(BmpHeader), BinFile) != sizeof(BmpHeader)) Suc = false;
	// 写入调色板
	for (i = 0, p[3] = 0; i < 256; i++)
	{
		p[0] = p[1] = p[2] = i;
		if (fwrite((void*)p, 1, 4, BinFile) != 4) { Suc = false; break; }
	}

	//write data
	for (int rowId = 0; rowId < h; rowId++)
	{
		int y = t+h-1 - rowId;
		//一行的数据
		BYTE* rowByte = (BYTE*)malloc(w);
		for (int xid = l; xid < l+w; xid++)
		{
			COLOR col = getPixel(xid, y);
			int b = GetBValue(col);   //COL_POSABLE_LINE
			int g = GetGValue(col);   //Green
			int r = GetRValue(col);   //Red
            rowByte[xid - l] = (b+g+r)/3;
		}
		//写入真实的数据
		if (fwrite((void*)rowByte, 1, w, BinFile) != (unsigned int)w)
			Suc = false;
		free(rowByte);
		//扩充的数据 这里填充0
		if (extend)
			if (fwrite((void*)ex, 1, extend, BinFile) != 1)
				Suc = false;
	}
	fclose(BinFile);
	if (extend)
		free(ex);
	return Suc;
}

int CReadBmp::save24bit(const char* path, RECT* rc)
{
	int l = 0, t = 0, w = _bih.biWidth, h = _bih.biHeight;
	if (rc)
	{
		l = rc->left;
		w = rc->right - rc->left + 1;
		t = rc->top;
		h = rc->bottom - rc->top + 1;
	}
	// Define BMP Size
	int rowByte = 0;
	if ((w * 3 % 4) != 0)
		rowByte = (w * 3) + 4 - (w * 3) % 4;
	else
		rowByte = w * 3;

	const int size = rowByte * h;
	// Part.1 Create Bitmap File Header
	BITMAPFILEHEADER fileHeader;
	fileHeader.bfType = 0x4D42;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	// Part.2 Create Bitmap Info Header
	BITMAPINFOHEADER bitmapHeader = { 0 };
	bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapHeader.biHeight = h;
	bitmapHeader.biWidth = w;
	bitmapHeader.biPlanes = 3;
	bitmapHeader.biBitCount = 24;
	bitmapHeader.biSizeImage = size;
	bitmapHeader.biCompression = 0; //BI_RGB
	// Part.3 Create Data
	BYTE* bits = NULL;
	if (rc)
	{
		bits = (BYTE*)malloc(size);
		memset(bits, 0x00, size);
		for (int y = t; y < t + h; y++)
		{
			for (int x = l; x < l + w; x++)
			{
				COLOR col = getPixel(x, y);
				//求该像素点在内存中的位置
				int a = h - (y - t) - 1;
				int index = (x - l) * 3;
				bits[a * rowByte + index + 0] = GetBValue(col);   //COL_POSABLE_LINE
				bits[a * rowByte + index + 1] = GetGValue(col);   //Green
				bits[a * rowByte + index + 2] = GetRValue(col);   //Red
			}
		}
	}
	else
		bits = _Buffer;
	// Graph
	// Write to file
	FILE* output = NULL;
	fopen_s(&output, path, "wb");
	if (output == NULL)
		printf("Cannot open file!\n");
	else
	{
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, output);
		fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, output);
		fwrite(bits, size, 1, output);
		fclose(output);
	}
	if (rc)
	{
		memset(bits, 0x00, size);
		free(bits);
		bits = NULL;
	}
	return 0;
}

COLOR CReadBmp::getPixel(int x, int y)
{
	if (x < 0 || x >= _bih.biWidth || y < 0 || y >= _bih.biHeight)
		return -1;
	BYTE* ptr = _Buffer + (_bih.biHeight - y - 1) * _LineByteWidth + x * 3;
	if (_bih.biBitCount == 32) 
		return *(unsigned long*)ptr;
	return RGB(*(ptr + 2), *(ptr + 1), *ptr);
}

bool CReadBmp::setPixel(COLOR color, int x, int y)
{
	int dpixeladd;
	BYTE* ptr;
	if (x < 0 || x >= _bih.biWidth || y < 0 || y >= _bih.biHeight)
		return false;
	ptr = _Buffer + (_bih.biHeight - y - 1) * _LineByteWidth + x * 3;
	dpixeladd = _bih.biBitCount / 8;
	if (dpixeladd == 3) 
	{
		*ptr = GetBValue(color);
		*(ptr+1) = GetGValue(color);
		*(ptr+2) = GetRValue(color);
	}
	else
		*(unsigned long*)ptr = color;
	return true;
}

bool CReadBmp::creatNULLfile(int w, int h, COLOR color)
{
	// Define BMP Size
	int rowByte = 0;
	if ((w * 3 % 4) != 0)
		rowByte = (w * 3) + 4 - (w * 3) % 4;
	else
		rowByte = w * 3;
	const int size = rowByte * h;
	_LineByteWidth = rowByte;
	// Part.1 Create Bitmap File Header
	_bfh.bfType = 0x4D42;
	_bfh.bfReserved1 = 0;
	_bfh.bfReserved2 = 0;
	_bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size;
	_bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	// Part.2 Create Bitmap Info Header
    BITMAPINFOHEADER bitmapHeader = { 0 };
	_bih.biSize = sizeof(BITMAPINFOHEADER);
	_bih.biHeight = h;
	_bih.biWidth = w;
	_bih.biPlanes = 3;
	_bih.biBitCount = 24;
	_bih.biSizeImage = size;
	_bih.biCompression = 0; //BI_RGB
	_Buffer = (BYTE*)malloc(size);
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{//求该像素点在内存中的位置
			int a = h - y - 1;
			int index = x * 3;
			if (y == 0 || y == h - 1 || x == 0 || x == w - 1)
			{
				_Buffer[a * rowByte + index + 0] = 0;   //COL_POSABLE_LINE
				_Buffer[a * rowByte + index + 1] = 0;   //Green
				_Buffer[a * rowByte + index + 2] = 0;   //Red
			}
			else
			{
				_Buffer[a * rowByte + index + 0] = GetBValue(color);   //COL_POSABLE_LINE
				_Buffer[a * rowByte + index + 1] = GetGValue(color);   //Green
				_Buffer[a * rowByte + index + 2] = GetRValue(color);   //Red
			}
		}
	}
	return false;
}

char* CReadBmp::wcharTochar(wchar_t* p)
{//用完这个函数之后要释放掉返回的结果
	size_t e;
	size_t len = wcslen(p) + 1;
	char* res = (char*)malloc(len * sizeof(char));
	wcstombs_s(&e, res, len, p, _TRUNCATE);
	return res;
}

wchar_t* CReadBmp::charTowchar(char* p)
{//用完这个函数之后要释放掉返回的结果
	size_t e;
	size_t len = strlen(p) + 1;
	wchar_t* res = (wchar_t*)malloc(len*sizeof(wchar_t));
	mbstowcs_s(&e, res, len, p, _TRUNCATE);
	return res;
}

void CReadBmp::binaryzation(COLOR l, COLOR r, double vHsv)
{
	for (int i = 0; i < getHigh(); i++)
		for (int j = 0; j < getWidth(); j++)
		{
			tagHsv t(getPixel(j, i));
			if (t.v < vHsv)
				setPixel(l, j, i);
			else
				setPixel(r, j, i);
		}
}

int CReadBmp::setOutline(COLOR need, COLOR background, COLOR set)
{
	int x = 0, y = 0;
	for (int i = 0; i < getHigh(); i++)
		for (int j = 0; j< getWidth(); j++)
		{
			if (getPixel(j, i) != need)
				continue;
			if (_beginX == -1)//定死遇到的第一个为轮廓开始点,以免后续分析问题摇摆太多
			{
				_beginX = j;
				_beginY = i;
			}
			bool bfind = false;
			//如果周围八个点有背景色就视为是轮廓点
			for (int y1 = -1; y1 < 2 && !bfind; y1++)
				for (int x1 = -1; x1 < 2 && !bfind; x1++)
					if (getPixel(j - x1, i - y1) == background) {
						setPixel(set, j, i);
						x = j;
						y = i;
						bfind = true;
					}
		}
	return 0;
}
