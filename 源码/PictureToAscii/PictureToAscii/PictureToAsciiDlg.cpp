
// PictureToAsciiDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "PictureToAscii.h"
#include "PictureToAsciiDlg.h"
#include "afxdialogex.h"
#include "CShowDlg.h"
#include "CImageProcessor.h"
#include <opencv2/opencv.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CString charToSend;//用于传递的CString
CString charToSave;//用于保存的CString

using namespace std;
using namespace cv;

Mat src, image;//src为原始图像,image为通过设置放缩后的图像
int WordW, WordH;

void CharactorPaintGenerate(CString Ccode);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPictureToAsciiDlg 对话框



CPictureToAsciiDlg::CPictureToAsciiDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PICTURETOASCII_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPictureToAsciiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, myCombo);
}

BEGIN_MESSAGE_MAP(CPictureToAsciiDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CPictureToAsciiDlg::OnBnClickedCancel)
//	ON_COMMAND(IDHELP, &CPictureToAsciiDlg::OnDisplayHelp)
	ON_COMMAND(ID_Save, &CPictureToAsciiDlg::OnSave)
	ON_COMMAND(ID_Quit, &CPictureToAsciiDlg::OnQuit)
	ON_COMMAND(ID_OpenFile, &CPictureToAsciiDlg::OnOpenfile)
	ON_COMMAND(ID_HELP, &CPictureToAsciiDlg::OnHelp)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CPictureToAsciiDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_OpenPic, &CPictureToAsciiDlg::OnBnClickedOpenpic)
	ON_BN_CLICKED(IDC_PicPreview, &CPictureToAsciiDlg::OnBnClickedPicpreview)
	ON_BN_CLICKED(IDC_WordPreview, &CPictureToAsciiDlg::OnBnClickedWordpreview)
	ON_BN_CLICKED(IDC_SaveWord, &CPictureToAsciiDlg::OnBnClickedSaveword)
END_MESSAGE_MAP()


// CPictureToAsciiDlg 消息处理程序

BOOL CPictureToAsciiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPictureToAsciiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPictureToAsciiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPictureToAsciiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPictureToAsciiDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}



void CPictureToAsciiDlg::OnSave()
{
	// TODO: 在此添加命令处理程序代码
	if (charToSave != _T("")) {
		CPictureToAsciiDlg::OnBnClickedSaveword();
	}
	else {
		MessageBox(_T("请先打开图像"), _T("警告"));
	}
}


void CPictureToAsciiDlg::OnQuit()
{
	// TODO: 在此添加命令处理程序代码
	CDialogEx::OnCancel();
}


void CPictureToAsciiDlg::OnOpenfile()
{
	// TODO: 在此添加命令处理程序代码
	CPictureToAsciiDlg::OnBnClickedOpenpic();
}


void CPictureToAsciiDlg::OnHelp()
{
	// TODO: 在此添加命令处理程序代码
	MessageBox(_T("1.具体使用说明请参见软件使用文档\n2.当字符画宽度超过1000、高度超过500或宽高比小于2:1时可能无法获得正常的预览效果\n3.字符集中仅允许输入英文与数字字符与特殊符号，暂不支持中文"),_T("注意事项"));
}


void CPictureToAsciiDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
}


//打开图像文件并初始化
void CPictureToAsciiDlg::OnBnClickedOpenpic()
{
	
	USES_CONVERSION;
	// TODO: 在此添加控件通知处理程序代码
	//利用CFileDialog选择文件
	CString defaultPath = CImageProcessor::GetDefaultPath();
	CFileDialog openFileDlg(TRUE, NULL, NULL, OFN_READONLY,
		_T("BMP文件(*.bmp)|*.bmp|JPG文件(*.jpg)|*.jpg|所有类型(*.*)|*.*||"));
	openFileDlg.m_ofn.lpstrInitialDir = defaultPath;   //默认路径
	if (openFileDlg.DoModal() == IDOK)
	{
		CString filename = openFileDlg.GetPathName();//获取想要打开的图像名称
		src = imread(W2A(filename));//使用opencv的库函数读取图像

		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);//设置编辑框可用
		GetDlgItem(IDC_EDIT2)->EnableWindow(TRUE);//设置编辑框可用
		GetDlgItem(IDC_PicPreview)->EnableWindow(TRUE);//设置按钮可用
		GetDlgItem(IDC_WordPreview)->EnableWindow(TRUE);//设置按钮可用
		GetDlgItem(IDC_SaveWord)->EnableWindow(TRUE);//设置按钮可用
		GetDlgItem(IDC_COMBO1)->EnableWindow(TRUE);//设置COMBO框可用

		myCombo.AddString(_T("#,.0123456789:;@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz$ "));
		myCombo.AddString(_T("@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,\"^`'. "));


		//设置编辑框初始值
		CString temp;
		temp.Format(_T("%d"), (int)static_cast<float>(src.cols));//int转CString,原图像的宽度
		GetDlgItem(IDC_EDIT1)->SetWindowTextW(temp);
		temp.Format(_T("%d"), (int)static_cast<float>(src.rows));//int转CString,原图像高度
		GetDlgItem(IDC_EDIT2)->SetWindowTextW(temp);

		GetDlgItem(IDC_FileName)->SetWindowTextW(_T("当前操作图像：") + filename);
	}
}


