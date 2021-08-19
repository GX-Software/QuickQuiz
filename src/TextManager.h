// TextManager.h: interface for the CTextManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTMANAGER_H__7457E148_7CD7_400F_AB48_B76E9A1B1EC1__INCLUDED_)
#define AFX_TEXTMANAGER_H__7457E148_7CD7_400F_AB48_B76E9A1B1EC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 定义字符集
#define CS_ANSI			0
#define CS_UTF16		1
#define CS_UTF16_BIG	2
#define CS_UTF8			3
#define CS_UTF8_NOBOM	4
#define CS_UNKNOWN		5

// 字串识别用
#define CASE_SPACE			case _L(' '): \
							case _L('\t'): \
							case _L('　')

#define CASE_RETURN			case _L('\r'): \
							case _L('\n')

#define CASE_LEFTBRACE		case _L('('): \
							case _L('['): \
							case _L('{'): \
							case _L('<'): \
							case _L('〔'): \
							case _L('〈'): \
							case _L('「'): \
							case _L('『'): \
							case _L('〖'): \
							case _L('【'): \
							case _L('（'): \
							case _L('［'): \
							case _L('｛')

#define CASE_RIGHTBRACE		case _L(')'): \
							case _L(']'): \
							case _L('}'): \
							case _L('>'): \
							case _L('〕'): \
							case _L('〉'): \
							case _L('」'): \
							case _L('』'): \
							case _L('〗'): \
							case _L('】'): \
							case _L('）'): \
							case _L('］'): \
							case _L('｝')

#define CASE_POINT			case _L('.'): \
							case _L('、'): \
							case _L('。'): \
							case _L('·'): \
							case _L('．')

class CTextManager  
{
public:
	CTextManager(int nCS = CS_UNKNOWN);
	virtual ~CTextManager();

	EBOOL LoadFile(LPCTSTR strFilePath);
	EBOOL SaveFile(LPCTSTR strFilePath, LPCTSTR pText, int nLen);

	inline int GetCS() { return m_nCS; }
	inline LPCTSTR GetText() { return (LPCTSTR)m_pText; }

	EBOOL SetDecode(int nCS);

	static LPSTR FUNCCALL SwitchSave(LPCTSTR lpszText, int nCS, int *pLen);
	static LPTSTR FUNCCALL SwitchLoad(LPCSTR lpszText, int nCS, int *pLen);

	static RETURN_E LPTSTR FUNCCALL LoadFileFilter(UINT nID);
	static RETURN_E LPTSTR FUNCCALL LoadFileString(FILE *fp, int nStrLenSize, int nStrSize = 0);
	static EBOOL FUNCCALL SaveFileString(FILE *fp, LPCTSTR szSave, int nStrLenSize, int nStrSize = 0);

	static BOOL FUNCCALL CheckValidCharactor(LPCTSTR string);
	static BOOL FUNCCALL CheckValidCharactor(CString &string);
	static BOOL FUNCCALL CheckSpaces(LPCWSTR lpwText, size_t uSize, int *nStart, int *nLen);
	static void FUNCCALL RemoveReturn(LPTSTR ptr, size_t ulLen);

	static int FUNCCALL MemSubStr(const void *str1, const void *str2, size_t n1, size_t n2);

	static long FUNCCALL GetRealIndex(LPCTSTR str, int nPos);
	static int FUNCCALL GetCharactersCount(LPCTSTR str);

	static BOOL FUNCCALL GetFilePath(CString &strPath, LPCTSTR strFile);

	static LPCTSTR s_szWinReturn;

protected:
	TCHAR m_strFilePath[MAX_PATH];

	LPBYTE m_pBuff;
	LPBYTE m_pText;

	int m_nCS;						// 所用字符集

protected:
	int CheckBom(LPBYTE lpszText);
	int IsTextUTF8(CONST LPVOID lpBuffer, int cb);

	static void FUNCCALL SwapWord(LPWORD pWord, int nCount);
};

#endif // !defined(AFX_TEXTMANAGER_H__7457E148_7CD7_400F_AB48_B76E9A1B1EC1__INCLUDED_)
