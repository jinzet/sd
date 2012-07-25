#ifndef _ZCAMERA_H_
#define _ZCAMERA_H_

#include <d3d9.h>
#include <d3dx9.h>

/**
 * @brief 简单的摄像机操作类
 * @author 金智汉
 * @todo 没有对上方90度或下方90度进行检验，从数值上来看是不稳定的.
 */
class ZCamera
{
	D3DXVECTOR3		m_vEye;			/// 摄像机的当前位置
	D3DXVECTOR3		m_vLookat;		/// 摄像机的视线位置
	D3DXVECTOR3		m_vUp;			/// 摄像机的上方向量

	D3DXVECTOR3		m_vView;		/// 摄像机指向的单位方向向量
	D3DXVECTOR3		m_vCross;		/// 摄像机的侧面向量 cross（view，up）

	D3DXMATRIXA16	m_matView;		/// 摄像机矩阵
	D3DXMATRIXA16	m_matBill;		/// 广告牌矩阵（摄像机的逆矩阵）
public:

	/// 创建者
	ZCamera();
	
	/// 得到摄像机矩阵.
	D3DXMATRIXA16*	GetViewMatrix() { return &m_matView; }
	
	/// 得到广告牌矩阵.
	D3DXMATRIXA16*	GetBillMatrix() { return &m_matBill; }
public:

	/// 设定创建摄像机矩阵的基本向量值.
	D3DXMATRIXA16*	SetView( D3DXVECTOR3* pvEye,D3DXVECTOR3* pvLookat,D3DXVECTOR3* pvUp);

	/// 设定摄像机的位置值.
	void			SetEye( D3DXVECTOR3* pv ) { m_vEye = *pv; }
	
	/// 得到摄像机的位置值.
	D3DXVECTOR3*	GetEye() { return &m_vEye; }
	
	/// 设定摄像机的视线值.
	void			SetLookat( D3DXVECTOR3* pv ) { m_vLookat = *pv; }

	/// 得到摄像机的视线值.
	D3DXVECTOR3*	GetLookat() { return &m_vLookat; }
	
	/// 设定摄像机的上方向量值.
	void			SetUp( D3DXVECTOR3* pv ) { m_vUp = *pv; }
	
	/// 得到摄像机的上方向量值.
	D3DXVECTOR3*	GetUp() { return &m_vUp; }
	
	/// 更新值.
	void			Flush() { SetView( &m_vEye, &m_vLookat, &m_vUp ); }

/// 以摄像机坐标系的X轴为轴旋转angle.
	D3DXMATRIXA16*	RotateLocalX( float angle );
	
	/// 以摄像机坐标系的Y轴为轴旋转angle.
	D3DXMATRIXA16*	RotateLocalY( float angle );

//	D3DXMATRIXA16*	RotateLocalZ( float angle );

	/// 移动到世界坐标系的 *pv值的位置.
	D3DXMATRIXA16*	MoveTo( D3DXVECTOR3* pv );
	
	/// 朝着摄像机坐标系X轴方向前进dist（后退时为-dist）
	D3DXMATRIXA16*	MoveLocalX( float dist );

	/// 朝着摄像机坐标系Y轴方向前进dist（后退时为-dist）
	D3DXMATRIXA16*	MoveLocalY( float dist );

	/// 朝着摄像机坐标系Z轴方向前进dist（后退时为-dist）
	D3DXMATRIXA16*	MoveLocalZ( float dist );
};

#endif // _ZCAMERA_H_
