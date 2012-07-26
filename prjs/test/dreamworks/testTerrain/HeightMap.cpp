/**-----------------------------------------------------------------------------
 * 文件: HeightMap.cpp
 *
 * 说明: HeightMap的实现 + Frustum Culling
 *       
*------------------------------------------------------------------
*------------------------------------------------------------------------------
 */
#define MAINBODY
#include <d3d9.h>
#include <d3dx9.h>
#include "ZCamera.h"
#include "ZFrustum.h"
#include "ZFLog.h"
#define WINDOW_W		500
#define WINDOW_H		500
#define WINDOW_TITLE	"HeightMap+Camera+Frustum"
#define BMP_HEIGHTMAP	"terrain.bmp" //"map128.bmp"
/**-----------------------------------------------------------
*  全局参数 
*-------------------------------------------------------------*/
ZCamera*				g_pCamera = NULL;	// Camera 类
ZFrustum*				g_pFrustum = NULL;	// Frustum 类
HWND					g_hwnd = NULL;
LPDIRECT3D9             g_pD3D       = NULL; // 创建D3D 设备的D3D对象参数
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // 渲染中使用的D3D设备
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // 储存顶点的顶点缓冲
LPDIRECT3DINDEXBUFFER9	g_pIB        = NULL; // 储存索引的索引缓冲
LPDIRECT3DTEXTURE9		g_pTexHeight = NULL; // 纹理高度图
LPDIRECT3DTEXTURE9		g_pTexDiffuse= NULL; // 纹理颜色图
D3DXMATRIXA16			g_matAni;
D3DXMATRIXA16			g_matWorld;
D3DXMATRIXA16			g_matView;
D3DXMATRIXA16			g_matProj;
DWORD					g_cxHeight = 0;			// 纹理的长度

DWORD					g_czHeight = 0;			// 纹理的宽度
DWORD					g_dwMouseX = 0;			// 鼠标的坐标
DWORD					g_dwMouseY = 0;			// 鼠标的坐标
int						g_nTriangles = 0;		// 要绘制的三角形的个数
D3DXVECTOR3*			g_pvHeightMap = NULL;	// 保存高度图的阵列
BOOL					g_bHideFrustum = TRUE;	// 不绘制平截头体?
BOOL					g_bLockFrustum = FALSE;	// 固定平截头体?
BOOL					g_bWireframe = FALSE;	// 采用线框进行绘制?
/// 定义用户顶点的结构体
struct CUSTOMVERTEX
{
	D3DXVECTOR3		p;
	D3DXVECTOR3		n;
	D3DXVECTOR2		t;
};
/// 表现用户顶点结构体相关信息的FVF值
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
struct MYINDEX
{
	WORD	_0, _1, _2;		/// WORD，16位索引
};
/**-----------------------------------------------------------
 * Direct3D初始化
*-------------------------------------------------------------*/
HRESULT InitD3D( HWND hWnd )
{
	// 创建一个用来创建设备的D3D对象
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	// 创建设备的结构体
	// 绘制复杂对象时需要Z－缓冲.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	/// 创建设备
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}
	// 基本卷起，CCW
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	// 起到Z－缓冲功能.
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	return S_OK;
}
/**-----------------------------------------------------
* 创建矩阵
*-----------------------------------------------------------*/
void InitMatrix()
{
	/// 创建世界矩阵
	D3DXMatrixIdentity( &g_matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );
    /// 创建视图矩阵
    D3DXVECTOR3 vEyePt( 0.0f, 100.0f, -(float)g_czHeight );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &g_matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &g_matView );
    /// 实际投影矩阵
	D3DXMatrixPerspectiveFovLH( &g_matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );
	/// 平截头体用投影矩阵
    D3DXMatrixPerspectiveFovLH( &g_matProj, D3DX_PI/4, 1.0f, 1.0f, 200.0f );
	/// 摄像机初始化
	g_pCamera->SetView( &vEyePt, &vLookatPt, &vUpVec );
}

