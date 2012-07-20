// dib.cpp : implementation file
//
//

#include "stdafx.h"
#include "dib.h"
#include "math.h"
#include "imageload.h"
#include <direct.h>
#include "imageloadobj.h"

#define ALLOC(s) malloc(s)
#define FREE(p) free(p)

IMPLEMENT_SERIAL(CDIB, CObject, 0 /* Schema number */ )
// Create a small DIB here so m_pBMI and m_pBits are always valid.
CDIB::CDIB()
{
	m_pBMI = NULL;
	m_pBits = NULL;
	m_bMyBits = TRUE;
	IsBkgnd=TRUE;
	Create(128,64,24);
	m_hDib=0;
	m_nQuality = 50;
	m_dx = 0;
	m_dy = 0;
	nCol=1;
	nRow=1;
	CurCol=0;
	CurRow=0;
	m_bTransIndex = 0xffffffff ;
	m_hDib=0;

	//pActionList=NULL;
	//pOperateAct=NULL;
	//pActionList=new CActionList;
	//pOperateAct=new CActionList;
	//step=1;
}

CDIB::~CDIB()
{
    // Free the memory.
	if(m_hDib)
	{
		GlobalUnlock(m_hDib);
		GlobalFree(m_hDib);
	}
	else
	{
    if (m_pBMI != NULL) free(m_pBMI);
    if (m_bMyBits && (m_pBits != NULL)) free(m_pBits);
	}


	//if(pActionList!=NULL)
	//{
	//	pActionList->ClearAll();
	//	delete pActionList;
	//	pActionList=NULL;
	//}
	//if(pOperateAct!=NULL)
	//{
	//	pOperateAct->ClearAll();
	//	delete pOperateAct;
	//	pOperateAct=NULL;
	//}
}

/////////////////////////////////////////////////////////////////////////////
// CDIB serialization

// We don't support this yet.
/////////////////////////////////////////////////////////////////////////////
// Private functions

static BOOL IsWinDIB(BITMAPINFOHEADER *pBIH)
{
    if (((BITMAPCOREHEADER*)pBIH)->bcSize == sizeof(BITMAPCOREHEADER)) {
        return FALSE;
    }
    return TRUE;
}

static int NumDIBColorEntries(BITMAPINFO* pBmpInfo) 
{
    BITMAPINFOHEADER* pBIH;
    BITMAPCOREHEADER* pBCH;
    int iColors, iBitCount;


    pBIH = &(pBmpInfo->bmiHeader);
    pBCH = (BITMAPCOREHEADER*) pBIH;

    // Start off by assuming the color table size from
    // the bit-per-pixel field.
    if (IsWinDIB(pBIH)) {
        iBitCount = pBIH->biBitCount;
    } else {
        iBitCount = pBCH->bcBitCount;
    }

    switch (iBitCount) {
    case 1:
        iColors = 2;
        break;
    case 4:
        iColors = 16;
        break;
    case 8:
        iColors = 256;
        break;
    default:
        iColors = 0;
        break;
    }

    // If this is a Windows DIB, then the color table length
    // is determined by the biClrUsed field if the value in
    // the field is nonzero.
    if (IsWinDIB(pBIH) && (pBIH->biClrUsed != 0)) {
        iColors = pBIH->biClrUsed;
    }

    // BUGFIX 18 Oct 94 NigelT
    // Make sure the value is reasonable since some products
    // will write out more then 256 colors for an 8 bpp DIB!!!
    int iMax = 0;
    switch (iBitCount) {
    case 1:
        iMax = 2;
        break;
    case 4:
        iMax = 16;
        break;
    case 8:
        iMax = 256;
        break;
    default:
        iMax = 0;
        break;
    }
    if (iMax) {
        if (iColors > iMax) {
            iColors = iMax;
        }
    }

    return iColors;
}


/////////////////////////////////////////////////////////////////////////////
// CDIB commands

BOOL CDIB::Create(int iWidth, int iHeight,int tempBitCount)
{
    // Delete any existing stuff.
	if(m_pBMI != NULL)
		free(m_pBMI);
    if(m_bMyBits && (m_pBits != NULL))
		free(m_pBits);

    // Allocate memory for the header.
	long lBitsSize ;
	if(tempBitCount==8)
	{
		//BITMAPINFO *m_pBMI
		m_pBMI = (BITMAPINFO*) malloc(sizeof(BITMAPINFOHEADER) + 256* sizeof(RGBQUAD));
		lBitsSize = ((iWidth + 3) & ~3) * iHeight;
	}
	else
	{
		m_pBMI = (BITMAPINFO*) malloc(sizeof(BITMAPINFOHEADER)
		                              +  sizeof(RGBQUAD));	
		long bitswidth = iWidth * tempBitCount;
		lBitsSize = (((bitswidth + 31) & (~31))/8) *iHeight;
	}
    if (!m_pBMI) 
	{
        TRACE("Out of memory for DIB header");
        return FALSE;
    }

    // Allocate memory for the bits (DWORD aligned).
    m_pBits = (BYTE*)malloc(lBitsSize);
    if (!m_pBits) 
	{
        TRACE("Out of memory for DIB bits");
        free(m_pBMI);
        m_pBMI = NULL;
        return FALSE;
    }
    m_bMyBits = TRUE;

    // Fill in the header info.
    BITMAPINFOHEADER* pBI = (BITMAPINFOHEADER*) m_pBMI;
    pBI->biSize = sizeof(BITMAPINFOHEADER);
    pBI->biWidth = iWidth;
    pBI->biHeight = iHeight;
    pBI->biPlanes = 1;
    pBI->biBitCount = tempBitCount;
    pBI->biCompression = BI_RGB;
    pBI->biSizeImage = 0;
    pBI->biXPelsPerMeter = 0;
    pBI->biYPelsPerMeter = 0;
    pBI->biClrUsed = 0;
    pBI->biClrImportant = 0;

    // Create an arbitrary color table (gray scale).
	if(tempBitCount==8)
	{
	    RGBQUAD* prgb = GetClrTabAddress();
	    for (int i = 0; i < 256; i++) 
		{
	        prgb->rgbBlue = prgb->rgbGreen = prgb->rgbRed = (BYTE) i;
	        prgb->rgbReserved = 0;
	        prgb++;
	    }
	}
    // Set all the bits to a known state (black).
    memset(m_pBits, 255, lBitsSize);

    return TRUE;
}



