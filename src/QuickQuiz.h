// QuickQuiz.h : main header file for the QUICKQUIZ application
//

#if !defined(AFX_QUICKQUIZ_H__FA1CD5BC_C553_4600_94ED_11F1F68500BD__INCLUDED_)
#define AFX_QUICKQUIZ_H__FA1CD5BC_C553_4600_94ED_11F1F68500BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"
#include "FixedRecentFileList.h"

class CDoc;

/////////////////////////////////////////////////////////////////////////////
// CApp:
// See QuickQuiz.cpp for the implementation of this class
//

class CApp : public CWinApp
{
public:
	CApp();

	void OnUpdateMRUFile(CCmdUI *pCmdUI);
	BOOL OpenRecentFile(CDoc *pDoc, UINT nID);
	BOOL IsAnyRecentFile();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CApp)
	afx_msg void OnAppAbout();
	afx_msg void OnClearMRU();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void LoadStdProfileSettings(UINT nMaxMRU = _AFX_MRU_COUNT);
	void ResetRegisterKey();

	BOOL CheckSimple();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUICKQUIZ_H__FA1CD5BC_C553_4600_94ED_11F1F68500BD__INCLUDED_)
