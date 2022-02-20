// CShowDlg.cpp: 实现文件
//

#include "pch.h"
#include "PictureToAscii.h"
#include "CShowDlg.h"
#include "afxdialogex.h"

extern CString charToSend;//传递的字符串
extern int WordH;//传递的字符串高度
extern int WordW;//传递的字符串宽度

// CShowDlg 对话框

IMPLEMENT_DYNAMIC(CShowDlg, CDialogEx)

CShowDlg::CShowDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CShowDlg::~CShowDlg()
{
}

void CShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowDlg, CDialogEx)
END_MESSAGE_MAP()


// CShowDlg 消息处理程序
void CShowDlg::OnPaint()
{
    
}

BOOL CShowDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    //charToSend = _T("Test");

    LOGFONT lf; //定义等宽登高字体结构
    lf.lfWeight = 1; //字体磅数


    lf.lfHeight = (1000 / WordW < 1)? 1: 1000 / WordW; //字体高度
    lf.lfWidth = (1000 / WordW < 1) ? 1 : 1000 / WordW; //字体宽度
    //以下代码理论上是上方两行代码的优化形式，因其可以通过同时计算预览字符画的宽和高来调整字体大小
    //但是实际测试中发现，即使调试的时候，字体的宽和高都是相等的，最后两者显示出来的效果却是不同的
    //对此我也很奇怪，但既然这代码就是针对用户违规操作做的优化，最后就放弃优化了（破防了）。
    /*if (1000 / WordW < 1 || 500 / WordH < 1) {
        lf.lfHeight = lf.lfWeight = 1;
    }
    else if (1000 / WordW <= 500 / WordH) {
        lf.lfHeight = lf.lfWeight = 1000 / WordW;
    }
    else if (1000 / WordW > 500 / WordH) {
        lf.lfHeight =  lf.lfWeight = 500 / WordH;
    }*/
    lf.lfUnderline = FALSE; //无下划线
    lf.lfStrikeOut = FALSE; //无删除线
    lf.lfItalic = FALSE; //非斜体
    lf.lfEscapement = 0;
    lf.lfCharSet = DEFAULT_CHARSET; //使用缺省字符集
    

    CFont myFont; //定义字体对象
    myFont.CreateFontIndirect(&lf); //创建逻辑字体


    GetDlgItem(IDC_EDIT1)->SetFont(&myFont);
    SetDlgItemText(IDC_EDIT1, _T(""));
    SetDlgItemText(IDC_EDIT1,charToSend);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}
