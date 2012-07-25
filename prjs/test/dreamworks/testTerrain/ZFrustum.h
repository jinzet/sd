
#ifndef _ZFRUSTUM_H_
#define _ZFRUSTUM_H_

#include <d3d9.h>
#include <d3dx9.h>

/// 即使不包含在平截头体内，稍微增加参数，使之包含在平截头体内的值
#define PLANE_EPSILON	5.0f

/**
 * 进行平截头体卷起的类
 *
 */


class ZFrustum
{
	D3DXVECTOR3	m_vtx[8];	/// 构成平截头体的8个顶点
	D3DXVECTOR3	m_vPos;	/// 当前摄像机的世界坐标
	D3DXPLANE	m_plane[6];	/// 构成平截头体的6个平面
public:
	/// 创建者
	ZFrustum();
	/// 输入摄像机（view）*投影矩阵，得到6个平面.
	BOOL	Make( D3DXMATRIXA16* pmatViewProj );
	/// 点v在平截头体内，返回TRUE，不在平截头体内，返回FALSE.
	BOOL	IsIn( D3DXVECTOR3* pv );
	/** 具有中心圆点（v）和半径（radius）
的边界球体（bounding sphere）在平截头体内，
	 返回TRUE，不在平截头体内，返回FALSE. */
	BOOL	IsInSphere( D3DXVECTOR3* pv, float radius );
	/// 在画面绘制平截头体.
	BOOL	Draw( LPDIRECT3DDEVICE9 pDev );
	/// 得到当前摄像机的世界坐标.
	D3DXVECTOR3*	GetPos() { return &m_vPos; }
};
/// 直接求得平面方程式的函数，现在没有在使用.
void ZMakePlane( D3DXPLANE* pPlane, D3DXVECTOR3* pv0, D3DXVECTOR3* pv1, D3DXVECTOR3* pv2 );

#endif // _ZFRUSTUM_H_
