// ImageLoadObj.cpp: implementation of the CImageLoadObj class.
//
// 图像处理模块（使用动态连接库）
// 将ImageLoad.dll的使用封装起来，可以将ImageLoad.dll改成其他名称使用
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageLoadObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
CImageLoadObj m_gImageObj;
//char JZT_IMGLOADOBJ_NAME[]="ImageLoad.dll";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageLoadObj::CImageLoadObj()
{
//	SetValue(JZT_IMGLOADOBJ,JZT_IMGLOADOBJ_NAME);

	pfnGetLastPicLibError = NULL;
	pfnFileType=NULL;

	pfnLoadBMP=NULL;
	pfnGetBMPInfo=NULL;
	pfnSaveBMP=NULL;

	pfnLoadGIF=NULL;
	pfnGetGIFInfo=NULL;
	pfnSaveGIF=NULL;

	pfnLoadPCX=NULL;
	pfnGetPCXInfo=NULL;
	pfnSavePCX=NULL;

	pfnLoadTGA=NULL;
	pfnGetTGAInfo=NULL;
	pfnSaveTGA=NULL;

	pfnLoadTIF=NULL;
	pfnGetTIFInfo=NULL;
	pfnSaveTIF=NULL;

	pfnLoadJPG=NULL;
	pfnGetJPGInfo=NULL;
	pfnSaveJPG=NULL;

	pfnMedianCut=NULL;
	pfnPopularity=NULL;

	hDll = NULL;
}

CImageLoadObj::~CImageLoadObj()
{
	FreeDll();
}

BOOL CImageLoadObj::LoadDll()
{
	if(hDll)return FALSE;

	hDll=LoadLibrary("ImageLoad.dll");
	if(INVALID_HANDLE_VALUE==hDll)
	{
		//AfxMessageBox("load failed");
		hDll = NULL;
		return FALSE;
	}
	pfnGetLastPicLibError=(PFGetLastPicLibError)GetProcAddress(hDll,"GetLastPicLibError");
	pfnFileType=(PFFileType)GetProcAddress(hDll,"FileType");

	pfnLoadBMP=(PFLoadBMP)GetProcAddress(hDll,"LoadBMP");
	pfnLoadGIF=(PFLoadGIF)GetProcAddress(hDll,"LoadGIF");
	pfnLoadPCX=(PFLoadPCX)GetProcAddress(hDll,"LoadPCX");
	pfnLoadTGA=(PFLoadTGA)GetProcAddress(hDll,"LoadTGA");
	pfnLoadTIF=(PFLoadTIF)GetProcAddress(hDll,"LoadTIF");
	pfnLoadJPG=(PFLoadJPG)GetProcAddress(hDll,"LoadJPG");

	pfnSaveBMP=(PFSaveBMP)GetProcAddress(hDll,"SaveBMP");
	pfnSaveGIF=(PFSaveGIF)GetProcAddress(hDll,"SaveGIF");
	pfnSavePCX=(PFSavePCX)GetProcAddress(hDll,"SavePCX");
	pfnSaveTGA=(PFSaveTGA)GetProcAddress(hDll,"SaveTGA");
	pfnSaveTIF=(PFSaveTIF)GetProcAddress(hDll,"SaveTIF");
	pfnSaveJPG=(PFSaveJPG)GetProcAddress(hDll,"SaveJPG");

	pfnGetBMPInfo=(PFGetBMPInfo)GetProcAddress(hDll,"GetBMPInfo");
	pfnGetGIFInfo=(PFGetGIFInfo)GetProcAddress(hDll,"GetGIFInfo");
	pfnGetPCXInfo=(PFGetPCXInfo)GetProcAddress(hDll,"GetPCXInfo");
	pfnGetTGAInfo=(PFGetTGAInfo)GetProcAddress(hDll,"GetTGAInfo");
	pfnGetTIFInfo=(PFGetTIFInfo)GetProcAddress(hDll,"GetTIFInfo");
	pfnGetJPGInfo=(PFGetJPGInfo)GetProcAddress(hDll,"GetJPGInfo");

	pfnMedianCut=(PFMedianCut)GetProcAddress(hDll,"MedianCut");
	pfnPopularity=(PFPopularity)GetProcAddress(hDll,"Popularity");
	return TRUE;
}
void CImageLoadObj::FreeDll()
{
	if(hDll)
	{
		FreeLibrary(hDll);
		hDll=NULL;
	}
}

int		CImageLoadObj::GetLastPicLibError()
{
	if(NULL==pfnGetLastPicLibError)return -1;
	return pfnGetLastPicLibError();
}
int		CImageLoadObj::FileType(const char *filename)
{
	if(NULL==pfnFileType)return -1;
	return pfnFileType(filename);
}