/**----------------------------------------------------
* 纹理初始化
*--------------------------------------------------------*/
HRESULT InitTexture()
{
	// 高度图纹理
	// 调用该函数处理D3DFMT_X8R8G8B8和D3DPOOL_MANAGED.
	if( FAILED( D3DXCreateTextureFromFileEx( g_pd3dDevice,
								 BMP_HEIGHTMAP, 
								 D3DX_DEFAULT, D3DX_DEFAULT, 
								 D3DX_DEFAULT, 0, 
								 D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, 
								 D3DX_DEFAULT, D3DX_DEFAULT, 0, 
								 NULL, NULL, &g_pTexHeight) ) )
		return E_FAIL;

	// 颜色图
	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, "tile2.tga", &g_pTexDiffuse) ) )  //"terrain_texture.jpg"
		return E_FAIL;

	return S_OK;
}
/**-----------------------------------------
* 创建顶点缓冲，设置顶点值.
 * HeightMap信息初始化.
*-----------------------------------------------------*/
HRESULT InitVB()
{
	D3DSURFACE_DESC		ddsd;
	D3DLOCKED_RECT		d3drc;
	g_pTexHeight->GetLevelDesc( 0, &ddsd );	// 纹理信息
	g_cxHeight = ddsd.Width;				// 纹理的长度
	g_czHeight = ddsd.Height;				// 纹理的宽度
	g_pLog->Log( "Texture Size:[%d,%d]", g_cxHeight, g_czHeight );
	// 创建高度图阵列
	g_pvHeightMap = new D3DXVECTOR3[g_cxHeight * g_czHeight];
	//创建顶点缓冲区 
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( ddsd.Width*
		ddsd.Height*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, 
		&g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}
	// 纹理存储器!
	g_pTexHeight->LockRect( 0, &d3drc, NULL, D3DLOCK_READONLY );


	VOID* pVertices;//顶点
	// 顶点缓冲!把所有顶点锁定到pVertices中
	if( FAILED( g_pVB->Lock( 0,
		g_cxHeight*g_czHeight*sizeof(CUSTOMVERTEX), 
		(void**)&pVertices, 0 ) ) )
		return E_FAIL;
	//结构体的实例
	CUSTOMVERTEX	v;
	//结构体的指针，所有点都附值过来
	CUSTOMVERTEX*	pV = (CUSTOMVERTEX*)pVertices;
	for( DWORD z = 0 ; z < g_czHeight ; z++ )
	{
		for( DWORD x = 0 ; x < g_cxHeight ; x++ )
		{
			v.p.x = (float)x-g_cxHeight/2.0f;		// 顶点的x坐标（以原点为准对齐网格）
			v.p.z = -((float)z-g_czHeight/2.0f);	// 顶点的z坐标（以顶点为准对齐网格），z轴指向显示器，乘以“-”.
			v.p.y = ((float)(*((LPDWORD)d3drc.pBits+x+z*(d3drc.Pitch/4))&0x000000ff))/10.0f;	/// 由于是DWORD，因此为pitch/4
			v.n.x = v.p.x;
			v.n.y = v.p.y;
			v.n.z = v.p.z;
			D3DXVec3Normalize( &v.n, &v.n );
			v.t.x = (float)x / (g_cxHeight-1);
			v.t.y = (float)z / (g_czHeight-1);
			*pV++ = v;		// 顶点储存到顶点缓冲
			// 顶点储存到高度图
			g_pvHeightMap[z * g_cxHeight + x] = v.p;	
		}
	}
	g_pVB->Unlock();
	g_pTexHeight->UnlockRect( 0 );
    return S_OK;
}
/**------------------------------------------------
* 索引缓冲初始化
*----------------------------------------------------*/
HRESULT InitIB()
{
 if( FAILED( g_pd3dDevice->CreateIndexBuffer( (g_cxHeight-1)*(g_czHeight-1)*2 * sizeof(MYINDEX), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT,&g_pIB, NULL ) ) )
    {
        return E_FAIL;
    }
	g_nTriangles = 0;
    return S_OK;
}
/**-------------------------------------------------
* 几何信息初始化
 *---------------------------------------------------- */
