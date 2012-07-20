
#ifndef _JPEG_
#define _JPEG_

//#include "windows.h"
#include "stdafx.h"
#include "dibpal.h"
#include "utility.h"

#define COLORTYPE_PALETTE	1
#define COLORTYPE_COLOR		2
#define COLORTYPE_ALPHA		4

class CJpeg
{
protected:
public:
	int quality;
	int Width;
	int Height;
	int Depth;	 				// (bits x pixel)
	int ColorType;				// Bit 1 = Palette used
									// Bit 2 = Color used
									// Bit 3 = Alpha used
	long EffWidth;
	LPBITMAPINFOHEADER lpbi;
	BYTE* RawImage;
	CDIBPal* imagePalette;
	void CreateGrayColourMap(int n);
public:
	CJpeg(int qua = 50);
	~CJpeg();

	BYTE* GetRawImage() { return RawImage; }	
	BOOL Inside(int x, int y)
	{
		return (0<=y && y<Height && 0<=x && x<Width);
	}
	int GetHeight(){return Height;};
	int GetWidth(){return Width;};
	long GetEffWidth() { return EffWidth; }

	virtual void Create(int width, int height, int deep, int colortype=-1);
	BOOL SetPalette(int n, BYTE *r, BYTE *g=0, BYTE *b=0);
	CDIBPal* GetPalette() const { return imagePalette; }

	BOOL ReadFile(const CString& imageFileName="");
	BOOL SaveFile(const CString& imageFileName="");
};

#endif

