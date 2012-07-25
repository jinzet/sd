
#include "ZFrustum.h"
#include "ZQuadTree.h"

#define IsInRect PtInRect

/// 最初循环节点创建者
ZQuadTree::ZQuadTree( int cx, int cy )
{
	int		i;
	m_pParent = NULL;
	for( i = 0 ; i < 4 ; i++ )
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
	}
	// 设置循环节点的4个角值
	m_nCorner[CORNER_TL]	= 0;
	m_nCorner[CORNER_TR]	= cx - 1;
	m_nCorner[CORNER_BL]	= cx * ( cy - 1 );
	m_nCorner[CORNER_BR]	= cx * cy - 1;
	m_nCenter				= ( m_nCorner[CORNER_TL] + 
		m_nCorner[CORNER_TR] + 
		m_nCorner[CORNER_BL] +
		m_nCorner[CORNER_BR] ) / 4;
	m_bCulled = FALSE;
	m_fRadius = 0.0f;
}

//ZQuadTree::ZQuadTree( int cx, int cy )
//{
//	int		i;
//	m_nCenter = 0;
//	for( i = 0 ; i < 4 ; i++ )
//	{
//		m_pChild[i] = NULL;
//	}
//	m_nCorner[CORNER_TL]	= 0;
//	m_nCorner[CORNER_TR]	= cx - 1;
//	m_nCorner[CORNER_BL]	= cx * ( cy - 1 );
//	m_nCorner[CORNER_BR]	= cx * cy - 1;
//	m_nCenter				= ( m_nCorner[CORNER_TL] + 
//		m_nCorner[CORNER_TR] + 
//		m_nCorner[CORNER_BL] +
//		m_nCorner[CORNER_BR] ) / 4;
//}

// 下层子节点创建者
ZQuadTree::ZQuadTree( ZQuadTree* pParent )
{
	int		i;
	m_pParent = pParent;
	m_nCenter = 0;
	for( i = 0 ; i < 4 ; i++ )
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
		m_nCorner[i] = 0;
	}
	m_bCulled = FALSE;
	m_fRadius = 0.0f;
}

///// 下层子节点创建者
//ZQuadTree::ZQuadTree( ZQuadTree* pParent )
//{
//	int		i;
//	m_nCenter = 0;
//	for( i = 0 ; i < 4 ; i++ )
//	{
//		m_pChild[i] = NULL;
//		m_nCorner[i] = 0;
//	}
//}
/// 删除者
ZQuadTree::~ZQuadTree()
{
	_Destroy();
}

/// 在存储器中删除四叉树.
void	ZQuadTree::_Destroy()
{
	for( int i = 0 ; i < 4 ; i++ ) DEL( m_pChild[i] );
}

/// 设置4个角值.
BOOL	ZQuadTree::_SetCorners( int nCornerTL, int nCornerTR, 
							   int nCornerBL, int nCornerBR )
{
	m_nCorner[CORNER_TL] = nCornerTL;
	m_nCorner[CORNER_TR] = nCornerTR;
	m_nCorner[CORNER_BL] = nCornerBL;
	m_nCorner[CORNER_BR] = nCornerBR;
	m_nCenter			 = ( m_nCorner[CORNER_TL] + 
		m_nCorner[CORNER_TR] + 
		m_nCorner[CORNER_BL] +
		m_nCorner[CORNER_BR] ) / 4;
	return TRUE;
}

/// 添加子节点.
ZQuadTree*	ZQuadTree::_AddChild( int nCornerTL, int nCornerTR, 
								 int nCornerBL, int nCornerBR )
{
	ZQuadTree*	pChild;
	pChild = new ZQuadTree( this );
	pChild->_SetCorners( nCornerTL, nCornerTR, nCornerBL, nCornerBR );
	return pChild;
}

