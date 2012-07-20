
// dreamworksView.cpp : implementation of the CdreamworksView class
//

#include "stdafx.h"
#include "dreamworks.h"

#include "dreamworksDoc.h"
#include "dreamworksView.h"
#include "OpenGL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COpenGL ogl;
// CdreamworksView

IMPLEMENT_DYNCREATE(CdreamworksView, CView)

BEGIN_MESSAGE_MAP(CdreamworksView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CdreamworksView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CdreamworksView construction/destruction

CdreamworksView::CdreamworksView()
{
	// TODO: add construction code here

}

CdreamworksView::~CdreamworksView()
{
}

BOOL CdreamworksView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CdreamworksView drawing

void CdreamworksView::OnDraw(CDC* /*pDC*/)
{
	CdreamworksDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	ogl.Draw();
}


// CdreamworksView printing


void CdreamworksView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CdreamworksView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CdreamworksView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CdreamworksView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CdreamworksView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CdreamworksView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CdreamworksView diagnostics

#ifdef _DEBUG
void CdreamworksView::AssertValid() const
{
	CView::AssertValid();
}

void CdreamworksView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CdreamworksDoc* CdreamworksView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CdreamworksDoc)));
	return (CdreamworksDoc*)m_pDocument;
}
#endif //_DEBUG


// CdreamworksView message handlers

int CdreamworksView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	ogl.SetHWnd(m_hWnd);
	ogl.Initialize();
	return 0;
}

void CdreamworksView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	ogl.Clear();
}

BOOL CdreamworksView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;

	return CView::OnEraseBkgnd(pDC);
}

void CdreamworksView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	ogl.InitSize(cx,cy);
}