HRESULT InitGeometry()
{
	if( FAILED( InitTexture() ) ) return E_FAIL;
	if( FAILED( InitVB() ) ) return E_FAIL;
	if( FAILED( InitIB() ) ) return E_FAIL;
	InitMatrix();
	// 保存最初的鼠标位置
	POINT	pt;
	GetCursorPos( &pt );
	g_dwMouseX = pt.x;
	g_dwMouseY = pt.y;
	return S_OK;
}
/**------------------------------------------------ 
* 删除初始化对象
*------------------------------------------------------------*/
VOID Cleanup()
{
    if( g_pTexHeight != NULL )        
        g_pTexHeight->Release();
   if( g_pTexDiffuse!= NULL )        
        g_pTexDiffuse->Release();
    if( g_pIB != NULL )        
        g_pIB->Release();
    if( g_pVB != NULL )        
        g_pVB->Release();
    if( g_pd3dDevice != NULL ) 
        g_pd3dDevice->Release();
    if( g_pD3D != NULL )       
        g_pD3D->Release();
	delete[] g_pvHeightMap;
}
/**-----------------------------------------------------
* 创建光源
*----------------------------------------------------------*/
VOID SetupLights()
{
    /// 创建材质（material）
    /// 在设备中创建一个材质.
    D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	g_pd3dDevice->SetMaterial( &mtrl );
	/// 创建光源
	D3DXVECTOR3 vecDir;	/// 方向光源（directional light）照射的方向
	D3DLIGHT9 light;		/// 光源结构体
	ZeroMemory( &light, sizeof(D3DLIGHT9) );	/// 将结构体清零.
	/// 光源类型（点光源、方向光源、聚光光源）
	light.Type       = D3DLIGHT_DIRECTIONAL;	
	light.Diffuse.r  = 1.0f;		/// 光源的颜色和亮度
	light.Diffuse.g  = 1.0f;
	light.Diffuse.b  = 0.0f;
	vecDir = D3DXVECTOR3( 1, 1, 1 );	/// 光源的固定
	vecDir = D3DXVECTOR3(cosf(GetTickCount()/350.0f),	/// 光源旋转
		1.0f,
		sinf(GetTickCount()/350.0f) );
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );	/// 光源的方向设定为单位向量.
	light.Range       = 1000.0f;	/// 光源能够照射到的最远距离
	g_pd3dDevice->SetLight( 0, &light );	/// 在设备设置0号光源
	g_pd3dDevice->LightEnable( 0, TRUE );	/// 打开0号光源
	/// 打开光源设置
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );	
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00909090 );		/// 设定环境光源（ambient light）的值
}
void LogStatus( void )
{
	g_pLog->Log( "Wireframe:%d", g_bWireframe );
	g_pLog->Log( "HideFrustum:%d", g_bHideFrustum );
	g_pLog->Log( "LockFrustum:%d", g_bLockFrustum );
}
void LogFPS(void)
{
	static DWORD	nTick = 0;
	static DWORD	nFPS = 0;

	/// 1秒过了?
	if( GetTickCount() - nTick > 1000 )
	{
		nTick = GetTickCount();
		/// FPS输出
		g_pLog->Log("FPS:%d", nFPS );

		/// 摄像机位置值输出
//		D3DXVECTOR3*	pv;
//		pv = g_pCamera->GetEye();
//		g_pLog->Log("EYE:[%f,%f,%f]",pv->x, pv->y, pv->z );
		nFPS = 0;
		LogStatus();	/// 状态信息输出（1次/秒）
		return;
	}
	nFPS++;
}
/**----------------------
* 使用平截头体的剔除技巧创建想要绘制的索引
*------------------------------------------------*/
HRESULT ProcessFrustumCull()
{
	DWORD		i[4];	// 临时储存的索引信息
	BOOL		b[4];	// 临时储存的平截头体剔除的结果值
	MYINDEX		idx;
    MYINDEX*	pI;
	//锁定到PI上
    if( FAILED( g_pIB->Lock( 0, (g_cxHeight-1)*(g_czHeight-1)*2 * sizeof(MYINDEX), (void**)&pI, 0 ) ) )
        return E_FAIL;
	//要绘制三角形的个数
	g_nTriangles = 0;
	//循环看每个点
	for( DWORD z = 0 ; z < g_czHeight-1 ; z++ )
	{
		for( DWORD x = 0 ; x < g_cxHeight-1 ; x++ )
		{
			//地图中的点
			i[0] = (z*g_cxHeight+x);			// 左侧上方
			i[1] = (z*g_cxHeight+x+1);			// 右侧上方
			i[2] = ((z+1)*g_cxHeight+x);		// 左侧下方
			i[3] = ((z+1)*g_cxHeight+x+1);		// 右侧下方
			//检测是否在范围内
			b[0] = g_pFrustum->IsIn( &g_pvHeightMap[ i[0] ] );	// 左侧上方顶点在平截头体内部?
			b[1] = g_pFrustum->IsIn( &g_pvHeightMap[ i[1] ] );	// 右侧上方顶点在平截头体内部?
			b[2] = g_pFrustum->IsIn( &g_pvHeightMap[ i[2] ] );	// 左侧下方顶点在平截头体内部?
			if( b[0] | b[1] | b[2] )	// 三者中只要有一个在平截头体内就要进行渲染.
			{
				idx._0 = i[0];
				idx._1 = i[1];
				idx._2 = i[2];
				*pI++ = idx;
				g_nTriangles++;			// 增加想要渲染的三角形个数
			}

			b[2] = g_pFrustum->IsIn( &g_pvHeightMap[ i[2] ] );	// 左侧下方顶点在平截头体内部?
			b[1] = g_pFrustum->IsIn( &g_pvHeightMap[ i[1] ] );	// 右侧上方顶点在平截头体内部?
			b[3] = g_pFrustum->IsIn( &g_pvHeightMap[ i[3] ] );	// 右侧下方顶点在平截头体内部?
			if( b[2] | b[1] | b[3] )	// 三者中只要有一个在平截头体内就要进行渲染.
			{
				idx._0 = i[2];
				idx._1 = i[1];
				idx._2 = i[3];
				*pI++ = idx;
				g_nTriangles++;
			}
		}
	}
    g_pIB->Unlock();

    return S_OK;
}
/** 鼠标输出处理*/
void ProcessMouse( void )
{
	POINT	pt;
	float	fDelta = 0.001f;	// 鼠标的灵敏度，该值越大移动越多.
	GetCursorPos( &pt );
	int dx = pt.x - g_dwMouseX;	// 鼠标的变化值
	int dy = pt.y - g_dwMouseY;	// 鼠标的变化值
	g_pCamera->RotateLocalX( dy * fDelta );	// 鼠标的Y轴旋转值为3D world的X轴旋转值
	g_pCamera->RotateLocalY( dx * fDelta );	// 鼠标的X轴旋转值为3D world的Y轴旋转值
	D3DXMATRIXA16*	pmatView = g_pCamera->GetViewMatrix();		// 得到摄像机矩阵.
	g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );			// 生成摄像机矩阵
	// 鼠标在窗口中央初始化
