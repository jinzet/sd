
#include "ZFLog.h"
#include "ZQuadTree.h"
#include "ZTerrain.h"
//#include "dib.h"

LPBYTE DibLoadHandle(LPSTR lpFilename)
{
	return NULL;
}
int DIB_CX(LPBYTE b)
{
	return 0;
}
int DIB_CY(LPBYTE b)
{
	return 0;
}
int* DIB_DATAXY_INV(LPBYTE b,int x,int z)
{
	return NULL;
}
void DibDeleteHandle(LPBYTE b)
{}

/// ������
ZTerrain::ZTerrain()
{
	m_cxDIB			= 0;
	m_czDIB			= 0;
	m_cyDIB			= 0;
	m_vfScale		= D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_pvHeightMap	= NULL;
	m_pd3dDevice	= NULL;
	for( int i = 0 ; i < MAX_TERRAIN_TEX ; i++ ) m_pTex[i] = NULL;
	m_pVB			= NULL;
	m_pIB			= NULL;
	m_nTriangles	= 0;
	m_pQuadTree		= NULL;
}
/// ɾ����
ZTerrain::~ZTerrain()
{
	_Destroy();
}
/**
* @brief ���ζ����ʼ��.
* @param pDev : �����D3D�豸 
* @param pvfScale : �߶�ͼ��x��y��zֵ��˵ĳߴ�ֵ��scale vector��
* @param fLODRatio : LOD����ʱʹ�õ�ֵ
* @param lpBMPFilename : �߶�ͼ��BMP�ļ���
* @param lpTexFilename : �����D3D�豸 
*/
HRESULT	ZTerrain::Create( LPDIRECT3DDEVICE9 pDev, 
						 D3DXVECTOR3* pvfScale, LPSTR lpBMPFilename, 
						 LPSTR lpTEXFilename[4] )
{
	m_pd3dDevice = pDev;
	m_vfScale = *pvfScale;
	if( FAILED( _BuildHeightMap( lpBMPFilename ) ) ) 
	{ _Destroy(); return E_FAIL; }
	if( FAILED( _LoadTextures( lpTEXFilename ) ) )
	{ _Destroy(); return E_FAIL; }
	if( FAILED( _CreateVIB() ) ) 
	{ _Destroy(); return E_FAIL; }
	m_pQuadTree = new ZQuadTree( m_cxDIB, m_czDIB );
	if( FAILED( _BuildQuadTree() ) ) 
	{ _Destroy(); return E_FAIL; }
	return S_OK;
}
/// �ڴ洢����ɾ�����ζ���.
HRESULT	ZTerrain::_Destroy()
{
	DEL( m_pQuadTree );
	DELS( m_pvHeightMap );
	REL( m_pVB );
	REL( m_pIB );
	for( int i = 0 ; i < 4 ; i++ ) REL( m_pTex[i] );
	return S_OK;
}
/// ��ȡ���ζ�����ʹ�õ�����.
HRESULT	ZTerrain::_LoadTextures( LPSTR lpTexFilename[4] )
{
	for( int i = 0 ; i < 4 ; i++ )
		D3DXCreateTextureFromFile( m_pd3dDevice, 
		lpTexFilename[i], &m_pTex[i] );
	return S_OK;
}

