// TextManager.cpp: implementation of the CTextManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CSCHECK_ANSI_INDEED		0
#define CSCHECK_UTF8_INDEED		2
#define CSCHECK_NOT_SURE		3

static BYTE UTF16BOM[2] = {0xFF, 0xFE};
static BYTE UTF16BBOM[2] = {0xFE, 0xFF};
static BYTE UTF8BOM[3] = {0xEF, 0xBB, 0xBF};

LPCTSTR CTextManager::s_szWinReturn = _T("\r\n");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextManager::CTextManager(int nCS /* = CS_UNKNOWN */) :
	m_pBuff(NULL), m_pText(NULL),
	m_nCS(nCS)
{
	ZeroMemory(m_strFilePath, sizeof(m_strFilePath));
}

CTextManager::~CTextManager()
{
	if (m_pBuff)
	{
		free(m_pBuff);
	}
}

EBOOL CTextManager::LoadFile(LPCTSTR strFilePath)
{
	FILE *fp = _tfopen(strFilePath, _T("rb"));
	if (!fp)
	{
		return Q_ERROR(ERROR_FILE_NOT_EXIST);
	}

	int nLen;
	
	// 文件已经打开
	fseek(fp, 0, SEEK_END);
	nLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	if (m_pBuff)
	{
		free(m_pBuff);
		m_pBuff = NULL;
	}
	// 读出文件内容
	m_pBuff = (LPBYTE)malloc(nLen + 2);
	if (!m_pBuff)
	{
		fclose(fp);
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	
	if (!fread(m_pBuff, nLen, 1, fp))
	{
		fclose(fp);
		return Q_ERROR(ERROR_READ_FILE_FAIL);
	}
	fclose(fp);
	// 给结尾加0
	m_pBuff[nLen] = m_pBuff[nLen + 1] = 0;

	m_nCS = CheckBom(m_pBuff);
	// 如果不含BOM头
	if (m_nCS != CS_UNKNOWN)
	{
		switch(m_nCS)
		{
		case CS_UTF16:
			m_pText = m_pBuff + sizeof(UTF16BOM);
			break;

		case CS_UTF16_BIG:
			m_pText = m_pBuff + sizeof(UTF16BBOM);
			break;

		case CS_UTF8:
			m_pText = m_pBuff + sizeof(UTF8BOM);
			break;

		default:
			ASSERT(FALSE);
		}
	}
	else
	{
		// 检查开头的部分
		int iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;
		IsTextUnicode(m_pBuff, nLen + 2, &iUniTest);

		// 如果通过测试，则说明是Unicode文件（包括大字端）
		if (iUniTest)
		{
			m_pText = m_pBuff;
			
			if (iUniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE)
			{
				m_nCS = CS_UTF16_BIG;
			}
			else
			{
				m_nCS = CS_UTF16;
			}
		}
		// 没有通过测试，则有可能是UTF-8
		else
		{
			switch(IsTextUTF8(m_pBuff, nLen + 2))
			{
			case CSCHECK_ANSI_INDEED:
				m_pText = m_pBuff;
				m_nCS = CS_ANSI;
				break;
				
			case CSCHECK_UTF8_INDEED:
				m_pText = m_pBuff + sizeof(UTF8BOM);
				m_nCS = CS_UTF8_NOBOM;
				break;
				
			default:
				m_pText = m_pBuff;
				m_nCS = CS_UNKNOWN;
				// 这里返回TRUE，是为了不报错
				// 随后就会弹出编码对话框
				return TRUE;
			}
		}
	}

	LPTSTR lpStr = SwitchLoad((LPCSTR)m_pText, m_nCS, NULL);
	if (!lpStr)
	{
		return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
	}
	free(m_pBuff);
	m_pBuff = (LPBYTE)lpStr;
	m_pText = m_pBuff;

	return Q_TRUE;
}

EBOOL CTextManager::SaveFile(LPCTSTR strFilePath, LPCTSTR pText, int nLen)
{
	if (CS_UNKNOWN == m_nCS)
	{
		m_nCS = CS_ANSI;
	}

	int nSize = 0;
	LPSTR lpStr = SwitchSave(pText, m_nCS, &nSize);
	if (!lpStr)
	{
		return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
	}

	if (m_pBuff)
	{
		free(m_pBuff);
	}
	m_pBuff = (LPBYTE)lpStr;
	m_pText = m_pBuff;

	FILE *fp = _tfopen(strFilePath, _T("wb"));
	if (!fp)
	{
		return Q_ERROR(ERROR_CREATE_FILE_FAIL);
	}

	switch(m_nCS)
	{
	case CS_UTF16:
		if (!fwrite(UTF16BOM, sizeof(UTF16BOM), 1, fp))
		{
			fclose(fp);
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		break;

	case CS_UTF16_BIG:
		if (!fwrite(UTF16BBOM, sizeof(UTF16BBOM), 1, fp))
		{
			fclose(fp);
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		break;

	case CS_UTF8:
		if (!fwrite(UTF8BOM, sizeof(UTF8BOM), 1, fp))
		{
			fclose(fp);
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		break;
	}
	
	if (!fwrite(m_pText, nSize, 1, fp))
	{
		fclose(fp);
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
	
	fclose(fp);
	return Q_TRUE;
}

EBOOL CTextManager::SetDecode(int nCS)
{
	// 只有在编码情况未知时才可重新设置编码方式
	if (CS_UNKNOWN != m_nCS)
	{
		return Q_TRUE;
	}

	m_nCS = nCS;
	LPTSTR lpszStr = SwitchLoad((LPCSTR)m_pBuff, m_nCS, NULL);
	if (!lpszStr)
	{
		return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
	}

	if (m_pBuff)
	{
		free(m_pBuff);
	}
	m_pBuff = (LPBYTE)lpszStr;
	m_pText = m_pBuff;
	return Q_TRUE;
}

int CTextManager::CheckBom(LPBYTE lpszText)
{
	if (!lpszText)
	{
		return CS_UNKNOWN;
	}

	if (!memcmp(lpszText, UTF16BOM, sizeof(UTF16BOM)))
	{
		return CS_UTF16;
	}
	else if (!memcmp(lpszText, UTF16BBOM, sizeof(UTF16BBOM)))
	{
		return CS_UTF16_BIG;
	}
	else if (!memcmp(lpszText, UTF8BOM, sizeof(UTF8BOM)))
	{
		return CS_UTF8;
	}
	else
	{
		return CS_UNKNOWN;
	}
}

int CTextManager::IsTextUTF8(CONST LPVOID lpBuffer, int cb)
{
	int encodingBytesCount = 0;
    BOOL allTextsAreASCIIChars = TRUE;
	LPBYTE ptr = (LPBYTE)lpBuffer;
	
	// 没有BOM头的情况
    for (int i = 0; i < cb; i++)
    {
        BYTE current = ptr[i];
		
		if (!current)
		{
			break;
		}
		else if ((current & 0x80) == 0x80)
        {                   
            allTextsAreASCIIChars = FALSE;
        }

        // 第一字节
		if (encodingBytesCount == 0)
		{
			// UTF-8和ANSI相同的部分
			if (!(current & 0x80))
			{
				continue;
			}
			
			if ((current & 0xC0) == 0xC0)
			{
				encodingBytesCount = 1;
				current <<= 2;
				
				// 计算一个UTF-8字符所占字节数
				while ((current & 0x80) == 0x80)
				{
					current <<= 1;
					encodingBytesCount++;
				}
			}                   
			else
			{
				// 第一字节最高位不是11，说明不是UTF-8
				return CSCHECK_ANSI_INDEED;
			}
		}
		// 非第一字节
		else
		{
			// UTF-8后续字节，高两位必须为10
			if ((current & 0xC0) == 0x80)
			{                       
				encodingBytesCount--;
			}
			else
			{
				// 后续规则不正确，说明不是UTF-8
				return CSCHECK_ANSI_INDEED;
			}
		}
    }
	
    if (encodingBytesCount)
    {
        // 后续字节不完整
		return CSCHECK_ANSI_INDEED;
    }

	// 所有字符高位为0，则按ANSI方式读取
	if (allTextsAreASCIIChars)
	{
		return CSCHECK_ANSI_INDEED;
	}
	
	// 不确定是何种编码
    return CSCHECK_NOT_SURE;
}

void FUNCCALL CTextManager::SwapWord(LPWORD pWord, int nCount)
{
	if (!pWord)
	{
		return;
	}

	int i;
	BYTE bySwap;
	LPBYTE ptr = (LPBYTE)pWord;
	
	for (i = 0; i < nCount; i++)
	{
		bySwap = ptr[2 * i];
		ptr[2 * i] = ptr[2 * i + 1];
		ptr[2 * i + 1] = bySwap;
	}
}

LPSTR FUNCCALL CTextManager::SwitchSave(LPCTSTR lpszText, int nCS, int *pLen)
{
	if (!lpszText)
	{
		return NULL;
	}

#ifdef _UNICODE
	switch(nCS)
	{
	case CS_ANSI:
		{
			int nCh = WideCharToMultiByte(CP_ACP, 0, lpszText, -1, NULL, 0, NULL, FALSE);
			LPSTR str = (LPSTR)malloc(nCh * sizeof(CHAR));
			if (!str)
			{
				return NULL;
			}
			
			nCh = WideCharToMultiByte(CP_ACP, 0, lpszText, -1, str, nCh, NULL, FALSE);
			if (!nCh)
			{
				free(str);
				return NULL;
			}

			if (pLen)
			{
				*pLen = nCh;
			}
			return str;
		}

	case CS_UTF16:
		{
			if (pLen)
			{
				*pLen = _tcslen(lpszText) * sizeof(TCHAR);
			}

			LPWSTR str = wcsdup(lpszText);
			return (LPSTR)str;
		}

	case CS_UTF16_BIG:
		{
			if (pLen)
			{
				*pLen = _tcslen(lpszText) * sizeof(TCHAR);
			}

			LPWSTR str = wcsdup(lpszText);
			if (!str)
			{
				return NULL;
			}
			SwapWord(str, wcslen(str));
			return (LPSTR)str;
		}

	case CS_UTF8:
	case CS_UTF8_NOBOM:
		{
			int nCh = WideCharToMultiByte(CP_UTF8, 0, lpszText, -1, NULL, 0, NULL, FALSE);
			LPSTR str = (LPSTR)malloc(nCh);
			if (!str)
			{
				return NULL;
			}

			nCh = WideCharToMultiByte(CP_UTF8, 0, lpszText, -1, str, nCh, NULL, FALSE);
			if (!nCh)
			{
				free(str);
				return NULL;
			}

			if (pLen)
			{
				*pLen = nCh;
			}
			return str;
		}

	default:
		return NULL;
	}
#else
	switch(nCS)
	{
	case CS_ANSI:
		{
			if (pLen)
			{
				*pLen = _tcslen(lpszText) * sizeof(TCHAR);
			}
			LPSTR str = strdup(lpszText);
			return str;
		}

	case CS_UTF16:
	case CS_UTF16_BIG:
		{
			int nWide = MultiByteToWideChar(CP_ACP, 0, lpszText, -1, NULL, 0);
			
			LPWSTR str = (LPWSTR)malloc(nWide * sizeof(WCHAR));
			if (!str)
			{
				return NULL;
			}
			
			nWide = MultiByteToWideChar(CP_ACP, 0, lpszText, -1, str, nWide);
			if (!nWide)
			{
				free(str);
				return NULL;
			}

			if (CS_UTF16_BIG == nCS)
			{
				SwapWord(str, nWide);
			}
			
			if (pLen)
			{
				*pLen = nWide * sizeof(WCHAR);
			}
			return (LPSTR)str;
		}

	case CS_UTF8:
	case CS_UTF8_NOBOM:
		{
			int nWide = MultiByteToWideChar(CP_ACP, 0, lpszText, -1, NULL, 0);
			
			LPWSTR strW = (LPWSTR)malloc(nWide * sizeof(WCHAR));
			if (!strW)
			{
				return NULL;
			}
			
			nWide = MultiByteToWideChar(CP_ACP, 0, lpszText, -1, strW, nWide);
			if (!nWide)
			{
				free(strW);
				return NULL;
			}

			int nCh = WideCharToMultiByte(CP_UTF8, 0, strW, -1, NULL, 0, NULL, FALSE);
			LPSTR strC = (LPSTR)malloc(nCh * sizeof(BYTE));
			if (!strC)
			{
				free(strW);
				return NULL;
			}

			nCh = WideCharToMultiByte(CP_UTF8, 0, strW, -1, strC, nCh, NULL, FALSE);
			if (!nCh)
			{
				free(strW);
				free(strC);
				return NULL;
			}

			free(strW);
			if (pLen)
			{
				*pLen = nCh * sizeof(TCHAR);
			}
			return strC;
		}

	default:
		return NULL;
	}
#endif
}

LPTSTR FUNCCALL CTextManager::SwitchLoad(LPCSTR lpszText, int nCS, int *pLen)
{
	if (!lpszText)
	{
		return NULL;
	}

#ifdef _UNICODE
	switch(nCS)
	{
	case CS_ANSI:
		{
			int nWide = MultiByteToWideChar(CP_ACP, 0, lpszText, -1, NULL, 0);

			LPTSTR str = (LPTSTR)malloc(nWide * sizeof(TCHAR));
			if (!str)
			{
				return NULL;
			}
			
			nWide = MultiByteToWideChar(CP_ACP, 0, lpszText, -1, str, nWide);
			if (!nWide)
			{
				free(str);
				return NULL;
			}

			if (pLen)
			{
				*pLen = nWide * sizeof(TCHAR);
			}
			return str;
		}

	case CS_UTF16:
		{
			if (pLen)
			{
				*pLen = wcslen((LPCWSTR)lpszText) * sizeof(TCHAR);
			}
			LPTSTR str = wcsdup((LPCTSTR)lpszText);
			return str;
		}

	case CS_UTF16_BIG:
		{
			if (pLen)
			{
				*pLen = wcslen((LPCWSTR)lpszText) * sizeof(TCHAR);
			}
			LPTSTR str = wcsdup((LPCTSTR)lpszText);
			SwapWord((LPWORD)str, wcslen((LPCTSTR)lpszText));
			return str;
		}

	case CS_UTF8:
	case CS_UTF8_NOBOM:
		{
			int nWide = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpszText, -1, NULL, 0);

			LPTSTR str = (LPTSTR)malloc(nWide * sizeof(TCHAR));
			if (!str)
			{
				return NULL;
			}

			nWide = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpszText, -1, str, nWide);
			if (!nWide)
			{
				free(str);
				return NULL;
			}

			if (pLen)
			{
				*pLen = nWide * sizeof(TCHAR);
			}
			return str;
		}
		break;

	default:
		return NULL;
	}
#else
	switch(nCS)
	{
	case CS_ANSI:
		{
			if (pLen)
			{
				*pLen = strlen(lpszText) * sizeof(TCHAR);
			}
			LPTSTR str = strdup(lpszText);
			return str;
		}

	case CS_UTF16:
	case CS_UTF16_BIG:
		{
			LPWSTR pWide = wcsdup((LPCWSTR)lpszText);
			if (!pWide)
			{
				return NULL;
			}

			if (CS_UTF16_BIG == nCS)
			{
				SwapWord((LPWORD)pWide, wcslen(pWide));
			}

			int nCh = WideCharToMultiByte(CP_ACP, 0, pWide, -1, NULL, 0, NULL, FALSE);
			LPTSTR str = (LPTSTR)malloc(nCh * sizeof(TCHAR));
			if (!str)
			{
				free(pWide);
				return NULL;
			}
			
			nCh = WideCharToMultiByte(CP_ACP, 0, pWide, -1, str, nCh, NULL, FALSE);
			if (!nCh)
			{
				free(pWide);
				free(str);
				return NULL;
			}
			
			free(pWide);
			if (pLen)
			{
				*pLen = nCh * sizeof(TCHAR);
			}
			return str;
		}

	case CS_UTF8:
	case CS_UTF8_NOBOM:
		{
			int nWide = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpszText, -1, NULL, 0);

			LPWSTR strW = (LPWSTR)malloc(nWide * sizeof(WCHAR));
			if (!strW)
			{
				return NULL;
			}

			nWide = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpszText, -1, strW, nWide);
			if (!nWide)
			{
				free(strW);
				return NULL;
			}

			int nCh = WideCharToMultiByte(CP_ACP, 0, strW, -1, NULL, 0, NULL, FALSE);

			LPTSTR strC = (LPTSTR)malloc(nCh * sizeof(TCHAR));
			if (!strC)
			{
				free(strW);
				return NULL;
			}

			nCh = WideCharToMultiByte(CP_ACP, 0, strW, -1, strC, nCh, NULL, FALSE);
			if (!nCh)
			{
				free(strW);
				free(strC);
				return NULL;
			}

			free(strW);
			if (pLen)
			{
				*pLen = nCh * sizeof(TCHAR);
			}
			return strC;
		}

	default:
		return NULL;
	}
#endif
}

RETURN_E LPTSTR FUNCCALL CTextManager::LoadFileFilter(UINT nID)
{
	LPTSTR szTemp = NULL;	
	int i = 0;
	int nLen = 0;
	
	do
	{
		i += 256;
		
		free(szTemp);
		szTemp = (LPTSTR)malloc(i * sizeof(TCHAR));
		if (!szTemp)
		{
			return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
		}
		nLen = LoadString(AfxGetInstanceHandle(), nID, szTemp, i);
		if (!nLen)
		{
			free(szTemp);
			return Q_ERROR_RET(ERROR_RESOURCE_NOT_EXIST, NULL);
		}
	} while(i - nLen <= 2);
	
	szTemp[nLen] = 0;
	for (i = 0; i < nLen; i++)
	{
		if (szTemp[i] == _T('|'))
		{
			szTemp[i] = 0;
		}
	}

	return Q_TRUE_RET(szTemp);
}

RETURN_E LPTSTR FUNCCALL CTextManager::LoadFileString(FILE *fp, int nStrLenSize, int nStrSize /* = 0 */)
{
	int nSize = 0;
	if (nStrLenSize)
	{
		if (!fread(&nSize, nStrLenSize, 1, fp))
		{
			return Q_ERROR_RET(ERROR_READ_FILE_FAIL, NULL);
		}
	}
	else
	{
		ASSERT(nStrSize);
		nSize = nStrSize * sizeof(WCHAR);
	}
	
	LPWSTR ptr = (LPWSTR)malloc(nSize);
	if (!ptr)
	{
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}
	if (!fread(ptr, nSize, 1, fp))
	{
		free(ptr);
		return Q_ERROR_RET(ERROR_READ_FILE_FAIL, NULL);
	}
	
#ifdef _UNICODE
	return Q_TRUE_RET(ptr);
#else
	LPTSTR ptrC = (LPTSTR)malloc(nSize);
	if (!ptrC)
	{
		free(ptr);
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}
	
	if (!WideCharToMultiByte(CP_ACP, 0, ptr, -1, ptrC, nSize, NULL, FALSE))
	{
		free(ptr);
		free(ptrC);
		return Q_ERROR_RET(ERROR_TEXT_TRANSCODING_FAIL, NULL);
	}
	
	free(ptr);
	return Q_TRUE_RET(ptrC);
#endif
}

EBOOL FUNCCALL CTextManager::SaveFileString(FILE *fp, LPCTSTR szSave, int nStrLenSize, int nStrSize /* = 0 */)
{
	int nSize;
	
	// 如果写入内容为空，则必不为数组写入
	if (!szSave)
	{
		ASSERT(nStrLenSize);
		
		nSize = 1 * sizeof(WCHAR);
		if (!fwrite(&nSize, nStrLenSize, 1, fp) ||
			EOF == fputwc(0, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		
		return Q_TRUE;
	}
	
#ifdef _UNICODE
	if (nStrLenSize)
	{
		nSize = (_tcslen(szSave) + 1) * sizeof(TCHAR);
		if (!fwrite(&nSize, nStrLenSize, 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
	}
	else
	{
		ASSERT(nStrSize);
		nSize = nStrSize * sizeof(WCHAR);
	}
	
	if (!fwrite(szSave, nSize, 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
#else
	int nCount;
	
	if (nStrLenSize)
	{
		nCount = MultiByteToWideChar(CP_ACP, 0, szSave, -1, NULL, 0);
		if (!nCount)
		{
			return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
		}
		nSize = nCount * sizeof(WCHAR);
		if (!fwrite(&nSize, nStrLenSize, 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
	}
	else
	{
		ASSERT(nStrSize);
		nCount = nStrSize;
		nSize = nStrSize * sizeof(WCHAR);
	}
	
	LPWSTR ptrW = (LPWSTR)malloc(nSize);
	if (!ptrW)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	
	if (!MultiByteToWideChar(CP_ACP, 0, szSave, -1, ptrW, nCount))
	{
		free(ptrW);
		return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
	}
	if (!fwrite(ptrW, nSize, 1, fp))
	{
		free(ptrW);
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
	
	free(ptrW);
#endif
	
	return Q_TRUE;
}

BOOL FUNCCALL CTextManager::CheckValidCharactor(LPCTSTR string)
{
	int i = 0, nLen;
	BOOL bOut = FALSE;
	
	if (!string)
	{
		return FALSE;
	}
	
	nLen = lstrlen(string);
	if (!nLen)
	{
		return FALSE;
	}
	
	while(!bOut && i < nLen)
	{
		switch(string[i])
		{
		case _T('\r'):
		case _T('\n'):
		case _T(' '):
		case _T('\t'):
			break;
#ifdef _UNICODE
		case _T('　'):
			break;
#else
		case (TCHAR)0xA1:
			if ((i + 1 == nLen) || (string[i + 1] != (TCHAR)0xA1))
			{
				bOut = TRUE;
			}
			else
			{
				i++;
			}
			break;
#endif
		default:
			bOut = TRUE;
			continue;
		}
		
		i++;
	}
	
	return (i != nLen);
}

// 检查是否有有效字符
BOOL FUNCCALL CTextManager::CheckValidCharactor(CString &string)
{
	return CheckValidCharactor((LPCTSTR)string);
}

/*
功能：	检测字符串开头和末尾的空格及换行等内容
返回值：字符串中有非空字符返回TRUE，否则返回FALSE
备注：	nStart和nLen可以为空，若为空，可以用来检测字符串中是否有有效字符
		输入的内容必须为宽字符
*/
BOOL FUNCCALL CTextManager::CheckSpaces(LPCWSTR lpwText, size_t uSize, int *nStart, int *nLen)
{
	ASSERT(lpwText);
	
	int i = 0;
	int s = -1, e = -1;
	size_t l = uSize;
	if (!uSize)
	{
		l = wcslen(lpwText);
	}
	
	// 从开头找
	for (i = 0; lpwText[i] && s < 0; ++i)
	{
		switch(lpwText[i])
		{
		CASE_RETURN:
		CASE_SPACE:
			break;
		
		default:
			s = i;
		}
	}
	
	// 整个字符串没有有效字符
	if (s < 0)
	{
		return FALSE;
	}
	
	// 从结尾找
	for (i = l - 1; i >= s && e < 0; --i)
	{
		switch(lpwText[i])
		{
		CASE_RETURN:
		CASE_SPACE:
			break;
		
		default:
			e = i;
		}
	}
	
	if (s > e)
	{
		return FALSE;
	}
	
	if (nStart)
	{
		*nStart = s;
	}
	if (nLen)
	{
		*nLen = e - s + 1;
	}
	return TRUE;
}

void FUNCCALL CTextManager::RemoveReturn(LPTSTR ptr, size_t ulLen)
{
	size_t a = 0, b = 0;
	if (!ulLen)
	{
		ulLen = _tcslen(ptr);
	}
	
	while(b < ulLen)
	{
		switch(ptr[b])
		{
		case _T('\r'):
		case _T('\n'):
			++b;
			continue;
		}
		
		ptr[a++] = ptr[b++];
	}
	ptr[a] = 0;
}

int FUNCCALL CTextManager::MemSubStr(const void *str1, const void *str2, size_t n1, size_t n2)
{
	const unsigned char *su1 = (const unsigned char*)str1, *su2 = (const unsigned char*)str2;
	const unsigned char *p = su1;
	while(1)
	{
		p = (unsigned char*)memchr(p, (int)(*su2), n1 - (p - su1));
		if (!p)
		{
			break;
		}
		else if (p - su1 + n2 <= n1)
		{
			if (!memcmp(p, su2, n2))
			{
				return (p - su1);
			}
			p++;
		}
		else
		{
			break;
		}
	}
	
	return -1;
}

long FUNCCALL CTextManager::GetRealIndex(LPCTSTR str, int nPos)
{
#ifdef _UNICODE
	return (long)nPos;
#else
	int nLen = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	if (!nLen)
	{
		return 0;
	}
	LPWSTR wPtr = (LPWSTR)malloc((nLen + 1) * sizeof(WCHAR));
	if (!wPtr)
	{
		return 0;
	}

	if (!MultiByteToWideChar(CP_ACP, 0, str, -1, wPtr, nLen))
	{
		free(wPtr);
		return 0;
	}

	nLen = WideCharToMultiByte(CP_ACP, 0, wPtr, nPos, NULL, NULL, NULL, FALSE);
	free(wPtr);
	return (long)nLen;
#endif
}

int FUNCCALL CTextManager::GetCharactersCount(LPCTSTR str)
{
#ifdef _UNICODE
	return _tcslen(str);
#else
	return MultiByteToWideChar(CP_ACP, 0, str, _tcslen(str), NULL, 0);
#endif
}

BOOL FUNCCALL CTextManager::GetFilePath(CString &strPath, LPCTSTR strFile)
{
	CString str;
	int i = _tcslen(strFile) - 1;
	while(i)
	{
		if (_T('\\') == strFile[i])
		{
			break;
		}
		--i;
	}
	if (i < 0)
	{
		return FALSE;
	}

	str = strFile;
	strPath = str.Left(i + 1);
	return TRUE;
}