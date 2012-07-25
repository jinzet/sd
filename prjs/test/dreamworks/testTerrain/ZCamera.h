#ifndef _ZCAMERA_H_
#define _ZCAMERA_H_

#include <d3d9.h>
#include <d3dx9.h>

/**
 * @brief �򵥵������������
 * @author ���Ǻ�
 * @todo û�ж��Ϸ�90�Ȼ��·�90�Ƚ��м��飬����ֵ�������ǲ��ȶ���.
 */
class ZCamera
{
	D3DXVECTOR3		m_vEye;			/// ������ĵ�ǰλ��
	D3DXVECTOR3		m_vLookat;		/// �����������λ��
	D3DXVECTOR3		m_vUp;			/// ��������Ϸ�����

	D3DXVECTOR3		m_vView;		/// �����ָ��ĵ�λ��������
	D3DXVECTOR3		m_vCross;		/// ������Ĳ������� cross��view��up��

	D3DXMATRIXA16	m_matView;		/// ���������
	D3DXMATRIXA16	m_matBill;		/// ����ƾ���������������
public:

	/// ������
	ZCamera();
	
	/// �õ����������.
	D3DXMATRIXA16*	GetViewMatrix() { return &m_matView; }
	
	/// �õ�����ƾ���.
	D3DXMATRIXA16*	GetBillMatrix() { return &m_matBill; }
public:

	/// �趨�������������Ļ�������ֵ.
	D3DXMATRIXA16*	SetView( D3DXVECTOR3* pvEye,D3DXVECTOR3* pvLookat,D3DXVECTOR3* pvUp);

	/// �趨�������λ��ֵ.
	void			SetEye( D3DXVECTOR3* pv ) { m_vEye = *pv; }
	
	/// �õ��������λ��ֵ.
	D3DXVECTOR3*	GetEye() { return &m_vEye; }
	
	/// �趨�����������ֵ.
	void			SetLookat( D3DXVECTOR3* pv ) { m_vLookat = *pv; }

	/// �õ������������ֵ.
	D3DXVECTOR3*	GetLookat() { return &m_vLookat; }
	
	/// �趨��������Ϸ�����ֵ.
	void			SetUp( D3DXVECTOR3* pv ) { m_vUp = *pv; }
	
	/// �õ���������Ϸ�����ֵ.
	D3DXVECTOR3*	GetUp() { return &m_vUp; }
	
	/// ����ֵ.
	void			Flush() { SetView( &m_vEye, &m_vLookat, &m_vUp ); }

/// �����������ϵ��X��Ϊ����תangle.
	D3DXMATRIXA16*	RotateLocalX( float angle );
	
	/// �����������ϵ��Y��Ϊ����תangle.
	D3DXMATRIXA16*	RotateLocalY( float angle );

//	D3DXMATRIXA16*	RotateLocalZ( float angle );

	/// �ƶ�����������ϵ�� *pvֵ��λ��.
	D3DXMATRIXA16*	MoveTo( D3DXVECTOR3* pv );
	
	/// �������������ϵX�᷽��ǰ��dist������ʱΪ-dist��
	D3DXMATRIXA16*	MoveLocalX( float dist );

	/// �������������ϵY�᷽��ǰ��dist������ʱΪ-dist��
	D3DXMATRIXA16*	MoveLocalY( float dist );

	/// �������������ϵZ�᷽��ǰ��dist������ʱΪ-dist��
	D3DXMATRIXA16*	MoveLocalZ( float dist );
};

#endif // _ZCAMERA_H_
