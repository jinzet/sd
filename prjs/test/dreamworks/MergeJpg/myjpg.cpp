// dib.cpp : implementation file
//
//
 
#include "stdafx.h"
#include "myjpg.h"

#include "jpeg.h"
#include "math.h"
#include "windowsx.h"
#include "dibpal.h"

#define ALLOC(s) malloc(s)
#define FREE(p) free(p)

// Create a small DIB here so m_pBMI and m_pBits are always valid.
CMYJPG::CMYJPG()
{
    m_pBMI = NULL;
    m_pBits = NULL;
    m_bMyBits = TRUE;
	IsBkgnd=TRUE;
    //Create(24,24,24);

	bJpeg = FALSE;
}

CMYJPG::~CMYJPG()
{
    // Free the memory.
	if(bJpeg)
	{
		GlobalFreePtr(m_pBMI);
	}
	else
	{
		if (m_pBMI != NULL) FREE(m_pBMI);
		if (m_bMyBits && (m_pBits != NULL)) FREE(m_pBits);
	}
	
}

/////////////////////////////////////////////////////////////////////////////
// CMYJPG serialization
// We don't support this yet.
void CMYJPG::Serialize(CArchive& ar)
{
    ar.Flush();
    CFile* fp = ar.GetFile();

    if (ar.IsStoring()) 
	{
        Save(fp);
    } 
	else 
	{
        Load(fp);
    }
}

