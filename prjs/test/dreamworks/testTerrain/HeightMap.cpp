/**-----------------------------------------------------------------------------
 * �ļ�: HeightMap.cpp
 *
 * ˵��: HeightMap��ʵ�� + Frustum Culling
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
*  ȫ�ֲ��� 
*-------------------------------------------------------------*/
ZCamera*				g_pCamera = NULL;	// Camera ��
ZFrustum*				g_pFrustum = NULL;	// Frustum ��
HWND					g_hwnd = NULL;
LPDIRECT3D9             g_pD3D       = NULL; // ����D3D �豸��D3D�������
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // ��Ⱦ��ʹ�õ�D3D�豸
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // ���涥��Ķ��㻺��
LPDIRECT3DINDEXBUFFER9	g_pIB        = NULL; // ������������������
LPDIRECT3DTEXTURE9		g_pTexHeight = NULL; // ����߶�ͼ
LPDIRECT3DTEXTURE9		g_pTexDiffuse= NULL; // ������ɫͼ
D3DXMATRIXA16			g_matAni;
D3DXMATRIXA16			g_matWorld;
D3DXMATRIXA16			g_matView;
D3DXMATRIXA16			g_matProj;
DWORD					g_cxHeight = 0;			// ����ĳ���

DWORD					g_czHeight = 0;			// ����Ŀ��
DWORD					g_dwMouseX = 0;			// ��������
DWORD					g_dwMouseY = 0;			// ��������
int						g_nTriangles = 0;		// Ҫ���Ƶ������εĸ���
D3DXVECTOR3*			g_pvHeightMap = NULL;	// ����߶�ͼ������
BOOL					g_bHideFrustum = TRUE;	// ������ƽ��ͷ��?
BOOL					g_bLockFrustum = FALSE;	// �̶�ƽ��ͷ��?
BOOL					g_bWireframe = FALSE;	// �����߿���л���?
/// �����û�����Ľṹ��
struct CUSTOMVERTEX
{
	D3DXVECTOR3		p;
	D3DXVECTOR3		n;
	D3DXVECTOR2		t;
};
/// �����û�����ṹ�������Ϣ��FVFֵ
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
struct MYINDEX
{
	WORD	_0, _1, _2;		/// WORD��16λ����
};
/**-----------------------------------------------------------
 * Direct3D��ʼ��
*-------------------------------------------------------------*/
HRESULT InitD3D( HWND hWnd )
{
	// ����һ�����������豸��D3D����
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	// �����豸�Ľṹ��
	// ���Ƹ��Ӷ���ʱ��ҪZ������.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	/// �����豸
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}
	// ��������CCW
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	// ��Z�����幦��.
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	return S_OK;
}
/**-----------------------------------------------------
* ��������
*-----------------------------------------------------------*/
void InitMatrix()
{
	/// �����������
	D3DXMatrixIdentity( &g_matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );
    /// ������ͼ����
    D3DXVECTOR3 vEyePt( 0.0f, 100.0f, -(float)g_czHeight );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &g_matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &g_matView );
    /// ʵ��ͶӰ����
	D3DXMatrixPerspectiveFovLH( &g_matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );
	/// ƽ��ͷ����ͶӰ����
    D3DXMatrixPerspectiveFovLH( &g_matProj, D3DX_PI/4, 1.0f, 1.0f, 200.0f );
	/// �������ʼ��
	g_pCamera->SetView( &vEyePt, &vLookatPt, &vUpVec );
}

