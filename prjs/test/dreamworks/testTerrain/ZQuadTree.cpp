
#include "ZFrustum.h"
#include "ZQuadTree.h"

#define IsInRect PtInRect

/// ���ѭ���ڵ㴴����
ZQuadTree::ZQuadTree( int cx, int cy )
{
	int		i;
	m_pParent = NULL;
	for( i = 0 ; i < 4 ; i++ )
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
	}
	// ����ѭ���ڵ��4����ֵ
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

// �²��ӽڵ㴴����
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

///// �²��ӽڵ㴴����
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
/// ɾ����
ZQuadTree::~ZQuadTree()
{
	_Destroy();
}

/// �ڴ洢����ɾ���Ĳ���.
void	ZQuadTree::_Destroy()
{
	for( int i = 0 ; i < 4 ; i++ ) DEL( m_pChild[i] );
}

/// ����4����ֵ.
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

/// ����ӽڵ�.
ZQuadTree*	ZQuadTree::_AddChild( int nCornerTL, int nCornerTR, 
								 int nCornerBL, int nCornerBR )
{
	ZQuadTree*	pChild;
	pChild = new ZQuadTree( this );
	pChild->_SetCorners( nCornerTL, nCornerTR, nCornerBL, nCornerBR );
	return pChild;
}

/// ʹ��4���²���Ĳ��������ٷָsubdivide��.
BOOL	ZQuadTree::_SubDivide()
{
	int		nTopEdgeCenter;
	int		nBottomEdgeCenter;
	int		nLeftEdgeCenter;
	int		nRightEdgeCenter;
	int		nCentralPoint;

	// �ϱ��м�
	nTopEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] ) / 2;
	// �±��м� 
	nBottomEdgeCenter	= ( m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 2;
	// ����м�
	nLeftEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL] ) / 2;
	// �ұ��м�
	nRightEdgeCenter	= ( m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR] ) / 2;
	// ������
	nCentralPoint		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	// ���ܽ����ٷָ��ˣ���ôSubDivide()����
	if( (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= 1 )
	{
		return FALSE;
	}

	// ���4���ӽڵ�
	m_pChild[CORNER_TL] = _AddChild( m_nCorner[CORNER_TL],nTopEdgeCenter, nLeftEdgeCenter, nCentralPoint );
	m_pChild[CORNER_TR] = _AddChild( nTopEdgeCenter,m_nCorner[CORNER_TR], nCentralPoint, nRightEdgeCenter );
	m_pChild[CORNER_BL] = _AddChild( nLeftEdgeCenter, nCentralPoint,m_nCorner[CORNER_BL], nBottomEdgeCenter );
	m_pChild[CORNER_BR] = _AddChild( nCentralPoint, nRightEdgeCenter,nBottomEdgeCenter, m_nCorner[CORNER_BR] );
	return TRUE;
}