//	SetCursor( NULL );	// 鼠标消失.
	RECT	rc;
	GetClientRect( g_hwnd, &rc );
	pt.x = (rc.right - rc.left) / 2;
	pt.y = (rc.bottom - rc.top) / 2;
	ClientToScreen( g_hwnd, &pt );
	SetCursorPos( pt.x, pt.y );
	g_dwMouseX = pt.x;
	g_dwMouseY = pt.y;
}
/**-----------------------------------------------
* 键盘输入处理*/
void ProcessKey( void )
{
	if( GetAsyncKeyState( 'A' ) ) g_pCamera->MoveLocalZ( 0.5f );	// 摄像机前进!
	if( GetAsyncKeyState( 'Z' ) ) g_pCamera->MoveLocalZ( -0.5f );	// 摄像机后退!
}
/**-----------------------------------------------------------------------------
 * 输入处理
*------------------------------------------------------------------------------
 */
void ProcessInputs( void )
{
	ProcessMouse();
	ProcessKey();
}
/* 创建动画*/
VOID Animate()
{
	D3DXMatrixIdentity( &g_matAni );

	SetupLights();
	ProcessInputs();

	D3DXMATRIXA16	m;
	D3DXMATRIXA16	*pView;
	pView = g_pCamera->GetViewMatrix();	// 从摄像机类获取矩阵信息.
	m = *pView * g_matProj;				// 进行View*Proj运算，得到世界坐标.
	if( !g_bLockFrustum )
		g_pFrustum->Make( &m );	// 使用View*Proj矩阵制作平截头体.
	ProcessFrustumCull();		// 以平截头体信息为基础，制作想要表现的三角形的索引.

	LogFPS();
}
/**-----------------------------------------------------------------------------
 * 绘制网格
 *------------------------------------------------------------------------------
 */
