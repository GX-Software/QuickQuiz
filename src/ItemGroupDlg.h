#if !defined(AFX_ITEMGROUPDLG_H__C7D1E6AA_5185_442F_9F95_3750B56EE420__INCLUDED_)
#define AFX_ITEMGROUPDLG_H__C7D1E6AA_5185_442F_9F95_3750B56EE420__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItemGroupDlg.h : header file
//

class CEditFrame;
class CImageFrame;

/////////////////////////////////////////////////////////////////////////////
// CItemGroupDlg dialog

class CItemGroupDlg : public CClrDialog
{
// Construction
public:
	CItemGroupDlg(CList *pList, CPaper *pPaper, CWnd* pParent = NULL);   // standard constructor
	virtual ~CItemGroupDlg();

	// 放在列表中便于管理，也便于引用
	inline CList* GetItemGroup() { return m_pList; }
	BOOL IsEdited() { return m_bEdited; }

// Dialog Data
	//{{AFX_DATA(CItemGroupDlg)
	enum { IDD = IDD_ITEMGROUP_DLG };
	CQEdit	m_cEditNumber;
	CQButton	m_cBtnResImage;
	CQButton	m_cBtnDescImage;
	CQButton	m_cBtnCopy;
	CQGroupBox	m_cGroup3;
	CQButton	m_cBtnText;
	CQButton	m_cBtnResolve;
	CQButton	m_cBtnDesc;
	CQEdit	m_cEditResolve;
	CQButton	m_cBtnDelG;
	CQButton	m_cBtnNewG;
	CQButton	m_cBtnNext;
	CQButton	m_cBtnPre;
	CQButton	m_cBtnPaste;
	CQButton	m_cBtnOk;
	CQGroupBox	m_cGroup2;
	CQGroupBox	m_cGroup1;
	CListCtrl	m_cList;
	CQEdit	m_cEditItemGrpName;
	CQEdit	m_cEditDesc;
	CQButton	m_cBtnUp;
	CQButton	m_cBtnNew;
	CQButton	m_cBtnEdit;
	CQButton	m_cBtnDown;
	CQButton	m_cBtnDel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemGroupDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CItemGroupDlg)
	virtual void OnOK() {}
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonDesc();
	afx_msg void OnButtonResolve();
	afx_msg void OnButtonPre();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonNewG();
	afx_msg void OnButtonDelG();
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();
	afx_msg void OnButtonNew();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonPaste();
	afx_msg void OnButtonText();
	afx_msg void OnButtonOk();
	afx_msg void OnDblClkListGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditChange();
	afx_msg void OnListGroupChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonCopy();
	afx_msg void OnSetDescImage();
	afx_msg void OnSetResImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	/*
	由于微软的反人类设计，程序控制改变ListCtrl也会发出通告
	故此SetShow可能会多次执行，加bRefreshList用于避免在程控改变时多次执行
	*/
	void SetShow();
	BOOL SaveCurrent();
	LPTSTR GetEditFrmSel();

	CEditFrame	*m_pEditWnd;

	CImageFrame	*m_pImageWnd;
	CDataList	m_ImageList;

	BOOL		m_bEdited; // 发生任意改动都会变TRUE
	BOOL		m_bModify; // 单个题目改动变TRUE，翻动题目后复位

	CList		*m_pList;
	CPaper		*m_pPaper;
	int			m_nCurShow;
	BOOL		m_bIsNew;

	CImageList	m_cImageList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMGROUPDLG_H__C7D1E6AA_5185_442F_9F95_3750B56EE420__INCLUDED_)
