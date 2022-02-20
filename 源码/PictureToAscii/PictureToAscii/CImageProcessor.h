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

//��ȡ��ִ���ļ�����Ŀ¼
CString CImageProcessor::GetDefaultPath()
{
	char temp[256];
	GetModuleFileName(NULL, (LPWSTR)temp, MAX_PATH);  //�ҵ���ִ���ļ�
	CString defaultExefile;
	defaultExefile.Format(_T("%s"), temp);
	int position = defaultExefile.ReverseFind('\\');  
	CString defaultPath = defaultExefile.Left(position + 1);
	return defaultPath;
}

//��ȡBMPͼ�����ݣ�����ͼ��ߺͿ�
RGBQUAD* CImageProcessor::GetDataFromBMP(CString fn, int& w, int& h)
{
	//���ļ�
	ifstream in(fn, ios::binary);
	if (!in)
	{
		AfxMessageBox(_T("���ļ�ʧ��"));
		return NULL;
	}

	//��ȡ�ļ�ͷ
	BITMAPFILEHEADER fileHead;
	in.read((char*)&fileHead, sizeof(BITMAPFILEHEADER));
	if (fileHead.bfType != 19778)  //'BM'
	{
		AfxMessageBox(_T("����BMP�ļ�"));
		return NULL;
	}

	//��ȡ��Ϣͷ
	BITMAPINFOHEADER infoHead;
	in.read((char*)&infoHead, sizeof(BITMAPINFOHEADER));
	w = infoHead.biWidth;               //��
	h = infoHead.biHeight;              //��
	int bits = infoHead.biBitCount;     //λ��
	int lw = ((w*bits + 31) / 32) * 4;   //�п��

	//��ȡ��ɫ��
	RGBQUAD* pallete = NULL;
	if (bits == 1 || bits == 4 || bits == 8)
	{
		int sz = (int)pow(2, bits);
		pallete = new RGBQUAD[sz];
		in.read((char*)pallete, sizeof(RGBQUAD)*sz);
	}

	//��ȡ������ɫ����
	RGBQUAD* colorData = new RGBQUAD[w * h];
	char* lp = new char[lw];               //һ������
	int i, x, y;	                       //ѭ������
	for (y = 0; y < h; y++)
	{
		RGBQUAD* p = colorData + y * w;   //��ʱָ��
		in.read(lp, lw);
		x = 0;
		switch (bits)
		{
		case 1:
			for (i = 0; i < lw; i++)	  //����ֽڴ���
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
			for (i = 0; i < lw; i++)	  //����ֽڴ���
			{
				byte num = (byte)lp[i];
				p[x++] = pallete[((num >> 4) & 15)]; if (x == w) break;
				p[x++] = pallete[(num & 15)]; if (x == w) break;
			}
			break;
		case 8:
			for (i = 0; i < lw; i++)	  //����ֽڴ���
			{
				byte num = (byte)lp[i];
				p[x++] = pallete[num]; if (x == w) break;
			}
			break;
		case 24:
			for (i = 0; i < lw; i += 3)	  //3�ֽ�һ������
			{
				p[x].rgbBlue = lp[i];
				p[x].rgbGreen = lp[i + 1];
				p[x].rgbRed = lp[i + 2];
				x++;
				if (x == w)	break;
			}
			break;
		case 32:
			for (i = 0; i < lw; i += 4)	  //4�ֽ�һ������
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

	//�ͷ���Դ���ر��ļ�
	if (pallete != NULL) delete(pallete);
	in.close();

	return colorData;
}

//��ȡJPGͼ�����ݣ�����ͼ��ߺͿ�
RGBQUAD* CImageProcessor::GetDataFromJPG(CString fn, int& w, int& h)
{
	CImage img;       //����CImage���ͼ�񣬻�ȡ��RGB����
	img.Load(fn);
	w = img.GetWidth();
	h = img.GetHeight();
	byte* data24 = (byte*)img.GetBits();//ͼ������
	int bpp = img.GetBPP();             //λ��������Ĭ����24λ����
	int pitch = img.GetPitch();         //�п�ȣ���һ���ֽ���
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
		data24 += pitch;  //ָ���л�����һ��
	}
	img.Destroy();
	return data32;
}

//�ڿؼ�����ʾͼ��
//parent -  ĸ����
//ID     -  �ؼ�id
//data   -  ͼ������32λ��
//w h    -  ͼ���͸�
void CImageProcessor::DispBMP(HWND parent, int ID, RGBQUAD* data, int w, int h)
{
	HWND hwnd = GetDlgItem(parent, ID);  //�ؼ���������ڻ�ȡ��������
	HDC hdc = GetDC(hwnd);               //�ؼ���������ڻ�ͼ
	CRect rect;
	GetClientRect(hwnd, &rect);          //�ؼ��ߴ緶Χ
	
	BITMAPINFO bi;                       //����bmp��Ϣ�ṹ�壬��Ϣͷ+��ɫ��
	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = w;
	bi.bmiHeader.biHeight = h;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = w * h * 4;
	
	SetStretchBltMode(hdc, COLORONCOLOR);//����StretchDIBitsģʽ
	int res = StretchDIBits(hdc,         
		0, 0, rect.Width(), rect.Height(),   //Ŀ��λ��
		0, 0, w, h,    //Դλ��
		data,          //����
		&bi,           //��Ϣ�ṹ��
		DIB_RGB_COLORS, SRCCOPY);
}

//����Ϊ32λBMPͼ��
void CImageProcessor::SaveBMP(CString fn, RGBQUAD* data, int w, int h)
{
	//���ļ�
	ofstream out(fn, ios::binary);
	if (!out)
	{
		AfxMessageBox(_T("���ļ�ʧ��"));
		return;
	}

	//д�ļ�ͷ
	BITMAPFILEHEADER fileHead;
	fileHead.bfType = 19778;                 //'BM'
	fileHead.bfSize = 14 + 40 + w * h * 4;   //�ļ���С
	fileHead.bfReserved1 = 0;                //����1
	fileHead.bfReserved2 = 0;                //����2
	fileHead.bfOffBits = 14 + 40;            //���ݿ�ʼ��λ��
	out.write((char*)&fileHead, 14);
	   
	//д��Ϣͷ
	BITMAPINFOHEADER infoHead;
	infoHead.biSize = 40;              //���ṹ���С
	infoHead.biWidth = w;              //��
	infoHead.biHeight = h;             //��
	infoHead.biPlanes = 1;   
	infoHead.biBitCount = 32;          //λ��
	infoHead.biCompression = 0;
	infoHead.biSizeImage = 0;
	infoHead.biXPelsPerMeter = 0;
	infoHead.biYPelsPerMeter = 0;
	infoHead.biClrUsed = 0;
	infoHead.biClrImportant = 0;
	out.write((char*)&infoHead, sizeof(BITMAPINFOHEADER));

	//д��ɫ����
	out.write((char*)data, w * h * 4);

	//�ͷ���Դ���ر��ļ�
	out.close();

}

//��ɫ
void CImageProcessor::Reverse(RGBQUAD* data, int w, int h)
{
	for (int i = 0; i < w*h; i++)
	{
		data[i].rgbBlue = 255 - data[i].rgbBlue;
		data[i].rgbGreen = 255 - data[i].rgbGreen;
		data[i].rgbRed = 255 - data[i].rgbRed;
	}
}

//��Ե���
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

//����
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