/**----------------------------------------------------
* �����ʼ��
*--------------------------------------------------------*/
HRESULT InitTexture()
{
	// �߶�ͼ����
	// ���øú�������D3DFMT_X8R8G8B8��D3DPOOL_MANAGED.
	if( FAILED( D3DXCreateTextureFromFileEx( g_pd3dDevice,
								 BMP_HEIGHTMAP, 
								 D3DX_DEFAULT, D3DX_DEFAULT, 
								 D3DX_DEFAULT, 0, 
								 D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, 
								 D3DX_DEFAULT, D3DX_DEFAULT, 0, 
								 NULL, NULL, &g_pTexHeight) ) )
		return E_FAIL;

	// ��ɫͼ
	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, "tile2.tga", &g_pTexDiffuse) ) )  //"terrain_texture.jpg"
		return E_FAIL;

	return S_OK;
}
/**-----------------------------------------
* �������㻺�壬���ö���ֵ.
 * HeightMap��Ϣ��ʼ��.
*-----------------------------------------------------*/
HRESULT InitVB()
{
	D3DSURFACE_DESC		ddsd;
	D3DLOCKED_RECT		d3drc;
	g_pTexHeight->GetLevelDesc( 0, &ddsd );	// ������Ϣ
	g_cxHeight = ddsd.Width;				// ����ĳ���
	g_czHeight = ddsd.Height;				// ����Ŀ��
	g_pLog->Log( "Texture Size:[%d,%d]", g_cxHeight, g_czHeight );
	// �����߶�ͼ����
	g_pvHeightMap = new D3DXVECTOR3[g_cxHeight * g_czHeight];
	//�������㻺���� 
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( ddsd.Width*
		ddsd.Height*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, 
		&g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}
	// ����洢��!
	g_pTexHeight->LockRect( 0, &d3drc, NULL, D3DLOCK_READONLY );


	VOID* pVertices;//����
	// ���㻺��!�����ж���������pVertices��
	if( FAILED( g_pVB->Lock( 0,
		g_cxHeight*g_czHeight*sizeof(CUSTOMVERTEX), 
		(void**)&pVertices, 0 ) ) )
		return E_FAIL;
	//�ṹ���ʵ��
	CUSTOMVERTEX	v;
	//�ṹ���ָ�룬���е㶼��ֵ����
	CUSTOMVERTEX*	pV = (CUSTOMVERTEX*)pVertices;
	for( DWORD z = 0 ; z < g_czHeight ; z++ )
	{
		for( DWORD x = 0 ; x < g_cxHeight ; x++ )
		{
			v.p.x = (float)x-g_cxHeight/2.0f;		// �����x���꣨��ԭ��Ϊ׼��������
			v.p.z = -((float)z-g_czHeight/2.0f);	// �����z���꣨�Զ���Ϊ׼�������񣩣�z��ָ����ʾ�������ԡ�-��.
			v.p.y = ((float)(*((LPDWORD)d3drc.pBits+x+z*(d3drc.Pitch/4))&0x000000ff))/10.0f;	/// ������DWORD�����Ϊpitch/4
			v.n.x = v.p.x;
			v.n.y = v.p.y;
			v.n.z = v.p.z;
			D3DXVec3Normalize( &v.n, &v.n );
			v.t.x = (float)x / (g_cxHeight-1);
			v.t.y = (float)z / (g_czHeight-1);
			*pV++ = v;		// ���㴢�浽���㻺��
			// ���㴢�浽�߶�ͼ
			g_pvHeightMap[z * g_cxHeight + x] = v.p;	
		}
	}
	g_pVB->Unlock();
	g_pTexHeight->UnlockRect( 0 );
    return S_OK;
}
/**------------------------------------------------
* ���������ʼ��
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
* ������Ϣ��ʼ��
 *---------------------------------------------------- */
