
#include "define.h"

#define MAX_TERRAIN_TEX 4

class ZTerrain
{
private:
	int						m_cxDIB;		/// DIB�ĳ�������
	int						m_czDIB;		/// DIB�Ŀ������
	int						m_cyDIB;		/// DIB�����߶�ֵ��0~255֮���ֵ��
	D3DXVECTOR3			m_vfScale;		/// x scale, y scale, z scale
	TERRAINVERTEX*		m_pvHeightMap;	/// �߶�ͼ�Ķ�������
	LPDIRECT3DDEVICE9	m_pd3dDevice;	/// �����D3D�豸
	LPDIRECT3DTEXTURE9	m_pTex[MAX_TERRAIN_TEX];	/// ����
	LPDIRECT3DVERTEXBUFFER9	  m_pVB;			/// ��������ö��㻺��
	LPDIRECT3DINDEXBUFFER9	m_pIB;			/// �����������������
	int						    m_nTriangles;	/// ��������εĸ���
	ZQuadTree*				m_pQuadTree;	/// �Ĳ��������ָ��
public:
	// ��������غ���
	ZTerrain();
	~ZTerrain();
	/**
	* @brief ���ζ����ʼ��.
	* @param pDev : �����D3D�豸 
	* @param pvfScale : �߶�ͼ x,y,zֵ��˵ĳߴ�ֵ(scale vector)
	* @param lpBMPFilename : �߶�ͼ�� BMP�ļ���
	* @param lpTexFilename : ����� D3D�豸 
	*/
	HRESULT		Create( LPDIRECT3DDEVICE9 pDev, 
		D3DXVECTOR3* pvfScale, 
		LPSTR lpBMPFilename, 
		LPSTR lpTexFilename[MAX_TERRAIN_TEX] );
	// ZTerrain�ڲ����õĺ���
private:
	/// �ڴ洢����ɾ�����ζ���.
	HRESULT		_Destroy();
	/// ��ȡ���ζ�����ʹ�õ�����.
	HRESULT		_LoadTextures( 
		LPSTR lpTexFilename[MAX_TERRAIN_TEX] );
	/// ��BMP�ļ��������߶�ͼ.
	HRESULT		_BuildHeightMap( LPSTR lpFilename );

	/// �����ʺ�BMP�ļ���С���Ĳ���.
	HRESULT		_BuildQuadTree();

	/// �������㻺�����������.
	HRESULT		_CreateVIB();

	/// �������������.
	HRESULT		_Render();
public:
	/// �õ�x��zλ�õĶ���ֵ.
	TERRAINVERTEX*	GetVertex( int x, int z ) { return (m_pvHeightMap+x+z*m_cxDIB); }

	/// �õ�x��zλ�õĸ߶�ֵ��y��.
	float		GetHeight( int x, int z ) 
	{ 
		return (GetVertex( x, z ))->p.y; 
	}
	/// �߶�ͼBMP�ļ��ĳ�������
	int			GetCXDIB() { return m_cxDIB; }

	/// �߶�ͼBMP�ļ��Ŀ������
	int			GetCZDIB() { return m_czDIB; }

	/// �߶�ͼ��ʵ��x����ֵ��column��
	float		GetCXTerrain() { return m_cxDIB * m_vfScale.x; }

	/// �߶�ͼ��ʵ��y��߶�ֵ��height��
	float		GetCYTerrain() { return m_cyDIB * m_vfScale.y; }

	/// �߶�ͼ��ʵ��z�᳤��ֵ��row��
	float		GetCZTerrain() { return m_czDIB * m_vfScale.z; }

	/// �������������.
	HRESULT		Draw();
};

