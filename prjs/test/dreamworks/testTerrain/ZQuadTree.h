
#include "define.h"
#include "ZFrustum.h"

class ZQuadTree
{
public:
	/// �Ĳ��������4���ǵĳ���ֵ
	enum			CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };
	/// �ڽ��ڵ㴦���õĳ���ֵ
	enum			{ EDGE_UP, EDGE_DN, EDGE_LT, EDGE_RT };
	/// �Ĳ�����ƽ��ͷ��Ĺ�ϵ
	enum		QuadLocation { FRUSTUM_OUT = 0,	/// ��ȫ����ƽ��ͷ��
		FRUSTUM_PARTIALLY_IN = 1,	/// ���ְ�����ƽ��ͷ��
		FRUSTUM_COMPLETELY_IN = 2,	/// ��ȫ������ƽ��ͷ��
		FRUSTUM_UNKNOWN = -1 };		/// ��֪��(^^;)
private:
	ZQuadTree*	m_pChild[4];		/// �Ĳ�����4���ӽڵ�
	ZQuadTree*	m_pParent;			/// ��ֹ����(Popping)�Ĳ���.
	ZQuadTree*	m_pNeighbor[4];		/// ��ֹ����(Popping)�Ĳ���.
	int			m_nCenter;			/// �Ĳ�������ĵ�һ��ֵ
	int			m_nCorner[4];		/// �Ĳ�������ĵڶ���ֵ
	///    TopLeft(TL)      TopRight(TR)
	///              0------1
	///              |      |
	///              |      |
	///              2------3
	/// BottomLeft(BL)      BottomRight(BR)
	BOOL		m_bCulled;			/// ƽ��ͷ���޳��Ľڵ�?
	float		m_fRadius;			/// �����ڵ�ı߽����壨bounding sphere���İ뾶
private:
	/// ����ӽڵ�.
	ZQuadTree*	_AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );
	/// ����4����ֵ.
	BOOL		_SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );
	/// ʹ��4���²���Ĳ��������ٷָsubdivide��.
	BOOL		_SubDivide();
	/// �����������ε�����.
	int		_GenTriIndex( int nTris, LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio );
	/// �ڴ洢����ɾ���Ĳ���.
	void		_Destroy();
	/// ���������ľ���.
	float		_GetDistance( D3DXVECTOR3* pv1, D3DXVECTOR3* pv2 ) 
	{
		return D3DXVec3Length( &(*pv2 - *pv1) );
	}
	/// ��������뵱ǰ�ڵ�ľ���Ϊ��׼���LODֵ.
	int			_GetLODLevel( TERRAINVERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio )
	{
		float d = _GetDistance( (D3DXVECTOR3*)(pHeightMap+m_nCenter),
			pCamera );
		return max( (int)( d * fLODRatio ), 1 );
	}
	/// ��ǰ�ڵ������ƽ��ͷ��?
	int			_IsInFrustum( TERRAINVERTEX* pHeightMap, 
		ZFrustum* pFrustum );
	/// ����_IsInFrustum()�����Ľ����ƽ��ͷ������޳�
	void		_FrustumCull( TERRAINVERTEX* pHeightMap, 
		ZFrustum* pFrustum );
	/// �����ڽ��ڵ㣨��ֹ�����εĹ��ѣ�
	void		_BuildNeighborNode( ZQuadTree* pRoot, 
		TERRAINVERTEX* pHeightMap, int cx );
	/// �����Ĳ�������Build()�����У�
	BOOL		_BuildQuadTree( TERRAINVERTEX* pHeightMap );
	/// �����Ĳ�������ѯ���ĸ���ֵһ�µĽڵ�.
	ZQuadTree*	_FindNode( TERRAINVERTEX* pHeightMap,
		int _0, int _1, int _2, int _3 );
	/// �õ��ĸ������Ϸ����·����󷽡��ҷ������ڽ��ڵ�����.
	int			_GetNodeIndex( int ed, int cx, int& _0, 
		int& _1, int& _2, int& _3 );
public:
	/// ���ѭ���ڵ㴴����
	ZQuadTree( int cx, int cy );
	/// �²��ӽڵ㴴����
	ZQuadTree( ZQuadTree* pParent );

	/// ɾ����
	~ZQuadTree();
	/// ��LOD�ȼ���������ǰ�ڵ��ǿ�������Ľڵ���?
	BOOL		IsVisible( TERRAINVERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio )
	{ 
		return ( (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= _GetLODLevel( pHeightMap, pCamera, fLODRatio ) ); 
	}
	/// �����Ĳ���.
	BOOL		Build( TERRAINVERTEX* pHeightMap );
	/// �õ��ǵ�����ֵ.
	void		GetCorner( int& _0, int& _1, int& _2, int& _3 )
	{ 
		_0 = m_nCorner[0]; 
		_1 = m_nCorner[1]; 
		_2 = m_nCorner[2]; 
		_3 = m_nCorner[3]; 
	}
	///	����������������������������εĸ���.
	int			GenerateIndex( LPVOID pIndex, TERRAINVERTEX* pHeightMap,ZFrustum* pFrustum, float fLODRatio );



	///////////////////////////////////////////////
private:
	/// ��ǰ�ӽ�Ϊ����������ӽ�?
	BOOL		_IsVisible() 
	{
		return ( m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= 1 );
	}
	/// �����������ε�����.
	int			_GenTriIndex( int nTriangles, LPVOID pIndex );

public:
	/// �����Ĳ���.
	BOOL		Build();
	///	����������������������������εĸ���.
	int			GenerateIndex( LPVOID pIB );
};


