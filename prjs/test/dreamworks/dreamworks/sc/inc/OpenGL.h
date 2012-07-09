
#ifndef _OPENGL_H_
#define _OPENGL_H_


class COpenGL
{
public:
	COpenGL();
	~COpenGL();
private:
	HWND m_hWnd;
	HDC m_hDC;
	HGLRC m_hRC;

public: //½Ó¿Ú
	void SetHWnd(HWND hwnd);
	HWND GetHWnd();
	BOOL Initialize();
	void InitSize(int cx, int cy);
	void InitSizeOrtho(int cx, int cy);

	void Draw();

	void Clear();
};


#endif //_OPENGL_H_