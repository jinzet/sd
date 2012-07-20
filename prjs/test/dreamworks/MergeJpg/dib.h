// dib.h : header file
//
// CDIB class
//

#ifndef __DIB__
#define __DIB__

//#include <stdio.h>
#include "afx.h"
#include <afxwin.h>

//class CActionList;

class CDIB: public CObject
{
	DECLARE_SERIAL(CDIB)
public:
    CDIB();
    ~CDIB();
	BOOL CDIB::Create(int iWidth, int iHeight,int tempBitCount);
    BITMAPINFO* GetBitmapInfoAddress()
	{return m_pBMI;}                        // Pointer to bitmap info
    void* GetBitsAddress()
	{return m_pBits;}                       // Pointer to the bits
    RGBQUAD* GetClrTabAddress()
	{return (LPRGBQUAD)(((BYTE*)(m_pBMI)) + sizeof(BITMAPINFOHEADER));}       // Pointer to color table
    int GetNumClrEntries();                     // Number of color table entries
    BOOL Create(int width, int height);         // Create a new DIB
    BOOL Create(BITMAPINFO* pBMI, BYTE* pBits); // Create from existing mem
    void* GetPixelAddress(int x, int y);
    BOOL Load(CFile* fp);               // Load from file
    BOOL Load(char* pszFileName = NULL);// Load DIB from disk file
    
    int GetWidth() {return DibWidth();}   // Image width
    int GetHeight() {return DibHeight();} // Image height
    void GetRect(RECT* pRect);

    void Draw(HDC dc, int x, int y);
	void Draw1(HDC dc, int x, int y);
	void StretchDraw(struct HDC__ * dc, int x, int y,int w,int h);
	void RotateDIB(CDIB *pd,int x,int y,int degree,COLORREF clrTrans );
	void CopyBits(CDIB* pdibDest, 
                    int xd, int yd,
                    int w,  int h,
                    int xs, int ys,
                    COLORREF clrTrans);
public:
    int m_id;
	int m_dx,m_dy;
	int nCol;
	int nRow;
	int CurCol;
	int CurRow;
	DWORD m_bTransIndex;

	void SetRowCell(int row){nRow=row;};
	void SetCurRow(int Row){CurRow=Row;}
    void SetColCell(int col){nCol=col;};
	void SetCurCol(int Col){CurCol=Col;};
	void SetId(int id){m_id=id;}
	void SetPosition(int x,int y){m_dx=x;m_dy=y;};
	void SetTransColor();

	int  GetCurCol(void){return CurCol;};
	int  GetCurRow(void){return CurRow;};
	int  GetDx(void){return m_dx;};
	int  GetDy(void){return m_dy;};
    int  GetCols(void){return nCol;};
	int  GetRows(void){return nRow;};
	int  GetId(void){return m_id;};
	int  GetBitCount();
protected:
    BYTE imap[256];
	BOOL IsBmp;					//TRUE bmp FALSE cbp
	BOOL IsBkgnd;				//TRUE background FALSE sprite
	int BitSize;				//only for cbp 
public:
	BOOL HitTest(CPoint point,BOOL bTrans);
	void Render(CDIB* pDIB);
public:
    BITMAPINFO* m_pBMI;         // Pointer to BITMAPINFO struct
    BYTE* m_pBits;              // Pointer to the bits
    BOOL  m_bMyBits;            // TRUE if DIB owns Bits memory
	HGLOBAL m_hDib;
	int m_nQuality;

public:
	//CString filename;
    int DibWidth()
	{return m_pBMI->bmiHeader.biWidth;}
    int DibHeight() 
	{return m_pBMI->bmiHeader.biHeight;}
    int StorageWidth()
	{
		if(m_pBMI == NULL)
		{
			MessageBox(NULL,"err","err",MB_OK);
			return -1;
		}
		if(m_pBMI == NULL)
			return -1;
		int bitswidth = m_pBMI->bmiHeader.biWidth * m_pBMI->bmiHeader.biBitCount;
		return (((bitswidth + 31) & (~31))/8);
	}

	int MemWidth()
	{return (m_pBMI->bmiHeader.biWidth + 3) & ~3;}
	
	virtual BOOL LoadImage(char* pszFilename);
	//CString sLinkDoc;
	//CString sVarName;
	//CString sToolTip;
	//int step;//运动步骤
	void Modify();
	void DrawRect(CDC* pDC,int x,int y);//画出边框方块
	//CActionList *pActionList;
	//CActionList *pOperateAct;
	int WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	bool GetNodeFrame(int node,int& nframe);
	void SetNodeFrame(int node,int nframe);
	bool GetSelectInfo(int& selnode,int& nframe);
	bool AnimateTest();
	bool TimerTest();
	bool DirectApproach();//直接逼近
	int  OperProc();
	void Draw(HDC dc, CRect& destRect,CRect& srcRect);
	void Read(CArchive &ar);
	void Save(CArchive &ar);
	bool AddFilename(void* plist);
};

#endif // __DIB__