/////////////////////////////////////////////////////////////////////////////
// Private functions
static BOOL IsWinDIB(BITMAPINFOHEADER *pBIH)
{
    ASSERT(pBIH);
    if (((BITMAPCOREHEADER*)pBIH)->bcSize == sizeof(BITMAPCOREHEADER)) 
	{
        return FALSE;
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
static int NumDIBColorEntries(BITMAPINFO* pBmpInfo) 
{
    BITMAPINFOHEADER* pBIH;
    BITMAPCOREHEADER* pBCH;
    int iColors, iBitCount;

    ASSERT(pBmpInfo);

    pBIH = &(pBmpInfo->bmiHeader);
    pBCH = (BITMAPCOREHEADER*) pBIH;

    // Start off by assuming the color table size from
    // the bit-per-pixel field.
    if (IsWinDIB(pBIH)) 
	{
        iBitCount = pBIH->biBitCount;
    } 
	else 
	{
        iBitCount = pBCH->bcBitCount;
    }

    switch (iBitCount) 
	{
    case 1:
        iColors = 2;
        break;
    case 4:
        iColors = 16;
        break;
    case 8:
        iColors = 256;
        break;
	case 24:
        iColors = 0;
        break;
    default:
        iColors = 0;
        break;
    }

    // If this is a Windows DIB, then the color table length
    // is determined by the biClrUsed field if the value in
    // the field is nonzero.
    if (IsWinDIB(pBIH) && (pBIH->biClrUsed != 0)) 
	{
        iColors = pBIH->biClrUsed;
    }

    // BUGFIX 18 Oct 94 NigelT
    // Make sure the value is reasonable since some products
    // will write out more then 256 colors for an 8 bpp DIB!!!
    int iMax = 0;
    switch (iBitCount) 
	{
    case 1:
        iMax = 2;
        break;
    case 4:
        iMax = 16;
        break;
    case 8:
        iMax = 256;
        break;
	case 24:
        iColors = 0;
        break;
    default:
        iMax = 0;
        break;
    }
    if (iMax) 
	{
        if (iColors > iMax) 
		{
            TRACE("Invalid color count");
            iColors = iMax;
        }
    }
    return iColors;
}

/////////////////////////////////////////////////////////////////////////
//
int CMYJPG::StorageWidth()
{
	if(m_pBMI == NULL) return -1;
	int bitswidth = m_pBMI->bmiHeader.biWidth * m_pBMI->bmiHeader.biBitCount;
	return (((bitswidth + 31) & (~31))/8);
}

//////////////////////////////////////////////////////////////////////////
//
int CMYJPG::GetBitCount()
{
	if(m_pBMI == NULL) return -1;
	return  m_pBMI->bmiHeader.biBitCount;
}

////////////////////////////////////////////////////////////////////////////
// Get the number of color table entries.
int CMYJPG::GetNumClrEntries()
{
    return NumDIBColorEntries(m_pBMI);
}

///////////////////////////////////////////////////////////////////////////
// Get a pointer to a pixel.
// NOTE: DIB scan lines are DWORD aligned. The scan line 
// storage width may be wider than the scan line image width
// so calc the storage width by rounding the image width 
// to the next highest DWORD value.
void* CMYJPG::GetPixelAddress(int x, int y)
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
}

/////////////////////////////////////////////////////////////////////////
// Get the bounding rectangle.
void CMYJPG::GetRect(CRect* pRect)
{
    pRect->top = 0;
    pRect->left = 0;
    pRect->bottom = DibHeight();
    pRect->right = DibWidth();
}

/////////////////////////////////////////////////////////////////////////////
// Create a new empty 8bpp DIB with a 256 entry color table.
BOOL CMYJPG::Create(int iWidth, int iHeight,int tempBitCount)
{
    // Delete any existing stuff.
    if (m_pBMI != NULL) FREE(m_pBMI);
    if (m_bMyBits && (m_pBits != NULL)) FREE(m_pBits);

    // Allocate memory for the header.
	long lBitsSize ;
	if(tempBitCount==8)
	{
		m_pBMI = (BITMAPINFO*) ALLOC(sizeof(BITMAPINFOHEADER)
		                              + 256* sizeof(RGBQUAD));
		lBitsSize = ((iWidth + 3) & ~3) * iHeight;
	}
	else
	{
		m_pBMI = (BITMAPINFO*) ALLOC(sizeof(BITMAPINFOHEADER)
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
    m_pBits = (BYTE*)ALLOC(lBitsSize);
    if (!m_pBits) 
	{
        TRACE("Out of memory for DIB bits");
        FREE(m_pBMI);
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
    memset(m_pBits, 0, lBitsSize);

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Create a CMYJPG structure from existing header and bits. The DIB
// won't delete the bits and makes a copy of the header.
BOOL CMYJPG::Create(BITMAPINFO* pBMI, BYTE* pBits)
{
    ASSERT(pBMI);
    ASSERT(pBits);
    if (m_pBMI != NULL) FREE(m_pBMI);
    m_pBMI = (BITMAPINFO*) ALLOC(sizeof(BITMAPINFOHEADER)
                                   +  NumDIBColorEntries(pBMI)* sizeof(RGBQUAD));
    ASSERT(m_pBMI);
    // Note: This will probably fail for < 256 color headers.
    memcpy(m_pBMI, pBMI, sizeof(BITMAPINFOHEADER)+
              NumDIBColorEntries(pBMI)* sizeof(RGBQUAD));
  
    if (m_bMyBits && (m_pBits != NULL)) FREE(m_pBits);
    m_pBits = pBits;
    m_bMyBits = FALSE; // We can't delete the bits.
    
	return TRUE;
}

BOOL CMYJPG::Clone(CMYJPG* pSrcDib)
{
	ASSERT(pSrcDib);

	if (m_pBMI != NULL) FREE(m_pBMI);
    m_pBMI = (BITMAPINFO*) ALLOC(sizeof(BITMAPINFOHEADER)
                                   +  NumDIBColorEntries(pSrcDib->m_pBMI)* sizeof(RGBQUAD));
    ASSERT(m_pBMI);
    // Note: This will probably fail for < 256 color headers.
    memcpy(m_pBMI, pSrcDib->m_pBMI, sizeof(BITMAPINFOHEADER)+
              NumDIBColorEntries(pSrcDib->m_pBMI)* sizeof(RGBQUAD));

	if(m_pBits != NULL) FREE(m_pBits);
	int iWidth = pSrcDib->GetWidth();
	int iHeight = pSrcDib->GetHeight();
	int tempBitCount = pSrcDib->GetBitCount();
	long lBitsSize ;
	if(tempBitCount==8)
	{
		lBitsSize = ((iWidth + 3) & ~3) * iHeight;
	}
	else
	{
		long bitswidth = iWidth * tempBitCount;
		lBitsSize = (((bitswidth + 31) & (~31))/8) *iHeight;
	}
	m_pBits = (BYTE*)ALLOC(lBitsSize);
	ASSERT(m_pBits);
	memcpy(m_pBits,pSrcDib->m_pBits,lBitsSize);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// Load a DIB from an open file.
BOOL CMYJPG::Load(CFile* fp)
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
		LPBITMAPINFO pInfo = (LPBITMAPINFO) ALLOC(iInfoSize);
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
			pInfo = (LPBITMAPINFO) ALLOC(iInfoSize);
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
			pInfo = (LPBITMAPINFO) ALLOC(iInfoSize);
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
	pBits = (BYTE*) ALLOC(iBitsSize);
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

		BYTE* pNewBits = (BYTE*) ALLOC(iNewBitsSize);
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
		FREE(pBits);
		pBits=pNewBits;
	}

    // Everything went OK.
    if (m_pBMI != NULL) FREE(m_pBMI);
		m_pBMI = pInfo;
    if (m_pBits != NULL) FREE(m_pBits);
		m_pBits = pBits;
	m_bMyBits=TRUE;

	return TRUE;
                
$ERROR: // Something went wrong.
    if (pInfo) FREE(pInfo);
    if (pBits) FREE(pBits);
    return FALSE;
}



BOOL CMYJPG::LoadJpeg(char* pszFilename)
{    
	CJpeg jpg;
	if(!jpg.ReadFile(pszFilename))
	{
		return( FALSE );
	}

	if (m_pBMI != NULL) FREE(m_pBMI);
    if (m_pBits != NULL) FREE(m_pBits);

	bJpeg = TRUE;
	
	m_pBMI=(BITMAPINFO* )(jpg.lpbi);

	m_pBits=(BYTE*)(jpg.RawImage);
	
	m_bMyBits=TRUE;
	return TRUE;
}
BOOL CMYJPG::SaveJpeg(char *pszFilename,int quality)
{
	CJpeg jpg(quality);

	jpg.lpbi = (BITMAPINFOHEADER *)m_pBMI;
	
	CDIBPal * pal = new CDIBPal;
	BOOL bNewPal = pal->Create(this);
	jpg.imagePalette = bNewPal ? pal : NULL;
	jpg.RawImage = m_pBits;
	jpg.Width = DibWidth();
	jpg.Height = DibHeight();
	jpg.Depth = m_pBMI->bmiHeader.biBitCount;
	jpg.ColorType = ( COLORTYPE_COLOR);//COLORTYPE_PALETTE |
	jpg.EffWidth =  (long)(((long)jpg.Width*jpg.Depth + 31) / 32) * 4;

	BOOL bRet = jpg.SaveFile(pszFilename);

	if(!bNewPal) delete pal;

	return( bRet );
}

void CMYJPG::Draw(CDC* dc, int x, int y,int bx,int by,int bw,int bh)
{
    ::StretchDIBits(dc->m_hDC,
                    x,                        // Destination x
                    y,                        // Destination y
                    bw,               // Destination width
                    bh,              // Destination height
                    bx,                        // Source x
                    by,                        // Source y
                    bw,               // Source width
                    bh,              // Source height
                    GetBitsAddress(),         // Pointer to bits
                    GetBitmapInfoAddress(),   // BITMAPINFO
                    DIB_RGB_COLORS,           // Options
                    SRCCOPY);                 // Raster operation code (ROP)
}

void CMYJPG::MyDraw(HDC hdc, int x, int y,int dw,int dh)
{
	HDC hMemDC;
	HBITMAP    hBitmap, hOldBitmap; // 位图句柄
	hMemDC=::CreateCompatibleDC(hdc);
	hBitmap = CreateCompatibleBitmap(hdc,DibWidth(),DibHeight());
	hOldBitmap=(HBITMAP)SelectObject(hMemDC, hBitmap);
    ::StretchDIBits(hMemDC,
                    0,                        // Destination x
                    0,                        // Destination y
                    DibWidth(),               // Destination width
                    DibHeight(),              // Destination height
                    0,                        // Source x
                    0,                        // Source y
                    DibWidth(),               // Source width
                    DibHeight(),              // Source height
                    GetBitsAddress(),         // Pointer to bits
                    GetBitmapInfoAddress(),   // BITMAPINFO
                    DIB_RGB_COLORS,           // Options
                    SRCCOPY);                 // Raster operation code (ROP)
	::StretchBlt(hdc,x,y,dw,dh,hMemDC,0,0,DibWidth(),DibHeight(),SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);
}

////////////////////////////////////////////////////////////////////////////
// Load a DIB from a disk file. If no file name is given, show
// an Open File dialog to get one.
BOOL CMYJPG::Load(LPSTR pszFileName)
{
    CString strFile;    

    if ((pszFileName == NULL) ||  (strlen(pszFileName) == 0)) {
        // Show an Open File dialog to get the name.
        CFileDialog dlg   (TRUE,    // Open
                           NULL,    // No default extension
                           NULL,    // No initial file name
                           OFN_FILEMUSTEXIST
                             | OFN_HIDEREADONLY,
                           "Image files (*.DIB, *.BMP)|*.DIB;*.BMP|All files (*.*)|*.*||");
        if (dlg.DoModal() == IDOK) {
            strFile = dlg.GetPathName();
        } else {
            return FALSE;
        }
    } else {
        // Copy the supplied file path.
        strFile = pszFileName;                    
    }


    // Try to open the file for read access.

	BOOL bResult = FALSE;

	CString ext=strFile;
	int index = ext.ReverseFind('.');
	if(index != -1)
	{
		ext = ext.Mid(index+1,ext.GetLength());
		ext.MakeLower();
	}

	if(ext=="bmp")
	{

		CFile file;
		if (! file.Open(strFile,
						CFile::modeRead | CFile::shareDenyWrite)) {
			//TRACE("Failed to open file");
			return FALSE;
		}

		bResult = Load(&file);
		file.Close();
	}
	else if(ext == "jpeg" || ext == "jpg")
	{
		bResult = LoadJpeg(pszFileName);
	}

    return bResult;
}

///////////////////////////////////////////////////////////////////////////
// Load a DIB from a resource id.
BOOL CMYJPG::Load(WORD wResid)
{
    ASSERT(wResid);
   HINSTANCE hInst = AfxGetResourceHandle();
    HRSRC hrsrc = ::FindResource(hInst, MAKEINTRESOURCE(wResid), "DIB");
	
    if (!hrsrc) {
        TRACE("DIB resource not found");
        return FALSE;
    }
    HGLOBAL hg = LoadResource(hInst, hrsrc);
    if (!hg) {
        TRACE("Failed to load DIB resource");
        return FALSE;
    }
    BYTE* pRes = (BYTE*) LockResource(hg);
    ASSERT(pRes);
    int iSize = ::SizeofResource(hInst, hrsrc);

    // Mark the resource pages as read/write so the mmioOpen
    // won't fail
    DWORD dwOldProt;
    BOOL b = ::VirtualProtect(pRes,
                              iSize,
                              PAGE_READWRITE,
                              &dwOldProt);
    ASSERT(b);

    // Now create the CMYJPG object. We will create a new header from the 
    // data in the resource image and copy the bits from the resource
    // to a new block of memory.  We can't use the resource image as-is 
    // because we might want to map the DIB colors and the resource memory
    // is write protected in Win32.

    BITMAPFILEHEADER* pFileHdr = (BITMAPFILEHEADER*)pRes;
    ASSERT(pFileHdr->bfType == 0x4D42); // BM file
    BITMAPINFOHEADER* pInfoHdr = (BITMAPINFOHEADER*) (pRes + sizeof(BITMAPFILEHEADER));
    ASSERT(pInfoHdr->biSize == sizeof(BITMAPINFOHEADER));  // must be a Win DIB
    BYTE* pBits = pRes + pFileHdr->bfOffBits;
    return Create((BITMAPINFO*)pInfoHdr, pBits);
    // Note: not required to unlock or free the resource in Win32
}

//////////////////////////////////////////////////////////////////////////
BOOL CMYJPG::Load(HINSTANCE hInst,WORD wResid)
{
    ASSERT(wResid);
    HRSRC hrsrc = ::FindResource(hInst, MAKEINTRESOURCE(wResid), "DIB");
	
    if (!hrsrc) {
        TRACE("DIB resource not found");
        return FALSE;
    }
    HGLOBAL hg = LoadResource(hInst, hrsrc);
    if (!hg) {
        TRACE("Failed to load DIB resource");
        return FALSE;
    }
    BYTE* pRes = (BYTE*) LockResource(hg);
    ASSERT(pRes);
    int iSize = ::SizeofResource(hInst, hrsrc);

    // Mark the resource pages as read/write so the mmioOpen
    // won't fail
    DWORD dwOldProt;
    BOOL b = ::VirtualProtect(pRes,
                              iSize,
                              PAGE_READWRITE,
                              &dwOldProt);
    ASSERT(b);

    // Now create the CMYJPG object. We will create a new header from the 
    // data in the resource image and copy the bits from the resource
    // to a new block of memory.  We can't use the resource image as-is 
    // because we might want to map the DIB colors and the resource memory
    // is write protected in Win32.

    BITMAPFILEHEADER* pFileHdr = (BITMAPFILEHEADER*)pRes;
    ASSERT(pFileHdr->bfType == 0x4D42); // BM file
    BITMAPINFOHEADER* pInfoHdr = (BITMAPINFOHEADER*) (pRes + sizeof(BITMAPFILEHEADER));
    ASSERT(pInfoHdr->biSize == sizeof(BITMAPINFOHEADER));  // must be a Win DIB
    BYTE* pBits = pRes + pFileHdr->bfOffBits;
    return Create((BITMAPINFO*)pInfoHdr, pBits);
    // Note: not required to unlock or free the resource in Win32
}

///////////////////////////////////////////////////////////////////////////
// Draw the DIB to a given DC.
void CMYJPG::Draw(CDC* pDC, int x, int y)
{
    ::StretchDIBits(pDC->GetSafeHdc(),
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
                    SRCCOPY);                 // Raster operation code (ROP)
}

////////////////////////////////////////////////////////////////////////
// Copy a rectangle of the DIB to another DIB.
// Note: We only support 8bpp DIBs here.
void CMYJPG::CopyBits(CMYJPG* pdibDest, 
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
    ASSERT(pDest);

	BYTE* pSrc = (BYTE*)GetPixelAddress(xs,ys);
	ASSERT(pSrc);
	BYTE* pSrcColorTab=(BYTE*)GetClrTabAddress();
	int iSrcBtyeWidth = StorageWidth();
	int iDestBtyeWidth = pdibDest->StorageWidth();
	if(clrTrans == 0xFFFFFFFF)
	{
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
	else
	{
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
					//else 
					//{
						//是透明位,不拷贝数据
					//}
				}
				pSrc -= iSrcBtyeWidth;
				pDest -= iDestBtyeWidth;
			}
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
				}
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
				}
				pSrc -= iSrcBtyeWidth;
				pDest -= iDestBtyeWidth;
			}
			return;
        }
    }

	return;
}

