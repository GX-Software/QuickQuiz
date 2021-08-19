#if !defined(AFX_IMAGEBAR_H__B98CB020_4558_4F16_95F9_0FD7647EF97D__INCLUDED_)
#define AFX_IMAGEBAR_H__B98CB020_4558_4F16_95F9_0FD7647EF97D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageBar.h : header file
//

#define WM_IMAGEBARNOTIFY		(WM_USER + 1)

enum ImageBarNodify {
	IMAGEBARNOTIFY_PRE = 0,
	IMAGEBARNOTIFY_NEXT,
	IMAGEBARNOTIFY_NEW,
	IMAGEBARNOTIFY_SET,
	IMAGEBARNOTIFY_DELETE
};

/////////////////////////////////////////////////////////////////////////////
// CImageBar dialog

class CImageBar : public CDialogBar
{
// Construction
public:
	CImageBar(CWnd* pParent = NULL);   // standard constructor
	~CImageBar();

	BOOL CreateBar(CWnd* pParentWnd, UINT uID);

	int GetDefaultWidth() { return m_nWidth; }
	int GetDefaultHeight() { return m_nHeight; }
	void SetStatus(size_t ulCount, size_t ulCur, size_t ulMax, LPCTSTR lpTitle);

	void GetImageTitle(CString &string);
	BOOL IsModified();

// Dialog Data
	//{{AFX_DATA(CImageBar)
	enum { IDD = IDD_IMAGESET_BAR };
	CQEdit	m_cEditTitle;
	CQButton	m_cBtnDelete;
	CQButton	m_cBtnSet;
	CQButton	m_cBtnNew;
	CQButton	m_cBtnPre;
	CQButton	m_cBtnNext;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nWidth;
	int m_nHeight;

	BOOL m_bTitleModify;

	// Generated message map functions
	//{{AFX_MSG(CImageBar)
	virtual void OnOK() {}
	virtual void OnCancel() {}
	virtual BOOL OnInitDialog(UINT wParam, LONG lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPre();
	afx_msg void OnNext();
	afx_msg void OnNew();
	afx_msg void OnSet();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEBAR_H__B98CB020_4558_4F16_95F9_0FD7647EF97D__INCLUDED_)
