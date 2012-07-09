
// dreamworks.h : main header file for the dreamworks application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CdreamworksApp:
// See dreamworks.cpp for the implementation of this class
//

class CdreamworksApp : public CWinAppEx
{
public:
	CdreamworksApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CdreamworksApp theApp;
