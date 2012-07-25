
#ifndef _ZFRUSTUM_H_
#define _ZFRUSTUM_H_

#include <d3d9.h>
#include <d3dx9.h>

/// ��ʹ��������ƽ��ͷ���ڣ���΢���Ӳ�����ʹ֮������ƽ��ͷ���ڵ�ֵ
#define PLANE_EPSILON	5.0f

/**
 * ����ƽ��ͷ��������
 *
 */


class ZFrustum
{
	D3DXVECTOR3	m_vtx[8];	/// ����ƽ��ͷ���8������
	D3DXVECTOR3	m_vPos;	/// ��ǰ���������������
	D3DXPLANE	m_plane[6];	/// ����ƽ��ͷ���6��ƽ��
public:
	/// ������
	ZFrustum();
	/// �����������view��*ͶӰ���󣬵õ�6��ƽ��.
	BOOL	Make( D3DXMATRIXA16* pmatViewProj );
	/// ��v��ƽ��ͷ���ڣ�����TRUE������ƽ��ͷ���ڣ�����FALSE.
	BOOL	IsIn( D3DXVECTOR3* pv );
	/** ��������Բ�㣨v���Ͱ뾶��radius��
�ı߽����壨bounding sphere����ƽ��ͷ���ڣ�
	 ����TRUE������ƽ��ͷ���ڣ�����FALSE. */
	BOOL	IsInSphere( D3DXVECTOR3* pv, float radius );
	/// �ڻ������ƽ��ͷ��.
	BOOL	Draw( LPDIRECT3DDEVICE9 pDev );
	/// �õ���ǰ���������������.
	D3DXVECTOR3*	GetPos() { return &m_vPos; }
};
/// ֱ�����ƽ�淽��ʽ�ĺ���������û����ʹ��.
void ZMakePlane( D3DXPLANE* pPlane, D3DXVECTOR3* pv0, D3DXVECTOR3* pv1, D3DXVECTOR3* pv2 );

#endif // _ZFRUSTUM_H_