// Create a new empty 8bpp DIB with a 256 entry color table.
BOOL CDIB::Create(int iWidth, int iHeight)
{
    // Delete any existing stuff.
    if (m_pBMI != NULL) free(m_pBMI);
    if (m_bMyBits && (m_pBits != NULL)) free(m_pBits);

    // Allocate memory for the header.
    m_pBMI = (BITMAPINFO*) malloc(sizeof(BITMAPINFOHEADER)
                                  + 256 * sizeof(RGBQUAD));
    if (!m_pBMI) {
        return FALSE;
    }

    // Allocate memory for the bits (DWORD aligned).
    int iBitsSize = ((iWidth + 3) & ~3) * iHeight;
    m_pBits = (BYTE*)malloc(iBitsSize);
    if (!m_pBits) {
        free(m_pBMI);
        m_pBMI = NULL;
        return FALSE;
    }
    m_bMyBits = TRUE;

    // Fill in the header info.
    BITMAPINFOHEADER* pBI = (BITMAPINFOHEADER*) m_pBMI;
    pBI->biSize = sizeof(BITMAPINFOHEADER);
    pBI->biWidth = iWidth;
    pBI->biHeight = iHeight;
    pBI->biPlanes = 1;
    pBI->biBitCount = 8;
    pBI->biCompression = BI_RGB;
    pBI->biSizeImage = 0;
    pBI->biXPelsPerMeter = 0;
    pBI->biYPelsPerMeter = 0;
    pBI->biClrUsed = 0;
    pBI->biClrImportant = 0;

    // Create an arbitrary color table (gray scale).
    RGBQUAD* prgb = GetClrTabAddress();
    for (int i = 0; i < 256; i++) {
        prgb->rgbBlue = prgb->rgbGreen = prgb->rgbRed = (BYTE) i;
        prgb->rgbReserved = 0;
        prgb++;
    }

    // Set all the bits to a known state (black).
    memset(m_pBits, 0, iBitsSize);

    return TRUE;
}

// Create a CDIB structure from existing header and bits. The DIB
// won't delete the bits and makes a copy of the header.
BOOL CDIB::Create(BITMAPINFO* pBMI, BYTE* pBits)
{

    if (m_pBMI != NULL) free(m_pBMI);
    m_pBMI = (BITMAPINFO*) malloc(sizeof(BITMAPINFOHEADER)
                                   + 256 * sizeof(RGBQUAD));
    
    // Note: This will probably fail for < 256 color headers.
    memcpy(m_pBMI, pBMI, sizeof(BITMAPINFOHEADER)+
             NumDIBColorEntries(pBMI) * sizeof(RGBQUAD));
  
//  	m_pBMI=pBMI;
    if (m_bMyBits && (m_pBits != NULL)) free(m_pBits);
    m_pBits = pBits;
    m_bMyBits = FALSE; // We can't delete the bits.
    return TRUE;
}