void DrawMesh( D3DXMATRIXA16* pMat )
{
	g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, g_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, pMat );
    g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    g_pd3dDevice->SetIndices( g_pIB );

	g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, g_cxHeight*g_czHeight, 0, g_nTriangles );
}
/**-----------------------------------------------------------------------------
 * 绘图
*------------------------------------------------------------------------------
 */
VOID Render()
{
    /// 后置缓冲和Z－缓冲初始化
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200,200,200), 1.0f, 0 );

	/// 创建动画矩阵
	Animate();
    /// 开始渲染
    //if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    //{
		g_pd3dDevice->BeginScene();
		g_pd3dDevice->SetTexture( 0, g_pTexDiffuse );							/// 在0号纹理层固定纹理（颜色图）
		g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	/// 0号纹理层的放大滤镜
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );		/// 0号纹理：使用0号纹理索引

		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE );

		DrawMesh( &g_matAni );
		if( !g_bHideFrustum ) g_pFrustum->Draw( g_pd3dDevice );

		/// 结束渲染
		g_pd3dDevice->EndScene();
    //}

    /// 显示后置缓冲的画面!
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}
/**-----------------------------------------------------------------------------
 * 窗口过程
*------------------------------------------------------------------------------
 */
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY :
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
		case WM_KEYDOWN : 
			switch( wParam )
			{
				case VK_ESCAPE :
					PostMessage( hWnd, WM_DESTROY, 0, 0L );
					break;
				case '1' :
					g_bWireframe = !g_bWireframe;
					break;
				case '2' :
					g_bLockFrustum = !g_bLockFrustum;
					g_bHideFrustum = !g_bLockFrustum;
					break;
			}
			break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}
/**-----------------------------------------------------------------------------
 * 程序的起始地址
*------------------------------------------------------------------------------
 */
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    /// 注册窗口类
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "BasicFrame", NULL };
    RegisterClassEx( &wc );

    /// 创建窗口
    HWND hWnd = CreateWindow( "BasicFrame", WINDOW_TITLE,
                              WS_OVERLAPPEDWINDOW, 100, 100, WINDOW_W, WINDOW_H,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	g_hwnd = hWnd;

	g_pLog = new ZFLog( ZF_LOG_TARGET_CONSOLE | ZF_LOG_TARGET_WINDOW );
	g_pCamera = new ZCamera;
	g_pFrustum = new ZFrustum;

    /// Direct3D初始化
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        if( SUCCEEDED( InitGeometry() ) )
        {
        	/// 显示窗口
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );

        	/// 消息循环
			MSG msg;
			ZeroMemory( &msg, sizeof(msg) );
			while( msg.message!=WM_QUIT )
			{
            	/// 消息队列中有消息时，调用相应的处理过程
				if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
				/// 如果没有需要处理的消息，调用Render()函数
					Render();
			}
		}
    }
	/// 删除注册的类
	delete g_pFrustum;
	delete g_pLog;
	delete g_pCamera;
    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}