HRESULT InitGeometry()
{
	if( FAILED( InitTexture() ) ) return E_FAIL;
	if( FAILED( InitVB() ) ) return E_FAIL;
	if( FAILED( InitIB() ) ) return E_FAIL;
	InitMatrix();
	// ������������λ��
	POINT	pt;
	GetCursorPos( &pt );
	g_dwMouseX = pt.x;
	g_dwMouseY = pt.y;
	return S_OK;
}
/**------------------------------------------------ 
* ɾ����ʼ������
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
* ������Դ
*----------------------------------------------------------*/
VOID SetupLights()
{
    /// �������ʣ�material��
    /// ���豸�д���һ������.
    D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	g_pd3dDevice->SetMaterial( &mtrl );
	/// ������Դ
	D3DXVECTOR3 vecDir;	/// �����Դ��directional light������ķ���
	D3DLIGHT9 light;		/// ��Դ�ṹ��
	ZeroMemory( &light, sizeof(D3DLIGHT9) );	/// ���ṹ������.
	/// ��Դ���ͣ����Դ�������Դ���۹��Դ��
	light.Type       = D3DLIGHT_DIRECTIONAL;	
	light.Diffuse.r  = 1.0f;		/// ��Դ����ɫ������
	light.Diffuse.g  = 1.0f;
	light.Diffuse.b  = 0.0f;
	vecDir = D3DXVECTOR3( 1, 1, 1 );	/// ��Դ�Ĺ̶�
	vecDir = D3DXVECTOR3(cosf(GetTickCount()/350.0f),	/// ��Դ��ת
		1.0f,
		sinf(GetTickCount()/350.0f) );
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );	/// ��Դ�ķ����趨Ϊ��λ����.
	light.Range       = 1000.0f;	/// ��Դ�ܹ����䵽����Զ����
	g_pd3dDevice->SetLight( 0, &light );	/// ���豸����0�Ź�Դ
	g_pd3dDevice->LightEnable( 0, TRUE );	/// ��0�Ź�Դ
	/// �򿪹�Դ����
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );	
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00909090 );		/// �趨������Դ��ambient light����ֵ
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

	/// 1�����?
	if( GetTickCount() - nTick > 1000 )
	{
		nTick = GetTickCount();
		/// FPS���
		g_pLog->Log("FPS:%d", nFPS );

		/// �����λ��ֵ���
//		D3DXVECTOR3*	pv;
//		pv = g_pCamera->GetEye();
//		g_pLog->Log("EYE:[%f,%f,%f]",pv->x, pv->y, pv->z );
		nFPS = 0;
		LogStatus();	/// ״̬��Ϣ�����1��/�룩
		return;
	}
	nFPS++;
}
/**----------------------
* ʹ��ƽ��ͷ����޳����ɴ�����Ҫ���Ƶ�����
*------------------------------------------------*/
HRESULT ProcessFrustumCull()
{
	DWORD		i[4];	// ��ʱ�����������Ϣ
	BOOL		b[4];	// ��ʱ�����ƽ��ͷ���޳��Ľ��ֵ
	MYINDEX		idx;
    MYINDEX*	pI;
	//������PI��
    if( FAILED( g_pIB->Lock( 0, (g_cxHeight-1)*(g_czHeight-1)*2 * sizeof(MYINDEX), (void**)&pI, 0 ) ) )
        return E_FAIL;
	//Ҫ���������εĸ���
	g_nTriangles = 0;
	//ѭ����ÿ����
	for( DWORD z = 0 ; z < g_czHeight-1 ; z++ )
	{
		for( DWORD x = 0 ; x < g_cxHeight-1 ; x++ )
		{
			//��ͼ�еĵ�
			i[0] = (z*g_cxHeight+x);			// ����Ϸ�
			i[1] = (z*g_cxHeight+x+1);			// �Ҳ��Ϸ�
			i[2] = ((z+1)*g_cxHeight+x);		// ����·�
			i[3] = ((z+1)*g_cxHeight+x+1);		// �Ҳ��·�
			//����Ƿ��ڷ�Χ��
			b[0] = g_pFrustum->IsIn( &g_pvHeightMap[ i[0] ] );	// ����Ϸ�������ƽ��ͷ���ڲ�?
			b[1] = g_pFrustum->IsIn( &g_pvHeightMap[ i[1] ] );	// �Ҳ��Ϸ�������ƽ��ͷ���ڲ�?
			b[2] = g_pFrustum->IsIn( &g_pvHeightMap[ i[2] ] );	// ����·�������ƽ��ͷ���ڲ�?
			if( b[0] | b[1] | b[2] )	// ������ֻҪ��һ����ƽ��ͷ���ھ�Ҫ������Ⱦ.
			{
				idx._0 = i[0];
				idx._1 = i[1];
				idx._2 = i[2];
				*pI++ = idx;
				g_nTriangles++;			// ������Ҫ��Ⱦ�������θ���
			}

			b[2] = g_pFrustum->IsIn( &g_pvHeightMap[ i[2] ] );	// ����·�������ƽ��ͷ���ڲ�?
			b[1] = g_pFrustum->IsIn( &g_pvHeightMap[ i[1] ] );	// �Ҳ��Ϸ�������ƽ��ͷ���ڲ�?
			b[3] = g_pFrustum->IsIn( &g_pvHeightMap[ i[3] ] );	// �Ҳ��·�������ƽ��ͷ���ڲ�?
			if( b[2] | b[1] | b[3] )	// ������ֻҪ��һ����ƽ��ͷ���ھ�Ҫ������Ⱦ.
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
/** ����������*/
void ProcessMouse( void )
{
	POINT	pt;
	float	fDelta = 0.001f;	// ���������ȣ���ֵԽ���ƶ�Խ��.
	GetCursorPos( &pt );
	int dx = pt.x - g_dwMouseX;	// ���ı仯ֵ
	int dy = pt.y - g_dwMouseY;	// ���ı仯ֵ
	g_pCamera->RotateLocalX( dy * fDelta );	// ����Y����תֵΪ3D world��X����תֵ
	g_pCamera->RotateLocalY( dx * fDelta );	// ����X����תֵΪ3D world��Y����תֵ
	D3DXMATRIXA16*	pmatView = g_pCamera->GetViewMatrix();		// �õ����������.
	g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );			// �������������
	// ����ڴ��������ʼ��
//	SetCursor( NULL );	// �����ʧ.
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
* �������봦��*/
void ProcessKey( void )
{
	if( GetAsyncKeyState( 'A' ) ) g_pCamera->MoveLocalZ( 0.5f );	// �����ǰ��!
	if( GetAsyncKeyState( 'Z' ) ) g_pCamera->MoveLocalZ( -0.5f );	// ���������!
}
/**-----------------------------------------------------------------------------
 * ���봦��
*------------------------------------------------------------------------------
 */
void ProcessInputs( void )
{
	ProcessMouse();
	ProcessKey();
}
/* ��������*/
VOID Animate()
{
	D3DXMatrixIdentity( &g_matAni );

	SetupLights();
	ProcessInputs();

	D3DXMATRIXA16	m;
	D3DXMATRIXA16	*pView;
	pView = g_pCamera->GetViewMatrix();	// ����������ȡ������Ϣ.
	m = *pView * g_matProj;				// ����View*Proj���㣬�õ���������.
	if( !g_bLockFrustum )
		g_pFrustum->Make( &m );	// ʹ��View*Proj��������ƽ��ͷ��.
	ProcessFrustumCull();		// ��ƽ��ͷ����ϢΪ������������Ҫ���ֵ������ε�����.

	LogFPS();
}
/**-----------------------------------------------------------------------------
 * ��������
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
 * ��ͼ
*------------------------------------------------------------------------------
 */
VOID Render()
{
    /// ���û����Z�������ʼ��
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200,200,200), 1.0f, 0 );

	/// ������������
	Animate();
    /// ��ʼ��Ⱦ
    //if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    //{
		g_pd3dDevice->BeginScene();
		g_pd3dDevice->SetTexture( 0, g_pTexDiffuse );							/// ��0�������̶�������ɫͼ��
		g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	/// 0�������ķŴ��˾�
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );		/// 0������ʹ��0����������

		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE );

		DrawMesh( &g_matAni );
		if( !g_bHideFrustum ) g_pFrustum->Draw( g_pd3dDevice );

		/// ������Ⱦ
		g_pd3dDevice->EndScene();
    //}

    /// ��ʾ���û���Ļ���!
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}
/**-----------------------------------------------------------------------------
 * ���ڹ���
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
 * �������ʼ��ַ
*------------------------------------------------------------------------------
 */
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    /// ע�ᴰ����
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "BasicFrame", NULL };
    RegisterClassEx( &wc );

    /// ��������
    HWND hWnd = CreateWindow( "BasicFrame", WINDOW_TITLE,
                              WS_OVERLAPPEDWINDOW, 100, 100, WINDOW_W, WINDOW_H,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	g_hwnd = hWnd;

	g_pLog = new ZFLog( ZF_LOG_TARGET_CONSOLE | ZF_LOG_TARGET_WINDOW );
	g_pCamera = new ZCamera;
	g_pFrustum = new ZFrustum;

    /// Direct3D��ʼ��
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        if( SUCCEEDED( InitGeometry() ) )
        {
        	/// ��ʾ����
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );

        	/// ��Ϣѭ��
			MSG msg;
			ZeroMemory( &msg, sizeof(msg) );
			while( msg.message!=WM_QUIT )
			{
            	/// ��Ϣ����������Ϣʱ��������Ӧ�Ĵ������
				if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
				/// ���û����Ҫ�������Ϣ������Render()����
					Render();
			}
		}
    }
	/// ɾ��ע�����
	delete g_pFrustum;
	delete g_pLog;
	delete g_pCamera;
    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}
