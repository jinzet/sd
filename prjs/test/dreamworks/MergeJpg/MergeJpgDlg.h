
// MergeJpgDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CMergeJpgDlg dialog
class CMergeJpgDlg : public CDialog
{
// Construction
public:
	CMergeJpgDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MERGEJPG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString mFmtStr;
	CString mRow0;
	CString mRow1;
	CString mCol0;
	CString mCol1;
	CString mMergedFile;
	long mMergedWidth;
	long mMergedHeight;
	afx_msg void OnBnClickedOk();
};
