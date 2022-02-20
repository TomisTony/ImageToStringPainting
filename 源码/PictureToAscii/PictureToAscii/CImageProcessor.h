#pragma once
#include "pch.h"
#include "CImageProcessor.h"
#include <Windows.h>
#include <fstream>

using namespace std;

typedef unsigned char byte;

byte Clip(double a)
{
	if (a < 0) return 0;
	if (a > 255) return 255;
	return (byte)a;
}

class CImageProcessor
{
public:
	static CString GetDefaultPath();
	static RGBQUAD* GetDataFromBMP(CString fn, int& w, int& h);
	static RGBQUAD* GetDataFromJPG(CString fn, int& w, int& h);
	static void DispBMP(HWND parent, int ID, RGBQUAD* data, int w, int h);
	static void SaveBMP(CString fn, RGBQUAD* data, int w, int h);

	static void Reverse(RGBQUAD* data, int w, int h);
	static void Edge(RGBQUAD* data, int w, int h);
	static void Mirror(RGBQUAD* data, int w, int h);
};

//获取可执行文件所在目录
CString CImageProcessor::GetDefaultPath()
{
	char temp[256];
	GetModuleFileName(NULL, (LPWSTR)temp, MAX_PATH);  //找到可执行文件
	CString defaultExefile;
	defaultExefile.Format(_T("%s"), temp);
	int position = defaultExefile.ReverseFind('\\');  
	CString defaultPath = defaultExefile.Left(position + 1);
	return defaultPath;
}

//获取BMP图像数据，保存图像高和宽
RGBQUAD* CImageProcessor::GetDataFromBMP(CString fn, int& w, int& h)
{
	//打开文件
	ifstream in(fn, ios::binary);
	if (!in)
	{
		AfxMessageBox(_T("打开文件失败"));
		return NULL;
	}

	//读取文件头
	BITMAPFILEHEADER fileHead;
	in.read((char*)&fileHead, sizeof(BITMAPFILEHEADER));
	if (fileHead.bfType != 19778)  //'BM'
	{
		AfxMessageBox(_T("不是BMP文件"));
		return NULL;
	}

	//读取信息头
	BITMAPINFOHEADER infoHead;
	in.read((char*)&infoHead, sizeof(BITMAPINFOHEADER));
	w = infoHead.biWidth;               //宽
	h = infoHead.biHeight;              //高
	int bits = infoHead.biBitCount;     //位数
	int lw = ((w*bits + 31) / 32) * 4;   //行宽度

	//读取调色板
	RGBQUAD* pallete = NULL;
	if (bits == 1 || bits == 4 || bits == 8)
	{
		int sz = (int)pow(2, bits);
		pallete = new RGBQUAD[sz];
		in.read((char*)pallete, sizeof(RGBQUAD)*sz);
	}

	//读取像素颜色数据
	RGBQUAD* colorData = new RGBQUAD[w * h];
	char* lp = new char[lw];               //一行数据
	int i, x, y;	                       //循环变量
	for (y = 0; y < h; y++)
	{
		RGBQUAD* p = colorData + y * w;   //临时指针
		in.read(lp, lw);
		x = 0;
		switch (bits)
		{
		case 1:
			for (i = 0; i < lw; i++)	  //逐个字节处理
			{
				byte num = lp[i];
				p[x++] = pallete[((num >> 7) & 1)]; if (x == w) break;
				p[x++] = pallete[((num >> 6) & 1)]; if (x == w) break;
				p[x++] = pallete[((num >> 5) & 1)]; if (x == w) break;
				p[x++] = pallete[((num >> 4) & 1)]; if (x == w) break;
				p[x++] = pallete[((num >> 3) & 1)]; if (x == w) break;
				p[x++] = pallete[((num >> 2) & 1)]; if (x == w) break;
				p[x++] = pallete[((num >> 1) & 1)]; if (x == w) break;
				p[x++] = pallete[(num & 1)]; if (x == w) break;
			}
			break;
		case 4:
			for (i = 0; i < lw; i++)	  //逐个字节处理
			{
				byte num = (byte)lp[i];
				p[x++] = pallete[((num >> 4) & 15)]; if (x == w) break;
				p[x++] = pallete[(num & 15)]; if (x == w) break;
			}
			break;
		case 8:
			for (i = 0; i < lw; i++)	  //逐个字节处理
			{
				byte num = (byte)lp[i];
				p[x++] = pallete[num]; if (x == w) break;
			}
			break;
		case 24:
			for (i = 0; i < lw; i += 3)	  //3字节一批处理
			{
				p[x].rgbBlue = lp[i];
				p[x].rgbGreen = lp[i + 1];
				p[x].rgbRed = lp[i + 2];
				x++;
				if (x == w)	break;
			}
			break;
		case 32:
			for (i = 0; i < lw; i += 4)	  //4字节一批处理
			{
				p[x].rgbBlue = lp[i];
				p[x].rgbGreen = lp[i + 1];
				p[x].rgbRed = lp[i + 2];
				p[x].rgbReserved = lp[i + 3];
				x++;
				if (x == w)	break;
			}
			break;
		default:
			break;
		}
	}

	//释放资源，关闭文件
	if (pallete != NULL) delete(pallete);
	in.close();

	return colorData;
}