void CMYJPG::CopyBitsRow(BYTE* pDest, int w, int xs, int ys, COLORREF clrTrans)
{
	BYTE* pSrc = (BYTE*)GetPixelAddress(xs,ys);
	BYTE* pSrcColorTab=(BYTE*)GetClrTabAddress();
	if((GetBitCount()==8))//8 bits位图文件转为24位位图
	{
		BYTE nPixel;
		for(int j=0;j<w;j++)
		{
			nPixel = *(pSrc+j);
			memcpy((pDest+(3*j)), (pSrcColorTab+(4*nPixel)), 3);//拷贝数据
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Save a DIB to a disk file.
// This is somewhat simplistic because we only deal with 256 color DIBs
// and we always write a 256 color table.
//始终用256色
BOOL CMYJPG::Save(CFile* fp)
{
    BITMAPFILEHEADER bfh;

    // Construct the file header.
    bfh.bfType = 0x4D42; // 'BM'
    bfh.bfSize = 
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        GetNumClrEntries()*sizeof(RGBQUAD) +
        StorageWidth() * DibHeight();
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        GetNumClrEntries()*sizeof(RGBQUAD);

    // Write the file header.
	//写文件头
    int iSize = sizeof(bfh);
    TRY 
	{
        fp->Write(&bfh, iSize);
    } 
	CATCH(CFileException, e) 
	{
        TRACE("Failed to write file header");
        return FALSE;
    } END_CATCH

    // Write the BITMAPINFO structure.
    // Note: we assume that there are always 256 colors in the
    // color table.
    ASSERT(m_pBMI);
    iSize = sizeof(BITMAPINFOHEADER) +
        GetNumClrEntries()*sizeof(RGBQUAD);
    TRY 
	{
        fp->Write(m_pBMI, iSize);
    } 
	CATCH(CFileException, e) 
	{
        TRACE("Failed to write BITMAPINFO");
        return FALSE;
    } END_CATCH

    // Write the bits.
    iSize = StorageWidth() * DibHeight();
    TRY 
	{
        fp->Write(m_pBits, iSize);
    } 
	CATCH(CFileException, e) 
	{
        TRACE("Failed to write bits");
        return FALSE;
    } END_CATCH

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////
// Save a DIB to a disk file. If no file name is given, show
// a File Save dialog to get one.
BOOL CMYJPG::Save(LPSTR pszFileName)
{
    CString strFile;    

    if ((pszFileName == NULL) ||  (strlen(pszFileName) == 0)) {
        // Show a File Save dialog to get the name.
        CFileDialog dlg   (FALSE,   // Save
                           NULL,    // No default extension
                           NULL,    // No initial file name
                           OFN_OVERWRITEPROMPT
                             | OFN_HIDEREADONLY,
                           "Image files (*.DIB, *.BMP)|*.DIB;*.BMP|All files (*.*)|*.*||");
        if (dlg.DoModal() == IDOK) {
            strFile = dlg.GetPathName();
        } else {
            return FALSE;
        }
    } else {
    
        // Copy the supplied file path.
        strFile = pszFileName;                    
    }

	BOOL bResult;

	CString ext=strFile;
	int index = ext.ReverseFind('.');
	if(index != -1)
	{
		ext = ext.Mid(index+1,ext.GetLength());
		ext.MakeLower();
	}
	else //if no ext ,default to bmp
		ext == "bmp";

	
	if(ext=="bmp")
	{
		// Try to open the file for write access.
		CFile file;
		if (!file.Open(strFile,
						CFile::modeReadWrite
						 | CFile::modeCreate
						 | CFile::shareExclusive)) {
			AfxMessageBox("Failed to open file");
			return FALSE;
		}

		bResult = Save(&file);
		file.Close();	
	}
	else if(ext == "jpeg" || ext == "jpg")
	{
		bResult = SaveJpeg(pszFileName);
	}
    if (!bResult) AfxMessageBox("Failed to save file");
    return bResult;
}

BOOL CMYJPG::OpenBigBmp(LPSTR pszFileName)
{
	//BOOL bResult;
	CString strFile;
	strFile = pszFileName;

	if (!mBigbmpfile.Open(strFile,
		CFile::modeReadWrite
		| CFile::modeCreate
		| CFile::shareExclusive)) {
			AfxMessageBox("Failed to open file");
			return FALSE;
	}

	//bResult = Save(&file);
	
	return TRUE;
}
void CMYJPG::WriteBigBmpHeader(int total_width,int total_height)
{
	BITMAPFILEHEADER bfh;
	bfh.bfType = 0x4D42; // 'BM'
	bfh.bfSize = sizeof(BITMAPFILEHEADER) +	sizeof(BITMAPINFOHEADER) +	GetNumClrEntries()*sizeof(RGBQUAD) + StorageWidth() * DibHeight() * total_width * total_height;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits =	sizeof(BITMAPFILEHEADER) +	sizeof(BITMAPINFOHEADER) +	GetNumClrEntries()*sizeof(RGBQUAD);
	int iSize = sizeof(bfh);
    TRY 
	{
		mBigbmpfile.SeekToBegin();
        mBigbmpfile.Write(&bfh, iSize);
    } 
	CATCH(CFileException, e) 
	{
        TRACE("Failed to write file header");
        return;
    } END_CATCH

	iSize = sizeof(BITMAPINFOHEADER) + GetNumClrEntries()*sizeof(RGBQUAD);//信息头
	BITMAPINFOHEADER bmi;
	bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biWidth = DibWidth() * total_width;
    bmi.biHeight = DibHeight() * total_height;
    bmi.biPlanes = 1;
    bmi.biBitCount = 24;
    bmi.biCompression = BI_RGB;
    bmi.biSizeImage = 0;
    bmi.biXPelsPerMeter = 0;
    bmi.biYPelsPerMeter = 0;
    bmi.biClrUsed = 0;
    bmi.biClrImportant = 0;
	TRY 
	{
        mBigbmpfile.Write(&bmi, iSize);
    } 
	CATCH(CFileException, e) 
	{
        TRACE("Failed to write BITMAPINFO");
        return;
    } END_CATCH

}
void CMYJPG::SeekToBmpData(ULONGLONG deltapos)
{
	ULONGLONG iSize = deltapos + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + GetNumClrEntries()*sizeof(RGBQUAD);
	mBigbmpfile.Seek(iSize,CFile::begin);
}
void CMYJPG::WriteBigBmp(BYTE* pBits,int iSize,ULONGLONG pos)
{
	mBigbmpfile.Seek(pos,CFile::begin);
	mBigbmpfile.Write(pBits, iSize);
}
void CMYJPG::WriteBigBmp(BYTE* pBits,int iSize)
{
	mBigbmpfile.Write(pBits, iSize);
}
void CMYJPG::CloseBigBmp()
{
	mBigbmpfile.Close();
}

//////////////////////////////////////////////////////////////////////////
// NOTE: This assumes all CMYJPG objects have 256 color table entries.
BOOL CMYJPG::MapColorsToPalette(CPalette *pPal)
{
	if(GetBitCount()==24)return TRUE;

    if (!pPal) 
	{
        TRACE("No palette to map to");
        return FALSE;
    }
    ASSERT(m_pBMI);
    ASSERT(m_pBMI->bmiHeader.biBitCount == 8);
    ASSERT(m_pBits);
    LPRGBQUAD pctThis = GetClrTabAddress();
    ASSERT(pctThis);
    // Build an index translation table to map this DIBs colors
    // to those of the reference DIB.
  //  BYTE imap[256];
    int iChanged = 0; // For debugging only
    for (int i = 0; i < 256; i++) 
	{
        imap[i] = (BYTE) pPal->GetNearestPaletteIndex(
                            RGB(pctThis->rgbRed,
                                pctThis->rgbGreen,
                                pctThis->rgbBlue));
        pctThis++;
        if (imap[i] != i) iChanged++; // For debugging
    }
    // Now map the DIB bits.
 	BYTE* pBits = (BYTE*)GetBitsAddress();
    int iSize = StorageWidth() * DibHeight();
    while (iSize--) {
        *pBits = imap[*pBits];
        pBits++;
    } 
    // Now reset the DIB color table so that its RGB values match
    // those in the palette.
    PALETTEENTRY pe[256];
    pPal->GetPaletteEntries(0, 256, pe);
    pctThis = GetClrTabAddress();
    for (int i = 0; i < 256; i++) {
        pctThis->rgbRed = pe[i].peRed;    
        pctThis->rgbGreen = pe[i].peGreen;    
        pctThis->rgbBlue = pe[i].peBlue;
        pctThis++;    
    }
    // Now say all the colors are in use
    m_pBMI->bmiHeader.biClrUsed = 256;
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////
//以下函数不提供真彩支持
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
BOOL CMYJPG::Load(LPSTR pszFileName,CRect* pRect)
{
    CString strFile;    

    if ((pszFileName == NULL) ||  (strlen(pszFileName) == 0)) {
        // Show an Open File dialog to get the name.
        CFileDialog dlg   (TRUE,    // Open
                           NULL,    // No default extension
                           NULL,    // No initial file name
                           OFN_FILEMUSTEXIST
                             | OFN_HIDEREADONLY,
                           "Image files (*.DIB, *.BMP)|*.DIB;*.BMP|All files (*.*)|*.*||");
        if (dlg.DoModal() == IDOK) {
            strFile = dlg.GetPathName();
        } else {
            return FALSE;
        }
    } else {
        // Copy the supplied file path.
        strFile = pszFileName;                    
    }

    // Try to open the file for read access.
    CFile file;
    if (! file.Open(strFile,
                    CFile::modeRead | CFile::shareDenyWrite)) {
        TRACE("Failed to open file");
        return FALSE;
    }

    BOOL bResult = Load(&file,pRect);
    file.Close();
    return bResult;
}

////////////////////////////////////////////////////////////////////////////
//
BOOL CMYJPG::Load(LPSTR pszFileName,int x,int y,int w,int h)
{
    CString strFile;    

    if ((pszFileName == NULL) 
    ||  (strlen(pszFileName) == 0)) {

        // Show an Open File dialog to get the name.
        CFileDialog dlg   (TRUE,    // Open
                           NULL,    // No default extension
                           NULL,    // No initial file name
                           OFN_FILEMUSTEXIST
                             | OFN_HIDEREADONLY,
                           "Image files (*.DIB, *.BMP)|*.DIB;*.BMP|All files (*.*)|*.*||");
        if (dlg.DoModal() == IDOK) {
            strFile = dlg.GetPathName();
        } else {
            return FALSE;
        }
    } else {
        // Copy the supplied file path.
        strFile = pszFileName;                    
    }

    // Try to open the file for read access.
    CFile file;
    if (! file.Open(strFile,
                    CFile::modeRead | CFile::shareDenyWrite)) {
        TRACE("Failed to open file");
        return FALSE;
    }

    BOOL bResult = Load(&file,x,y,w,h);
    file.Close();
    return bResult;
}

/////////////////////////////////////////////////////////////////////////
//
BOOL CMYJPG::Load(CFile* fp,CRect* pRect)
{    
    BITMAPINFO* pBmpInfo = NULL;
    BYTE* pBits = NULL;
    // Get the current file position.
    DWORD dwFileStart = fp->GetPosition();

    // Read the file header to get the file size and to
    // find where the bits start in the file.
    BITMAPFILEHEADER BmpFileHdr;
    int iBytes;
    iBytes = fp->Read(&BmpFileHdr, sizeof(BmpFileHdr));
    if (iBytes != sizeof(BmpFileHdr)) {
        TRACE("Failed to read file header");
        goto $abort;
    }

    // Check that we have the magic 'BM' at the start.
    if (BmpFileHdr.bfType != 0x4D42 && BmpFileHdr.bfType != 0x9608) {
        TRACE("Not a bitmap file");
        goto $abort;
    }

    // Make a wild guess that the file is in Windows DIB
    // format and read the BITMAPINFOHEADER. If the file turns
    // out to be a PM DIB file we'll convert it later.
    BITMAPINFOHEADER BmpInfoHdr;
    iBytes = fp->Read(&BmpInfoHdr, sizeof(BmpInfoHdr)); 
    if (iBytes != sizeof(BmpInfoHdr)) {
        TRACE("Failed to read BITMAPINFOHEADER");
        goto $abort;
    }

    // Check that we got a real Windows DIB file.
    if (BmpInfoHdr.biSize != sizeof(BITMAPINFOHEADER)) {
        if (BmpInfoHdr.biSize != sizeof(BITMAPCOREHEADER)) {
            TRACE(" File is not Windows or PM DIB format");
            goto $abort;
        }
		return FALSE;
        // Set a flag to convert PM file to Win format later.
      }

    // Work out how much memory we need for the BITMAPINFO
    // structure, color table and then for the bits.  
    // Allocate the memory blocks.
    // Copy the BmpInfoHdr we have so far,
    // and then read in the color table from the file.
    int iColors;
    int iColorTableSize;
    iColors = NumDIBColorEntries((LPBITMAPINFO) &BmpInfoHdr);
    iColorTableSize = iColors * sizeof(RGBQUAD);
    int iBitsSize;
    int iBISize;
    // Always allocate enough room for 256 entries.
    iBISize = sizeof(BITMAPINFOHEADER)    
           + 256 * sizeof(RGBQUAD);
    

    // Allocate the memory for the header.
    pBmpInfo = (LPBITMAPINFO) ALLOC(iBISize);
    if (!pBmpInfo) {
        TRACE("Out of memory for DIB header");
        goto $abort;
    }

    // Copy the header we already have.
    memcpy(pBmpInfo, &BmpInfoHdr, sizeof(BITMAPINFOHEADER));
	if(pRect->left<0)pRect->left=0;
	if(pRect->top<0)pRect->top=0;
	if(pRect->top>=BmpInfoHdr.biHeight)
		pRect->top=BmpInfoHdr.biHeight-1;
	if(pRect->right>=BmpInfoHdr.biWidth)
		pRect->right=BmpInfoHdr.biWidth-1;

	if(pRect->bottom>=BmpInfoHdr.biHeight)
		pRect->bottom=BmpInfoHdr.biHeight-1;
	if(pRect->right>=BmpInfoHdr.biWidth)
		pRect->right=BmpInfoHdr.biWidth-1;

	pBmpInfo->bmiHeader.biWidth=pRect->Width();
	pBmpInfo->bmiHeader.biHeight=pRect->Height();
	pBmpInfo->bmiHeader.biSizeImage=((pBmpInfo->bmiHeader.biWidth + 3) & ~3)*pBmpInfo->bmiHeader.biHeight;
	iBitsSize =pBmpInfo->bmiHeader.biSizeImage; 
  
    // Now read the color table from the file.
    
       // Read the color table from the file.
    iBytes = fp->Read(((LPBYTE) pBmpInfo) + sizeof(BITMAPINFOHEADER),
                             iColorTableSize);
    if (iBytes != iColorTableSize)
    {
        TRACE("Failed to read color table");
        goto $abort;
    }
     
    // Allocate the memory for the bits
    // and read the bits from the file.
	int iStoreS,iStoreD,h,w;
	BYTE* pDest;
	int nOffset,hh;


	pBits = (BYTE*) ALLOC(iBitsSize);
	if (!pBits) 
	{
        TRACE("Out of memory for DIB bits");
        goto $abort;
    }
	h=pRect->Height();
	hh=h;
	w=pRect->Width();
	iStoreS = (BmpInfoHdr.biWidth + 3) & ~3;
	iStoreD = (w + 3) & ~3;
	nOffset=dwFileStart + BmpFileHdr.bfOffBits+(BmpInfoHdr.biHeight-h-pRect->top) * iStoreS
			+pRect->left;
	fp->Seek(nOffset, CFile::begin);
	pDest =pBits;//+(pRect->Height()-h+1) * iStoreD;
  
   if(BmpFileHdr.bfType == 0x4D42)
	{
		//BMP format
    while (h) {
        fp->Read(pDest,w);
		nOffset=nOffset+iStoreS;
		fp->Seek(nOffset,CFile::begin );
        pDest += iStoreD;
		h--;
    }
	    BitSize	=iBitsSize;
	}
    if (m_pBMI != NULL) FREE(m_pBMI);
    m_pBMI = pBmpInfo; 
    if (m_bMyBits && (m_pBits != NULL)) FREE(m_pBits);
    m_pBits = pBits;
    m_bMyBits = TRUE;
    return TRUE;

$abort: // Something went wrong.
    if (pBmpInfo) FREE(pBmpInfo);
    if (pBits) FREE(pBits);
    return FALSE;

}

/////////////////////////////////////////////////////////////////////////////
//
BOOL CMYJPG::Load(CFile* fp,int x,int y,int w,int h)
{
    
    BITMAPINFO* pBmpInfo = NULL;
    BYTE* pBits = NULL;
    // Get the current file position.
    DWORD dwFileStart = fp->GetPosition();

    // Read the file header to get the file size and to
    // find where the bits start in the file.
    BITMAPFILEHEADER BmpFileHdr;
    int iBytes;
    iBytes = fp->Read(&BmpFileHdr, sizeof(BmpFileHdr));
    if (iBytes != sizeof(BmpFileHdr)) {
        TRACE("Failed to read file header");
        goto $abort;
    }

    // Check that we have the magic 'BM' at the start.
    if (BmpFileHdr.bfType != 0x4D42
		 && BmpFileHdr.bfType != 0x9608) {
        TRACE("Not a bitmap file");
        goto $abort;
    }

    // Make a wild guess that the file is in Windows DIB
    // format and read the BITMAPINFOHEADER. If the file turns
    // out to be a PM DIB file we'll convert it later.
    BITMAPINFOHEADER BmpInfoHdr;
    iBytes = fp->Read(&BmpInfoHdr, sizeof(BmpInfoHdr)); 
    if (iBytes != sizeof(BmpInfoHdr)) {
        TRACE("Failed to read BITMAPINFOHEADER");
        goto $abort;
    }

    // Check that we got a real Windows DIB file.
    if (BmpInfoHdr.biSize != sizeof(BITMAPINFOHEADER)) {
        if (BmpInfoHdr.biSize != sizeof(BITMAPCOREHEADER)) {
            TRACE(" File is not Windows or PM DIB format");
            goto $abort;
        }
		return FALSE;
        // Set a flag to convert PM file to Win format later.
      }

    // Work out how much memory we need for the BITMAPINFO
    // structure, color table and then for the bits.  
    // Allocate the memory blocks.
    // Copy the BmpInfoHdr we have so far,
    // and then read in the color table from the file.
    int iColors;
    int iColorTableSize;
    iColors = NumDIBColorEntries((LPBITMAPINFO) &BmpInfoHdr);
    iColorTableSize = iColors * sizeof(RGBQUAD);
    int iBitsSize;
    int iBISize;
    // Always allocate enough room for 256 entries.
    iBISize = sizeof(BITMAPINFOHEADER)    
           + 256 * sizeof(RGBQUAD);
    

    // Allocate the memory for the header.
    pBmpInfo = (LPBITMAPINFO) ALLOC(iBISize);
    if (!pBmpInfo) {
        TRACE("Out of memory for DIB header");
        goto $abort;
    }

    // Copy the header we already have.
    memcpy(pBmpInfo, &BmpInfoHdr, sizeof(BITMAPINFOHEADER));
	if(w<0)w=0;
	if(h<0)h=0;
	if(x<0)x=0;
	if(y<0)y=0;
	if(y>BmpInfoHdr.biHeight)
		y=BmpInfoHdr.biHeight;
	if(x>BmpInfoHdr.biWidth)
		x=BmpInfoHdr.biWidth;

	if(y+h>BmpInfoHdr.biHeight)
		h=BmpInfoHdr.biHeight-y;
	if(x+w>BmpInfoHdr.biWidth)
		w=BmpInfoHdr.biWidth-x;
	

	
	pBmpInfo->bmiHeader.biWidth=w;
	pBmpInfo->bmiHeader.biHeight=h;
	pBmpInfo->bmiHeader.biSizeImage=((pBmpInfo->bmiHeader.biWidth + 3) & ~3)*pBmpInfo->bmiHeader.biHeight;
	iBitsSize =pBmpInfo->bmiHeader.biSizeImage; 
  
    // Now read the color table from the file.
    
       // Read the color table from the file.
    iBytes = fp->Read(((LPBYTE) pBmpInfo) + sizeof(BITMAPINFOHEADER),
                             iColorTableSize);
    if (iBytes != iColorTableSize)
    {
        TRACE("Failed to read color table");
        goto $abort;
    }
     
    // Allocate the memory for the bits
    // and read the bits from the file.
	int iStoreS,iStoreD;
	BYTE* pDest;
	int nOffset;


	pBits = (BYTE*) ALLOC(iBitsSize);
	if (!pBits) 
	{
        TRACE("Out of memory for DIB bits");
        goto $abort;
    }
	iStoreS = (BmpInfoHdr.biWidth + 3) & ~3;
	iStoreD = (w + 3) & ~3;
	nOffset=dwFileStart + BmpFileHdr.bfOffBits+(BmpInfoHdr.biHeight-h-y) * iStoreS
			+x;
	fp->Seek(nOffset, CFile::begin);
	pDest =pBits;
  
   if(BmpFileHdr.bfType == 0x4D42)
	{
		//BMP format
    while (h) {
        fp->Read(pDest,w);
		nOffset=nOffset+iStoreS;
		fp->Seek(nOffset,CFile::begin );
        pDest += iStoreD;
		h--;
    }
	    BitSize	=iBitsSize;
	}
    if (m_pBMI != NULL) FREE(m_pBMI);
    m_pBMI = pBmpInfo; 
    if (m_bMyBits && (m_pBits != NULL)) FREE(m_pBits);
    m_pBits = pBits;
    m_bMyBits = TRUE;
    return TRUE;
                
$abort: // Something went wrong.
    if (pBmpInfo) FREE(pBmpInfo);
    if (pBits) FREE(pBits);
    return FALSE;

}

void FillArea(int oldW,int oldH,int newW,int newH,BYTE *psrc,BYTE *pdes);
//////////////////////////////////////////////////////////////////////////
//
void CMYJPG::ScaleDIB(CMYJPG *pd,int xd,int yd,int w,int h,COLORREF clrTrans )
{
//	FillArea(DibWidth(),DibHeight(),w,h,m_pBits,pd->m_pBits);
	
	if(w==0||h==0)return ;
	float xScale =(float) GetWidth()/w;
	float yScale =(float) GetHeight()/h;
	if(xScale==0||yScale==0)return;
	if(xd<0)xd=0;
	if(xd>pd->GetWidth())xd=pd->GetWidth();
	if(yd<0)yd=0;
	if(yd>pd->GetHeight())yd=pd->GetHeight();
	if(xd+w>pd->GetWidth())w=pd->GetWidth()-xd;
	if(yd+h>pd->GetHeight())h=pd->GetHeight()-yd;
	float ytemp = (float)0;
	for(int y=0;y<h;y++){
		float xtemp =(float)0;
		if((yd+y)<=pd->GetHeight()&&(yd+y)>=0){
			for(int x=0;x<w;x++)
				if((xd+x)<=pd->GetWidth()&&(xd+x)>=0){
				if(xtemp<=GetWidth()&&xtemp>=0
					&&ytemp<=GetHeight()&&ytemp>=0)
					CopyBits(pd,xd+x,yd+y,1,1,(int)xtemp,(int)ytemp,clrTrans);
					xtemp+= xScale;	 
				}

			ytemp+= yScale;	 
		}
	}

}

///////////////////////////////////////////////////////////////////////////
//
void CMYJPG::RotateDIB(CMYJPG *pd,int x,int y,int degree,COLORREF clrTrans )
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

////////////////////////////////////////////////////////////////////////////
//
void CMYJPG::ScaleRotateDIB(CMYJPG *pd,int x,int y,int w,int h,int degree,COLORREF clrTrans)
{
	if(w==0||h==0)return;
	if(x<0||y<0)return;
	float rad_degree=(float)(degree*2*3.1415926/360);
	float sin_val=(float)sin(rad_degree);
	float cos_val=(float)cos(rad_degree);
	float xScale =(float) GetWidth()/w;
	float yScale =(float) GetHeight()/h;
	if(xScale==0||yScale==0)return;
	int srcW=GetWidth();
	int srcH=GetHeight();
	int xd,yd;
	float ytemp =(float)0;
       for (int ys=0;ys<srcH-2;ys++) {
	   	float xtemp = (float)0;
        for (int xs=0;xs<srcW-2;xs++) {
          xd=(int)((float)x+(float)xs*cos_val-(float)ys*sin_val); 
    	  yd=(int)((float)y+(float)ys*cos_val+(float)xs*sin_val);
    	    CopyBits(pd,xd,yd,1,1,(int)xtemp,(int)ytemp,clrTrans);
			xtemp +=xScale;
         } 
		 ytemp +=yScale;
     }	
}

//////////////////////////////////////////////////////////////////////////////
//
BOOL CMYJPG::Compress(char* pszFileName)
{
    CString strFile;    

    if ((pszFileName == NULL) 
    ||  (strlen(pszFileName) == 0)) {

        // Show an Open File dialog to get the name.
        CFileDialog dlg   (TRUE,    // Open
                           NULL,    // No default extension
                           NULL,    // No initial file name
                           OFN_FILEMUSTEXIST
                             | OFN_HIDEREADONLY,
                           "Image files (*.DIB, *.BMP)|*.DIB;*.BMP|All files (*.*)|*.*||");
        if (dlg.DoModal() == IDOK) {
            strFile = dlg.GetPathName();
        } else {
            return FALSE;
        }
    } else {
        // Copy the supplied file path.
        strFile = pszFileName;                    
    }

    // Try to open the file for read access.
    CFile file;
    if (! file.Open(strFile,
                    CFile::modeRead | CFile::shareDenyWrite)) {
        TRACE("Failed to open file");
        return FALSE;
    }

    BOOL bResult = Load(&file);
    file.Close();
	if(!bResult)  return FALSE;
//compress
	int csize=0;
	int bsize=StorageWidth() * DibHeight();
    BYTE* c_pBits;
    c_pBits = (BYTE*)ALLOC(2*bsize);
    if (!c_pBits) {
        TRACE("Out of memory for CBP bits");
        return FALSE;
    }
    if(m_bMyBits)
	{
	  BYTE* pSrc=m_pBits;
	  BYTE* pDes=c_pBits;
		for(int nnn=1;nnn<=bsize;)
		 {
			 *pDes=*pSrc;
			 UINT num=0;
			 do{
			   num++;
			   pSrc++;
			   nnn++;
			 }while(*pSrc==*pDes && num<255 && nnn<=bsize);
			 pDes++;
			 *pDes=(BYTE)num;
			 pDes++;
			 csize=csize+2;
		 }

	}else{
        TRACE("no memory for BMP bits");
        return FALSE;
	}	
//save    
	int  strlen=strFile.GetLength();
	strFile.SetAt(strlen-3,'c');
   	strFile.SetAt(strlen-2,'b');
	strFile.SetAt(strlen-1,'p');

	CFile filed;
    if (!filed.Open(strFile,
                    CFile::modeReadWrite
                     | CFile::modeCreate
                     | CFile::shareExclusive)) {
        AfxMessageBox("Failed to open file");
        return FALSE;
    }
	CFile* fp;
	fp=&filed;
	BITMAPFILEHEADER bfh;

    // Construct the file header.
    bfh.bfType = 0x9608; // 'BM'
    bfh.bfSize = 
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        256 * sizeof(RGBQUAD) +	csize;
        //cxf StorageWidth() * DibHeight();
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        256 * sizeof(RGBQUAD);

    // Write the file header.
    int iSize = sizeof(bfh);
    TRY {
        fp->Write(&bfh, iSize);
    } CATCH(CFileException, e) {
        TRACE("Failed to write file header");
	if(c_pBits != NULL) FREE(c_pBits);
        file.Close();
        return FALSE;
    } END_CATCH

    // Write the BITMAPINFO structure.
    // Note: we assume that there are always 256 colors in the
    // color table.
    ASSERT(m_pBMI);
    iSize = 
        sizeof(BITMAPINFOHEADER) +
        256 * sizeof(RGBQUAD);
    TRY {
        fp->Write(m_pBMI, iSize);
    } CATCH(CFileException, e) {
        TRACE("Failed to write BITMAPINFO");
	if(c_pBits != NULL) FREE(c_pBits);
        file.Close();
        return FALSE;
    } END_CATCH

    // Write the bits.
 //cxf   iSize = StorageWidth() * DibHeight();
    TRY {
        fp->Write(c_pBits, csize);
    } CATCH(CFileException, e) {
        TRACE("Failed to write bits");
	if(c_pBits != NULL) FREE(c_pBits);
        file.Close();
        return FALSE;
    } END_CATCH
	if(c_pBits != NULL) FREE(c_pBits);

	filed.Close();
    return TRUE;

}

///////////////////////////////////////////////////////////////////////////
//
void CMYJPG::FillColor(int xd,int yd,int w,int h,COLORREF clrFilled)
{
	ASSERT(m_pBMI->bmiHeader.biBitCount==8);
    if (w == 0 || h == 0) return;
	if((xd+w)>GetWidth()) w=GetWidth()-xd;
	if((yd+h)>GetHeight()) h=GetHeight()-yd;
//    ASSERT((clrFilled & 0xFF000000) == 0x01000000);
    BYTE bClr = LOBYTE(LOWORD(clrFilled));
    BYTE* pDest = (BYTE*)GetPixelAddress(xd, yd + h - 1);
    int iSinc = StorageWidth()-w;
	int iCount;
    while (h--) {
        iCount = w;    // Number of pixels to scan.
        while (iCount--) {
			//copy fill color
			*pDest++=bClr;
		}
		pDest+=iSinc;
	}
}

