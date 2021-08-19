// MemBmp.cpp: implementation of the CMemBmp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "quickquiz.h"
#include "MemBmp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemBmp::CMemBmp() :
	m_hDC(NULL), m_hBitmap(NULL), m_hOldBitmap(NULL)
{

}

CMemBmp::~CMemBmp()
{
	ClearBmp();
}

RETURN_E HDC CMemBmp::CreateBmp(HDC hDC, LPRECT Rect, BOOL bInitClr /* = FALSE */, COLORREF clr /* = 0 */)
{
	if (!hDC)
	{
		return Q_RET(NULL);
	}
	m_rcClient = Rect;

	ClearBmp();
	m_hDC = CreateCompatibleDC(hDC);
	if (!m_hDC)
	{
		return Q_ERROR_RET(ERROR_CREATE_DC_FAIL, NULL);
	}

	m_hBitmap = CreateCompatibleBitmap(hDC, m_rcClient.Width(), m_rcClient.Height());
	if (!m_hBitmap)
	{
		return Q_ERROR_RET(ERROR_CREATE_DDB_FAIL, NULL);
	}

	m_hOldBitmap = (HBITMAP)SelectObject(m_hDC, m_hBitmap);
	if (bInitClr)
	{
		FillSolidRect(clr);
	}

	return Q_TRUE_RET(m_hDC);
}

RETURN_E HDC CMemBmp::CreateBmp(HDC hDC, int nWidth, int nHeight, BOOL bInitClr /* = FALSE */, COLORREF clr /* = 0 */)
{
	RECT rc = {0, 0, nWidth, nHeight};
	return CreateBmp(hDC, &rc, bInitClr, clr);
}

RETURN_E HDC CMemBmp::CreateBmp(HDC hDC, HBITMAP hBmp)
{
	BITMAP bitmap = {0};
	HDC hPaintDC;
	HBITMAP hOldBitmap;

	if (!hDC || !hBmp)
	{
		return Q_RET(NULL);
	}

	if (!GetObject(hBmp, sizeof(BITMAP), &bitmap))
	{
		return Q_ERROR_RET(ERROR_CREATE_DDB_FAIL, NULL);
	}
	m_rcClient.SetRect(0, 0, bitmap.bmWidth, bitmap.bmHeight);
	
	ClearBmp();
	m_hDC = CreateCompatibleDC(hDC);
	if (!m_hDC)
	{
		return Q_ERROR_RET(ERROR_CREATE_DC_FAIL, NULL);
	}
	
	m_hBitmap = CreateCompatibleBitmap(hDC, m_rcClient.Width(), m_rcClient.Height());
	if (!m_hBitmap)
	{
		return Q_ERROR_RET(ERROR_CREATE_DDB_FAIL, NULL);
	}
	
	m_hOldBitmap = (HBITMAP)SelectObject(m_hDC, m_hBitmap);

	hPaintDC = CreateCompatibleDC(hDC);
	if (!hPaintDC)
	{
		return Q_ERROR_RET(ERROR_CREATE_DC_FAIL, NULL);
	}
	hOldBitmap = (HBITMAP)SelectObject(hPaintDC, hBmp);
	BitBlt(m_hDC, 0, 0, m_rcClient.Width(), m_rcClient.Height(), hPaintDC, 0, 0, SRCCOPY);
	
	SelectObject(hPaintDC, hOldBitmap);
	DeleteDC(hPaintDC);

	return Q_TRUE_RET(m_hDC);
}

void CMemBmp::FillSolidRect(COLORREF clr)
{
	HBRUSH hbr = CreateSolidBrush(clr);
	FillRect(m_hDC, m_rcClient, hbr);
	DeleteObject(hbr);
}

BOOL CMemBmp::Draw(HDC hPaintDC, int x, int y)
{
	if (!hPaintDC || !m_hDC)
	{
		return FALSE;
	}

	return BitBlt(hPaintDC, x, y, m_rcClient.Width(), m_rcClient.Height(),
				  m_hDC, 0, 0, SRCCOPY);
}

HDC CMemBmp::CopyBmp(CMemBmp *pBmp)
{
	if (!CreateBmp(pBmp->m_hDC, pBmp->m_rcClient.Width(), pBmp->m_rcClient.Height()))
	{
		return NULL;
	}
	if (!pBmp->Draw(m_hDC, 0, 0))
	{
		return NULL;
	}
	return m_hDC;
}

void CMemBmp::ClearBmp()
{
	if (!m_hDC || !m_hBitmap)
	{
		return;
	}

	ASSERT(m_hOldBitmap);
	DeleteObject(SelectObject(m_hDC, m_hOldBitmap));
	DeleteDC(m_hDC);

	m_hOldBitmap = NULL;
	m_hBitmap = NULL;
	m_hDC = NULL;
}