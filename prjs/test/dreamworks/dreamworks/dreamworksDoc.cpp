
// dreamworksDoc.cpp : implementation of the CdreamworksDoc class
//

#include "stdafx.h"
#include "dreamworks.h"

#include "dreamworksDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CdreamworksDoc

IMPLEMENT_DYNCREATE(CdreamworksDoc, CDocument)

BEGIN_MESSAGE_MAP(CdreamworksDoc, CDocument)
END_MESSAGE_MAP()


// CdreamworksDoc construction/destruction

CdreamworksDoc::CdreamworksDoc()
{
	// TODO: add one-time construction code here

}

CdreamworksDoc::~CdreamworksDoc()
{
}

BOOL CdreamworksDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CdreamworksDoc serialization

void CdreamworksDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CdreamworksDoc diagnostics

#ifdef _DEBUG
void CdreamworksDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CdreamworksDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CdreamworksDoc commands
