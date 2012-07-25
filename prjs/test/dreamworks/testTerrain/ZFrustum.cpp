
#include "ZFrustum.h"
#include "ZFLog.h"

ZFrustum::ZFrustum()
{
	ZeroMemory( m_vtx, sizeof( m_vtx[0] ) * 8 );
	ZeroMemory( m_plane, sizeof( m_plane[0] ) * 6 );
}
// 输入摄像机(view)*投影矩阵(projection)，得到6个平面.
BOOL ZFrustum::Make( D3DXMATRIXA16* pmatViewProj )
{
	int				i;
	D3DXMATRIXA16	matInv;
	// 如果经过投影矩阵，所有的三维世界坐标的点都变为 (-1,-1,0) ~ (1,1,1)之间的值.
	// 将同次空间的临界值填入m_vtx.
	m_vtx[0].x = -1.0f;	m_vtx[0].y = -1.0f;	m_vtx[0].z = 0.0f;
	m_vtx[1].x =  1.0f;	m_vtx[1].y = -1.0f;	m_vtx[1].z = 0.0f;
	m_vtx[2].x =  1.0f;	m_vtx[2].y = -1.0f;	m_vtx[2].z = 1.0f;
	m_vtx[3].x = -1.0f;	m_vtx[3].y = -1.0f;	m_vtx[3].z = 1.0f;
	m_vtx[4].x = -1.0f;	m_vtx[4].y =  1.0f;	m_vtx[4].z = 0.0f;
	m_vtx[5].x =  1.0f;	m_vtx[5].y =  1.0f;	m_vtx[5].z = 0.0f;
	m_vtx[6].x =  1.0f;	m_vtx[6].y =  1.0f;	m_vtx[6].z = 1.0f;
	m_vtx[7].x = -1.0f;	m_vtx[7].y =  1.0f;	m_vtx[7].z = 1.0f;
	// 求得view * proj的逆矩阵.
	D3DXMatrixInverse(&matInv, NULL, pmatViewProj );
	// Vertex_最终 = Vertex_local * Matrix_world * Matrix_view * Matrix_Proj ,
	// Vertex_world = Vertex_local * Matrix_world的缘由,
	// Vertex_最终 = Vertex_world * Matrix_view * Matrix_Proj .
	// Vertex_最终 = Vertex_world * ( Matrix_view * Matrix_Proj ) 
	// 逆矩阵( Matrix_view * Matrix_Proj )^-1两边相乘
	// Vertex_最终 * 逆矩阵( Matrix_view * Matrix_Proj )^-1 = Vertex_World .
	// 因此, m_vtx * matInv = Vertex_world, 可以得到世界坐标系的Frustum坐标.
	for( i = 0; i < 8; i++ )
		D3DXVec3TransformCoord( &m_vtx[i], &m_vtx[i], &matInv );
	// 0号和 5号为平截头体中近平面的左上方和右下方，将两个坐标相加再除2，
	// 可以得到摄像机的坐标.(不是完全一致.)
	m_vPos = ( m_vtx[0] + m_vtx[5] ) / 2.0f;
	// 通过得到的世界坐标制作平截头体平面.
	// 向量由平截头体内部指向外部的平面.
//	D3DXPlaneFromPoints(&m_plane[0], m_vtx+4, m_vtx+7, m_vtx+6);	// 上平面(top)
//	D3DXPlaneFromPoints(&m_plane[1], m_vtx  , m_vtx+1, m_vtx+2);	// 下平面(bottom)
//	D3DXPlaneFromPoints(&m_plane[2], m_vtx  , m_vtx+4, m_vtx+5);	// 近平面(near)
	D3DXPlaneFromPoints(&m_plane[3], m_vtx+2, m_vtx+6, m_vtx+7);	// 远平面(far)
	D3DXPlaneFromPoints(&m_plane[4], m_vtx  , m_vtx+3, m_vtx+7);	// 左平面(left)
	D3DXPlaneFromPoints(&m_plane[5], m_vtx+1, m_vtx+5, m_vtx+6);	// 右平面(right)
	return TRUE;
}
/// 如果一个点 v在平截头体内，就会返回TRUE，反之为FALSE.
BOOL ZFrustum::IsIn( D3DXVECTOR3* pv )
{
	float		fDist;
//	int			i;
	// 现在只使用 left, right, far plane.
//	for( i = 0 ; i < 6 ; i++ )
	{
		fDist = D3DXPlaneDotCoord( &m_plane[3], pv );
		if( fDist > PLANE_EPSILON ) return FALSE;	// plane的 normal向量指向 far，如果为正数，表示在平截头体外部.
		fDist = D3DXPlaneDotCoord( &m_plane[4], pv );
		if( fDist > PLANE_EPSILON ) return FALSE;	// plane的 normal向量指向 left，如果为正数，表示在平截头体的左边.
		fDist = D3DXPlaneDotCoord( &m_plane[5], pv );
		if( fDist > PLANE_EPSILON ) return FALSE;	// plane的 normal向量指向 right，如果为正数，表示在平截头体的右边.
	}
	return TRUE;
}
/** 如果具有中心(v)和 半径(radius)的边界球体位于平截头体内，
 *  就会返回TRUE，反之为FALSE.
 */