/// 使用4个下层对四叉树进行再分割（subdivide）.
BOOL	ZQuadTree::_SubDivide()
{
	int		nTopEdgeCenter;
	int		nBottomEdgeCenter;
	int		nLeftEdgeCenter;
	int		nRightEdgeCenter;
	int		nCentralPoint;

	// 上边中间
	nTopEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] ) / 2;
	// 下边中间 
	nBottomEdgeCenter	= ( m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 2;
	// 左边中间
	nLeftEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL] ) / 2;
	// 右边中间
	nRightEdgeCenter	= ( m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR] ) / 2;
	// 正中央
	nCentralPoint		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	// 不能进行再分割了？那么SubDivide()结束
	if( (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= 1 )
	{
		return FALSE;
	}

	// 添加4个子节点
	m_pChild[CORNER_TL] = _AddChild( m_nCorner[CORNER_TL],nTopEdgeCenter, nLeftEdgeCenter, nCentralPoint );
	m_pChild[CORNER_TR] = _AddChild( nTopEdgeCenter,m_nCorner[CORNER_TR], nCentralPoint, nRightEdgeCenter );
	m_pChild[CORNER_BL] = _AddChild( nLeftEdgeCenter, nCentralPoint,m_nCorner[CORNER_BL], nBottomEdgeCenter );
	m_pChild[CORNER_BR] = _AddChild( nCentralPoint, nRightEdgeCenter,nBottomEdgeCenter, m_nCorner[CORNER_BR] );
	return TRUE;
}

/// 创建输出多边形的索引.
int		ZQuadTree::_GenTriIndex( int nTris, LPVOID pIndex )
{
	if( _IsVisible() )
	{
#ifdef _USE_INDEX16
		LPWORD p = ((LPWORD)pIndex) + nTris * 3;
#else
		LPDWORD p = ((LPDWORD)pIndex) + nTris * 3;
#endif
		// 左上方三角形
		*p++ = m_nCorner[0];
		*p++ = m_nCorner[1];
		*p++ = m_nCorner[2];
		nTris++;
		// 右下方三角形
		*p++ = m_nCorner[2];
		*p++ = m_nCorner[1];
		*p++ = m_nCorner[3];
		nTris++;
		return nTris;
	}
	if( m_pChild[CORNER_TL] ) nTris = m_pChild[CORNER_TL]->_GenTriIndex( nTris, pIndex );
	if( m_pChild[CORNER_TR] ) nTris = m_pChild[CORNER_TR]->_GenTriIndex( nTris, pIndex );
	if( m_pChild[CORNER_BL] ) nTris = m_pChild[CORNER_BL]->_GenTriIndex( nTris, pIndex );
	if( m_pChild[CORNER_BR] ) nTris = m_pChild[CORNER_BR]->_GenTriIndex( nTris, pIndex );
	return nTris;
}

