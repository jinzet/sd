// dibpal.h : header file
//
// CDIBPal class
//

#ifndef __DIBPAL__
#define __DIBPAL__

#include "Mmsystem.h"

#include "myjpg.h"

class CDIBPal : public CPalette
{
public:
    CDIBPal();
	CDIBPal(const CDIBPal *pal);
	CDIBPal(const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
    ~CDIBPal();
    BOOL Create(CMYJPG *pDIB);            // Create from a DIB
    int GetNumColors();                 // Get the number of colors
                                        // in the palette.
    void Draw(CDC* pDC, CRect* pRect, BOOL bBkgnd = FALSE); 
    BOOL SetSysPalColors();
    BOOL Load(char* pszFileName = NULL);
    BOOL Load(CFile* fp);  
    BOOL Load(UINT hFile);
    BOOL Load(HMMIO hmmio);
    BOOL Save(CFile* fp);  
    BOOL Save(UINT hFile);
    BOOL Save(HMMIO hmmio);

	//add by ljh
	BOOL Create(const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
  int GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue);
  BOOL GetRGB(const int pixel, unsigned char *red, unsigned char *green, unsigned char *blue);
};

#endif // __DIBPAL__

