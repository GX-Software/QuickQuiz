// MemBmp.h: interface for the CMemBmp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMBMP_H__4DDBBF35_0442_486A_87CF_00AB5C59B68B__INCLUDED_)
#define AFX_MEMBMP_H__4DDBBF35_0442_486A_87CF_00AB5C59B68B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMemBmp  
{
public:
	CMemBmp();
	virtual ~CMemBmp();

	RETURN_E HDC CreateBmp(HDC hDC, LPRECT Rect, BOOL bInitClr = FALSE, COLORREF clr = 0);
	RETURN_E HDC CreateBmp(HDC hDC, int nWidth, int nHeight, BOOL bInitClr = FALSE, COLORREF clr = 0);
	RETURN_E HDC CreateBmp(HDC hDC, HBITMAP hBmp);
	void FillSolidRect(COLORREF clr);
	BOOL Draw(HDC hPaintDC, int x, int y);

	HDC CopyBmp(CMemBmp *pBmp);
	void ClearBmp();

	int GetWidth() { return m_rcClient.Width(); }
	int GetHeight() { return m_rcClient.Height(); }
	HDC GetDC() { return m_hDC; }
	HBITMAP GetBmp() { return m_hBitmap; }

protected:
	HDC m_hDC;
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;

	CRect m_rcClient;
};

#endif // !defined(AFX_MEMBMP_H__4DDBBF35_0442_486A_87CF_00AB5C59B68B__INCLUDED_)
