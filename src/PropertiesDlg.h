#if !defined(AFX_PROPERTIESDLG_H__0E2AA9F1_92A9_4C0C_B594_041604F1BD06__INCLUDED_)
#define AFX_PROPERTIESDLG_H__0E2AA9F1_92A9_4C0C_B594_041604F1BD06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaperPropertiesDlg.h : header file
//

#include "ClrDialog.h"
#include "ItemShowWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CPaperPropertiesDlg dialog

class CPaperPropertiesDlg : public CClrDialog
{
// Construction
public:
	CPaperPropertiesDlg(CWnd* pParent = NULL);   // standard constructor

	inline void SetPaper(CPaper *pPaper, BOOL bShowStoreList)
		{ m_pPaper = pPaper; m_bShowStoreList = bShowStoreList; }

// Dialog Data
	//{{AFX_DATA(CPaperPropertiesDlg)
	enum { IDD = IDD_PAPERPROP_DLG };
	CQGroupBox	m_cGroup;
	CQButton	m_cBtnOK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaperPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPaper *m_pPaper;
	BOOL m_bShowStoreList;

	// Generated message map functions
	//{{AFX_MSG(CPaperPropertiesDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTypePropertiesDlg dialog

class CTypePropertiesDlg : public CClrDialog
{
	// Construction
public:
	CTypePropertiesDlg(CPaper *pPaper, CWnd* pParent = NULL);   // standard constructor
	
	void SetType(CList *pList) { m_pList = pList; }
	
	// Dialog Data
	//{{AFX_DATA(CTypePropertiesDlg)
	enum { IDD = IDD_TYPEPROP_DLG };
	CQEdit	m_cEditNumber;
	CQButton	m_cBtnOK;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTypePropertiesDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	CPaper *m_pPaper;
	CList *m_pList;
	
	// Generated message map functions
	//{{AFX_MSG(CTypePropertiesDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillFocusQNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CItemPropertiesDlg dialog

class CImageFrame;

class CItemPropertiesDlg : public CClrDialog
{
	// Construction
public:
	CItemPropertiesDlg(CTempList *pList, int nInitShow = 0, CWnd* pParent = NULL);   // standard constructor
	virtual ~CItemPropertiesDlg();
	
	// Dialog Data
	//{{AFX_DATA(CItemPropertiesDlg)
	enum { IDD = IDD_ITEMPROP_DLG };
	CQComboBox	m_cComboFont;
	CQButton	m_cBtnStore;
	CQButton	m_cBtnCancel;
	CQGroupBox	m_cGroup1;
	CQButton	m_cBtnPre;
	CQButton	m_cBtnNext;
	CItemShowWnd	m_cShow;
	//}}AFX_DATA
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemPropertiesDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	CTempList *m_pList;
	int m_nCurShow;

protected:
	void SetShow();
	void RefreshStore();
	
	// Generated message map functions
	//{{AFX_MSG(CItemPropertiesDlg)
	virtual void OnOK() {}
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonPre();
	afx_msg void OnButtonNext();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnStore();
	afx_msg void OnSelChangeFontSize();
	//}}AFX_MSG
	afx_msg LRESULT OnItemShowWndNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESDLG_H__0E2AA9F1_92A9_4C0C_B594_041604F1BD06__INCLUDED_)