BOOL ZFrustum::IsInSphere( D3DXVECTOR3* pv, float radius )
{
	float		fDist;
	fDist = D3DXPlaneDotCoord( &m_plane[3], pv );
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;	// 平面和中心圆点的距离比半径大的话，表示不在平截头体内.
	fDist = D3DXPlaneDotCoord( &m_plane[4], pv );
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;	// 平面和中心圆点的距离比半径大的话，表示不在平截头体内.
	fDist = D3DXPlaneDotCoord( &m_plane[5], pv );
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;	// 平面和中心圆点的距离比半径大的话，表示不在平截头体内.
	return TRUE;
}
/// 在画面上绘制平截头体.
BOOL ZFrustum::Draw( LPDIRECT3DDEVICE9 pDev )
{
	WORD		index[] = { 0, 1, 2,
							0, 2, 3,
							4, 7, 6,
							4, 6, 5,
							1, 5, 6,
							1, 6, 2,
							0, 3, 7,
							0, 7, 4,
							0, 4, 5,
							0, 5, 1,
							3, 7, 6,
							3, 6, 2 };

    D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	typedef struct tagVTX
	{
		D3DXVECTOR3	p;
	} VTX;
	VTX		vtx[8];
	for( int i = 0 ; i < 8 ; i++ )
		vtx[i].p = m_vtx[i];
	pDev->SetFVF( D3DFVF_XYZ );
	pDev->SetStreamSource( 0, NULL, 0, sizeof(VTX) );
	pDev->SetTexture( 0, NULL );
	pDev->SetIndices( 0 );
	pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	pDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// 上、下平面绘制为蓝色.
    pDev->SetRenderState( D3DRS_LIGHTING, TRUE );
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    pDev->SetMaterial( &mtrl );
	pDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 8, 4, index, D3DFMT_INDEX16, vtx, sizeof( vtx[0] ) );
	// 左、右平面绘制为绿色.
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    pDev->SetMaterial( &mtrl );
	pDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 8, 4, index+4*3, D3DFMT_INDEX16, vtx, sizeof( vtx[0] ) );
	// 远、近平面绘制为红色.
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    pDev->SetMaterial( &mtrl );
	pDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 8, 4, index+8*3, D3DFMT_INDEX16, vtx, sizeof( vtx[0] ) );
	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    pDev->SetRenderState( D3DRS_LIGHTING, FALSE );
	return TRUE;
}
void ZMakePlane( D3DXPLANE* pPlane, D3DXVECTOR3* pv0, D3DXVECTOR3* pv1, D3DXVECTOR3* pv2 )
{
	D3DXPlaneFromPoints( pPlane, pv0, pv1, pv2 );
//  直接求取平面方程式的源代码
//	for OpenGL
//	D3DXVECTOR3	v0, v1, v2;
//	v1 = *pv1 - *pv0;
//	v2 = *pv2 - *pv0;
//	D3DXVec3Cross( &v0, &v1, &v2 );
//	D3DXVec3Normalize( &v0, &v0 );

//	pPlane->a = v0.x;
//	pPlane->b = v0.y;
//	pPlane->c = v0.z;
//	pPlane->d = -( v0.x * pv0->x + v0.y * pv0->y + v0.z * pv0->z );
}