// 创建输出多边形的索引.
int		ZQuadTree::_GenTriIndex( int nTris, LPVOID pIndex,TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio )
{
	// 如果是剔除的节点，直接返回
	if( m_bCulled )
	{
		m_bCulled = FALSE;
		return nTris;
	}

#ifdef _USE_INDEX16
	LPWORD p = ((LPWORD)pIndex) + nTris * 3;
#else
	LPDWORD p = ((LPDWORD)pIndex) + nTris * 3;
#endif
	// 当前节点必须输出?
	if( IsVisible( pHeightMap, pFrustum->GetPos(), fLODRatio ) )
	{
		// 如果是最下层的节点，不能进行再分割（subdivide）的话，直接输出并返回.
		if( m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= 1 )
		{
			*p++ = m_nCorner[0];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[2];
			nTris++;
			*p++ = m_nCorner[2];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[3];
			nTris++;
			return nTris;
		}
		BOOL	b[4];
		// 上方邻近节点（neightbor node）可以输出?
		if( m_pNeighbor[EDGE_UP] ) b[EDGE_UP] =
			m_pNeighbor[EDGE_UP]->IsVisible( pHeightMap, pFrustum->GetPos(), fLODRatio );
		// 下方邻近节点（neightbor node）可以输出?
		if( m_pNeighbor[EDGE_DN] ) b[EDGE_DN] =
			m_pNeighbor[EDGE_DN]->IsVisible( pHeightMap, pFrustum->GetPos(), fLODRatio );
		// 左方邻近节点（neightbor node）可以输出?
		if( m_pNeighbor[EDGE_LT] ) b[EDGE_LT] =	m_pNeighbor[EDGE_LT]->IsVisible( pHeightMap, pFrustum->GetPos(), fLODRatio );
		// 右方邻近节点（neightbor node）可以输出?
		if( m_pNeighbor[EDGE_RT] ) b[EDGE_RT] =	m_pNeighbor[EDGE_RT]->IsVisible( pHeightMap, pFrustum->GetPos(), fLODRatio );
		// 所有的邻近节点都可以输出的话，当前节点和邻近节点就是同一个LOD 
		// 就不需要进行再分割.
		if( b[EDGE_UP] && b[EDGE_DN] && b[EDGE_LT] && b[EDGE_RT] )
		{
			*p++ = m_nCorner[0];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[2];
			nTris++;
			*p++ = m_nCorner[2];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[3];
			nTris++;
			return nTris;
		}
		int		n;
		if( !b[EDGE_UP] ) // 需要上方再分割吗?
		{
			n = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TL]; *p++ = n;
			nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_TR];
			nTris++;
		}
		else	// 不需要上方再分割的情况
		{
			*p++ = m_nCenter; 
			*p++ = m_nCorner[CORNER_TL]; 
			*p++ = m_nCorner[CORNER_TR]; nTris++; 
		}
		if( !b[EDGE_DN] ) // 需要下方再分割吗?
		{
			n = ( m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BR]; *p++ = n;
			nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_BL];
			nTris++;
		}
		else	// 不需要下方再分割的情况
		{ 
			*p++ = m_nCenter; 
			*p++ = m_nCorner[CORNER_BR]; 
			*p++ = m_nCorner[CORNER_BL]; nTris++;
		}
		if( !b[EDGE_LT] ) // 需要左侧再分割吗?
		{
			n = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL] ) / 2;
			*p++ = m_nCenter; 
			*p++ = m_nCorner[CORNER_BL]; 
			*p++ = n; nTris++;
			*p++ = m_nCenter; 
			*p++ = n; *p++ = m_nCorner[CORNER_TL]; 
			nTris++;
		}
		else	// 不需要左侧再分割的情况
		{ 
			*p++ = m_nCenter; 
			*p++ = m_nCorner[CORNER_BL];
			*p++ = m_nCorner[CORNER_TL];
			nTris++; 
		}
		if( !b[EDGE_RT] ) // 需要右侧再分割吗?
		{
			n = ( m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR] ) / 2;
			*p++ = m_nCenter; 
			*p++ = m_nCorner[CORNER_TR]; 
			*p++ = n; 
			nTris++;
			*p++ = m_nCenter;
			*p++ = n;
			*p++ = m_nCorner[CORNER_BR]; 
			nTris++;
		}
		else	// 不需要右侧再分割的情况
		{ 
			*p++ = m_nCenter; 
			*p++ = m_nCorner[CORNER_TR]; 
			*p++ = m_nCorner[CORNER_BR];
			nTris++; 
		}
		return nTris;	// 由于该节点下的子节点没有检索的必要，返回!
	}
	// 检索子节点
	if( m_pChild[CORNER_TL] ) nTris =
		m_pChild[CORNER_TL]->_GenTriIndex( nTris, pIndex, pHeightMap,
		pFrustum, fLODRatio );
	if( m_pChild[CORNER_TR] ) nTris =
		m_pChild[CORNER_TR]->_GenTriIndex( nTris, pIndex, pHeightMap,
		pFrustum, fLODRatio );
	if( m_pChild[CORNER_BL] ) nTris =
		m_pChild[CORNER_BL]->_GenTriIndex( nTris, pIndex, pHeightMap,
		pFrustum, fLODRatio );
	if( m_pChild[CORNER_BR] ) nTris =
		m_pChild[CORNER_BR]->_GenTriIndex( nTris, pIndex, pHeightMap,
		pFrustum, fLODRatio );
	return nTris;
}