/// �����������ε�����.
int		ZQuadTree::_GenTriIndex( int nTris, LPVOID pIndex )
{
	if( _IsVisible() )
	{
#ifdef _USE_INDEX16
		LPWORD p = ((LPWORD)pIndex) + nTris * 3;
#else
		LPDWORD p = ((LPDWORD)pIndex) + nTris * 3;
#endif
		// ���Ϸ�������
		*p++ = m_nCorner[0];
		*p++ = m_nCorner[1];
		*p++ = m_nCorner[2];
		nTris++;
		// ���·�������
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

// �����������ε�����.
int		ZQuadTree::_GenTriIndex( int nTris, LPVOID pIndex,TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio )
{
	// ������޳��Ľڵ㣬ֱ�ӷ���
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
	// ��ǰ�ڵ�������?
	if( IsVisible( pHeightMap, pFrustum->GetPos(), fLODRatio ) )
	{
		// ��������²�Ľڵ㣬���ܽ����ٷָsubdivide���Ļ���ֱ�����������.
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
		// �Ϸ��ڽ��ڵ㣨neightbor node���������?
		if( m_pNeighbor[EDGE_UP] ) b[EDGE_UP] =
			m_pNeighbor[EDGE_UP]->IsVisible( pHeightMap, pFrustum->GetPos(), fLODRatio );
		// �·��ڽ��ڵ㣨neightbor node���������?
		if( m_pNeighbor[EDGE_DN] ) b[EDGE_DN] =
			m_pNeighbor[EDGE_DN]->IsVisible( pHeightMap, pFrustum->GetPos(), fLODRatio );
		// ���ڽ��ڵ㣨neightbor node���������?
		if( m_pNeighbor[EDGE_LT] ) b[EDGE_LT] =	m_pNeighbor[EDGE_LT]->IsVisible( pHeightMap, pFrustum->GetPos(), fLODRatio );
		// �ҷ��ڽ��ڵ㣨neightbor node���������?
		if( m_pNeighbor[EDGE_RT] ) b[EDGE_RT] =	m_pNeighbor[EDGE_RT]->IsVisible( pHeightMap, pFrustum->GetPos(), fLODRatio );
		// ���е��ڽ��ڵ㶼��������Ļ�����ǰ�ڵ���ڽ��ڵ����ͬһ��LOD 
		// �Ͳ���Ҫ�����ٷָ�.
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
		if( !b[EDGE_UP] ) // ��Ҫ�Ϸ��ٷָ���?
		{
			n = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TL]; *p++ = n;
			nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_TR];
			nTris++;
		}
		else	// ����Ҫ�Ϸ��ٷָ�����
		{
			*p++ = m_nCenter; 
			*p++ = m_nCorner[CORNER_TL]; 
			*p++ = m_nCorner[CORNER_TR]; nTris++; 
		}
		if( !b[EDGE_DN] ) // ��Ҫ�·��ٷָ���?
		{
			n = ( m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BR]; *p++ = n;
			nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_BL];
			nTris++;
		}
		else	// ����Ҫ�·��ٷָ�����
		{ 
			*p++ = m_nCenter; 
			*p++ = m_nCorner[CORNER_BR]; 
			*p++ = m_nCorner[CORNER_BL]; nTris++;
		}
		if( !b[EDGE_LT] ) // ��Ҫ����ٷָ���?
		{
			n = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL] ) / 2;
			*p++ = m_nCenter; 
			*p++ = m_nCorner[CORNER_BL]; 
			*p++ = n; nTris++;
			*p++ = m_nCenter; 
			*p++ = n; *p++ = m_nCorner[CORNER_TL]; 
			nTris++;
		}
		else	// ����Ҫ����ٷָ�����
		{ 
			*p++ = m_nCenter; 
			*p++ = m_nCorner[CORNER_BL];
			*p++ = m_nCorner[CORNER_TL];
			nTris++; 
		}
		if( !b[EDGE_RT] ) // ��Ҫ�Ҳ��ٷָ���?
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
		else	// ����Ҫ�Ҳ��ٷָ�����
		{ 
			*p++ = m_nCenter; 
			*p++ = m_nCorner[CORNER_TR]; 
			*p++ = m_nCorner[CORNER_BR];
			nTris++; 
		}
		return nTris;	// ���ڸýڵ��µ��ӽڵ�û�м����ı�Ҫ������!
	}
	// �����ӽڵ�
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

