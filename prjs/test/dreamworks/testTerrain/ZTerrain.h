
#include "define.h"

#define MAX_TERRAIN_TEX 4

class ZTerrain
{
private:
	int						m_cxDIB;		/// DIB的长度像素
	int						m_czDIB;		/// DIB的宽度像素
	int						m_cyDIB;		/// DIB的最大高度值（0~255之间的值）
	D3DXVECTOR3			m_vfScale;		/// x scale, y scale, z scale
	TERRAINVERTEX*		m_pvHeightMap;	/// 高度图的顶点阵列
	LPDIRECT3DDEVICE9	m_pd3dDevice;	/// 输出用D3D设备
	LPDIRECT3DTEXTURE9	m_pTex[MAX_TERRAIN_TEX];	/// 纹理
	LPDIRECT3DVERTEXBUFFER9	  m_pVB;			/// 地形输出用顶点缓冲
	LPDIRECT3DINDEXBUFFER9	m_pIB;			/// 地形输出用索引缓冲
	int						    m_nTriangles;	/// 输出三角形的个数
	ZQuadTree*				m_pQuadTree;	/// 四叉树对象的指针
public:
	// 创建的相关函数
	ZTerrain();
	~ZTerrain();
	/**
	* @brief 地形对象初始化.
	* @param pDev : 输出用D3D设备 
	* @param pvfScale : 高度图 x,y,z值相乘的尺寸值(scale vector)
	* @param lpBMPFilename : 高度图用 BMP文件名
	* @param lpTexFilename : 输出用 D3D设备 
	*/
	HRESULT		Create( LPDIRECT3DDEVICE9 pDev, 
		D3DXVECTOR3* pvfScale, 
		LPSTR lpBMPFilename, 
		LPSTR lpTexFilename[MAX_TERRAIN_TEX] );
	// ZTerrain内部调用的函数
private:
	/// 在存储器中删除地形对象.
	HRESULT		_Destroy();
	/// 读取地形对象中使用的纹理.
	HRESULT		_LoadTextures( 
		LPSTR lpTexFilename[MAX_TERRAIN_TEX] );
	/// 打开BMP文件，创建高度图.
	HRESULT		_BuildHeightMap( LPSTR lpFilename );

	/// 创建适合BMP文件大小的四叉树.
	HRESULT		_BuildQuadTree();

	/// 创建顶点缓冲和索引缓冲.
	HRESULT		_CreateVIB();

	/// 地形输出到画面.
	HRESULT		_Render();
public:
	/// 得到x、z位置的顶点值.
	TERRAINVERTEX*	GetVertex( int x, int z ) { return (m_pvHeightMap+x+z*m_cxDIB); }

	/// 得到x、z位置的高度值（y）.
	float		GetHeight( int x, int z ) 
	{ 
		return (GetVertex( x, z ))->p.y; 
	}
	/// 高度图BMP文件的长度像素
	int			GetCXDIB() { return m_cxDIB; }

	/// 高度图BMP文件的宽度像素
	int			GetCZDIB() { return m_czDIB; }

	/// 高度图的实际x轴宽度值（column）
	float		GetCXTerrain() { return m_cxDIB * m_vfScale.x; }

	/// 高度图的实际y轴高度值（height）
	float		GetCYTerrain() { return m_cyDIB * m_vfScale.y; }

	/// 高度图的实际z轴长度值（row）
	float		GetCZTerrain() { return m_czDIB * m_vfScale.z; }

	/// 地形输出到画面.
	HRESULT		Draw();
};