HGLOBAL CImageLoadObj::LoadBMP(const char *filename)
{
	if(NULL==pfnLoadBMP)return NULL;
	HGLOBAL hr = pfnLoadBMP(filename);//调用动态连接库中的函数
	return hr;
}
HGLOBAL CImageLoadObj::LoadGIF(const char *filename)
{
	if(NULL==pfnLoadGIF)return NULL;
	HGLOBAL hr = pfnLoadGIF(filename);//调用动态连接库中的函数
	return hr;
}
HGLOBAL CImageLoadObj::LoadPCX(const char *filename)
{
	if(NULL==pfnLoadPCX)return NULL;
	HGLOBAL hr = pfnLoadPCX(filename);//调用动态连接库中的函数
	return hr;
}
HGLOBAL CImageLoadObj::LoadTGA(const char *filename)
{
	if(NULL==pfnLoadTGA)return NULL;
	HGLOBAL hr = pfnLoadTGA(filename);//调用动态连接库中的函数
	return hr;
}
HGLOBAL CImageLoadObj::LoadTIF(const char *filename)
{
	if(NULL==pfnLoadTIF)return NULL;
	HGLOBAL hr = pfnLoadTIF(filename);//调用动态连接库中的函数
	return hr;
}
HGLOBAL CImageLoadObj::LoadJPG(const char *filename)
{
	if(NULL==pfnLoadJPG)return NULL;
	HGLOBAL hr = pfnLoadJPG(filename);//调用动态连接库中的函数
	return hr;
}


BOOL	CImageLoadObj::SaveBMP(const char *filename,HGLOBAL hr)
{
	if(NULL==pfnSaveBMP)return FALSE;
	return pfnSaveBMP(filename,hr);
}
BOOL	CImageLoadObj::SaveGIF(const char *filename,HGLOBAL hr)
{
	if(NULL==pfnSaveGIF)return FALSE;
	return pfnSaveGIF(filename,hr);
}
BOOL	CImageLoadObj::SavePCX(const char *filename,HGLOBAL hr)
{
	if(NULL==pfnSavePCX)return FALSE;
	return pfnSavePCX(filename,hr);
}
BOOL	CImageLoadObj::SaveTGA(const char *filename,HGLOBAL hr)
{
	if(NULL==pfnSaveTGA)return FALSE;
	return pfnSaveTGA(filename,hr);
}
BOOL	CImageLoadObj::SaveTIF(const char *filename,HGLOBAL hr)
{
	if(NULL==pfnSaveTIF)return FALSE;
	return pfnSaveTIF(filename,hr);
}
BOOL	CImageLoadObj::SaveJPG(const char *filename,HGLOBAL hr,int ii)
{
	if(NULL==pfnSaveJPG)return FALSE;
	return pfnSaveJPG(filename,hr,ii);
}


BOOL	CImageLoadObj::GetBMPInfo(const char *filename, int *w, int *h, int *planes, int *bits, int *colors)
{
	if(NULL==pfnGetBMPInfo)return FALSE;
	return pfnGetBMPInfo(filename,w,h,planes,bits,colors);
}
BOOL	CImageLoadObj::GetGIFInfo(const char *filename, int *w, int *h, int *planes, int *bits, int *colors)
{
	if(NULL==pfnGetGIFInfo)return FALSE;
	return pfnGetGIFInfo(filename,w,h,planes,bits,colors);
}
BOOL	CImageLoadObj::GetPCXInfo(const char *filename, int *w, int *h, int *planes, int *bits, int *colors)
{
	if(NULL==pfnGetPCXInfo)return FALSE;
	return pfnGetPCXInfo(filename,w,h,planes,bits,colors);
}
BOOL	CImageLoadObj::GetTGAInfo(const char *filename, int *w, int *h, int *planes, int *bits, int *colors)
{
	if(NULL==pfnGetTGAInfo)return FALSE;
	return pfnGetTGAInfo(filename,w,h,planes,bits,colors);
}
BOOL	CImageLoadObj::GetTIFInfo(const char *filename, int *w, int *h, int *planes, int *bits, int *colors)
{
	if(NULL==pfnGetTIFInfo)return FALSE;
	return pfnGetTIFInfo(filename,w,h,planes,bits,colors);
}
BOOL	CImageLoadObj::GetJPGInfo(const char *filename, int *w, int *h, int *planes, int *bits, int *colors)
{
	if(NULL==pfnGetJPGInfo)return FALSE;
	return pfnGetJPGInfo(filename,w,h,planes,bits,colors);
}

WORD CImageLoadObj::MedianCut(WORD History[], BYTE ColMap[][3], int MaxColors)
{
	if(NULL==pfnMedianCut)return 0;
	return pfnMedianCut(History,ColMap,MaxColors);
}
WORD CImageLoadObj::Popularity(unsigned char *pBits, int nBits, int nWidth, int nHeight, BYTE ColorMap[][3])
{
	if(NULL==pfnPopularity)return 0;
	return pfnPopularity(pBits,nBits,nWidth,nHeight,ColorMap);
}