// ��ǰ�ڵ������ƽ��ͷ��?
int ZQuadTree::_IsInFrustum( TERRAINVERTEX* pHeightMap,ZFrustum* pFrustum )
{
	BOOL	b[4];
	BOOL	bInSphere;
	// �ڱ߽�������?
	bInSphere = pFrustum->IsInSphere( 
		(D3DXVECTOR3*)(pHeightMap+m_nCenter), m_fRadius );
	// ���ڱ߽������ڵĻ�������ʡ�Ե㵥λ��ƽ��ͷ��Ĳ��ԣ�����
	if( !bInSphere ) return FRUSTUM_OUT;	
	// �Ĳ�����4��߽�ƽ��ͷ��Ĳ���
	b[0] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[0]) );
	b[1] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[1]) );
	b[2] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[2]) );
	b[3] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[3]) );
	// 4������ƽ��ͷ���ڲ�
	if( (b[0] + b[1] + b[2] + b[3]) == 4 ) return FRUSTUM_COMPLETELY_IN;
	// һ������ƽ��ͷ���ڲ�
	return FRUSTUM_PARTIALLY_IN;
}
// ����_IsInFrustum()�����Ľ����ƽ��ͷ������޳�
void	ZQuadTree::_FrustumCull( TERRAINVERTEX* pHeightMap,ZFrustum* pFrustum)
{
	int ret;
	ret = _IsInFrustum( pHeightMap, pFrustum );
	switch( ret )
	{
		// ��ȫ������ƽ��ͷ�壬����Ҫ�����²����
	case FRUSTUM_COMPLETELY_IN :				
		m_bCulled = FALSE;
		return;
		// ���ְ�����ƽ��ͷ�壬��Ҫ�����²�ڵ����
	case FRUSTUM_PARTIALLY_IN :					
		m_bCulled = FALSE;
		break;
		// ��ȫ����ƽ��ͷ�壬����Ҫ�����²�ڵ����
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
// �õ��ĸ������Ϸ����·����󷽡��ҷ������ڽ��ڵ�����.
int	ZQuadTree::_GetNodeIndex( int ed, int cx, int& _0, int& _1, 
							 int& _2, int& _3 )
{
	int		n, _a, _b, _c, _d, gap;
	_a	= _0;
	_b	= _1;
	_c	= _2;
	_d  = _3;
	gap	= _b - _a;	// ��ǰ�ڵ�����ҿ��ֵ
	switch( ed )
	{
	case EDGE_UP:	// �Ϸ��ڽ��ڵ������
		_0 = _a - cx * gap;
		_1 = _b - cx * gap;
		_2 = _a;
		_3 = _b;
		break;
	case EDGE_DN:	// �·��ڽ��ڵ������
		_0 = _c;
		_1 = _d;
		_2 = _c + cx * gap;
		_3 = _d + cx * gap;
		break;
	case EDGE_LT:	// ����ڽ��ڵ������
		_0 = _a - gap;
		_1 = _a;
		_2 = _c - gap;
		_3 = _c;
		break;
	case EDGE_RT:	// �Ҳ��ڽ��ڵ������
		_0 = _b;
		_1 = _b + gap;
		_2 = _d;
		_3 = _d + gap;
		break;
	}
	n = ( _0 + _1 + _2 + _3 ) / 4;	// ������ڵ������
	if( !IS_IN_RANGE( n, 0, cx * cx - 1 ) ) return -1;
	return n;
}	
// �����Ĳ�������ѯ���ĸ���ֵһ�µĽڵ�.
ZQuadTree* ZQuadTree::_FindNode( TERRAINVERTEX* pHeightMap, int _0, int _1, int _2, int _3 )
{
	ZQuadTree*	p = NULL;
	// �����һ�µĽڵ㣬���ؽڵ�ֵ
	if( (m_nCorner[0] == _0) && (m_nCorner[1] == _1) && (m_nCorner[2] == _2) && (m_nCorner[3] == _3) )
		return this;
	// ���ӽڵ���?
	if( m_pChild[0] )
	{
		RECT	rc;
		POINT	pt;
		int n = ( _0 + _1 + _2 + _3 ) / 4;
		// ��ǰ��ͼ�ϵ�λ��
		pt.x = (int)pHeightMap[n].p.x;
		pt.y = (int)pHeightMap[n].p.z;
		// ���ĸ���ֵΪ��׼������ӽڵ���ͼ�ķ�Χ.
		SetRect( &rc,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].p.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].p.z,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].p.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].p.z );
		// ptֵ�ڷ�Χ�ڵĻ��������ӽڵ�.
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
// �����ڽ��ڵ㣨��ֹ�����εĹ��ѣ�
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
		// ����ڽ��ڵ���ĸ���ֵ.
		n = _GetNodeIndex( i, cx, _0, _1, _2, _3 );
		// �ɽ�ֵ�õ��ڽ��ڵ��ָ��.
		if( n >= 0 ) m_pNeighbor[i] = pRoot->_FindNode( pHeightMap,
			_0, _1, _2, _3 );
	}
	// ���ص��õ��ӽڵ�
	if( m_pChild[0] )
	{
		m_pChild[0]->_BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[1]->_BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[2]->_BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[3]->_BuildNeighborNode( pRoot, pHeightMap, cx );
	}
}
// �����Ĳ�������Build()�����У�
BOOL	ZQuadTree::_BuildQuadTree( TERRAINVERTEX* pHeightMap )
{
	if( _SubDivide() )
	{
		// ������Ϸ������·��ľ���.
		D3DXVECTOR3 v = *((D3DXVECTOR3*)
			(pHeightMap+m_nCorner[CORNER_TL])) - 
			*((D3DXVECTOR3*)(pHeightMap+m_nCorner[CORNER_BR]));
		// V�ľ�����ǰ����ýڵ�ı߽�����İ뾶, 
		// ��2��ð뾶.
		m_fRadius	  = D3DXVec3Length( &v ) / 2.0f;
		m_pChild[CORNER_TL]->_BuildQuadTree( pHeightMap );
		m_pChild[CORNER_TR]->_BuildQuadTree( pHeightMap );
		m_pChild[CORNER_BL]->_BuildQuadTree( pHeightMap );
		m_pChild[CORNER_BR]->_BuildQuadTree( pHeightMap );
	}
	return TRUE;
}
// �����Ĳ���.
BOOL	ZQuadTree::Build( TERRAINVERTEX* pHeightMap )
{
	// �����Ĳ���
	_BuildQuadTree( pHeightMap );
	// �����ڽ��ڵ�
	_BuildNeighborNode( this, pHeightMap, m_nCorner[CORNER_TR]+1 );
	return TRUE;
}
//	����������������������������εĸ���.
int		ZQuadTree::GenerateIndex( LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio )
{
	// �Ƚ���ƽ��ͷ����޳����ų����޳��Ľڵ�.
	_FrustumCull( pHeightMap, pFrustum );
	// ������������ε�����֮�󷵻������εĸ���.
	return _GenTriIndex( 0, pIndex, pHeightMap, pFrustum, fLODRatio );
}

/// �����Ĳ���.
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

///	����������������������������εĸ���.
int		ZQuadTree::GenerateIndex( LPVOID pIndex )
{
	return _GenTriIndex( 0, pIndex );
}
