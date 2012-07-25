
#include "define.h"
#include "ZFrustum.h"

class ZQuadTree
{
public:
	/// 四叉数保存的4个角的常数值
	enum			CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };
	/// 邻近节点处理用的常数值
	enum			{ EDGE_UP, EDGE_DN, EDGE_LT, EDGE_RT };
	/// 四叉树和平截头体的关系
	enum		QuadLocation { FRUSTUM_OUT = 0,	/// 完全脱离平截头体
		FRUSTUM_PARTIALLY_IN = 1,	/// 部分包含在平截头体
		FRUSTUM_COMPLETELY_IN = 2,	/// 完全包含在平截头体
		FRUSTUM_UNKNOWN = -1 };		/// 不知道(^^;)
private:
	ZQuadTree*	m_pChild[4];		/// 四叉树的4个子节点
	ZQuadTree*	m_pParent;			/// 防止龟裂(Popping)的产生.
	ZQuadTree*	m_pNeighbor[4];		/// 防止龟裂(Popping)的产生.
	int			m_nCenter;			/// 四叉树保存的第一个值
	int			m_nCorner[4];		/// 四叉树保存的第二个值
	///    TopLeft(TL)      TopRight(TR)
	///              0------1
	///              |      |
	///              |      |
	///              2------3
	/// BottomLeft(BL)      BottomRight(BR)
	BOOL		m_bCulled;			/// 平截头体剔除的节点?
	float		m_fRadius;			/// 包含节点的边界球体（bounding sphere）的半径
private:
	/// 添加子节点.
	ZQuadTree*	_AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );
	/// 设置4个角值.
	BOOL		_SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );
	/// 使用4个下层对四叉树进行再分割（subdivide）.
	BOOL		_SubDivide();
	/// 创建输出多边形的索引.
	int		_GenTriIndex( int nTris, LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio );
	/// 在存储器中删除四叉树.
	void		_Destroy();
	/// 求得两个点的距离.
	float		_GetDistance( D3DXVECTOR3* pv1, D3DXVECTOR3* pv2 ) 
	{
		return D3DXVec3Length( &(*pv2 - *pv1) );
	}
	/// 以摄像机与当前节点的距离为标准求得LOD值.
	int			_GetLODLevel( TERRAINVERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio )
	{
		float d = _GetDistance( (D3DXVECTOR3*)(pHeightMap+m_nCenter),
			pCamera );
		return max( (int)( d * fLODRatio ), 1 );
	}
	/// 当前节点包含在平截头体?
	int			_IsInFrustum( TERRAINVERTEX* pHeightMap, 
		ZFrustum* pFrustum );
	/// 根据_IsInFrustum()函数的结果对平截头体进行剔除
	void		_FrustumCull( TERRAINVERTEX* pHeightMap, 
		ZFrustum* pFrustum );
	/// 绘制邻近节点（防止三角形的龟裂）
	void		_BuildNeighborNode( ZQuadTree* pRoot, 
		TERRAINVERTEX* pHeightMap, int cx );
	/// 创建四叉树（在Build()函数中）
	BOOL		_BuildQuadTree( TERRAINVERTEX* pHeightMap );
	/// 检索四叉树，查询与四个角值一致的节点.
	ZQuadTree*	_FindNode( TERRAINVERTEX* pHeightMap,
		int _0, int _1, int _2, int _3 );
	/// 得到四个方向（上方、下方、左方、右方）的邻近节点索引.
	int			_GetNodeIndex( int ed, int cx, int& _0, 
		int& _1, int& _2, int& _3 );
public:
	/// 最初循环节点创建者
	ZQuadTree( int cx, int cy );
	/// 下层子节点创建者
	ZQuadTree( ZQuadTree* pParent );

	/// 删除者
	~ZQuadTree();
	/// 从LOD等级来看，当前节点是可以输出的节点吗?
	BOOL		IsVisible( TERRAINVERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio )
	{ 
		return ( (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= _GetLODLevel( pHeightMap, pCamera, fLODRatio ) ); 
	}
	/// 创建四叉树.
	BOOL		Build( TERRAINVERTEX* pHeightMap );
	/// 得到角的索引值.
	void		GetCorner( int& _0, int& _1, int& _2, int& _3 )
	{ 
		_0 = m_nCorner[0]; 
		_1 = m_nCorner[1]; 
		_2 = m_nCorner[2]; 
		_3 = m_nCorner[3]; 
	}
	///	创建三角形索引，返回输出三角形的个数.
	int			GenerateIndex( LPVOID pIndex, TERRAINVERTEX* pHeightMap,ZFrustum* pFrustum, float fLODRatio );



	///////////////////////////////////////////////
private:
	/// 当前子节为可以输出的子节?
	BOOL		_IsVisible() 
	{
		return ( m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= 1 );
	}
	/// 创建输出多边形的索引.
	int			_GenTriIndex( int nTriangles, LPVOID pIndex );

public:
	/// 创建四叉树.
	BOOL		Build();
	///	创建三角形索引，返回输出三角形的个数.
	int			GenerateIndex( LPVOID pIB );
};