// Load a DIB from an open file.
BOOL CDIB::Load(CFile* fp)
{
    BITMAPINFO* pInfo = NULL;
    BYTE* pBits = NULL;

	int iInfoSize;
	DWORD iBitsSize;
    // Get the current file position.
    DWORD dwFileStart = fp->GetPosition();

    // Read the file header to get the file size and to
    // find where the bits start in the file.
    BITMAPFILEHEADER BmpFileHead;
    DWORD iBytes;
    iBytes = fp->Read(&BmpFileHead, sizeof(BmpFileHead));
    if (iBytes != sizeof(BmpFileHead)) 
	{
        TRACE("Failed to read file header");
        goto $ERROR;
    }

    // Check that we have the magic 'BM' at the start.
    if ((BmpFileHead.bfType != 0x4D42) && (BmpFileHead.bfType != 0x9608)) 
	{
        TRACE("Not a bitmap file");
        goto $ERROR;
    }

    // Make a wild guess that the file is in Windows DIB
    // format and read the BITMAPINFOHEADER. If the file turns
    // out to be a PM DIB file we'll convert it later.
    BITMAPINFOHEADER BmpInfoHead;
    iBytes = fp->Read(&BmpInfoHead, sizeof(BmpInfoHead)); 
    if (iBytes != sizeof(BmpInfoHead)) 
	{
        TRACE("Failed to read BITMAPINFOHEADER");
        goto $ERROR;
    }

    // Check that we got a real Windows DIB file.
    if (BmpInfoHead.biSize != sizeof(BITMAPINFOHEADER)) 
	{
        if (BmpInfoHead.biSize != sizeof(BITMAPCOREHEADER)) 
		{
            TRACE(" File is not Windows or PM DIB format");
            goto $ERROR;
        }
        // Back up the file pointer and read the BITMAPCOREHEADER
        // and create the BITMAPINFOHEADER from it.
        fp->Seek(dwFileStart + sizeof(BITMAPFILEHEADER), CFile::begin);
        BITMAPCOREHEADER BmpCoreHdr;
        iBytes = fp->Read(&BmpCoreHdr, sizeof(BmpCoreHdr)); 
        if (iBytes != sizeof(BmpCoreHdr)) 
		{
            TRACE("Failed to read BITMAPCOREHEADER");
            goto $ERROR;
        }

        BmpInfoHead.biSize = sizeof(BITMAPINFOHEADER);
        BmpInfoHead.biWidth = (int) BmpCoreHdr.bcWidth;
        BmpInfoHead.biHeight = (int) BmpCoreHdr.bcHeight;
        BmpInfoHead.biPlanes = BmpCoreHdr.bcPlanes;
        BmpInfoHead.biBitCount = BmpCoreHdr.bcBitCount;
        BmpInfoHead.biCompression = BI_RGB;
        BmpInfoHead.biSizeImage = 0;
        BmpInfoHead.biXPelsPerMeter = 0;
        BmpInfoHead.biYPelsPerMeter = 0;
        BmpInfoHead.biClrUsed = 0;
        BmpInfoHead.biClrImportant = 0;

		int iColors = NumDIBColorEntries((LPBITMAPINFO) &BmpInfoHead);
		int iColorTableSize = iColors * sizeof(RGBQUAD);

		// Always allocate enough room for colour entries.
		iInfoSize = sizeof(BITMAPINFOHEADER) + iColors * sizeof(RGBQUAD);

		// Allocate the memory for the header.
		LPBITMAPINFO pInfo = (LPBITMAPINFO) malloc(iInfoSize);
		if (!pInfo) 
		{
			TRACE("Out of memory for DIB header");
			goto $ERROR;
		}
		// Copy the header we already have.
		memcpy(pInfo, &BmpInfoHead, sizeof(BITMAPINFOHEADER));
        // Read each PM color table entry in turn and convert it
        // to Win DIB format as we go.
        LPRGBQUAD lpRGB;
        lpRGB = (LPRGBQUAD) ((LPBYTE) pInfo + sizeof(BITMAPINFOHEADER));
        int i;
        RGBTRIPLE rgbt;
        for (i=0; i<iColors; i++) 
		{
            iBytes = fp->Read(&rgbt, sizeof(RGBTRIPLE));
            if (iBytes != sizeof(RGBTRIPLE)) 
			{
                TRACE("Failed to read RGBTRIPLE");
                goto $ERROR;
            }
            lpRGB->rgbBlue = rgbt.rgbtBlue;
            lpRGB->rgbGreen = rgbt.rgbtGreen;
            lpRGB->rgbRed = rgbt.rgbtRed;
            lpRGB->rgbReserved = 0;
            lpRGB++;
        }
    }
	else
	{
		int iColors = NumDIBColorEntries((LPBITMAPINFO) &BmpInfoHead);
		if (iColors == -1)
		{
			AfxMessageBox("unknow format bitmap");
			goto $ERROR;
		}
		////////////////////////////////////////////////////////////////////
		if(iColors == 0)
		{
			iInfoSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD);
			pInfo = (LPBITMAPINFO) malloc(iInfoSize);
			if (!pInfo)
			{
				AfxMessageBox("Insufficient memory for DIB header");
				goto $ERROR;
			}
			memcpy(pInfo, &BmpInfoHead, sizeof(BITMAPINFOHEADER));
		}
		else
		{
			iInfoSize = sizeof(BITMAPINFOHEADER) + iColors * sizeof(RGBQUAD);
			pInfo = (LPBITMAPINFO) malloc(iInfoSize);
			if (!pInfo)
			{
				AfxMessageBox("Insufficient memory for DIB header");
				goto $ERROR;
			}
			memcpy(pInfo, &BmpInfoHead, sizeof(BITMAPINFOHEADER));
			
			int iColorTableSize = iColors * sizeof(RGBQUAD);
			iBytes = fp->Read((((LPBYTE) pInfo) + sizeof(BITMAPINFOHEADER)), iColorTableSize);
			if (iBytes != (DWORD)iColorTableSize)
			{
				AfxMessageBox("$ERROR reading color table");
				goto $ERROR;
			}
		}
	}

	iBitsSize = BmpFileHead.bfSize - BmpFileHead.bfOffBits;
	pBits = (BYTE*) malloc(iBitsSize);
	if (pBits == NULL)
	{
		AfxMessageBox("Insufficient memory to read DIB bits");
		goto $ERROR;
	}

	// Seek to the bits in the file.
	fp->Seek(dwFileStart + BmpFileHead.bfOffBits, CFile::begin);
	// Read the bits.
	iBytes = fp->Read(pBits, iBitsSize);
	if (iBytes != iBitsSize) 
	{
		AfxMessageBox("Insufficient memory to read DIB bits");
		goto $ERROR;
	}
	//follow modified by cxf for CBP format
	if(BmpFileHead.bfType == 0x9608)
	{
		m_pBMI = pInfo;
		DWORD iNewBitsSize = StorageWidth() * DibHeight();
		m_pBMI = NULL;

		BYTE* pNewBits = (BYTE*) malloc(iNewBitsSize);
		if (pNewBits == NULL)
		{
			AfxMessageBox("Insufficient memory to read DIB bits");
			goto $ERROR;
		}

		BYTE* pSrc=pBits;
		BYTE* pDes=pNewBits; 
		for(DWORD nnn=0;nnn < iBitsSize;)
		{
			if(*(pSrc+1) > 0)
			{
				memset(pDes,*pSrc,*(pSrc+1));
				pDes=pDes+(*(pSrc+1));
				pSrc=pSrc+2;
				nnn=nnn+2;
			}
			else
			{	//not repeat 
    			int xxx=*pSrc; 
				memcpy(pDes,pSrc+2,xxx);
				pDes=pDes+xxx;
	 			pSrc=pSrc+2+xxx;
	 			nnn=nnn+2+xxx;
			}
		}//for nnn
		free(pBits);
		pBits=pNewBits;
	}

    // Everything went OK.
    if (m_pBMI != NULL) free(m_pBMI);
		m_pBMI = pInfo;
    if (m_pBits != NULL) free(m_pBits);
		m_pBits = pBits;
	m_bMyBits=TRUE;

	return TRUE;
                
