
// BlurDetectorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BlurDetector.h"
#include "BlurDetectorDlg.h"
#include "afxdialogex.h"
#include "blurDetect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBlurDetectorDlg 对话框



CBlurDetectorDlg::CBlurDetectorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBlurDetectorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBlurDetectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBlurDetectorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_FILE_OPEN, &CBlurDetectorDlg::OnFileOpen)
	ON_COMMAND(ID_BLUR_DETECT, &CBlurDetectorDlg::OnBlurDetect)
END_MESSAGE_MAP()


// CBlurDetectorDlg 消息处理程序

BOOL CBlurDetectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	calcuXabs();
	cv::ocl::setUseOpenCL(true);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CBlurDetectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBlurDetectorDlg::OnPaint()
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
	else if (!m_Image.IsNull())
	{
		CRect rect;
		GetClientRect(&rect);
		CPaintDC dc(this);
		m_Image.Draw(dc.GetSafeHdc(), rect);
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBlurDetectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CBlurDetectorDlg::OnFileOpen()
{
	CFileDialog dlg(TRUE);
	if (IDOK == dlg.DoModal())
	{
		CString path = dlg.GetPathName();
		m_Image.Destroy();
		HRESULT hr = S_OK;
		try {
			hr = m_Image.Load(path);
		} catch (...){ hr = S_FALSE; }
		Invalidate(TRUE);
	}
}


void CBlurDetectorDlg::OnBlurDetect()
{
	if (!m_Image.IsNull())
	{
		int w = m_Image.GetWidth(), h = m_Image.GetHeight(), 
			C = m_Image.GetBPP() / 8, L = m_Image.GetPitch();
		if (1 == C || 3 == C || 4 == C)
		{
			BYTE *addr = (BYTE*)m_Image.GetBits() + (h-1) * L;
			Mat m(h, w, CV_MAKETYPE(CV_8U, C), addr, abs(L)), gray;
			if (1 != C) cvtColor(m, gray, CV_RGB2GRAY);
			char result[64] = { 0 };
			sprintf_s(result, "%f", BlurDetect(1 == C ? m : gray));
			MessageBox(CString(result), _T("Info"), MB_ICONINFORMATION);
		}
	}
}


BOOL CBlurDetectorDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE: case VK_RETURN:
			return TRUE;
		case VK_F2:
			OnFileOpen();
			return TRUE;
		case VK_F5:
			OnBlurDetect();
			return TRUE;
		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
