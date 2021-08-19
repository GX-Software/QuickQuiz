#if !defined(AFX_ALLBUILDDLG_H__D297F72A_0074_4E31_AD3A_19B8D95D933E__INCLUDED_)
#define AFX_ALLBUILDDLG_H__D297F72A_0074_4E31_AD3A_19B8D95D933E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AllBuildDlg.h : header file
//

#include "ClrDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CAllBuildDlg dialog

class CAllBuildDlg : public CClrDialog
{
// Construction
public:
	CAllBuildDlg(LPTSTR pText, int nType, CList *pAddList, CWnd* pParent = NULL);   // standard constructor
	~CAllBuildDlg();

	inline CList* GetList() { return &m_List; }
	inline int GetMakeAnswerCount() { return m_nMakeAnswerCount; }
	inline int GetStopPos() { return m_nStopPos; }

// Dialog Data
	//{{AFX_DATA(CAllBuildDlg)
	enum { IDD = IDD_ALLBUILD_DLG };
	CQEdit	m_cEditTips;
	CQButton	m_cBtnNumSep;
	CQComboBox	m_cCombo;
	CQGroupBox	m_cGroup;
	CQButton	m_cBtnHandleIndex;
	CQButton	m_cBtnDetectAswAfterQ;
	CQButton	m_cBtnDetectAswInQ;
	CQButton	m_cBtnAutoSkip;
	CQButton	m_cBtnSave;
	BOOL	m_bHandleIndex;
	BOOL	m_bAutoSkip;
	BOOL	m_bDetectAswInQ;
	BOOL	m_bDetectAswAfterQ;
	BOOL	m_bNumSep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAllBuildDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAllBuildDlg)
	virtual void OnOK() {}
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	afx_msg void OnCheckDetectAswInQ();
	afx_msg void OnCheckDetectAswAfterQ();
	afx_msg void OnSelChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetOptionsCheckBoxes();

	void MakeList();
	int GetRealIndex();

	int JumpSpaces(LPCWSTR pWText);
	BOOL GetLines(int nType, LPCWSTR pWText, BOOL bMarkLastLine, size_t *uStart, size_t *uLen);

	BOOL CheckNumIndex(LPCWSTR pWText, int *nJump);
	BOOL CheckOptionTitle(LPCWSTR pWText, int *nJump);

	int CheckQuote(int nType, LPWSTR pWText, size_t uLen, DWORD *dwAnswer);
	BOOL DetectU32AnswerInQ(CItem *pItem, LPWSTR strDescription);
	BOOL DetectU32AnswerAfterQ(CItem *pItem, LPWSTR pWText, size_t uLen);

	BOOL DetectTextAnswerInQ(CItem *pItem, LPWSTR strWDescription, BOOL bDefaultBlank = FALSE);
	BOOL DetectTextAnswerAfterQ(CItem *pItem, LPCWSTR pWText, size_t uLen);

protected:
	LPTSTR m_pText;
	LPWSTR m_pWText;
	LPWSTR m_strWDescription;
	int m_nWDesLen;

	int m_nType;
	CList *m_pAddList;

	CList m_List;
	int m_nMakeAnswerCount;

	int m_nStopPos;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALLBUILDDLG_H__D297F72A_0074_4E31_AD3A_19B8D95D933E__INCLUDED_)
