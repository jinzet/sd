// ImageLoadObj.h: interface for the CImageLoadObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGELOADOBJ_H__5E7E553D_E3D9_4A3B_A25F_D81CB0228CF1__INCLUDED_)
#define AFX_IMAGELOADOBJ_H__5E7E553D_E3D9_4A3B_A25F_D81CB0228CF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "BaseObj.h"

#define IMAGETYPE_NONE 0
#define IMAGETYPE_BMP 1
#define IMAGETYPE_GIF 2
#define IMAGETYPE_PCX 3
#define IMAGETYPE_TGA 4
#define IMAGETYPE_JPG 5
#define IMAGETYPE_TIF 6

#define IMAGETYPE_FIRSTTYPE IMAGETYPE_BMP
#define IMAGETYPE_LASTTYPE IMAGETYPE_TIF

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*PFGetLastPicLibError)();
typedef int (*PFFileType)(const char *);

typedef HGLOBAL (*PFLoadBMP)(const char *);
typedef BOOL (*PFGetBMPInfo)(const char *, int *, int *, int *, int *, int *);
typedef BOOL (*PFSaveBMP)(const char *, HGLOBAL);

typedef HGLOBAL (*PFLoadGIF)(const char *);
typedef BOOL (*PFGetGIFInfo)(const char *, int *, int *, int *, int *, int *);
typedef BOOL (*PFSaveGIF)(const char *,HGLOBAL);

typedef HGLOBAL (*PFLoadPCX)(const char *);
typedef BOOL (*PFGetPCXInfo)(const char *, int *, int *, int *, int *, int *);
typedef BOOL (*PFSavePCX)(const char *,HGLOBAL);

typedef HGLOBAL (*PFLoadTGA)(const char *);
typedef BOOL (*PFGetTGAInfo)(const char *, int *, int *, int *, int *, int *);
typedef BOOL (*PFSaveTGA)(const char *,HGLOBAL);

typedef HGLOBAL (*PFLoadTIF)(const char *);
typedef BOOL (*PFGetTIFInfo)(const char *, int *, int *, int *, int *, int *);
typedef BOOL (*PFSaveTIF)(const char *,HGLOBAL);

typedef HGLOBAL (*PFLoadJPG)(const char *);
typedef BOOL (*PFGetJPGInfo)(const char *, int *, int *, int *, int *, int *);
typedef BOOL (*PFSaveJPG)(const char *,HGLOBAL,int);

typedef WORD (*PFMedianCut)(WORD Hist[], BYTE ColMap[][3], int);
typedef WORD (*PFPopularity)(unsigned char *pBits, int nBits, int nWidth, int nHeight, BYTE ColorMap[][3]);

#ifdef __cplusplus
}
#endif

class CImageLoadObj //: public CBaseObj  
{
	PFGetLastPicLibError pfnGetLastPicLibError;
	PFFileType pfnFileType;

	PFLoadBMP pfnLoadBMP;
	PFGetBMPInfo pfnGetBMPInfo;
	PFSaveBMP pfnSaveBMP;

	PFLoadGIF pfnLoadGIF;
	PFGetGIFInfo pfnGetGIFInfo;
	PFSaveGIF pfnSaveGIF;

	PFLoadPCX pfnLoadPCX;
	PFGetPCXInfo pfnGetPCXInfo;
	PFSavePCX pfnSavePCX;

	PFLoadTGA pfnLoadTGA;
	PFGetTGAInfo pfnGetTGAInfo;
	PFSaveTGA pfnSaveTGA;

	PFLoadTIF pfnLoadTIF;
	PFGetTIFInfo pfnGetTIFInfo;
	PFSaveTIF pfnSaveTIF;

	PFLoadJPG pfnLoadJPG;
	PFGetJPGInfo pfnGetJPGInfo;
	PFSaveJPG pfnSaveJPG;

	PFMedianCut pfnMedianCut;
	PFPopularity pfnPopularity;

	HINSTANCE hDll;
public:
	int		GetLastPicLibError();
	int		FileType(const char *);

	HGLOBAL LoadBMP(const char *);////////////
	BOOL	GetBMPInfo(const char *, int *, int *, int *, int *, int *);
	BOOL	SaveBMP(const char *, HGLOBAL);

	HGLOBAL LoadGIF(const char *);
	BOOL	GetGIFInfo(const char *, int *, int *, int *, int *, int *);
	BOOL	SaveGIF(const char *, HGLOBAL);////////////

	HGLOBAL LoadPCX(const char *);
	BOOL	GetPCXInfo(const char *, int *, int *, int *, int *, int *);
	BOOL	SavePCX(const char *, HGLOBAL);

	HGLOBAL LoadTGA(const char *);
	BOOL	GetTGAInfo(const char *, int *, int *, int *, int *, int *);
	BOOL	SaveTGA(const char *, HGLOBAL);

	HGLOBAL LoadTIF(const char *);//只能处理24位色的tif
	BOOL	GetTIFInfo(const char *, int *, int *, int *, int *, int *);
	BOOL	SaveTIF(const char *, HGLOBAL);

	HGLOBAL LoadJPG(const char *);
	BOOL	GetJPGInfo(const char *, int *, int *, int *, int *, int *);
	BOOL	SaveJPG(const char *, HGLOBAL,int);

	WORD MedianCut(WORD Hist[], BYTE ColMap[][3], int);
	WORD Popularity(unsigned char *pBits, int nBits, int nWidth, int nHeight, BYTE ColorMap[][3]);

	BOOL LoadDll();
	void FreeDll();
	CImageLoadObj();
	virtual ~CImageLoadObj();

};
extern CImageLoadObj m_gImageObj;

#endif // !defined(AFX_IMAGELOADOBJ_H__5E7E553D_E3D9_4A3B_A25F_D81CB0228CF1__INCLUDED_)
