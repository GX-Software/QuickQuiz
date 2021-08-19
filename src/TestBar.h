#if !defined(AFX_TESTBAR_H__8479DF06_955A_46B7_8219_6BA2F069FE0F__INCLUDED_)
#define AFX_TESTBAR_H__8479DF06_955A_46B7_8219_6BA2F069FE0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestBar.h : header file
//

#include "QProgressCtrl.h"
#include "ItemShowWnd.h"

#define WM_TESTBARNOTIFY		(WM_USER + 3)

#define TBN_TIMEOUT				0
#define TBN_USERHANDIN			1
#define TBN_MARKITEM			2
#define TBN_TOPMOST				3		// lParam：是否要求总在最前
#define TBN_SETFONTSIZE			4		// lParam：字体信息
#define TBN_SAVEITEM			5
#define TBN_ONLYFALSE			6
#define TBN_SHOWSINGLE			7
#define TBN_PRE					8
#define TBN_NEXT				9

// 字体信息
#define TBP_SMALLFONT			0
#define TBP_NORMALFONT			1
#define TBP_LARGEFONT			2
#define TBP_GIANTFONT			3

#define TESTBARTYPE_TEST		0
#define TESTBARTYPE_TESTVIEW	1

/////////////////////////////////////////////////////////////////////////////
// CTestBar dialog

class CTestBar : public CDialogBar
{
// Construction
public:
	CTestBar(CWnd* pParent = NULL);   // standard constructor

	void SetTime(long lTime);
	void SetType(int nType);
	void UpdateState(CItem *pItem);
	void UpdateList(int nWrongCount);
	void SetFontSize(long lFont);

// Dialog Data
	//{{AFX_DATA(CTestBar)
	enum { IDD = IDD_TEST_BAR };
	CQButton	m_cBtnPre;
	CQButton	m_cBtnNext;
	CQButton	m_cCheckSingle;
	CQButton	m_cCheckOnlyFalse;
	CQButton	m_cBtnSave;
	CQComboBox	m_cComboFont;
	CQProgressCtrl	m_cProgress;
	CQButton	m_cCheckTopMost;
	CQButton	m_cBtnMark;
	CQButton	m_cBtnHandIn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	long m_lTime;
	BOOL m_bNoTimeLimit;

	// Generated message map functions
	//{{AFX_MSG(CTestBar)
	virtual void OnOK() {}
	virtual void OnCancel() {}
	virtual BOOL OnInitDialog(UINT wParam, LONG lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHandIn();
	afx_msg void OnMark();
	afx_msg void OnTopMost();
	afx_msg void OnSelChangeFontSize();
	afx_msg void OnSave();
	afx_msg void OnOnlyFalse();
	afx_msg void OnShowSingle();
	afx_msg void OnButtonPre();
	afx_msg void OnButtonNext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	static void FUNCCALL SetProgressText(CWnd *pDlg, CString &string);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTBAR_H__8479DF06_955A_46B7_8219_6BA2F069FE0F__INCLUDED_)