$ERROR: // Something went wrong.
    if (pInfo) free(pInfo);
    if (pBits) free(pBits);
    return FALSE;
}
// Load a DIB from a disk file. If no file name is given, show
// an Open File dialog to get one.
BOOL CDIB::Load(char* pszFileName)
{
    
    // Try to open the file for read access.
	if(!pszFileName)
		return FALSE;
	CFile file;
	if(!file.Open(pszFileName,CFile::modeRead | CFile::shareDenyWrite))
	{
		CString s;
		s.Format("不能打开文件 %s!",pszFileName);
		MessageBox(NULL,s,"错误",MB_OK);
		TRACE("Failed to open file");
		return FALSE;
	}
	BOOL bResult = Load(&file);
	file.Close();

	//CString s=pszFileName;
	//s.MakeLower();
	//if(s.Find(apppath)>=0)
	//{
	//	int len=strlen(apppath);
	//	s=s.Mid(len);
	//}
	//filename = s;
    return bResult;
	
}

BOOL CDIB::LoadImage(char* pszFilename)
{
	int m_nImageType = m_gImageObj.FileType( pszFilename );
	if( m_nImageType == 0 )
	{
		//m_nLastError = IMAGELIB_UNSUPPORTED_FILETYPE;
		return(FALSE);
	}

	//KillImage();

	//m_pszFilename = new char [strlen(pszFilename)+1];
	//if( m_pszFilename != NULL ) strcpy( m_pszFilename, pszFilename );
	

	switch( m_nImageType ){
		case IMAGETYPE_BMP:
			m_hDib = m_gImageObj.LoadBMP( pszFilename );
			if( m_hDib == NULL ){
				//m_nLastError = ::GetLastPicLibError();
				return( FALSE );
				}
			break;
		case IMAGETYPE_GIF:
			m_hDib = m_gImageObj.LoadGIF( pszFilename );
			if( m_hDib == NULL ){
				//m_nLastError = ::GetLastPicLibError();
				return( FALSE );
				}
			break;
		case IMAGETYPE_JPG:
			m_hDib = m_gImageObj.LoadJPG( pszFilename );
			if( m_hDib == NULL ){
				//m_nLastError = ::GetLastPicLibError();
				return( FALSE );
				}
			break;
		case IMAGETYPE_PCX:
			m_hDib = m_gImageObj.LoadPCX( pszFilename );
			if( m_hDib == NULL ){
				//m_nLastError = ::GetLastPicLibError();
				return( FALSE );
				}
			break;
		case IMAGETYPE_TGA:
			m_hDib = m_gImageObj.LoadTGA( pszFilename );
			if( m_hDib == NULL ){
				//m_nLastError = ::GetLastPicLibError();
				return( FALSE );
				}
			break;
		case IMAGETYPE_TIF:
			m_hDib = m_gImageObj.LoadTIF( pszFilename );
			if( m_hDib == NULL ){
				//m_nLastError = ::GetLastPicLibError();
				return( FALSE );
				}
			break;
		}


	BYTE *pTemp;
	
	pTemp = (BYTE *) ::GlobalLock( m_hDib );
	if( pTemp == NULL ) return(FALSE);

	if (m_pBMI != NULL)
	{
		FREE(m_pBMI);
		m_pBMI=NULL;
	}
    if (m_pBits != NULL)
	{
		FREE(m_pBits);
		m_pBits=NULL;
	}
	
	m_pBMI=(BITMAPINFO* )(pTemp+sizeof(BITMAPFILEHEADER));
	int m_nPlanes = m_pBMI->bmiHeader.biPlanes;
	int m_nBits = m_pBMI->bmiHeader.biBitCount;
	int m_nColors = 1 << m_nBits;
	if( m_nPlanes > 1 ) m_nColors <<= ( m_nPlanes - 1 );
	if( m_nBits >= 16 ) m_nColors = 0;
	
	int m_nPaletteBytes=0;
	if( m_nBits <= 8 ) m_nPaletteBytes = m_nColors * sizeof( RGBQUAD );
	//int ImageSize = ::GlobalSize(m_hDib);
	//int BitSize=ImageSize-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+m_nPaletteBytes);
	m_pBits=pTemp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+m_nPaletteBytes;
	
	m_bMyBits=TRUE;
	//CString s=pszFilename;
	//s.MakeLower();
	//if(s.Find(apppath)>=0)
	//{
	//	int len=strlen(apppath);
	//	s=s.Mid(len);
	//}
	//filename = s;
	return TRUE;
}

