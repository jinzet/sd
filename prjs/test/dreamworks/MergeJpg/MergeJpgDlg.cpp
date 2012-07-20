
// MergeJpgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MergeJpg.h"
#include "MergeJpgDlg.h"
#include "dib.h"
#include "myjpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMergeJpgDlg dialog




CMergeJpgDlg::CMergeJpgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMergeJpgDlg::IDD, pParent)
	, mFmtStr(_T(""))
	, mRow0(_T(""))
	, mRow1(_T(""))
	, mCol0(_T(""))
	, mCol1(_T(""))
	, mMergedFile(_T(""))
	, mMergedWidth(0)
	, mMergedHeight(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMergeJpgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mFmtStr);
	DDX_Text(pDX, IDC_EDIT2, mRow0);
	DDX_Text(pDX, IDC_EDIT3, mRow1);
	DDX_Text(pDX, IDC_EDIT4, mCol0);
	DDX_Text(pDX, IDC_EDIT5, mCol1);
	DDX_Text(pDX, IDC_EDIT6, mMergedFile);
	DDX_Text(pDX, IDC_EDIT7, mMergedWidth);
	DDX_Text(pDX, IDC_EDIT8, mMergedHeight);
}

BEGIN_MESSAGE_MAP(CMergeJpgDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CMergeJpgDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMergeJpgDlg message handlers

BOOL CMergeJpgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMergeJpgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMergeJpgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMergeJpgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BYTE* pBits=0;

// 将文件合并到指定的行和列
void MergeAFile(CMYJPG* pBigBmp,char* filename,int rows,int cols,int width,int height)
{
	CMYJPG jpg;
	jpg.Load(filename);
	
	ULONGLONG ww = width*256*3;
	ULONGLONG hh = rows*256;
	for(int j = 0;j<256;j++)
	{
		jpg.CopyBitsRow(pBits,256,0,256-j-1);
		pBigBmp->SeekToBmpData( (hh+j)*ww + cols*256*3 );
		pBigBmp->WriteBigBmp(pBits,256*3);
	}
	
}

void MerageAll()
{
	char filen[255];
	ULONGLONG tWidth=60;//303   75-149  75  64
	ULONGLONG tHeight=61;//AA-CG  AA-AZ 26 BA-BZ 26 CA-CG 7 ==59    HX 7*26+24=206  CC 2*26+2=54 CI 60 CK  CJ 61
	ULONGLONG totalsize = tWidth*tHeight*256*256*3L+1024L;

	CMYJPG pdib;
	pdib.Create(256,256,24);
	if(pdib.OpenBigBmp("h:/xan-30m.bmp")){
		pBits = (BYTE*)malloc(256*3);
		pdib.WriteBigBmpHeader(tWidth,tHeight);
		pdib.WriteBigBmp(pBits,128,totalsize);
		pdib.SeekToBmpData();
		for(int row=0;row<tHeight;row++)
		{
			for(int col=0;col<tWidth;col++)
			{
				int rrow=row;
				int ccol=col+1;
				//sprintf(filen,"G:/西飞项目/最新数据/20120509航线256图/xan-km/xan-km/xan-km_%c%c_%03d.jpg",'A'+rrow/26,'A'+rrow%26,ccol);
				sprintf(filen,"G:/西飞项目/最新数据/20120507四个机场256小图/xian/xian/xan-30m_%c%c_%02d.jpg",'A'+rrow/26,'A'+rrow%26,ccol);
				MergeAFile(&pdib,filen, tHeight-row-1, col, tWidth, tHeight);
				TRACE("finished %d %d\n",row,col);
			}
		}
		TRACE("finished all");
		pdib.CloseBigBmp();
		free(pBits);
	}
}

void CMergeJpgDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
	UpdateData();
	MerageAll();
	MessageBox("ok");
	//int rows=10;
	//int cols=10;
	// 创建bmp文件
	//CDIB pdib;
	//pdib.Load("G:/西飞项目/最新数据/20120509航线256图/cq-shh/cq-shh/cq-shh_AA_001.jpg");
	//int w = pdib.GetWidth();
	//int h = pdib.GetHeight();

	//CMYJPG jpg;
	//jpg.Load("G:/西飞项目/最新数据/20120509航线256图/cq-shh/cq-shh/cq-shh_BE_058.jpg");
	//int w = jpg.DibWidth();
	//int h = jpg.DibHeight();
	//int sw = jpg.StorageWidth();
	//BYTE * pBits;
	//pBits = (BYTE*)jpg.GetBitsAddress();

	//int bigw=512,bigh=256;
	//CMYJPG pdib;
	//pdib.Create(256,256,24);
	////pdib.Save("h:/ttt.bmp");
	//if(pdib.OpenBigBmp("h:/ttt.bmp")){
	//	BYTE* pBits = (BYTE*)malloc(256*3);
	//	pdib.WriteBigBmpHeader();
	//	pdib.WriteBigBmp(pBits,128,1*1024*1024);
	//	pdib.SeekToBmpData();
	//	for(int cols = 0;cols<h;cols++)
	//	{
	//		jpg.CopyBitsRow(pBits,256,0,h-cols-1);
	//		pdib.SeekToBmpData(cols*bigw*3);
	//		pdib.WriteBigBmp(pBits,256*3);
	//		//pdib.WriteBigBmp(pBits,256*3);
	//	}
	//	//pdib.WriteBigBmp(pBits,128,1*1024*1024);
	//	pdib.CloseBigBmp();
	//	free(pBits);
	//}
}