void CPictureToAsciiDlg::OnBnClickedPicpreview()
{
	// TODO: 在此添加控件通知处理程序代码
	CString WB, HB;//字符画宽度字符数和字符画高度字符数
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(WB);
	GetDlgItem(IDC_EDIT2)->GetWindowTextW(HB);

	int width = _ttoi(WB);
	//定义想要扩大或者缩小后的宽度
	int height = _ttoi(HB);
	//定义想要扩大或者缩小后的高度
	resize(src, image, Size(width, height));

	imshow("原图像", src);
	imshow("按字符画宽高放缩后图像", image);
	waitKey(0);
	

}

//预览字符画
void CPictureToAsciiDlg::OnBnClickedWordpreview()
{
	// TODO: 在此添加控件通知处理程序代码
	
	//获取字符集
	CString code;
	myCombo.GetWindowTextW(code);
	if (code.IsEmpty()) {
		MessageBox(_T("请先输入或从下拉菜单中选择字符集"), _T("警告"));
		return;
	}

	//根据字符画参数生成image
	CString WB, HB;//字符画宽度字符数和字符画高度字符数
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(WB);
	GetDlgItem(IDC_EDIT2)->GetWindowTextW(HB);

	int width = _ttoi(WB);
	//定义想要扩大或者缩小后的宽度
	int height = _ttoi(HB);
	//定义想要扩大或者缩小后的高度
	if (width <= 0 || height <= 0) {
		MessageBox( _T("字符画宽度或高度只能为正值"),_T("警告"));
		return;
	}

	if (width > 1000 || height > 500 ) {
		MessageBox(_T("当字符画宽度超过1000时或高度超过500时，预览将无法正常显示"), _T("警告"));
	}
	if (width / height < 2) {
		MessageBox(_T("当字符画宽高比小于2:1时，预览可能无法正常显示"), _T("警告"));
	}

	resize(src, image, Size(width, height));
	imwrite(".\\temp.bmp", image);//暂时缓存修改后图像
	CharactorPaintGenerate(code);
	CShowDlg newDlg;
	newDlg.DoModal();
	remove(".\\temp.bmp");
}

//字符画生成函数
void CharactorPaintGenerate(CString Ccode) {
	CString filename = _T(".\\temp.bmp");
	RGBQUAD* curColorData = NULL;   //图像数据32位
	int bmpW;                 //图像宽
	int bmpH;                 //图像高


	//CString Ccode = _T("#,.0123456789:;@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz$ ");
	//char code[] = "#,.0123456789:;@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz$ ";

	if (curColorData != NULL)
	{
		delete(curColorData);
		curColorData = NULL;
	}
	//获取图像数据
	curColorData = CImageProcessor::GetDataFromBMP(filename, bmpW, bmpH);

	int length;//字符集长度
	int gray;//灰度值
	float unit;//灰度映射标准
	
	CString temp;
	length = Ccode.GetLength();
	unit = (256.0 + 1) / length;
	charToSend = _T("");
	charToSave = _T("");
	WordW = bmpW;
	WordH = bmpH;

	for (int i = bmpH-1; i >= 0; i--) {
		for (int j = 0; j < bmpW; j++) {
			
			gray = int(0.2126 * curColorData[i * bmpW+j].rgbRed +
				0.7152 * curColorData[i * bmpW + j].rgbGreen +
				0.0722 * curColorData[i * bmpW + j].rgbBlue);  //计算灰度
			charToSend += Ccode.GetAt((int)(gray/unit));  //不同的灰度对应着不同的字符
			charToSave += Ccode.GetAt((int)(gray / unit));  //不同的灰度对应着不同的字符
		}
		charToSend += _T("\r\n");
		charToSave += _T("\n");
	}


}

void CPictureToAsciiDlg::OnBnClickedSaveword()
{
	// TODO: 在此添加控件通知处理程序代码

	//首先计算charToSend

		CString WB, HB;//字符画宽度字符数和字符画高度字符数
		GetDlgItem(IDC_EDIT1)->GetWindowTextW(WB);
		GetDlgItem(IDC_EDIT2)->GetWindowTextW(HB);

		int width = _ttoi(WB);
		//定义想要扩大或者缩小后的宽度
		int height = _ttoi(HB);
		//定义想要扩大或者缩小后的高度

		CString code;
		myCombo.GetWindowTextW(code);
		if (code.IsEmpty()) {
			MessageBox(_T("请先输入或从下拉菜单中选择字符集"), _T("警告"));
			return;
		}

		resize(src, image, Size(width, height));
		imwrite(".\\temp.bmp", image);//暂时缓存修改后图像
		CharactorPaintGenerate(code);
		remove(".\\temp.bmp");
	
	
	//利用CFileDialog打开文件
	CString defaultPath = CImageProcessor::GetDefaultPath();
	CFileDialog saveFileDlg(false, NULL, NULL, OFN_OVERWRITEPROMPT,
		_T("txt文件(*.txt)|所有类型(*.*)|*.*||"));
	saveFileDlg.m_ofn.lpstrInitialDir = defaultPath;   //默认路径
	if (saveFileDlg.DoModal() == IDOK)
	{
		CString filename = saveFileDlg.GetPathName();
		//给文件添加扩展名
		filename += _T(".txt");
		
		
		CStdioFile File;
		File.Open(filename, CFile::modeReadWrite | CFile::modeNoTruncate | CFile::modeCreate);
		File.SeekToEnd();
		File.WriteString(charToSave);
		File.Close();
		
	}
}