// Load a DIB from a resource id.




// Get the number of color table entries.
int CDIB::GetNumClrEntries()
{
    return NumDIBColorEntries(m_pBMI);
}

// NOTE: This assumes all CDIB objects have 256 color table entries.

// Get a pointer to a pixel.
// NOTE: DIB scan lines are DWORD aligned. The scan line 
// storage width may be wider than the scan line image width
// so calc the storage width by rounding the image width 
// to the next highest DWORD value.
void* CDIB::GetPixelAddress(int x, int y)
{
    // Note: This version deals only with 8 bpp DIBs.
    ASSERT(m_pBMI->bmiHeader.biBitCount == 8||m_pBMI->bmiHeader.biBitCount == 24);
    // Make sure it's in range and if it isn't return zero.
    if ((x >= DibWidth()) || (y >= DibHeight())) {
        TRACE("Attempt to get out of range pixel address");
        return NULL;
    }

    // Calculate the scan line storage width.
	int xwidth;
	if(m_pBMI->bmiHeader.biBitCount == 8)
		xwidth = (int) x ;
	else
		xwidth = (int)( x * 3);
	int ywidth = (int)((DibHeight() - 1 - y) * StorageWidth());
	
	return (void*)(m_pBits + xwidth + ywidth);    
}// Get the bounding rectangle.
void CDIB::GetRect(RECT* pRect)
{
    pRect->top = 0;
    pRect->left = 0;
    pRect->bottom = DibHeight();
    pRect->right = DibWidth();
}

void CDIB::Draw1(HDC dc, int x, int y)
{
    ::StretchDIBits(dc,
                    x,                        // Destination x
                    y,                        // Destination y
                    DibWidth()/nCol,               // Destination width
                    DibHeight()/nRow,              // Destination height
                    CurCol*(DibWidth()/nCol),                        // Source x
                    (3-CurRow)*(DibHeight()/nRow),                        // Source y
                    DibWidth()/nCol,               // Source width
                    DibHeight()/nRow,              // Source height
                    GetBitsAddress(),         // Pointer to bits
                    GetBitmapInfoAddress(),   // BITMAPINFO
                    DIB_RGB_COLORS,           // Options
                    SRCCOPY);                 // Raster operation code (ROP) SRCCOPY
}
void CDIB::Draw(HDC dc, int x, int y)
{
	CRect rect;
	::GetClipBox(dc,&rect);
//	::StretchDIBits(dc,
//					rect.left,                        // Destination x
//					rect.top,                        // Destination y
//					rect.Width(),               // Destination width
//					rect.Height(),              // Destination height
//					rect.left-x,                        // Source x
//					y+DibHeight()-rect.bottom,                        // Source y
//					rect.Width(),               // Source width
//					rect.Height(),              // Source height
//					GetBitsAddress(),         // Pointer to bits
//					GetBitmapInfoAddress(),   // BITMAPINFO
//					DIB_RGB_COLORS,           // Options
//					SRCCOPY);                 // Raster operation code (ROP) SRCCOPY
	::StretchDIBits(dc,
					x,                        // Destination x
					y,                        // Destination y
					DibWidth(),               // Destination width
					DibHeight(),              // Destination height
					0,                        // Source x
					0,                        // Source y
					DibWidth(),               // Source width
					DibHeight(),              // Source height
					GetBitsAddress(),         // Pointer to bits
					GetBitmapInfoAddress(),   // BITMAPINFO
					DIB_RGB_COLORS,           // Options
					SRCCOPY);                 // Raster operation code (ROP) SRCCOPY
}



