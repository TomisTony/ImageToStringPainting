
// PictureToAsciiDlg.h: 头文件
//

#pragma once


// CPictureToAsciiDlg 对话框
class CPictureToAsciiDlg : public CDialogEx
{
// 构造
public:
	CPictureToAsciiDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PICTURETOASCII_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
//	afx_msg void OnDisplayHelp();
	afx_msg void OnSave();
	afx_msg void OnQuit();
	afx_msg void OnOpenfile();
	afx_msg void OnHelp();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedPreview();
	afx_msg void OnBnClickedOpenpic();
	afx_msg void OnBnClickedPicpreview();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedWordpreview();
	afx_msg void OnBnClickedSaveword();
	CComboBox myCombo;
};
