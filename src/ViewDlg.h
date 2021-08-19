#if !defined(AFX_VIEWDLG_H__616BFA5A_3D1F_4852_ACC6_3C2997BBDCCD__INCLUDED_)
#define AFX_VIEWDLG_H__616BFA5A_3D1F_4852_ACC6_3C2997BBDCCD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewDlg.h : header file
//
#include "ClrDialog.h"
#include "ChooseTypeDlg.h"

#include "ItemShowWnd.h"

class CListFrame;

/////////////////////////////////////////////////////////////////////////////
// CViewDlg dialog

class CViewDlg : public CClrDialog
{
// Construction
public:
	CViewDlg(CPaper *pPaper, CWnd* pParent = NULL);   // standard constructor
	~CViewDlg();

	void SetCurSel(int nIndex);

// Dialog Data
	//{{AFX_DATA(CViewDlg)
	enum { IDD = IDD_VIEW_DLG };
	CQComboBox	m_cComboFont;
	CQButton	m_cCheckAll;
	CQGroupBox	m_cGroup1;
	CQButton	m_cBtnCancel;
	CQButton	m_cBtnSave;
	CQButton	m_cBtnPre;
	CQButton	m_cBtnOption;
	CQButton	m_cBtnNext;
	CItemShowWnd	m_cShow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewDlg)
	virtual void OnOK() {}
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonPre();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonOption();
	afx_msg void OnSelChangeFontSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int GetMaxStep();
	void FindNext();
	void SetShow();

protected:
	CPaper *m_pPaper;
	CTempList m_List;	// 用于寻找下一道题目
	int m_nCurShow;

	int m_nCurStep;
	int m_nMaxStep;

	CListFrame *m_pListWnd;

	TYPEPARA m_Para;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWDLG_H__616BFA5A_3D1F_4852_ACC6_3C2997BBDCCD__INCLUDED_)