// 当前节点包含在平截头体?
int ZQuadTree::_IsInFrustum( TERRAINVERTEX* pHeightMap,ZFrustum* pFrustum )
{
	BOOL	b[4];
	BOOL	bInSphere;
	// 在边界球体内?
	bInSphere = pFrustum->IsInSphere( 
		(D3DXVECTOR3*)(pHeightMap+m_nCenter), m_fRadius );
	// 不在边界球体内的话，可以省略点单位的平截头体的测试，返回
	if( !bInSphere ) return FRUSTUM_OUT;	
	// 四叉数的4组边界平截头体的测试
	b[0] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[0]) );
	b[1] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[1]) );
	b[2] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[2]) );
	b[3] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[3]) );
	// 4个都在平截头体内部
	if( (b[0] + b[1] + b[2] + b[3]) == 4 ) return FRUSTUM_COMPLETELY_IN;
	// 一部分在平截头体内部
	return FRUSTUM_PARTIALLY_IN;
}
// 根据_IsInFrustum()函数的结果对平截头体进行剔除
void	ZQuadTree::_FrustumCull( TERRAINVERTEX* pHeightMap,ZFrustum* pFrustum)
{
	int ret;
	ret = _IsInFrustum( pHeightMap, pFrustum );
	switch( ret )
	{
		// 完全包含在平截头体，不需要进行下层检索
	case FRUSTUM_COMPLETELY_IN :				
		m_bCulled = FALSE;
		return;
		// 部分包含在平截头体，需要进行下层节点检索
	case FRUSTUM_PARTIALLY_IN :					
		m_bCulled = FALSE;
		break;
		// 完全脱离平截头体，不需要进行下层节点检索
	case FRUSTUM_OUT :						
		m_bCulled = TRUE;
		return;
	}
	if( m_pChild[0] ) m_pChild[0]->_FrustumCull( pHeightMap,
		pFrustum );
	if( m_pChild[1] ) m_pChild[1]->_FrustumCull( pHeightMap,
		pFrustum );
	if( m_pChild[2] ) m_pChild[2]->_FrustumCull( pHeightMap,
		pFrustum );
	if( m_pChild[3] ) m_pChild[3]->_FrustumCull( pHeightMap,
		pFrustum );
}
// 得到四个方向（上方、下方、左方、右方）的邻近节点索引.
int	ZQuadTree::_GetNodeIndex( int ed, int cx, int& _0, int& _1, 
							 int& _2, int& _3 )
{
	int		n, _a, _b, _c, _d, gap;
	_a	= _0;
	_b	= _1;
	_c	= _2;
	_d  = _3;
	gap	= _b - _a;	// 当前节点的左右宽度值
	switch( ed )
	{
	case EDGE_UP:	// 上方邻近节点的索引
		_0 = _a - cx * gap;
		_1 = _b - cx * gap;
		_2 = _a;
		_3 = _b;
		break;
	case EDGE_DN:	// 下方邻近节点的索引
		_0 = _c;
		_1 = _d;
		_2 = _c + cx * gap;
		_3 = _d + cx * gap;
		break;
	case EDGE_LT:	// 左侧邻近节点的索引
		_0 = _a - gap;
		_1 = _a;
		_2 = _c - gap;
		_3 = _c;
		break;
	case EDGE_RT:	// 右侧邻近节点的索引
		_0 = _b;
		_1 = _b + gap;
		_2 = _d;
		_3 = _d + gap;
		break;
	}
	n = ( _0 + _1 + _2 + _3 ) / 4;	// 正中央节点的索引
	if( !IS_IN_RANGE( n, 0, cx * cx - 1 ) ) return -1;
	return n;
}	
// 检索四叉树，查询与四个角值一致的节点.
ZQuadTree* ZQuadTree::_FindNode( TERRAINVERTEX* pHeightMap, int _0, int _1, int _2, int _3 )
{
	ZQuadTree*	p = NULL;
	// 如果是一致的节点，返回节点值
	if( (m_nCorner[0] == _0) && (m_nCorner[1] == _1) && (m_nCorner[2] == _2) && (m_nCorner[3] == _3) )
		return this;
	// 有子节点吗?
	if( m_pChild[0] )
	{
		RECT	rc;
		POINT	pt;
		int n = ( _0 + _1 + _2 + _3 ) / 4;
		// 当前贴图上的位置
		pt.x = (int)pHeightMap[n].p.x;
		pt.y = (int)pHeightMap[n].p.z;
		// 以四个角值为基准，求得子节点贴图的范围.
		SetRect( &rc,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].p.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].p.z,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].p.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].p.z );
		// pt值在范围内的话，进入子节点.
		if( IsInRect( &rc, pt ) )
			return m_pChild[0]->_FindNode( pHeightMap, _0, _1, _2, _3 );
		SetRect( &rc,
			(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].p.x, 
			(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].p.z, 
			(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].p.x, 
			(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].p.z );
		if( IsInRect( &rc, pt ) )
			return m_pChild[1]->_FindNode( pHeightMap, _0, _1, _2, _3 );
		SetRect( &rc,
			(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].p.x,
			(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].p.z,
			(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].p.x,
			(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].p.z );
		if( IsInRect( &rc, pt ) )
			return m_pChild[2]->_FindNode( pHeightMap, _0, _1, _2, _3 );
		SetRect( &rc,
			(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].p.x,
			(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].p.z, 
			(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].p.x, 
			(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].p.z );
		if( IsInRect( &rc, pt ) )
			return m_pChild[3]->_FindNode( pHeightMap, _0, _1, _2, _3 );
	}
	return NULL;
}
// 绘制邻近节点（防止三角形的龟裂）
void	ZQuadTree::_BuildNeighborNode( ZQuadTree* pRoot,
									  TERRAINVERTEX* pHeightMap, int cx )
{
	int				n;
	int				_0, _1, _2, _3;
	for( int i=0 ; i<4 ; i++ )
	{
		_0 = m_nCorner[0];
		_1 = m_nCorner[1];
		_2 = m_nCorner[2];
		_3 = m_nCorner[3];
		// 求得邻近节点的四个角值.
		n = _GetNodeIndex( i, cx, _0, _1, _2, _3 );
		// 由角值得到邻近节点的指针.
		if( n >= 0 ) m_pNeighbor[i] = pRoot->_FindNode( pHeightMap,
			_0, _1, _2, _3 );
	}
	// 返回调用的子节点
	if( m_pChild[0] )
	{
		m_pChild[0]->_BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[1]->_BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[2]->_BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[3]->_BuildNeighborNode( pRoot, pHeightMap, cx );
	}
}
// 创建四叉树（在Build()函数中）
BOOL	ZQuadTree::_BuildQuadTree( TERRAINVERTEX* pHeightMap )
{
	if( _SubDivide() )
	{
		// 求得左上方和右下方的距离.
		D3DXVECTOR3 v = *((D3DXVECTOR3*)
			(pHeightMap+m_nCorner[CORNER_TL])) - 
			*((D3DXVECTOR3*)(pHeightMap+m_nCorner[CORNER_BR]));
		// V的距离就是包含该节点的边界球体的半径, 
		// 除2求得半径.
		m_fRadius	  = D3DXVec3Length( &v ) / 2.0f;
		m_pChild[CORNER_TL]->_BuildQuadTree( pHeightMap );
		m_pChild[CORNER_TR]->_BuildQuadTree( pHeightMap );
		m_pChild[CORNER_BL]->_BuildQuadTree( pHeightMap );
		m_pChild[CORNER_BR]->_BuildQuadTree( pHeightMap );
	}
	return TRUE;
}
// 创建四叉树.
BOOL	ZQuadTree::Build( TERRAINVERTEX* pHeightMap )
{
	// 创建四叉树
	_BuildQuadTree( pHeightMap );
	// 绘制邻近节点
	_BuildNeighborNode( this, pHeightMap, m_nCorner[CORNER_TR]+1 );
	return TRUE;
}
//	创建三角形索引，返回输出三角形的个数.
int		ZQuadTree::GenerateIndex( LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio )
{
	// 先进行平截头体的剔除，排除被剔除的节点.
	_FrustumCull( pHeightMap, pFrustum );
	// 创建输出三角形的索引之后返回三角形的个数.
	return _GenTriIndex( 0, pIndex, pHeightMap, pFrustum, fLODRatio );
}

/// 创建四叉树.
BOOL	ZQuadTree::Build()
{
	if( _SubDivide() )
	{
		m_pChild[CORNER_TL]->Build();
		m_pChild[CORNER_TR]->Build();
		m_pChild[CORNER_BL]->Build();
		m_pChild[CORNER_BR]->Build();
	}
	return TRUE;
}

///	创建三角形索引，返回输出三角形的个数.
int		ZQuadTree::GenerateIndex( LPVOID pIndex )
{
	return _GenTriIndex( 0, pIndex );
}
