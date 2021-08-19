#if !defined(AFX_QPROGRESSCTRL_H__CC75B177_B8BF_412A_A0BC_943438012B86__INCLUDED_)
#define AFX_QPROGRESSCTRL_H__CC75B177_B8BF_412A_A0BC_943438012B86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QProgressCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQProgressCtrl window

typedef void (FUNCCALL *pSetText)(CWnd *pParent, CString &string);

class CQProgressCtrl : public CProgressCtrl
{
// Construction
public:
	CQProgressCtrl(pSetText pFunc = NULL);

	typedef enum _ALIGN_TEXT_
	{
		ALIGN_LEFT = 0x00,
		ALIGN_CENTER,
		ALIGN_RIGHT
	}ALIGN_TEXT;

// Attributes
public:

// Operations
public:
	inline void SetBarColor(COLORREF clr) { m_clrBarColor = clr; }
	inline COLORREF GetBarColor() { return m_clrBarColor; }
	inline void SetBarBkColor(COLORREF clr) { m_clrBarBkColor = clr; }
	inline COLORREF GetBarBkColor() { return m_clrBarBkColor; }
	inline void SetTextColor(COLORREF clr) { m_clrTextColor = clr; }
	inline COLORREF GetTextColor() { return m_clrTextColor; }
	inline void SetTextBkColor(COLORREF clr) { m_clrTextBkColor = clr; }
	inline COLORREF GetTextBkColor() { return m_clrTextBkColor; }

	inline void SetAlarm1Color(COLORREF clr) { m_clrAlarm1Color = clr; }
	inline COLORREF GetAlarm1Color() { return m_clrAlarm1Color; }
	inline void SetAlarm1TextBkColor(COLORREF clr) { m_clrAlarm1TextBkColor = clr; }
	inline COLORREF GetAlarm1TextBkColor() { return m_clrAlarm1TextBkColor; }
	inline void SetAlarm2Color(COLORREF clr) { m_clrAlarm2Color = clr; }
	inline COLORREF GetAlarm2Color() { return m_clrAlarm2Color; }
	inline void SetAlarm2TextBkColor(COLORREF clr) { m_clrAlarm2TextBkColor = clr; }
	inline COLORREF GetAlarm2TextBkColor() { return m_clrAlarm2TextBkColor; }
	
	inline void SetTextAlign(ALIGN_TEXT AlignText) { m_AlignText = AlignText; }
	inline ALIGN_TEXT GetTextAlign() { return m_AlignText; }
	inline void ShowPercent(BOOL bShowPercent = TRUE) { m_bShowPercent = bShowPercent; }
	inline BOOL IsShowPercent() { return m_bShowPercent; }

	inline void SetAlarm1Percent(float fAlarm)
	{
		m_fAlarm1Percent = fAlarm;
	}
	inline void SetAlarm2Percent(float fAlarm)
	{
		m_fAlarm2Percent = fAlarm;
		if (m_fAlarm1Percent < fAlarm)
		{
			m_fAlarm1Percent = fAlarm;
		}
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQProgressCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQProgressCtrl();

protected:
	COLORREF m_clrBarColor;
	COLORREF m_clrBarBkColor;
	COLORREF m_clrTextColor;
	COLORREF m_clrTextBkColor;
	COLORREF m_clrAlarm1Color;
	COLORREF m_clrAlarm1TextBkColor;
	COLORREF m_clrAlarm2Color;
	COLORREF m_clrAlarm2TextBkColor;
	ALIGN_TEXT m_AlignText;
	BOOL m_bShowPercent;
	CFont m_font; // Only for vertical style

	float m_fAlarm1Percent;
	float m_fAlarm2Percent;

	pSetText m_fpSetText;

	// Generated message map functions
protected:
	//{{AFX_MSG(CQProgressCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPROGRESSCTRL_H__CC75B177_B8BF_412A_A0BC_943438012B86__INCLUDED_)
