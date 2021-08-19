#if !defined(AFX_JPEGCOVERDLG_H__57419B05_14EA_4BCA_AE78_F50E9D656A5D__INCLUDED_)
#define AFX_JPEGCOVERDLG_H__57419B05_14EA_4BCA_AE78_F50E9D656A5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JpegCoverDlg.h : header file
//

#include "MemBmp.h"

class CImageFrame;

/////////////////////////////////////////////////////////////////////////////
// CJpegCoverDlg dialog

class CJpegCoverDlg : public CClrDialog
{
// Construction
public:
	CJpegCoverDlg(CPaper *pPaper, CWnd* pParent = NULL);   // standard constructor
	~CJpegCoverDlg();

	inline BOOL IsModify() { return m_bModify; }
	LPPAPERCOVERINFO GetCoverInfo();

// Dialog Data
	//{{AFX_DATA(CJpegCoverDlg)
	enum { IDD = IDD_JPEGCOVER_DLG };
	CQEdit	m_cEditQuality;
	CQButton	m_cBtnView;
	CQColorSetter	m_cTxtColor;
	CQColorSetter	m_cBkColor;
	CQButton	m_cRadioNo;
	CQButton	m_cBtnOK;
	CQButton	m_cBtnCancel;
	CQButton	m_cRadioDefault;
	CQButton	m_cRadioUser;
	CQEdit	m_cEditWidth;
	CQEdit	m_cEditTitle;
	CQEdit	m_cEditJpegChoose;
	CQEdit	m_cEditHeight;
	CQButton	m_cBtnOpen;
	int		m_nRadioType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJpegCoverDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_strTitle;
	int m_nWidth;
	int m_nHeight;
	COLORREF m_clrBk;
	COLORREF m_clrTxt;
	int m_nQuatlity;
	CString m_strJpegPath;

	CPaper *m_pPaper;
	BOOL m_bModify;
	PAPERCOVERINFO m_inf;

	CImageFrame *m_pImgFrame;
	CMemBmp m_Bmp;

	// Generated message map functions
	//{{AFX_MSG(CJpegCoverDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnOpen();
	afx_msg void OnView();
	afx_msg void OnSetBkColor();
	afx_msg void OnSetTxtColor();
	afx_msg void OnRadio();
	afx_msg void OnChangeEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPEGLIBDLG_H__57419B05_14EA_4BCA_AE78_F50E9D656A5D__INCLUDED_)
