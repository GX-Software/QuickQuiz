// ClrCommon.cpp : implementation file
//

#include "StdAfx.h"
#include "ClrCommon.h"

#define COLORGREEN				RGB(204, 232, 207)
#define COLORDARK				RGB(25, 25, 25)

#define COLORSILVER				RGB(192, 192, 192)
#define COLORBLACK				RGB(0, 0, 0)
#define COLORDARKTEXT			RGB(128, 128, 128)

#define COLORGREENSHADOW		RGB(173, 218, 174)
#define COLORHIGHLIGHTGREEN		RGB(134, 239, 134)
#define COLORBORDERGREEN		RGB(0, 145, 0)

#define COLORDARKSHADOW			RGB(42, 49, 67)
#define COLORHIGHLIGHTDARK		RGB(75, 81, 190)
#define COLORDARKSELECT			RGB(38, 79, 120)

// »­Ë¢
int g_nColorType = CLRTYPE_DEFAULT;
HBRUSH g_hGreenBrush = NULL;
HBRUSH g_hDarkBrush = NULL;

void FUNCCALL AfxInitGlobalBrush()
{
	g_hGreenBrush = CreateSolidBrush(COLORGREEN);
	g_hDarkBrush = CreateSolidBrush(COLORDARK);
}

void FUNCCALL AfxDestroyGlobalBrush()
{
	if (g_hGreenBrush)
	{
		DeleteObject(g_hGreenBrush);
	}
	if (g_hDarkBrush)
	{
		DeleteObject(g_hDarkBrush);
	}
}

static COLORREF FUNCCALL GetQClrSys(UINT uColorIndex)
{
	int nIndex;
	switch(uColorIndex)
	{
	case QCOLOR_FACE:
		nIndex = COLOR_BTNFACE;
		break;

	case QCOLOR_EDITFACE:
		nIndex = COLOR_WINDOW;
		break;

	case QCOLOR_SHADOW:
		nIndex = COLOR_3DDKSHADOW;
		break;

	case QCOLOR_HIGHLIGHT:
		nIndex = COLOR_BTNHIGHLIGHT;
		break;

	case QCOLOR_BORDER:
		nIndex = COLOR_ACTIVEBORDER;
		break;

	case QCOLOR_TEXT:
	case QCOLOR_TEXTFOCUS:
		nIndex = COLOR_WINDOWTEXT;
		break;
		
	case QCOLOR_TEXTHIGHLIGHT:
		nIndex = COLOR_HIGHLIGHTTEXT;
		break;

	case QCOLOR_TEXTDISABLE:
		nIndex = COLOR_GRAYTEXT;
		break;

	case QCOLOR_TEXTSELECTBACK:
		nIndex = COLOR_HIGHLIGHT;
		break;

	default:
		ASSERT(FALSE);
		return 0;
	}

	return GetSysColor(nIndex);
}

static COLORREF FUNCCALL GetQClrGreen(UINT uColorIndex)
{
	switch(uColorIndex)
	{
	case QCOLOR_FACE:
	case QCOLOR_EDITFACE:
		return COLORGREEN;

	case QCOLOR_SHADOW:
		return COLORGREENSHADOW;

	case QCOLOR_HIGHLIGHT:
		return COLORHIGHLIGHTGREEN;

	case QCOLOR_BORDER:
		return COLORBORDERGREEN;

	case QCOLOR_TEXT:
	case QCOLOR_TEXTFOCUS:
		return COLORBLACK;

	case QCOLOR_TEXTHIGHLIGHT:
		return GetSysColor(COLOR_HIGHLIGHTTEXT);

	case QCOLOR_TEXTDISABLE:
		return GetSysColor(COLOR_GRAYTEXT);

	case QCOLOR_TEXTSELECTBACK:
		return COLORBORDERGREEN;

	default:
		ASSERT(FALSE);
		return 0;
	}
}

static COLORREF FUNCCALL GetQClrDark(UINT uColorIndex)
{
	switch(uColorIndex)
	{
	case QCOLOR_FACE:
		return COLORDARK;

	case QCOLOR_EDITFACE:
		return COLORBLACK;
		
	case QCOLOR_SHADOW:
		return COLORDARKSHADOW;
		
	case QCOLOR_HIGHLIGHT:
		return COLORHIGHLIGHTDARK;
		
	case QCOLOR_BORDER:
		return GetSysColor(COLOR_3DSHADOW);

	case QCOLOR_TEXT:
		return COLORDARKTEXT;

	case QCOLOR_TEXTFOCUS:
	case QCOLOR_TEXTHIGHLIGHT:
		return COLORSILVER;

	case QCOLOR_TEXTDISABLE:
		return GetSysColor(COLOR_GRAYTEXT);

	case QCOLOR_TEXTSELECTBACK:
		return COLORDARKSELECT;
		
	default:
		ASSERT(FALSE);
		return 0;
	}
}

COLORREF FUNCCALL AfxGetQColor(UINT uColorIndex)
{
	switch(g_nColorType)
	{
	case CLRTYPE_DEFAULT:
		return GetQClrSys(uColorIndex);

	case CLRTYPE_GREEN:
		return GetQClrGreen(uColorIndex);

	case CLRTYPE_DARK:
		return GetQClrDark(uColorIndex);

	default:
		return 0;
	}
}

HBRUSH FUNCCALL AfxGetQBrush()
{
	switch(g_nColorType)
	{
	case CLRTYPE_GREEN:
		return g_hGreenBrush;

	case CLRTYPE_DARK:
		return g_hDarkBrush;

	default:
		return NULL;
	}
}