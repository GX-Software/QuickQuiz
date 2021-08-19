// TabPage.h: interface for the CTabPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TABPAGE_H__EAEDC1DC_F8D9_43ED_9E2F_83CDDB460BB7__INCLUDED_)
#define AFX_TABPAGE_H__EAEDC1DC_F8D9_43ED_9E2F_83CDDB460BB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClrDialog.h"

class CTabPage : public CClrDialog
{
public:
	CTabPage(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CTabPage();

	BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);

	virtual void OnSetActive() {}
	virtual BOOL OnKillActive(BOOL bIsDestroy) { return TRUE; }
	virtual void SetShow(const CItem *pItem) { ASSERT(FALSE); }

	virtual BOOL IsModify() { return (m_bModify); }
	virtual int CheckSaveable() { ASSERT(FALSE); return 0; }

protected:
	// Generated message map functions
	//{{AFX_MSG(CTabPage)
	virtual void OnOK() {}
	virtual void OnCancel() {}
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	HRESULT EnableDialogTexture();

	BOOL m_bModify;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABPAGE_H__EAEDC1DC_F8D9_43ED_9E2F_83CDDB460BB7__INCLUDED_)
