#if !defined(AFX_IMAGEFRAME_H__C8F09536_CD1A_4892_8147_6F6A2A8B6BDB__INCLUDED_)
#define AFX_IMAGEFRAME_H__C8F09536_CD1A_4892_8147_6F6A2A8B6BDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageFrame.h : header file
//

#include "MemBmp.h"
#include "ImageBar.h"

/*
�����ڱ���ʹ�ø����ڽ�������
�����ڿ��Թر�ʱ��ʵ��Ϊ����
*/

enum {
	IMAGEDRAW_DDB = 0,
	IMAGEDRAW_JPEG
};

class CMemBmp;
class CDataList;

/////////////////////////////////////////////////////////////////////////////
// CImageFrame window

class CImageFrame : public CFrameWnd
{
// Construction
public:
	CImageFrame();

// Attributes
public:

// Operations
public:
	BOOL CreateFrame(LPCTSTR lpszWindowName, CWnd* pParentWnd = NULL, const RECT& rect = CFrameWnd::rectDefault, DWORD dwExStyle = 0);

	/* �����ô˺���ʱ����ζ�ſ������ͼƬ������ʾ������ */
	void SetImageList(CDataList *pList, int nInitShow);
	BOOL IsModified();

	/* �����ô˺���ʱ����ζ��ֻ��ʾһ�ŷ���ͼ������ʾ������ */
	void ShowCover(CMemBmp *pBmp);

	/* ���浱ǰҳ�����Ϣ */
	void Save();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageFrame)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImageFrame();

	// Generated message map functions
protected:
	//{{AFX_MSG(CImageFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg LRESULT ImageBarNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	void SetShow();
	void DrawImage(PITEMIMAGE pImage);
	void DrawInitial(UINT uID);

	BOOL LoadImage(BOOL bAdd);
	BOOL DeleteImage();

protected:
	BOOL m_bModify;
	BOOL m_bShowBar;

	int m_nDrawType;
	CMemBmp m_Bmp;

	CDataList *m_pImageList;
	int m_nCurShow;
	CString m_strTitle;

	CImageBar m_cBar;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEFRAME_H__C8F09536_CD1A_4892_8147_6F6A2A8B6BDB__INCLUDED_)
