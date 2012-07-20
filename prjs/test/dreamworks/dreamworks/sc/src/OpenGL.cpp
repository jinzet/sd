
#include "windows.h"

#include "gl/gl.h"
#include "gl/glu.h"
#include "OpenGL.h"

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

COpenGL::COpenGL()
{
	m_hWnd = NULL;
	m_hDC = NULL;
	m_hRC = NULL;
}

COpenGL::~COpenGL()
{

}

void COpenGL::SetHWnd(HWND hwnd)
{
	m_hWnd = hwnd;
}

HWND COpenGL::GetHWnd()
{
	return m_hWnd;
}

BOOL COpenGL::Initialize()
{
	if(m_hWnd==NULL)
		return FALSE;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), // Structure size.
		1,                             // Structure version number.
		PFD_DRAW_TO_WINDOW |           // Property flags.
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,                            // 24-bit color.
		0, 0, 0, 0, 0, 0,              // Not concerned with these.
		0, 0, 0, 0, 0, 0, 0,           // No alpha or accum buffer.
		32,                            // 32-bit depth buffer.
		0, 0,                          // No stencil or aux buffer.
		PFD_MAIN_PLANE,                // Main layer type.
		0,                             // Reserved.
		0, 0, 0                        // Unsupported.
	};

	m_hDC=GetDC(m_hWnd);

    int pixelFormat =ChoosePixelFormat(m_hDC, &pfd);
	if (pixelFormat == 0) 
	{
		MessageBox(NULL, L"ChoosePixelFormat() failed:  "
			   L"Cannot find a suitable pixel format.", L"Error", MB_OK); 
		return FALSE;
    } 
	//pixelFormat = 3;
    BOOL success =SetPixelFormat(m_hDC, pixelFormat, &pfd);
	if(!success)
	{
		MessageBox(m_hWnd,L"SetPixelFormat is error!!",L"Message",MB_OK);
		return FALSE;
	}
    DescribePixelFormat(m_hDC, pixelFormat,sizeof(pfd), &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE)
	{
		MessageBox(m_hWnd,L"Please set display mode 16 bit!!",L"Message",MB_OK);
		return FALSE;
	}

    m_hRC = wglCreateContext(m_hDC);
	if(!m_hRC)
	{
		//MessageBox(m_hWnd,"wglCreateContext is error!!","Message",MB_OK);
		return FALSE;
	}
	success=wglMakeCurrent(m_hDC, m_hRC);
	if(!success)
	{
		MessageBox(m_hWnd,L"SetPixelFormat is error!!",L"Message",MB_OK);
		return FALSE;
	}
	return TRUE;
}

void COpenGL::InitSize(int cx, int cy)
{
	if(m_hDC==NULL||m_hRC==NULL)
		return;
	if(cx<=0||cy<=0)
		return;

	if(wglGetCurrentContext()!=m_hRC)
		wglMakeCurrent(m_hDC, m_hRC);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//vcx=cx;
	//vcy=cy;
	glViewport(0,0,cx,cy);

	//glFrustum(vleft,vright,vbottom,vtop,vnear,vfar);
	gluPerspective(45,(GLfloat)cx/(GLfloat)cy, 1.0f, 10000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void COpenGL::InitSizeOrtho(int cx, int cy)
{
	if(m_hDC==NULL||m_hRC==NULL) return;
	if(cx<=0||cy<=0) return;
	//vcx=cx;
	//vcy=cy;
	if(wglGetCurrentContext()!=m_hRC)
		wglMakeCurrent(m_hDC, m_hRC);
	glViewport(0, 0, cx, cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (cx <= cy) 
		glOrtho (-1.5, 1.5, -1.5*(GLfloat)cy/(GLfloat)cx, 
			1.5*(GLfloat)cy/(GLfloat)cx, -10.0, 10.0);
	else
		glOrtho (-1.5*(GLfloat)cx/(GLfloat)cy, 
			1.5*(GLfloat)cx/(GLfloat)cy, -1.5, 1.5, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void COpenGL::Draw()
{
	if(m_hWnd==NULL)
		return;
	if(wglGetCurrentContext()!=m_hRC)
		wglMakeCurrent(m_hDC, m_hRC);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.65f, 0.78f, 0.93f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

//	LookAt(m_hJds);
//	RenderJDS(m_hJds);

	glFlush();
	SwapBuffers(m_hDC);
}

void COpenGL::Clear()
{
	if(m_hWnd==NULL)
		return;

	wglMakeCurrent(m_hDC, NULL);
	wglDeleteContext(m_hRC);
	ReleaseDC(m_hWnd,m_hDC);

	m_hWnd=NULL;
}

