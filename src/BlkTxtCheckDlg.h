#if !defined(AFX_BLKTXTCHECKDLG_H__2615CBD0_AACC_4257_AFCD_2DE6B072CC28__INCLUDED_)
#define AFX_BLKTXTCHECKDLG_H__2615CBD0_AACC_4257_AFCD_2DE6B072CC28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlkTxtCheckDlg.h : header file
//

#include "ItemShowWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CBlkTxtCheckDlg dialog

typedef struct _tagBlkTxtScore
{
	int nScore;
	BOOL bUser;
}SCOREINFO;

class CBlkTxtCheckDlg : public CClrDialog
{
// Construction
public:
	CBlkTxtCheckDlg(CList *pList, CTestPara *pPara, CWnd* pParent = NULL);   // standard constructor
	~CBlkTxtCheckDlg();

// Dialog Data
	//{{AFX_DATA(CBlkTxtCheckDlg)
	enum { IDD = IDD_BLKTXTCHECK_DLG };
	CQComboBox	m_cComboFont;
	CQButton	m_cBtnCancel;
	CQGroupBox	m_cGroup2;
	CQGroupBox	m_cGroup1;
	CQEdit	m_cEditTxtScore;
	CQButton	m_cBtnPre;
	CQButton	m_cBtnNext;
	CItemShowWnd m_cUser;
	CItemShowWnd m_cReal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlkTxtCheckDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void InitTmpList();
	void SetShow();
	void SaveCurrent();
	float GetSimilarity(CItem *pItem);
	int GetMaxPoint();

// Implementation
protected:
	CTestPara *m_pPara;
	CList *m_pList;

	CTempList m_TmpList;
	int m_nCurShow;
	BOOL *m_pUserSet;

	CTextSimilarity m_cSimilarity;

	// Generated message map functions
	//{{AFX_MSG(CBlkTxtCheckDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonPre();
	afx_msg void OnButtonNext();
	virtual void OnCancel();
	afx_msg void OnKillFocus();
	afx_msg void OnSelChangeFontSize();
	//}}AFX_MSG
	afx_msg LRESULT OnItemShowWndNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLKTXTCHECKDLG_H__2615CBD0_AACC_4257_AFCD_2DE6B072CC28__INCLUDED_)