/// ��BMP�ļ��������߶�ͼ.
HRESULT	ZTerrain::_BuildHeightMap( LPSTR lpFilename )
{
	LPBYTE	pDIB = DibLoadHandle( lpFilename );
	if( !pDIB ) return E_FAIL;

	m_cxDIB = DIB_CX( pDIB );
	m_czDIB = DIB_CY( pDIB );

	// ���m_cxDIB��m_czDIB���ǣ�2^n+1���Ļ�����Ҫ����E_FAIL�����޸�

	m_pvHeightMap = new TERRAINVERTEX[m_cxDIB * m_czDIB];

	TERRAINVERTEX v;
	for( int z = 0 ; z < m_czDIB ; z++ )
	{
		for( int x = 0 ; x < m_cxDIB ; x++ )
		{
			v.p.x = (float)( ( x - m_cxDIB / 2 ) * m_vfScale.x );
			v.p.z = -(float)( ( z - m_czDIB / 2 ) * m_vfScale.z );
			v.p.y = (float)( *( DIB_DATAXY_INV( pDIB, x, z ) ) ) * m_vfScale.y;
			D3DXVec3Normalize( &v.n, &v.p );
			v.t.x = (float)x / (float)( m_cxDIB - 1 );
			v.t.y = (float)z / (float)( m_czDIB - 1 );
			m_pvHeightMap[x + z * m_czDIB] = v;
		}
	}
	DibDeleteHandle( pDIB );
	return S_OK;
}
/// �����ʺ�BMP�ļ���С���Ĳ���.
HRESULT	ZTerrain::_BuildQuadTree()
{
	m_pQuadTree->Build();
	return S_OK;
}
/// �������㻺�����������.
HRESULT	ZTerrain::_CreateVIB()
{
	// ����VB
	if( FAILED( m_pd3dDevice->CreateVertexBuffer(
		m_cxDIB*m_czDIB*sizeof(TERRAINVERTEX),
		0, TERRAINVERTEX::FVF,
		D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
	{
		_Destroy();
		return E_FAIL;
	}
	VOID* pVertices;
	if( FAILED( m_pVB->Lock( 0, m_cxDIB*m_czDIB*sizeof(TERRAINVERTEX),
		(void**)&pVertices, 0 ) ) )
	{
		_Destroy();
		return E_FAIL;
	}
	memcpy( pVertices, m_pvHeightMap,
		m_cxDIB*m_czDIB*sizeof(TERRAINVERTEX) );
	m_pVB->Unlock();
#ifdef _USE_INDEX16
	// ����IB
	if( FAILED( m_pd3dDevice->CreateIndexBuffer( 
		(m_cxDIB-1)*(m_czDIB-1)*2 * sizeof(TRIINDEX), 
		0, D3DFMT_INDEX16, D3DPOOL_DEFAULT,
		&m_pIB, NULL ) ) )
	{
		_Destroy();
		return E_FAIL;
	}
	g_pLog->Log( "Using 16bit index buffer..." );
#else
	// ����IB
	if( FAILED( m_pd3dDevice->CreateIndexBuffer( 
		(m_cxDIB-1)*(m_czDIB-1)*2 * sizeof(TRIINDEX), 
		0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, 
		&m_pIB, NULL ) ) )
	{
		_Destroy();
		return E_FAIL;
	}
	g_pLog->Log( "Using 32bit index buffer..." );
#endif
	return S_OK;
}
/// �������������.
HRESULT	ZTerrain::_Render()
{
	m_pd3dDevice->SetTexture( 0, m_pTex[0] );								// ��0�������̶�������ɫͼ��
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,
		D3DTEXF_LINEAR );	// 0�������ķŴ��˾�
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );	// 0������ʹ��0����������
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,
		D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,
		D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,
		D3DTA_DIFFUSE );
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0,
		sizeof(TERRAINVERTEX) );
	m_pd3dDevice->SetFVF( TERRAINVERTEX::FVF );
	m_pd3dDevice->SetIndices( m_pIB );
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0,
		m_cxDIB * m_czDIB, 0, m_nTriangles );
	return S_OK;
}

/// �������������.
HRESULT	ZTerrain::Draw()
{
	LPDWORD		pI;
	if( FAILED( m_pIB->Lock( 0, 
		(m_cxDIB-1)*(m_czDIB-1)*2 * sizeof(TRIINDEX),
		(void**)&pI, 0 ) ) )
	{
		return E_FAIL;
	}
	m_nTriangles = m_pQuadTree->GenerateIndex( pI );
	m_pIB->Unlock();
	//g_pLog->Log( "Triangles=%d", m_nTriangles );
	_Render();
	return S_OK;
}
