// WordByWord.cpp : implementation file
//

#include "stdafx.h"
#include "WordByWord.h"

#include "MemBmp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RETURNLISTINCREASE		16

/////////////////////////////////////////////////////////////////////////////
// CWordByWord

CWordByWord::CWordByWord() :
	m_strText(NULL),
	m_rcClient(0, 0, 0, 0), m_nHeight(16),
	m_ReturnIndex(NULL), m_nReturnListCount(0), m_nLineCount(0),
	m_nMaxLineCount(0), m_nFirstShowIndex(0), m_nCurIndex(0), m_nCurLine(0),
	m_nCurLineFirst(0),
	m_bIsShowing(FALSE), m_bIsEnd(FALSE)
{
}

CWordByWord::~CWordByWord()
{
	if (m_strText)
	{
		free(m_strText);
	}

	if (m_ReturnIndex)
	{
		free(m_ReturnIndex);
	}
}


BEGIN_MESSAGE_MAP(CWordByWord, CWnd)
	//{{AFX_MSG_MAP(CWordByWord)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWordByWord message handlers

void CWordByWord::PreSubclassWindow() 
{
	GetClientRect(m_rcClient);
	
	CWnd::PreSubclassWindow();
}

void CWordByWord::SetText(LPCTSTR string)
{
	GetClientRect(m_rcClient);

	if (m_strText)
	{
		free(m_strText);
		m_strText = NULL;
	}

	m_strText = _tcsdup(string);

	ZeroMemory(m_ReturnIndex, m_nReturnListCount * sizeof(int));
	m_nLineCount = 0;

	HDC dc = ::GetDC(m_hWnd);
	HFONT hOldFont = (HFONT)SelectObject(dc, (HFONT)SendMessage(WM_GETFONT, 0, 0));

	int nLineSize = 0;
	SIZE Size;
	LPTSTR ptr = m_strText;
	int nLen;
	while(1)
	{
		nLen = _tcscspn(ptr, CTextManager::s_szWinReturn);

		GetTextExtentExPoint(dc, ptr, nLen,
			m_rcClient.Width(), &nLineSize, NULL, &Size);

		m_nLineCount++;
		InCreateReturnList(m_nLineCount);

		while(*(ptr + nLineSize) == _T('\r') || *(ptr + nLineSize) == _T('\n'))
		{
			nLineSize++;
		}
		m_ReturnIndex[m_nLineCount - 1] = nLineSize;

		// 到达末尾了
		if (!ptr[nLineSize])
		{
			break;
		}

		ptr += nLineSize;
	}
	m_nHeight = Size.cy;

	SelectObject(dc, hOldFont);
	::ReleaseDC(m_hWnd, dc);

	m_nFirstShowIndex = 0;
	m_nCurIndex = 0;
	m_nCurLine = 0;
	m_nCurLineFirst = 0;

	m_bIsShowing = FALSE;
	m_bIsEnd = FALSE;
	Invalidate(FALSE);
}

void CWordByWord::Show(int nInter, int nMaxLineCount /* = 0 */)
{
	if (m_bIsShowing)
	{
		return;
	}

	m_nMaxLineCount = m_rcClient.Height() / m_nHeight;
	if (nMaxLineCount > 0 && nMaxLineCount < m_nMaxLineCount)
	{
		m_nMaxLineCount = nMaxLineCount;
	}

	m_bIsShowing = TRUE;
	m_bIsEnd = FALSE;
	SetTimer(1, nInter, NULL);
}

void CWordByWord::Stop()
{
	if (!m_bIsShowing)
	{
		return;
	}

	m_bIsShowing = FALSE;
	KillTimer(1);
}

void CWordByWord::OnPaint() 
{
	CPaintDC paintDC(this);
	HDC hPaintDC = paintDC.GetSafeHdc();
	
	CMemBmp Mem;
	HDC dc = Mem.CreateBmp(hPaintDC, m_rcClient);
	ASSERT(dc);
	
	// 开始绘制了
	HBRUSH brush = (HBRUSH)GetParent()->SendMessage(WM_CTLCOLORSTATIC, (WPARAM)dc, (LPARAM)m_hWnd);
	FillRect(dc, m_rcClient, brush);
	
	HFONT hFont = (HFONT)SendMessage(WM_GETFONT, 0, 0);
	if (!hFont)
	{
		hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	}
	HFONT hOldFont = (HFONT)SelectObject(dc, hFont);

	DisplayText(dc);
	SelectObject(dc, hOldFont);

	Mem.Draw(hPaintDC, m_rcClient.left, m_rcClient.top);
}

void CWordByWord::DisplayText(HDC dc)
{
	if (!m_strText)
	{
		return;
	}

	int i = max(0, m_nCurLine - m_nMaxLineCount + 1), y = m_rcClient.top;
	int nAll = m_nFirstShowIndex;
	LPTSTR ptr = m_strText + m_nFirstShowIndex;

	for (; i < m_nLineCount; i++)
	{
		if (m_nCurIndex >= nAll + m_ReturnIndex[i])
		{
			TextOut(dc, m_rcClient.left, y, ptr, m_ReturnIndex[i]);
		}
		else
		{
			TextOut(dc, m_rcClient.left, y, ptr, m_nCurIndex - nAll);
			break;
		}
		ptr += m_ReturnIndex[i];
		nAll += m_ReturnIndex[i];
		y += m_nHeight;
	}
}

void CWordByWord::InCreateReturnList(int nNeedLen)
{
	BOOL bIncrease = FALSE;
	while(m_nReturnListCount < nNeedLen)
	{
		m_nReturnListCount += RETURNLISTINCREASE;
		bIncrease = TRUE;
	}

	if (bIncrease)
	{
		m_ReturnIndex = (int*)realloc(m_ReturnIndex, sizeof(int) * m_nReturnListCount);
		ZeroMemory(m_ReturnIndex, m_nReturnListCount * sizeof(int));
	}
}

void CWordByWord::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case 1:
		if (!SetNextWord())
		{
			m_bIsShowing = FALSE;
			m_bIsEnd = TRUE;
			KillTimer(1);

			NMHDR nmh;
			nmh.hwndFrom = m_hWnd;
			nmh.idFrom = GetDlgCtrlID();
			nmh.code = WWN_SHOWEND;

			GetParent()->SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
		}
		Invalidate(FALSE);
		break;
	}
	
	CWnd::OnTimer(nIDEvent);
}

BOOL CWordByWord::SetNextWord()
{
	if (!m_strText[m_nCurIndex])
	{
		return FALSE;
	}

#ifdef _UNICODE
	m_nCurIndex++;
#else
	if (m_strText[m_nCurIndex] & 0x80)
	{
		m_nCurIndex++;
	}
	m_nCurIndex++;
#endif

	while(m_strText[m_nCurIndex] == _T('\r') || m_strText[m_nCurIndex] == _T('\n') ||
		  m_strText[m_nCurIndex] == _T(' '))
	{
		m_nCurIndex++;
	}

	// 当前行显示完了
	if (m_nCurIndex > m_nCurLineFirst + m_ReturnIndex[m_nCurLine])
	{
		m_nCurLineFirst += m_ReturnIndex[m_nCurLine];
		m_nCurLine++;
		if (m_nCurLine >= m_nMaxLineCount)
		{
			m_nFirstShowIndex += m_ReturnIndex[m_nCurLine - m_nMaxLineCount];
		}
	}

	return TRUE;
}