//获取JPG图像数据，保存图像高和宽
RGBQUAD* CImageProcessor::GetDataFromJPG(CString fn, int& w, int& h)
{
	CImage img;       //利用CImage类打开图像，获取其RGB数据
	img.Load(fn);
	w = img.GetWidth();
	h = img.GetHeight();
	byte* data24 = (byte*)img.GetBits();//图像数据
	int bpp = img.GetBPP();             //位数，下面默认是24位处理
	int pitch = img.GetPitch();         //行宽度，即一行字节数
	RGBQUAD* data32 = new RGBQUAD[w*h];
	for (int y = h-1; y >= 0; y--)
	{
		for (int x = 0; x < w; x++)
		{
			int pos = y * w + x;
			data32[pos].rgbBlue = data24[x * 3];
			data32[pos].rgbGreen = data24[x * 3 + 1];
			data32[pos].rgbRed = data24[x * 3 + 2];
		}
		data24 += pitch;  //指针切换到下一行
	}
	img.Destroy();
	return data32;
}

//在控件上显示图像
//parent -  母窗体
//ID     -  控件id
//data   -  图像数据32位，
//w h    -  图像宽和高
void CImageProcessor::DispBMP(HWND parent, int ID, RGBQUAD* data, int w, int h)
{
	HWND hwnd = GetDlgItem(parent, ID);  //控件句柄，用于获取窗体属性
	HDC hdc = GetDC(hwnd);               //控件句柄，用于绘图
	CRect rect;
	GetClientRect(hwnd, &rect);          //控件尺寸范围
	
	BITMAPINFO bi;                       //定义bmp信息结构体，信息头+颜色表
	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = w;
	bi.bmiHeader.biHeight = h;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = w * h * 4;
	
	SetStretchBltMode(hdc, COLORONCOLOR);//设置StretchDIBits模式
	int res = StretchDIBits(hdc,         
		0, 0, rect.Width(), rect.Height(),   //目标位置
		0, 0, w, h,    //源位置
		data,          //数据
		&bi,           //信息结构体
		DIB_RGB_COLORS, SRCCOPY);
}

//保存为32位BMP图像
void CImageProcessor::SaveBMP(CString fn, RGBQUAD* data, int w, int h)
{
	//打开文件
	ofstream out(fn, ios::binary);
	if (!out)
	{
		AfxMessageBox(_T("打开文件失败"));
		return;
	}

	//写文件头
	BITMAPFILEHEADER fileHead;
	fileHead.bfType = 19778;                 //'BM'
	fileHead.bfSize = 14 + 40 + w * h * 4;   //文件大小
	fileHead.bfReserved1 = 0;                //保留1
	fileHead.bfReserved2 = 0;                //保留2
	fileHead.bfOffBits = 14 + 40;            //数据开始的位置
	out.write((char*)&fileHead, 14);
	   
	//写信息头
	BITMAPINFOHEADER infoHead;
	infoHead.biSize = 40;              //本结构体大小
	infoHead.biWidth = w;              //宽
	infoHead.biHeight = h;             //高
	infoHead.biPlanes = 1;   
	infoHead.biBitCount = 32;          //位数
	infoHead.biCompression = 0;
	infoHead.biSizeImage = 0;
	infoHead.biXPelsPerMeter = 0;
	infoHead.biYPelsPerMeter = 0;
	infoHead.biClrUsed = 0;
	infoHead.biClrImportant = 0;
	out.write((char*)&infoHead, sizeof(BITMAPINFOHEADER));

	//写颜色数据
	out.write((char*)data, w * h * 4);

	//释放资源，关闭文件
	out.close();

}

//反色
void CImageProcessor::Reverse(RGBQUAD* data, int w, int h)
{
	for (int i = 0; i < w*h; i++)
	{
		data[i].rgbBlue = 255 - data[i].rgbBlue;
		data[i].rgbGreen = 255 - data[i].rgbGreen;
		data[i].rgbRed = 255 - data[i].rgbRed;
	}
}

//边缘检测
void CImageProcessor::Edge(RGBQUAD* data, int w, int h)
{
	for (int y = 0; y < h-1; y++)
	{
		for (int x = 0; x < w-1; x++)
		{
			int pos0 = y * w + x;
			int pos1 = pos0 + w + 1;
			data[pos0].rgbBlue = Clip(data[pos0].rgbBlue - data[pos1].rgbBlue);
			data[pos0].rgbGreen = Clip(data[pos0].rgbGreen - data[pos1].rgbGreen);
			data[pos0].rgbRed = Clip(data[pos0].rgbRed - data[pos1].rgbRed);
		}
	}
}

//镜像
void CImageProcessor::Mirror(RGBQUAD* data, int w, int h)
{
	for (int y = 0; y < h - 1; y++)
	{
		for (int x = 0; x <w/2; x++)
		{
			int pos0 = y * w + x;
			int pos1 = y * w + (w - x - 1);
			swap(data[pos0].rgbBlue, data[pos1].rgbBlue);
			swap(data[pos0].rgbGreen, data[pos1].rgbGreen);
			swap(data[pos0].rgbRed, data[pos1].rgbRed);
		}
	}
}



