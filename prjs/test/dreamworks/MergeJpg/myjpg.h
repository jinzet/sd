// dib.h : header file
//
// CMYJPG class
//

#ifndef MYJPG_H_2001_5_21
#define MYJPG_H_2001_5_21

#define HHH 1024
#define VVV 768

class CMYJPG
{
public:
    CMYJPG();
    virtual ~CMYJPG();
	BITMAPINFO* m_pBMI;         // Pointer to BITMAPINFO struct
	BYTE* m_pBits;              // Pointer to the bits
protected:
   // BITMAPINFO* m_pBMI;         // Pointer to BITMAPINFO struct
   // BYTE* m_pBits;              // Pointer to the bits
    BOOL  m_bMyBits;            // TRUE if DIB owns Bits memory
	BOOL bJpeg;
public:
    BITMAPINFO* GetBitmapInfoAddress()
        {return m_pBMI;}							// Pointer to bitmap info
    virtual void* GetBitsAddress()
        {return m_pBits;}							// Pointer to the bits
    RGBQUAD* GetClrTabAddress()
        {return (LPRGBQUAD)(((BYTE*)(m_pBMI)) 
            + sizeof(BITMAPINFOHEADER));}			// Pointer to color table
    int GetNumClrEntries();							// Number of color table entries

    int DibWidth()
        {return m_pBMI->bmiHeader.biWidth;}
    int DibHeight() 
        {return m_pBMI->bmiHeader.biHeight;}

    virtual int GetWidth() {return DibWidth();}		// Image width
    virtual int GetHeight() {return DibHeight();}	// Image height
    virtual void GetRect(CRect* pRect);
 	
	int StorageWidth();
	int GetBitCount();
	//**************************************************************************
    BOOL Create(int iWidth, int iHeight,int tempBitCount=8);	// Create a new DIB
	//为位值使用一个指定的内存区来初始化一个CMYJPG 对象
    BOOL Create(BITMAPINFO* pBMI, BYTE* pBits);				// Create from existing mem
	BOOL Clone(CMYJPG* pSrcDib);
	//特定位图像素存储的字节的内存地址

    virtual BOOL Load(CFile* fp);               // Load from file
    virtual BOOL Load(LPSTR pszFileName = NULL);// Load DIB from disk file
    virtual BOOL Load(WORD wResid);             // Load DIB from resource
	virtual BOOL Load(HINSTANCE hInst,WORD wResid);
	virtual BOOL LoadJpeg(char* pszFilename);
	virtual BOOL SaveJpeg(char *pszFilename,int quality=50);
	virtual void Draw(CDC* dc, int x, int y,int bx,int by,int bw,int bh);

    virtual BOOL Save(LPSTR pszFileName = NULL);// Save DIB to disk file

	CFile mBigbmpfile;
	BOOL OpenBigBmp(LPSTR pszFileName);//保存大图，采用定位写入方式写数据
	void WriteBigBmpHeader(int total_width,int total_height);
	void SeekToBmpData(ULONGLONG deltapos=0);
	void WriteBigBmp(BYTE* pBits,int iSize,ULONGLONG pos);
	void WriteBigBmp(BYTE* pBits,int iSize);
	void CloseBigBmp();

    virtual BOOL Save(CFile* fp);               // Save to file
    virtual void Serialize(CArchive& ar);

	virtual BOOL MapColorsToPalette(CPalette* pPal);
    virtual void Draw(CDC* pDC, int x, int y);
	void MyDraw(HDC hdc, int x, int y,int dw,int dh);

	virtual void* GetPixelAddress(int x, int y);
	//将一个位图的全部或部分拷贝到另一个中去
    virtual void CopyBits(CMYJPG* pDIB, 
                          int xd, int yd,
                          int w,  int h,
                          int xs, int ys,
                          COLORREF clrTrans = 0xFFFFFFFF);
	virtual void CopyBitsRow(BYTE* pDest, int w, int xs, int ys, COLORREF clrTrans = 0xFFFFFFFF);

//**********************************************************************
//以下函数不提供真彩支持
//**********************************************************************
protected:
    BYTE imap[256];
	BOOL IsBmp;					//TRUE bmp FALSE cbp
	BOOL IsBkgnd;				//TRUE background FALSE sprite
	int BitSize;				//only for cbp 
public:
	////////////////////unused/////////////////////
	BOOL Load(CFile* fp,CRect* pRect);
	BOOL Load(CFile* fp,int x,int y,int w,int h);
	BOOL Load(LPSTR pszFileName,int x,int y,int w,int h);
	BOOL Load(LPSTR pszFileName,CRect* pRect);

	void FillColor(int xd,int yd,int w,int h,COLORREF clrFilled=0x01000000);
	void ScaleDIB(CMYJPG *pd,int xd,int yd,int w,int h,COLORREF clrTrans = 0xFFFFFFFF);
	void RotateDIB(CMYJPG *pd,int x,int y,int degree,COLORREF clrTrans = 0xFFFFFFFF);
	void ScaleRotateDIB(CMYJPG *pd,int x,int y,int w,int h,int degree,COLORREF clrTrans= 0xFFFFFFFF);
	////////////////////unused/////////////////////
    virtual BOOL Compress(char* pszFileName = NULL);// Load DIB from disk file and save
};

#endif // __DIB__