void CDIB::StretchDraw(struct HDC__ * dc, int x, int y,int w,int h)
{
    ::StretchDIBits(dc,
                    x,                        // Destination x
                    y,                        // Destination y
                    w,               // Destination width
                    h,              // Destination height
                    0,                        // Source x
                    0,                        // Source y
                    DibWidth(),               // Source width
                    DibHeight(),              // Source height
                    GetBitsAddress(),         // Pointer to bits
                    GetBitmapInfoAddress(),   // BITMAPINFO
                    DIB_RGB_COLORS,           // Options
                    SRCCOPY);                 // Raster operation code (ROP)
}
void CDIB::Draw(HDC dc, CRect& destRect,CRect& srcRect)
{
    ::StretchDIBits(dc,
                    destRect.left,                        // Destination x
                    destRect.top,                        // Destination y
                    destRect.Width(),               // Destination width
                    destRect.Height(),              // Destination height
                    srcRect.left,                        // Source x
                    srcRect.top,                        // Source y
                    srcRect.Width(),               // Source width
                    srcRect.Height(),              // Source height
                    GetBitsAddress(),         // Pointer to bits
                    GetBitmapInfoAddress(),   // BITMAPINFO
                    DIB_RGB_COLORS,           // Options
                    SRCCOPY);                 // Raster operation code (ROP)
}
void CDIB::CopyBits(CDIB* pdibDest, 
                    int xd, int yd,
                    int w,  int h,
                    int xs, int ys,
                    COLORREF clrTrans)
{
    ASSERT(m_pBMI->bmiHeader.biBitCount == 8||m_pBMI->bmiHeader.biBitCount == 24);
    ASSERT(pdibDest);

    // Test for silly cases.
    if (w == 0 || h == 0) return;

    // Get pointers to the start points in the source and destination
    // DIBs. Note that the start points will be the bottom-left
    // corner of the DIBs because the scan lines are reversed in memory.
    BYTE* pDest = (BYTE*)pdibDest->GetPixelAddress(xd, yd);
	if(pDest==NULL) return;
    ASSERT(pDest);

	BYTE* pSrc = (BYTE*)GetPixelAddress(xs,ys);
	ASSERT(pSrc);
	BYTE* pSrcColorTab=(BYTE*)GetClrTabAddress();
	int iSrcBtyeWidth = StorageWidth();
	int iDestBtyeWidth = pdibDest->StorageWidth();
	if(clrTrans == 0xFFFFFFFF){
		if(GetBitCount() == pdibDest->GetBitCount())	//如果位图文件BITS相同折直接拷贝数据
		{
			int LineBtyeCount =(int)((w * GetBitCount())/8);        
			for(int i=0; i < h; i++) 
			{
				memcpy(pDest, pSrc, LineBtyeCount);
				pSrc -= iSrcBtyeWidth;
				pDest -= iDestBtyeWidth;
			}
			return;
		}
		if((GetBitCount()==8) && (pdibDest->GetBitCount()==24))//8 bits位图文件转为24位位图
		{
			BYTE nPixel;
			for(int i=0;i <h;i++)
			{
				for(int j=0;j<w;j++)
				{
					nPixel = *(pSrc+j);
					memcpy((pDest+(3*j)), (pSrcColorTab+(4*nPixel)), 3);//拷贝数据
				}
				pSrc -= iSrcBtyeWidth;
				pDest -= iDestBtyeWidth;
			}
			return;
		}
	}
	else{
        if((GetBitCount()==8) && (pdibDest->GetBitCount()==8))
		{
			BYTE nTransIndex = LOBYTE(LOWORD(clrTrans));	//用于8位位图的透明位		
			BYTE nPixel;									//用于8位位图的位判断
			for(int i=0;i <h;i++)
			{
				for(int j=0;j<w;j++)
				{
					nPixel = *(pSrc+j);					
					if (nPixel != nTransIndex) //判断是否透明位 
					{
						memcpy(pDest+j, pSrc+j, 1);//是透明位,不拷贝数据
					} 
					else 
					{
						//是透明位,不拷贝数据
					}
				}//end of for(int j=0;j<nWidth;j++)
				pSrc -= iSrcBtyeWidth;
				pDest -= iDestBtyeWidth;
			}//end of for(int i=0;i <nHeight;i++)
			return;
		}

		// 24位位图做透明处理
		if((GetBitCount()==24) && (pdibDest->GetBitCount()==24))
		{
			BYTE b = GetRValue(clrTrans);
			BYTE g = GetGValue(clrTrans);
			BYTE r = GetBValue(clrTrans);
			
			BYTE* pSrcByte = pSrc;					   
			for(int i=0;i <h;i++)
			{
				for(int j=0;j<w;j++)
				{
					if ((b==*pSrcByte) && (g==*(pSrcByte+1)) && (r==*(pSrcByte+2)))//判断是否透明位 
					{
						//是透明位,不拷贝数据
					} 
					else 
					{						
						memcpy((pDest+(3*j)), (pSrc+(3*j)), 3);//不是透明位,拷贝数据
					}
					pSrcByte = pSrcByte + 3;				
				}//end of for(int j=0;j<nWidth;j++)
				pSrc -= iSrcBtyeWidth;
				pDest -= iDestBtyeWidth;
				pSrcByte = pSrc;
			}
			return;
        }
	    // 256色位图做透明处理
		if((GetBitCount()==8) && (pdibDest->GetBitCount()==24))
		{
			BYTE nTransIndex = LOBYTE(LOWORD(clrTrans));			
			BYTE nPixel;
			for(int i=0;i <h;i++)
			{
				for(int j=0;j<w;j++)
				{
					nPixel = *(pSrc+j);					
					if (nPixel != nTransIndex) //判断是否透明位 
					{
						memcpy((pDest+(3*j)), (pSrcColorTab+(4*nPixel)), 3);//拷贝数据
					} 
					else 
					{
						//是透明位,不拷贝数据
					}
				}//end of for(int j=0;j<nWidth;j++)
				pSrc -= iSrcBtyeWidth;
				pDest -= iDestBtyeWidth;
			}//end of for(int i=0;i <nHeight;i++)
			return;
        }
    }

	return;
}          
void CDIB::RotateDIB(CDIB *pd,int x,int y,int degree,COLORREF clrTrans )
{
   if(x<0||y<0)return;
   float rad_degree=(float)(degree*2*3.1415926/360);
   float sin_val=(float)sin(rad_degree);
   float cos_val=(float)cos(rad_degree);
   int srcW=GetWidth();
   int srcH=GetHeight();
   int xd,yd;
       for (int ys=0;ys<srcH-2;ys++) {
        for (int xs=0;xs<srcW-2;xs++) {
          xd=(int)((float)x+(float)xs*cos_val-(float)ys*sin_val); 
    	  yd=(int)((float)y+(float)ys*cos_val+(float)xs*sin_val);
    	  if (!(xd<0||xd>=(int)pd->GetWidth()||yd>=(int)pd->GetHeight()||yd<0)) 
    	    CopyBits(pd,xd,yd,1,1,xs,ys,clrTrans);
         } 
     }	
} 


