
#ifndef _UTILITY_
#define _UTILITY_

#include "stdafx.h"


#define DibPtr(lpbi)            ((lpbi)->biCompression == BI_BITFIELDS \
                                       ? (LPVOID)(DibColors(lpbi) + 3) \
                                       : (LPVOID)(DibColors(lpbi) + (UINT)(lpbi)->biClrUsed))
#define DibColors(lpbi)         ((RGBQUAD FAR *)((LPBYTE)(lpbi) + (int)(lpbi)->biSize))
#define DibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
                                    ? (int)(1 << (int)(lpbi)->biBitCount)          \
                                    : (int)(lpbi)->biClrUsed)
#define DibCompression(lpbi)    (DWORD)(((LPBITMAPINFOHEADER)(lpbi))->biCompression)

typedef     LPBITMAPINFOHEADER PDIB;
typedef     HANDLE             HDIB;

BOOL        DibSetUsage(PDIB pdib, HPALETTE hpal,UINT wUsage);
PDIB        DibCreate(int bits, int dx, int dy);

#endif