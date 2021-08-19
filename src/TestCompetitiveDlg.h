#if !defined(AFX_TESTCOMPETITIVEDLG_H__226E7E7E_D6CB_46E7_9AB3_9A0C29E8E774__INCLUDED_)
#define AFX_TESTCOMPETITIVEDLG_H__226E7E7E_D6CB_46E7_9AB3_9A0C29E8E774__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestCompetitiveDlg.h : header file
//

#include "TabPage.h"
#include "ChooseTypeDlg.h"
#include "WordByWord.h"
#include "StopWatch.h"

class CEditFrame;

/////////////////////////////////////////////////////////////////////////////
// CTestCompetitiveDlg dialog

class CTestCompetitiveDlg : public CTabPage
{
// Construction
public:
	CTestCompetitiveDlg(CPaper *pPaper, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTestCompetitiveDlg)
	enum { IDD = IDD_TESTCOMP_DLG };
	CQGroupBox	m_cGroup3;
	CQGroupBox	m_cGroup2;
	CQGroupBox	m_cGroup1;
	CQEdit	m_cEditInter;
	CQButton	m_cCheckShowType;
	CQButton	m_cBtnSave;
	CQButton	m_cBtnPre;
	CQButton	m_cBtnOption;
	CQButton	m_cBtnNext;
	CQButton	m_cBtnNew;
	CWordByWord	m_cShow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestCompetitiveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTestCompetitiveDlg)
	virtual void OnOK() {}
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButtonPre();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonNew();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonOption();
	//}}AFX_MSG
	afx_msg void OnShowEnd(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
	
	CItem* FindNext();
	void InitShow();

	void SetShow();
	BOOL CheckInput();

	void OnStepInit();
	void OnStepWait(UINT nChar);
	void OnStepEnd();
	void OnStepShow();

protected:
	CRect m_rcButton;
	int m_nStep;

	CPaper *m_pPaper;
	CTempList m_List;

	CItem *m_pCurShow;
	int m_nCurShow;
	int m_nNewest; // 未练习过的最新题目

	TYPEPARA m_Para;
	int m_nInter;
	CStopWatch m_Time;
	UINT m_uChar;

	CString m_string;
	CEditFrame *m_pEditWnd;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTCOMPETITIVEDLG_H__226E7E7E_D6CB_46E7_9AB3_9A0C29E8E774__INCLUDED_)