void CDIB::Render(CDIB *pDIB)
{
	// Get the sprite rectangle.
	CRect rcDraw(m_dx,m_dy,m_dx+DibWidth()/nCol,m_dy+DibHeight()/nRow);

	CRect rcDest;
	pDIB->GetRect(&rcDest);
	rcDest.IntersectRect(&rcDest,&rcDraw);
	if(rcDest.IsRectEmpty()) return;

    // Copy the image of the sprite.
    CopyBits(pDIB,                         // Dest DIB
             rcDest.left,                  // Dest x
             rcDest.top,                   // Dest y
             rcDest.right - rcDest.left,   // Width
             rcDest.bottom - rcDest.top,   // Height
             DibWidth()/nCol*CurCol,            // Source x //rcDraw.left - m_dx + 
             DibHeight()/nRow*CurRow,             // Source y//rcDraw.top - m_dy+
             m_bTransIndex); //PALETTEINDEX(m_bTransIndex)); // Transparent color index
}

int CDIB::GetBitCount()
{
	if(m_pBMI == NULL) return -1;
	return  m_pBMI->bmiHeader.biBitCount;
}


BOOL CDIB::HitTest(CPoint point,BOOL bTrans)
{
    // Test if the point is inside the sprite rectangle
    if ((point.x > m_dx) 
    && (point.x < m_dx + GetWidth()/nCol)
    && (point.y > m_dy)
    && (point.y < m_dy + GetHeight())) {
		if(!bTrans)
			return TRUE;
        // See if this point is transparent by testing whether
        // the pixel value is the same as the top left
        // corner value. Note that top left of the
        // image is bottom left in the DIB.
		if(GetBitCount()==24)
		{
        	DWORD* p = (DWORD*)GetPixelAddress(point.x - m_dx, point.y - m_dy);
			DWORD dw = *p & 0x00FFFFFF;
	        if (dw!= m_bTransIndex) 
			{
	            return TRUE; // hit
	        }
		}
		else
		{
        	BYTE* p = (BYTE*)GetPixelAddress(point.x - m_dx, point.y - m_dy);
			DWORD dw = *p & 0x00FFFFFF;
	        if (dw!= m_bTransIndex) 
			{
            	return TRUE; // hit
        	}
		}
    }
    return FALSE;
}


void CDIB::SetTransColor()
{
	
   if(GetBitCount()==8)
	   {
	    BYTE* p = (BYTE*)GetPixelAddress(0, 0);
		ASSERT(p);
		m_bTransIndex = *p;
	   }
  else
      {
		DWORD* p = (DWORD*)GetPixelAddress(0, 0);
		ASSERT(p);
	    // Get the pixel value and save it.
	    m_bTransIndex = * p;
		m_bTransIndex = m_bTransIndex & 0x00FFFFFF;
      }

}

void CDIB::Modify()
{
	//CDibSettingDlg dlg;
	//dlg.pData = this;
	//dlg.DoModal();
}

