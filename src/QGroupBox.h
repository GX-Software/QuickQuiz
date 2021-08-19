#if !defined(AFX_XPGROUPBOX_H__F70D755B_9C4B_4F4A_A1FB_AFF720C29717__INCLUDED_)
#define AFX_XPGROUPBOX_H__F70D755B_9C4B_4F4A_A1FB_AFF720C29717__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XPGroupBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQGroupBox window

class CQGroupBox : public CButton
{
	DECLARE_DYNAMIC(CQGroupBox);

// Construction
public:
	CQGroupBox();

	enum QGroupBoxStyle
		{ XPGB_FRAME,  
		  XPGB_WINDOW};

// Attributes
public:

// Operations
public:
   virtual CQGroupBox& SetBorderColor(COLORREF clrBorder);
   virtual CQGroupBox& SetCatptionTextColor(COLORREF clrText);
   virtual CQGroupBox& SetBackgroundColor(COLORREF clrBKClient);
   virtual CQGroupBox& SetBackgroundColor(COLORREF clrBKTilte,  COLORREF clrBKClient);
   virtual CQGroupBox& SetQGroupStyle(QGroupBoxStyle eStyle); 
    
   virtual CQGroupBox& SetText(LPCTSTR lpszTitle);
   virtual CQGroupBox& SetFontBold(BOOL bBold);
   virtual CQGroupBox& SetFontName(const CString& strFont, BYTE byCharSet = ANSI_CHARSET);
   virtual CQGroupBox& SetFontUnderline(BOOL bSet);
   virtual CQGroupBox& SetFontItalic(BOOL bSet);
   virtual CQGroupBox& SetFontSize(int nSize);
   virtual CQGroupBox& SetFont(LOGFONT lf);

   virtual CQGroupBox& SetAlignment(DWORD dwType);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQGroupBox)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQGroupBox();

	// Generated message map functions
protected:
	void UpdateSurface();
	void ReconstructFont();
	//{{AFX_MSG(CQGroupBox)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CString		m_strTitle;

	COLORREF	m_clrBorder;
	COLORREF	m_clrTitleBackground;
	COLORREF	m_clrClientBackground;
	COLORREF	m_clrTitleText;
	
	QGroupBoxStyle		m_nType;
	DWORD       m_dwAlignment;  

	LOGFONT			m_lf;
	CFont			m_font;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPGROUPBOX_H__F70D755B_9C4B_4F4A_A1FB_AFF720C29717__INCLUDED_)