void CDIB::DrawRect(CDC* pDC,int x,int y)
{
	CRect rcDraw(m_dx+x,m_dy+y,x+m_dx+DibWidth()/nCol,y+m_dy+DibHeight()/nRow);
	CBrush br(RGB(255,255,255));
	pDC->FrameRect(&rcDraw,&br);
	//pDC->Rectangle(&rcDraw);
}
bool CDIB::GetNodeFrame(int node,int& nframe)
{
	if(node==0)
	{
		nframe=CurCol+CurRow*nCol;
		return true;
	}
//	if(node==CurRow)
//	{
//		nframe=CurCol;
//		return true;
//	}
	return false;
}
void CDIB::SetNodeFrame(int node,int nframe)
{
	if(node==0)
	{
		CurRow=nframe/nCol;
		CurRow%=nRow;
		CurCol=nframe%nCol;
	}
//	if(node<nRow&&node>=0)
//	{
//		CurRow=node;
//	}
//	if(nframe<nCol&&nframe>=0)
//	{
//		CurCol=nframe;
//	}
}
bool CDIB::AnimateTest()
{
	if(nRow==1&&nCol==1) return false;
	CurRow++;
	if(CurRow>=nRow)
	{
		CurRow=0;
		CurCol++;
		if(CurCol>=nCol)
		{
			CurCol=0;
		}
	}
	return true;
}
bool CDIB::TimerTest()
{
	bool r=false;
	//if(pActionList!=NULL)
	//{
	//	POSITION pos=pActionList->GetHeadPosition();
	//	if(pos==NULL)
	//	{
	//		return DirectApproach();//默认为自动直接逼近
	//	}
	//	while(pos)
	//	{
	//		CAction *p = (CAction *)pActionList->GetNext(pos);
	//		if(p->TimerTest(this))
	//			r=true;

	//		int step;
	//		if(p->CondDibJump(step,this))
	//		{
	//			if(step>0)
	//			{
	//				for(int i=0;i<step;i++)
	//				{
	//					if(pos)
	//						p = (CAction *)pActionList->GetNext(pos);
	//					if(pos)
	//						;
	//					else
	//						return r;
	//				}
	//			}
	//			else if(step<0)
	//			{
	//				for(int i=0;i>step;i--)
	//				{
	//					if(pos)
	//						p = (CAction *)pActionList->GetPrev(pos);
	//					if(pos)
	//						;
	//					else
	//						return r;
	//				}
	//			}
	//		}
	//	}
	//}
	//else
	//{
	//	r=DirectApproach();
	//}
	return r;
}
bool CDIB::GetSelectInfo(int& selnode,int& nframe)
{
	selnode=CurRow;
	nframe=CurCol;
	return true;
}
bool CDIB::DirectApproach()
{
	bool r=false;
	//int minf=0,maxf=1;
	//float val;
	//r=ReadVarNameValue(sVarName.GetBuffer(sVarName.GetLength()),val);
	//sVarName.ReleaseBuffer();
	////将浮点型数据转化为整型
	//if(r)
	//{
	//	int value;
	//	if(val>1.0f) val=1.0f;
	//	if(val<0) val=0.0f;
	//	value=int(minf+(maxf-minf)*val);//计算出帧值
	//	//开始逼近
	//	int nframe;
	//	if(GetNodeFrame(0,nframe))
	//	{
	//		if(nframe==value)
	//		{
	//			return false;
	//		}
	//		if(abs(nframe-value)<abs(step))
	//		{
	//			SetNodeFrame(0,value);
	//			return true;
	//		}
	//		if(nframe>value)
	//			nframe-=step;
	//		else
	//			nframe+=step;
	//		SetNodeFrame(0,nframe);
	//	}
	//}
	return r;
}
void CDIB::Read(CArchive &ar)
{
	//ar>>filename;
	//ar>>m_dx;
	//ar>>m_dy;
	//ar>>nRow;
	//ar>>CurRow;
	//ar>>nCol;
	//ar>>CurCol;
	//ar>>m_id;
	//ar>>sToolTip;
	//ar>>sLinkDoc;
	//ar>>sVarName;

	//if(version>=26)
	//{
	//	int i;
	//	ar>>i;
	//	if(i==1)
	//	{
	//		if(pActionList==NULL) pActionList = new CActionList;
	//		pActionList->Read(ar);
	//	}
	//}
}
void CDIB::Save(CArchive &ar)
{
	//ar<<filename;
	//ar<<m_dx;
	//ar<<m_dy;
	//ar<<nRow;
	//ar<<CurRow;
	//ar<<nCol;
	//ar<<CurCol;
	//ar<<m_id;
	//ar<<sToolTip;
	//ar<<sLinkDoc;
	//ar<<sVarName;
	////version 26
	//if(pActionList!=NULL)
	//	ar<<(int)1;
	//else ar<<0;
	//pActionList->Save(ar);
}
int  CDIB::OperProc()
{
	int r=0;
	//if(pActionList!=NULL)
	//{
	//	POSITION pos=pActionList->GetHeadPosition();
	//	while(pos)
	//	{
	//		CAction *p = (CAction *)pActionList->GetNext(pos);
	//		if(p->ProcessOper(this))
	//			r=1;

	//		int step;
	//		if(p->CondDibJump(step,this))
	//		{
	//			if(step>0)
	//			{
	//				for(int i=0;i<step;i++)
	//				{
	//					if(pos)
	//						p = (CAction *)pActionList->GetNext(pos);
	//					if(pos)
	//						;
	//					else
	//						return r;
	//				}
	//			}
	//			else if(step<0)
	//			{
	//				for(int i=0;i>step;i--)
	//				{
	//					if(pos)
	//						p = (CAction *)pActionList->GetPrev(pos);
	//					if(pos)
	//						;
	//					else
	//						return r;
	//				}
	//			}
	//		}
	//	}
	//}
	return r;
}
int CDIB::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_COMMAND:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONDOWN:
		{
			return OperProc();
		}
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_RBUTTONUP:
		break;
	case WM_KEYDOWN:
		break;
	case WM_TIMER:
		{
			//if(pActionList==NULL)
			//{
			//	DirectApproach();
			//	return 1;
			//}
			//else
			//{
			//	if(TimerTest())
			//		return 1;
			//}
			////if(TimerTest())
			////	return 1;
		}
		break;
//	case WM_ERASEBKGND:
//		break;
//	case WM_SETCURSOR:
//		break;
//	case WM_DESTROY:
//		break;
	default:
		return 0;
	}
	return 0;
}
bool CDIB::AddFilename(void* plist)
{
	//CUsedFileList* p = (CUsedFileList*)plist;
	//CString s=sLinkDoc;
	//p->AddFilename(s);
	//s=filename;
	//p->AddFilename(s);
	return true;
}
