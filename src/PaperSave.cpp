// PaperSave.cpp: implementation of the CPaper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Paper.h"

#include "zconf.h"
#include "zlib.h"
#include "AES.h"
#include "ImageManager.h"

#include "Choise.h"
#include "Judge.h"
#include "Blank.h"
#include "Text.h"

static LPCSTR lpszAppKey = "QuickQuiz";
static LPCSTR lpszXMLHead = "<?xml ";
static LPCSTR lpszSaveKey = "QuickQuiz_Save";

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LPCTSTR paper_lpszBinExt			= _T("qux");
LPCTSTR paper_lpszOldBinExt			= _T("quiz");
LPCTSTR paper_lpszSavExt			= _T("sav");

static LPCTSTR paper_lpszQXmlExt	= _T("qxml");
static LPCTSTR paper_lpszXmlExt		= _T("xml");
static LPCTSTR paper_lpszTxtExt		= _T("txt");
static LPCTSTR paper_lpszJpgExt		= _T("jpg");
static LPCTSTR paper_lpszJpegExt	= _T("jpeg");

int CPaper::GetFileExt(LPCTSTR strFilePath)
{
	ASSERT(strFilePath);

	int i = _tcslen(strFilePath) - 1;
	for (; i >= 0; i--)
	{
		if (strFilePath[i] == _T('.'))
		{
			i++;
			break;
		}
	}

	if (!_tcscmp(strFilePath + i, paper_lpszOldBinExt))
	{
		return PAPER_FILETYPE_OLDBIN;
	}
	else if (!_tcscmp(strFilePath + i, paper_lpszQXmlExt) ||
			 !_tcscmp(strFilePath + i, paper_lpszXmlExt) ||
			 !_tcscmp(strFilePath + i, paper_lpszTxtExt))
	{
		return PAPER_FILETYPE_XML;
	}
	else if (!_tcscmp(strFilePath + i, paper_lpszJpgExt) ||
			 !_tcscmp(strFilePath + i, paper_lpszJpegExt))
	{
		return PAPER_FILETYPE_JPEG;
	}
	else
	{
		return PAPER_FILETYPE_BIN;
	}
}

void CPaper::GetSaveFilePath(LPTSTR strPath)
{
	_tcsncpy(strPath, m_strFilePath, MAX_PATH);
	
	int i;
	for (i = _tcslen(strPath) - 1; i >= 0; i--)
	{
		if (_T('.') == strPath[i])
		{
			break;
		}
	}
	
	_tcscpyn(strPath + i + 1, paper_lpszSavExt, 4);
}

EBOOL CPaper::LoadPaper(LPCTSTR strFilePath, password funcPass)
{
	// �����������������Ϣ����Ϊ�ڶ�ȡ�����п����в�Ӱ���ȡ���д�������
	Q_CLEARERROR();

	m_nDefSaveStyle = GetFileExt(strFilePath);
	_tcsncpy(m_strFilePath, strFilePath, MAX_PATH);

	LPBYTE buf = NULL;
	int nSize, nCount = 0;
	FILE *fp = _tfopen(strFilePath, _T("rb"));
	if (!fp)
	{
		return Q_ERROR(ERROR_FILE_NOT_EXIST);
	}
	fseek(fp, 0, SEEK_END);
	nSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	buf = (LPBYTE)malloc(nSize);
	if (!buf)
	{
		fclose(fp);
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	if (!fread(buf, nSize, 1, fp))
	{
		fclose(fp);
		free(buf);
		return Q_ERROR(ERROR_READ_FILE_FAIL);
	}
	fseek(fp, 0, SEEK_SET);

	// ��ȡ��ͷ��JPEG����
	if (PAPER_FILETYPE_JPEG == m_nDefSaveStyle)
	{
		nCount = CTextManager::MemSubStr(buf, lpszXMLHead, nSize, strlen(lpszXMLHead));
		if (nCount >= 0)
		{
			m_nDefSaveStyle = PAPER_FILETYPE_JPEGXML;
		}
		else
		{
			nCount = CTextManager::MemSubStr(buf, lpszAppKey, nSize, strlen(lpszAppKey));
			if (nCount >= 0)
			{
				m_nDefSaveStyle = PAPER_FILETYPE_JPEGBIN;
				fseek(fp, nCount, SEEK_SET);
			}
			else
			{
				m_nDefSaveStyle = PAPER_FILETYPE_BIN;
				fseek(fp, 0, SEEK_SET);
			}
		}

		if (m_nDefSaveStyle == PAPER_FILETYPE_JPEGBIN ||
			m_nDefSaveStyle == PAPER_FILETYPE_JPEGXML)
		{
			// �����ǳ�ʼ�����ʲ��漰�����б�����
			ClearCoverInfo(&m_sCover);
			m_sCover.lpData = (LPBYTE)malloc(nCount);
			if (!m_sCover.lpData)
			{
				fclose(fp);
				free(buf);
				return Q_ERROR(ERROR_ALLOC_FAIL);
			}
			memcpy(m_sCover.lpData, buf, nCount);
			m_sCover.ulDataSize = nCount;
		}
	}

	EBOOL ret;
	if (PAPER_FILETYPE_BIN == m_nDefSaveStyle ||
		PAPER_FILETYPE_JPEGBIN == m_nDefSaveStyle)
	{
		fclose(fp);
		ret = LoadPaperM(buf + nCount, nSize - nCount, funcPass);
		free(buf);
	}
	else if (PAPER_FILETYPE_OLDBIN == m_nDefSaveStyle)
	{
		free(buf);
		ret = LoadPaperF(fp, funcPass);
		fclose(fp);
	}
	else
	{
		fclose(fp);
		ret = LoadXML((PCHAR)(buf + nCount));
		free(buf);
	}
	if (!ret)
	{
		return Q_ERROR(ERROR_INNER);
	}

	// ������з���ȴʵ��û����Ϊ������ĸ�ʽ����˴����ɷ���
	if (m_sCover.nType != PAPER_COVERTYPE_NONE && !m_sCover.ulDataSize)
	{
		if (!MakeJpegCover(NULL, &m_sCover))
		{
			return Q_ERROR(ERROR_INNER);
		}
	}

	// ������������б�
	ResetTypeList();

	// ��ȡ�ղ���Ŀ
	LoadSave(NULL);
	
	m_bModify = FALSE;
	m_bCanEdit = FALSE;

	// ���ﷵ��TRUE����Ϊ�˲������д��󵫲�Ӱ���ȡ������
	return TRUE;
}

EBOOL CPaper::SavePaper(LPCTSTR strFilePath, int nSaveStyle)
{
	LPCTSTR pStrFilePath = NULL;

	if (nSaveStyle & PAPER_FILETYPE_WIN95)
	{
		nSaveStyle &= ~(PAPER_FILETYPE_WIN95);
		m_bEncodeMBCS = TRUE;
	}
	else
	{
		m_bEncodeMBCS = FALSE;
	}
	if (nSaveStyle & PAPER_FILETYPE_MAXCOMP)
	{
		m_bMaxCompatible = TRUE;
	}
	else
	{
		m_bMaxCompatible = FALSE;
	}
	nSaveStyle = nSaveStyle & 0xFFL;

	if (strFilePath)
	{
		pStrFilePath = strFilePath;
		m_nDefSaveStyle = nSaveStyle;
	}
	else
	{
		pStrFilePath = m_strFilePath;
	}

	FILE *fp = _tfopen(pStrFilePath, _T("wb"));
	if (!fp)
	{
		return Q_ERROR(ERROR_CREATE_FILE_FAIL);
	}

	if (PAPER_FILETYPE_JPEGBIN == m_nDefSaveStyle ||
		PAPER_FILETYPE_JPEGXML == m_nDefSaveStyle)
	{
		LPPAPERCOVERINFO ppc = GetCoverInfo();
		if (ppc->nType == PAPER_COVERTYPE_NONE)
		{
			switch(m_nDefSaveStyle)
			{
			case PAPER_FILETYPE_JPEGBIN:
				m_nDefSaveStyle = PAPER_FILETYPE_BIN;
				break;

			case PAPER_FILETYPE_JPEGXML:
				m_nDefSaveStyle = PAPER_FILETYPE_XML;
				break;
			}
		}
		else
		{
			if (!fwrite(ppc->lpData, ppc->ulDataSize, 1, fp))
			{
				return Q_ERROR(ERROR_WRITE_FILE_FAIL);
			}
		}
	}

	BOOL ret;
	if (PAPER_FILETYPE_BIN == m_nDefSaveStyle ||
		PAPER_FILETYPE_JPEGBIN == m_nDefSaveStyle)
	{
		m_dwVersion = FILEVERSION;
		ret = SavePaperM(fp);
		fclose(fp);
	}
	else if (PAPER_FILETYPE_OLDBIN == m_nDefSaveStyle)
	{
		m_dwVersion = FILEVERSION_1X;
		ret = SavePaperF(fp);
		fclose(fp);
	}
	else if (PAPER_FILETYPE_XML == m_nDefSaveStyle ||
			 PAPER_FILETYPE_JPEGXML == m_nDefSaveStyle)
	{
		// ������Ϊ����ģʽʱ����Ȼ�˳�����
		m_nEncryption = 0;
		ZeroMemory(m_uMD5, sizeof(m_uMD5));

		ret = SaveXML(fp, m_bEncodeMBCS);
		fclose(fp);
	}
	if (!ret)
	{
		return Q_ERROR(ERROR_INNER);
	}

	m_bModify = FALSE;
	
	// ������Ȿ��Ϣ
	SaveSave();
	
	// ����֮�󱣴����к�
	m_sUndoStep.nSaveIndex = m_sUndoStep.nCurStep;
	
	if (strFilePath)
	{
		_tcsncpy(m_strFilePath, strFilePath, MAX_PATH);
	}
	
	return Q_TRUE;
}

/*
���ܣ�	��ȡ2.x��׼����ʽ��.qux��ʽ��
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	����������ȡ1.x��׼����ʽ��.quiz��ʽ��
*/
EBOOL CPaper::LoadPaperM(LPBYTE p, DWORD dwSize, password funcPass)
{
	if (!p || !dwSize)
	{
		return Q_ERROR(ERROR_INNER);
	}

	DWORD dwReadSize = 0, dwRead = 0;

	// �����ʼ��һ��QuickQuiz����
	dwReadSize = 9 * sizeof(char);
	if (dwRead + dwReadSize > dwSize)
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}
	if (strncmp((PCHAR)p, lpszAppKey, dwReadSize))
	{
		return Q_ERROR(ERROR_HEAD_NOT_FOUND);
	}
	dwRead += dwReadSize;
	p += dwReadSize;

	// Ȼ�����ĸ��ֽڵİ汾��Ϣ
	// �����°��ʽ����Ϊ4���������ٸ���
	dwReadSize = sizeof(DWORD);
	if (dwRead + dwReadSize > dwSize)
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}
	memcpy(&m_dwVersion, p, dwReadSize);
	if (m_dwVersion < FILEVERSION)
	{
		return Q_ERROR(ERROR_INVALID_VERSION);
	}
	dwRead += dwReadSize;
	p += dwReadSize;

	// ����Ƿ�����
	dwReadSize = sizeof(BYTE);
	if (dwRead + dwReadSize > dwSize)
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}
	m_nEncryption = (int)*p;
	dwRead += dwReadSize;
	p += dwReadSize;

	// �����ⱻ������
	if (IsLocked())
	{
		// 4���ֽڵ�MD5����
		dwReadSize = sizeof(m_uMD5);
		if (dwRead + dwReadSize > dwSize)
		{
			return Q_ERROR(ERROR_FILE_INCOMPLETE);
		}
		memcpy(m_uMD5, p, sizeof(m_uMD5));
		dwRead += dwReadSize;
		p += dwReadSize;

		// ���ýӿں�����������ִ�
		if (!funcPass(m_cPassword, m_uMD5))
		{
			m_cPassword[0] = 0;
			return Q_ERROR_NOMSG;
		}
		if (AES_OK != AES_SetKey((byte*)m_cPassword, strlen(m_cPassword)))
		{
			m_cPassword[0] = 0;
			return Q_ERROR(ERROR_INNER);
		}
	}

	// һ�ֽ�ѹ����ʽ
	// 0Ϊ��ѹ����1Ϊzlibѹ��
	// ��ѹ���������ѹ�����ļ���С�Ȳ�ѹ������
	dwReadSize = sizeof(BYTE);
	if (dwRead + dwReadSize > dwSize)
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}
	m_nCompressType = (BOOL)*p;
	dwRead += dwReadSize;
	p += dwReadSize;

	// ���ֽں������ݿ��С
	// ԭʼ���ݿ�ĳ����ǰ���ĩβ0��
	ULONG ulLenData = 0, ulLenRaw = 0;
	dwReadSize = sizeof(ULONG);
	if (dwRead + dwReadSize > dwSize)
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}
	memcpy(&ulLenData, p, dwReadSize);
	dwRead += dwReadSize;
	p += dwReadSize;

	// ���ȳ����ڴ�
	if (dwRead + ulLenData > dwSize)
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}

	// �����ѹ�����
	if (PAPER_COMPRESS_ZLIB == m_nCompressType)
	{
		// ���ֽ�ѹ��ǰ����
		dwReadSize = sizeof(ULONG);
		if (dwRead + dwReadSize > dwSize)
		{
			return Q_ERROR(ERROR_FILE_INCOMPLETE);
		}
		memcpy(&ulLenRaw, p, dwReadSize);
		dwRead += dwReadSize;
		p += dwReadSize;
	}

	// �����������
	LPBYTE n = NULL;
	if (IsLocked())
	{
		size_t s = 0;
		
		AES_Decrypt_CBC(NULL, ulLenData, NULL, &s);
		ASSERT(s > 0);
		
		n = (LPBYTE)malloc(s);
		if (!n)
		{
			return Q_ERROR(ERROR_ALLOC_FAIL);
		}
		
		if (AES_OK != AES_Decrypt_CBC(p, ulLenData, n, &s))
		{
			free(n);
			return Q_ERROR(ERROR_DECRYPT_FAIL);
		}
	}
	else
	{
		n = (LPBYTE)malloc(ulLenData);
		if (!n)
		{
			return Q_ERROR(ERROR_ALLOC_FAIL);
		}
		memcpy(n, p, ulLenData);
	}

	// ���Ϊzlibѹ������ѹ��
	if (PAPER_COMPRESS_ZLIB == m_nCompressType)
	{
		LPBYTE u = (LPBYTE)malloc(ulLenRaw);
		if (!n)
		{
			free(n);
			return Q_ERROR(ERROR_ALLOC_FAIL);
		}

		if (Z_OK != uncompress(u, &ulLenRaw, n, ulLenData))
		{
			free(u);
			free(n);
			return Q_ERROR(ERROR_EXPANSION_FAIL);
		}

		free(n);
		n = u;
	}

	// ��ѹ����ַ���XML����
	int ret = LoadXML((char*)n);
	free(n);
	if (!ret)
	{
		return Q_ERROR(ERROR_INNER);
	}
	
	return Q_RET(TRUE);
}

/*
���ܣ�	��1.x�汾���ɵ������ж�ȡ
����ֵ���ɹ��򷵻�TRUE�����򷵻�FALSE
��ע��	ֻ�ܶ�ȡ���汾��3
*/
EBOOL CPaper::LoadPaperF(FILE *fp, password funcPass)
{
	ASSERT(fp);

	int i = 0, nType = TYPE_DEFAULT;
	CItem *pItem = NULL;
	LPWSTR ptr = NULL;
	char Head[10] = {0};
	int nCustomCount = 0;

	// �����ʼ��һ��QuickQuiz����
	if (!fread(Head, 9 * sizeof(char), 1, fp))
	{
		return Q_ERROR(ERROR_READ_FILE_FAIL);
	}
	if (strncmp(Head, lpszAppKey, 9))
	{
		return Q_ERROR(ERROR_HEAD_NOT_FOUND);
	}

	// Ȼ�����ĸ��ֽڵİ汾��Ϣ
	if (!fread(&m_dwVersion, sizeof(DWORD), 1, fp))
	{
		return Q_ERROR(ERROR_READ_FILE_FAIL);
	}
	if (m_dwVersion > FILEVERSION_1X)
	{
		return Q_ERROR(ERROR_INVALID_VERSION);
	}

	// �ȶ�ȡ�������
	LPTSTR ptrC = CTextManager::LoadFileString(fp, sizeof(WORD));
	if (!ptrC)
	{
		return Q_ERROR(ERROR_INNER);
	}
	if (!CItem::SetDescription(ptrC))
	{
		free(ptrC);
		return Q_ERROR(ERROR_INNER);
	}
	free(ptrC);

	// ����Ƿ�����
	m_nEncryption = fgetc(fp);
	if (EOF == m_nEncryption)
	{
		m_nEncryption = ENCRYPT_NO;
		return Q_ERROR(ERROR_READ_FILE_FAIL);
	}
	// �����ⱻ�����ˣ������4���ֽڵ�MD5����
	else if (IsLocked())
	{
		if (!fread(m_uMD5, sizeof(m_uMD5), 1, fp))
		{
			return Q_ERROR(ERROR_READ_FILE_FAIL);
		}

		// ���ýӿں�����������ִ�
		if (!funcPass(m_cPassword, m_uMD5))
		{
			m_cPassword[0] = 0;
			return Q_ERROR_NOMSG;
		}
	}

	// VERSION2�������ܣ��Զ�����Ŀ����
	// ���֧��10���Զ�����Ŀ����
	if (m_dwVersion >= 2)
	{
		// �����Զ���������
		nCustomCount = fgetc(fp);
		if (EOF == nCustomCount)
		{
			return Q_ERROR(ERROR_READ_FILE_FAIL);
		}
		if (nCustomCount > TYPE_MAXCUSTOMCOUNT)
		{
			return Q_ERROR(ERROR_CUSTOM_TYPE_INCORRECT);
		}

		if (nCustomCount)
		{
			int nMax = 0, nLen = 0;
			// ��󻺳��ֽ���
			if (!fread(&nMax, sizeof(DWORD), 1, fp))
			{
				return Q_ERROR(ERROR_READ_FILE_FAIL);
			}
			ptr = (LPWSTR)malloc(nMax);
			if(!ptr)
			{
				return Q_ERROR(ERROR_ALLOC_FAIL);
			}
			
			CTempList *pList = NULL;
			// ���ζ����Զ���������
			for (i = 0; i < nCustomCount; i++)
			{
				// ������Ŀ����
				if (!fread(&nType, sizeof(DWORD), 1, fp))
				{
					return Q_ERROR(ERROR_READ_FILE_FAIL);
				}
				
				pList = new CTempList(nType);
				if (!pList)
				{
					free(ptr);
					return Q_ERROR(ERROR_ALLOC_FAIL);
				}
				
				nLen = 0;
				if (!fread(&nLen, sizeof(WORD), 1, fp))
				{
					free(ptr);
					return Q_ERROR(ERROR_READ_FILE_FAIL);
				}
				if(nLen > nMax)
				{
					free(ptr);
					return Q_ERROR(ERROR_CUSTOM_TYPE_INCORRECT);
				}

				if (!fread(ptr, nLen, 1, fp))
				{
					free(ptr);
					return Q_ERROR(ERROR_READ_FILE_FAIL);
				}
#ifdef _UNICODE
				if (!pList->SetDescription(ptr))
				{
					return Q_ERROR(ERROR_INNER);
				}
#else
				LPTSTR ptrC = CTextManager::SwitchLoad((LPCSTR)ptr, CS_UTF16, NULL);
				if (!ptrC)
				{
					free(ptr);
					return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
				}
				
				if (!pList->SetDescription(ptrC))
				{
					return Q_ERROR(ERROR_INNER);
				}
				free(ptrC);
#endif
				CList::AddItem(pList);
			}
			
			free(ptr);
		}
	}
	
	while(1)
	{
		// ��ȡ��Ŀ����
		// VERSION2�Ժ�����ռ��4�ֽ�
		if (m_dwVersion == 1)
		{
			i = 0;
			if (!fread(&i, sizeof(BYTE), 1, fp))
			{
				if (feof(fp))
				{
					// ��������ѭ������ζ����������
					break;
				}
				return Q_ERROR(ERROR_READ_FILE_FAIL);
			}
		}
		else
		{
			if (!fread(&i, sizeof(DWORD), 1, fp))
			{
				if (feof(fp))
				{
					// ��������ѭ������ζ����������
					break;
				}
				return Q_ERROR(ERROR_READ_FILE_FAIL);
			}
		}
		
		nType = CItem::GetType(i);
		i = CItem::GetCustomIndex(i);
		switch(nType)
		{
		case TYPE_DEFAULT:
			continue;
			
		case TYPE_SCHOISE:
		case TYPE_MCHOISE:
			pItem = new CChoise(TYPE_SCHOISE == nType);
			break;
			
		case TYPE_JUDGE:
			pItem = new CJudge;
			break;
			
		case TYPE_BLANK:
			pItem = new CBlank;
			break;

		case TYPE_TEXT:
			pItem = new CText;
			break;

		case TYPE_GROUP:
			pItem = new CItemGroup;
			break;

		default:
			return Q_ERROR(ERROR_READ_FILE_FAIL);
		}
		
		if (!pItem)
		{
			return Q_ERROR(ERROR_ALLOC_FAIL);
		}
		
		// ���������л����ݣ���һ����ȡʧ�ܣ���ֱ�ӷ���
		if (!pItem->LoadFile(fp, m_dwVersion))
		{
			delete pItem;
			m_bModify = FALSE;
			return Q_RET(TRUE);
		}
		
		pItem->SetCustomIndex(i);
		if (!AddItem(pItem, TYPE_DEFAULT))
		{
			delete pItem;
			m_bModify = FALSE;
			return Q_RET(TRUE);
		}

		// ����һ��CRC��
		pItem->CalcCRC();
	}

	m_bModify = FALSE;
	return Q_TRUE;
}

EBOOL CPaper::LoadXML(PCHAR b)
{
	TiXmlDocument Doc;
	TiXmlElement *pLibrary = NULL;
	TiXmlElement *pNode = NULL;
	LPTSTR lpString = NULL;
	int nEncode = CS_UTF8;
	int nType = TYPE_DEFAULT;
	EBOOL bRet = FALSE;
	
	Doc.Parse(b);
	if (Doc.Error())
	{
		return Q_ERROR(ERROR_XML_PARSE_FAIL);
	}

	// ������
	TiXmlDeclaration *pDeclear = Doc.FirstChild()->ToDeclaration();
	if (!pDeclear)
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}
	
	if (!strcmp(pDeclear->Encoding(), xml_lpszDeclar_Encoding))
	{
		nEncode = CS_UTF8;
	}
	else
	{
		// �������β�����Windows�ķ���
		HDC hDC = GetDC(NULL);
		switch(GetTextCharset(hDC))
		{
		case ANSI_CHARSET:
			nEncode = CS_UTF8;
			break;
			
		case GB2312_CHARSET:
			nEncode = CS_ANSI;
			break;
			
		default:
			ReleaseDC(NULL, hDC);
			return Q_ERROR(ERROR_INVALID_TEXT_CHARSET);
		}
		ReleaseDC(NULL, hDC);
	}

	// �����ڵ�
	pLibrary = Doc.RootElement();
	if (!pLibrary || strcmp(pLibrary->Value(), xml_lpszNode_Library))
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}

	// ����ʶ
	pNode = pLibrary->FirstChildElement(xml_lpszNode_Key);
	if (!pNode || strcmp(pNode->GetText(), lpszAppKey))
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}

	// �������
	pNode = pLibrary->FirstChildElement(xml_lpszNode_Title);
	if (!pNode || !(lpString = CTextManager::SwitchLoad(pNode->GetText(), nEncode, NULL)))
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}
	if (!CItem::SetDescription(lpString))
	{
		return Q_ERROR(ERROR_INNER);
	}
	free(lpString);

	// ������Ϣ
	pNode = pLibrary->FirstChildElement(xml_lpszNode_Cover);
	if (pNode)
	{
		int r, g, b;

		// �����ǳ�ʼ�����ʲ��漰�����б�����
		ClearCoverInfo(&m_sCover);

		m_sCover.nType = PAPER_COVERTYPE_DEFAULT;
		if (!pNode->Attribute(xml_lpszAttribute_Width, &(m_sCover.nWidth)))
		{
			m_sCover.nWidth = 300;
		}
		else
		{
			m_sCover.nWidth = max(1, m_sCover.nWidth);
		}

		if (!pNode->Attribute(xml_lpszAttribute_Width, &(m_sCover.nHeight)))
		{
			m_sCover.nHeight = 300;
		}
		else
		{
			m_sCover.nHeight = max(1, m_sCover.nHeight);
		}
		
		lpString = CTextManager::SwitchLoad(pNode->Attribute(xml_lpszAttribute_ColorBk), nEncode, NULL);
		if (lpString)
		{
			_stscanf(lpString, _T("#%02X%02X%02X"), &r, &g, &b);
			free(lpString);
			m_sCover.clrBk = RGB(r, g, b);
		}
		else
		{
			m_sCover.clrBk = RGB(255, 255, 255);
		}

		lpString = CTextManager::SwitchLoad(pNode->Attribute(xml_lpszAttribute_ColorText), nEncode, NULL);
		if (lpString)
		{
			_stscanf(lpString, _T("#%02X%02X%02X"), &r, &g, &b);
			free(lpString);
			m_sCover.clrText = RGB(r, g, b);
		}
		else
		{
			m_sCover.clrText = RGB(0, 0, 0);
		}

		if (!pNode->Attribute(xml_lpszAttribute_Quality, &(m_sCover.nQuality)))
		{
			m_sCover.nQuality = 85;
		}
		else
		{
			m_sCover.nQuality = max(0, m_sCover.nQuality);
		}

		if (pNode->GetText())
		{
			lpString = CTextManager::SwitchLoad(pNode->GetText(), nEncode, NULL);
			if (!lpString)
			{
				return Q_ERROR(ERROR_INNER);
			}
			m_sCover.strTitle = lpString;
		}
		else
		{
			m_sCover.strTitle = NULL;
		}
	}
	else
	{
		if (m_sCover.ulDataSize > 0)
		{
			m_sCover.nType = PAPER_COVERTYPE_CUSTOM;
		}
		else
		{
			m_sCover.nType = PAPER_COVERTYPE_NONE;
		}
	}

	// �Զ�����Ŀ����
	// Ϊ�˼����ԣ������ж�������Ƿ��в���index���Ե���
	// ���������������index�Ű���index���ж�ȡ�����򲻼��index
	int nRight = 1, nLoad = 0;
	BOOL bWithIndex = TRUE;
	pNode = pLibrary->FirstChildElement(xml_lpszNode_Custom);
	while(pNode)
	{
		if (!pNode->Attribute(xml_lpszAttribute_Index, &nLoad) ||
			nLoad < 0)
		{
			bWithIndex = FALSE;
			break;
		}
		pNode = pNode->NextSiblingElement(xml_lpszNode_Custom);
	}
	
	CTempList *pList = NULL;
	if (!bWithIndex)
	{
		pNode = pLibrary->FirstChildElement(xml_lpszNode_Custom);
	}
	while(1)
	{
		if (bWithIndex)
		{
			pNode = pLibrary->FirstChildElement(xml_lpszNode_Custom);
			while(pNode)
			{
				pNode->Attribute(xml_lpszAttribute_Index, &nLoad);
				if (nLoad == nRight)
				{
					break;
				}
				pNode = pNode->NextSiblingElement(xml_lpszNode_Custom);
			}
			// δ�ҵ���Ӧ���Զ������ͣ��Ͳ�����
			if (!pNode)
			{
				break;
			}
		}
		nType = XMLGetTypeIndex(pNode->Attribute(xml_lpszAttribute_Type));
		pList = new CTempList(nType);
		if (!pList)
		{
			return Q_ERROR(ERROR_ALLOC_FAIL);
		}
		pList->SetCustomIndex(nRight);

		lpString = CTextManager::SwitchLoad(pNode->GetText(), nEncode, NULL);
		if (!lpString)
		{
			delete pList;
			return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
		}
		
		bRet = pList->SetDescription(lpString);
		free(lpString);
		if (!bRet)
		{
			return Q_ERROR(ERROR_INNER);
		}

		if (!pNode->Attribute(xml_lpszAttribute_QNum, &nLoad))
		{
			nLoad = 0;
		}
		pList->SetQNumber(nLoad);
		
		CList::AddItem(pList);
		++nRight;

		if (!bWithIndex)
		{
			pNode = pNode->NextSiblingElement(xml_lpszAttribute_Custom);
			if (!pNode)
			{
				break;
			}
		}
	}

	// ��ȡ��Ŀ
	// �����°���ļ��У���Ŀ��Ҳ����qstn���д洢
	pNode = pLibrary->FirstChildElement(xml_lpszNode_Question);
	CItem *pItem = NULL;
	int nGroup = 0;
	while(pNode)
	{
		switch(XMLGetTypeIndex(pNode->Attribute(xml_lpszAttribute_Type)))
		{
		case TYPE_SCHOISE:
			pItem = new CChoise(TRUE);
			break;

		case TYPE_MCHOISE:
			pItem = new CChoise(FALSE);
			break;

		case TYPE_JUDGE:
			pItem = new CJudge;
			break;

		case TYPE_BLANK:
			pItem = new CBlank;
			break;

		case TYPE_TEXT:
			pItem = new CText;
			break;

		case TYPE_GROUP:
			pItem = new CItemGroup;
			++nGroup; // ͳ��qstn��Ŀ��
			break;

		default:
			pItem = NULL;
		}

		if (pItem)
		{
			if (!pItem->ReadXML(pNode, nEncode, FALSE) || !AddItem(pItem, TYPE_DEFAULT))
			{
				delete pItem;
			}
			else
			{
				pItem->CalcCRC();
			}
		}

		pNode = pNode->NextSiblingElement(xml_lpszNode_Question);
	}

	// ���¸����Զ�����Ŀ�����
	// ��ֹxml�ļ����Զ�����Ŀ�鲻��1��ʼ
	if (GetCustomTypeCount())
	{
		int nCust = 1;
		pList = GetTypeList(TYPE_CUSTOMRIGHT);
		while(pList)
		{
			pList->SetCustomIndex(nCust++);
			pList = (CTempList*)pList->GetNext();
		}
	}

	// Ϊ�˶Ծɰ���м��ݣ�����qstn��Ŀ��ʱ��
	// ����ȡgrp��Ŀ�飬
	// ���򣬽������ȡ�κ�grp��Ŀ��
	if (!nGroup)
	{
		CItemGroup *pGroup = NULL;
		pNode = pLibrary->FirstChildElement(xml_lpszNode_Group);
		while(pNode)
		{
			pGroup = new CItemGroup;
			if (!pGroup)
			{
				break;
			}
			
			if (!pGroup->ReadXML(pNode, nEncode, TRUE) || !AddItem(pGroup, TYPE_DEFAULT))
			{
				delete pGroup;
			}
			else
			{
				pGroup->CalcCRC();
			}
			
			pNode = pNode->NextSiblingElement(xml_lpszNode_Group);
		}
	}
	
	m_bModify = FALSE;
	return Q_RET(TRUE);
}

/*
���ܣ�	��2.x��׼����ʽ��.qux��ʽ������
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	
*/
EBOOL CPaper::SavePaperM(FILE *fp)
{
	size_t size, xmlsize;

	// ���ͷ��
	if (!fwrite(lpszAppKey, 9 * sizeof(char), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// 4�ֽڰ汾��
	m_dwVersion = FILEVERSION;
	if (!fwrite(&m_dwVersion, sizeof(DWORD), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// ����Ƿ�����
	if (!fwrite(&m_nEncryption, sizeof(BYTE), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
	if (IsLocked())
	{
		if (!fwrite(m_uMD5, sizeof(m_uMD5), 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		if (AES_OK != AES_SetKey((byte*)m_cPassword, strlen(m_cPassword)))
		{
			return Q_ERROR(ERROR_ENCRYPT_FAIL);
		}
	}

	// ת��ΪXML��ʽ
	TiXmlDocument Doc;
	if (!PaperToXML(&Doc, FALSE))
	{
		return Q_ERROR(ERROR_INNER);
	}
	TiXmlPrinter Printer;
	if (!Doc.Accept(&Printer))
	{
		return Q_ERROR(ERROR_XML_ENCODE_FAIL);
	}
	xmlsize = Printer.Size() + 1;
	
	// ���ѹ����Ĵ�С
	ULONG ulC = compressBound(xmlsize);
	LPBYTE p = (LPBYTE)malloc(ulC);
	if (!p)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	if (Z_OK != compress(p, &ulC, (LPBYTE)Printer.CStr(), xmlsize))
	{
		free(p);
		return Q_ERROR(ERROR_COMPRESS_FAIL);
	}

	// ���ѹ����ĳ��ȳ���ѹ��ǰ
	if (xmlsize < ulC)
	{
		// ����ѹ����Ϣ��ֱ�ӱ���Ϊԭ����
		// ����ʱ������ĩβ��0
		free(p);
		p = (LPBYTE)_tcsdup((LPCTSTR)Printer.CStr());
		if (!p)
		{
			return Q_ERROR(ERROR_ALLOC_FAIL);
		}
		size = xmlsize;
		m_nCompressType = PAPER_COMPRESS_NO;
	}
	// ���ѹ���󳤶Ȳ�����ѹ��ǰ
	else
	{
		size = ulC;
		m_nCompressType = PAPER_COMPRESS_ZLIB;
	}
	if (!fwrite(&m_nCompressType, sizeof(BYTE), 1, fp))
	{
		free(p);
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// �������ݿ��С
	// ���������ǰ�Ĵ�С
	// ���ܹ����п��ܲ������룬��ʵ�����ݿ��С������˲�ͬ
	// �ڶ�ȡʱ��Ҫ�������
	if (!fwrite(&size, sizeof(DWORD), 1, fp))
	{
		free(p);
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// ��������
	if (IsLocked())
	{
		size_t alignsize;
		AES_Encrypt_CBC(NULL, size, NULL, &alignsize);
		byte* e = (byte*)malloc(alignsize);
		if (!e)
		{
			free(p);
			return Q_ERROR(ERROR_ALLOC_FAIL);
		}
		// ����Ĳ�����0xcd��ȫ
		memcpy(e, p, size);
		memset(e + size, 0xcd, alignsize - size);
		if (AES_OK != AES_Encrypt_CBC(e, alignsize, e, &alignsize))
		{
			free(e);
			return Q_ERROR(ERROR_ENCRYPT_FAIL);
		}
		free(p);
		p = e;
		size = alignsize;
	}

	// ���ѹ����ʽΪzlib
	if (PAPER_COMPRESS_ZLIB == m_nCompressType)
	{
		// ѹ��ǰ����
		if (!fwrite(&xmlsize, sizeof(DWORD), 1, fp))
		{
			free(p);
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
	}

	// ѹ��������
	if (!fwrite(p, size, 1, fp))
	{
		free(p);
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
	free(p);

	return Q_TRUE;
}

/*
���ܣ�	��1.x��׼����ʽ��.quiz��ʽ������
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	1.x���һ��İ汾��Ϊ3
*/
EBOOL CPaper::SavePaperF(FILE *fp)
{
	ASSERT(fp);

	int i;

	// ���ͷ��
	if (!fwrite(lpszAppKey, 9 * sizeof(char), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// д��汾��Ϣ
	i = FILEVERSION_1X;
	if (!fwrite(&i, sizeof(DWORD), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// ���ȴ洢�������
	if (!CTextManager::SaveFileString(fp, GetDescription(), sizeof(WORD)))
	{
		return Q_ERROR(ERROR_INNER);
	}

	// ����Ƿ�����
	i = IsLocked();
	if (!fwrite(&i, sizeof(BYTE), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
	if (IsLocked())
	{
		if (!fwrite(m_uMD5, sizeof(m_uMD5), 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
	}

	// VERSION2�������ܣ��Զ�����Ŀ����
	int nCustomCount = GetCustomTypeCount();
	if (m_dwVersion >= 2)
	{
		// д���Զ���������
		if (EOF == fputc(nCustomCount, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}

		if (nCustomCount > 0)
		{
			int nMax = 0, nLen = 0;
			
			CItem *pCust = GetTypeList(TYPE_CUSTOMRIGHT);
			while(pCust)
			{
				if (pCust->IsUndoValid(&m_sUndoStep))
				{
					nLen = pCust->GetDescriptionSaveLen();
					nMax = max(nLen, nMax);
				}

				pCust = pCust->GetNext();
			}
			// ��󻺳��ֽ���
			if (!fwrite(&nMax, sizeof(DWORD), 1, fp))
			{
				return Q_ERROR(ERROR_WRITE_FILE_FAIL);
			}

			// ����д���Զ�������
			pCust = GetTypeList(TYPE_CUSTOMRIGHT);
			while(pCust)
			{
				i = pCust->GetRawType();
				if (!fwrite(&i, sizeof(DWORD), 1, fp))
				{
					return Q_ERROR(ERROR_WRITE_FILE_FAIL);
				}

				if (!CTextManager::SaveFileString(fp, pCust->GetDescription(), sizeof(short)))
				{
					return Q_ERROR(ERROR_WRITE_FILE_FAIL);
				}

				pCust = pCust->GetNext();
			}
		}
	}
	
	CItem *pItem = m_pItemList->GetHead();
	while(pItem)
	{
		if (pItem->IsUndoValid(&m_sUndoStep))
		{
			if (1 == m_dwVersion)
			{
				i = pItem->GetType();
				if (!fwrite(&i, sizeof(BYTE), 1, fp))
				{
					return Q_ERROR(ERROR_WRITE_FILE_FAIL);
				}
			}
			else
			{
				i = pItem->GetRawType();
				if (!fwrite(&i, sizeof(DWORD), 1, fp))
				{
					return Q_ERROR(ERROR_WRITE_FILE_FAIL);
				}
			}

			if (!pItem->SaveFile(fp))
			{
				return Q_ERROR(ERROR_INNER);
			}
		}

		pItem = pItem->GetNext();
	}
	
	return Q_TRUE;
}

EBOOL CPaper::SaveXML(FILE *fp, BOOL bEncodeMBCS)
{
	ASSERT(fp);

	TiXmlDocument Doc;
	if (!PaperToXML(&Doc, bEncodeMBCS))
	{
		return Q_ERROR(ERROR_XML_PARSE_FAIL);
	}
	
	if (!Doc.SaveFile(fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	return Q_TRUE;
}

EBOOL CPaper::PaperToXML(TiXmlDocument *pDoc, BOOL bEncodeMBCS)
{
	TiXmlElement *pNode = NULL;
	TiXmlText *pText = NULL;
	LPSTR lpString = NULL;
	int nEncode = m_bEncodeMBCS ? CS_ANSI : CS_UTF8;
	TCHAR szVersion[64] = {0};
	LPCSTR lpEncodeStr;

	if (bEncodeMBCS)
	{
		// �������β�����Windows�ķ���
		HDC hDC = GetDC(NULL);
		switch(GetTextCharset(hDC))
		{
		case ANSI_CHARSET:
			lpEncodeStr = xml_lpszDeclar_Encoding;
			break;

		case GB2312_CHARSET:
			lpEncodeStr = xml_lpszDeclar_Encoding95;
			break;

		default:
			ReleaseDC(NULL, hDC);
			return Q_ERROR(ERROR_INVALID_TEXT_CHARSET);
		}
		ReleaseDC(NULL, hDC);
	}
	else
	{
		lpEncodeStr = xml_lpszDeclar_Encoding;
	}

	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", lpEncodeStr, "");
	if (!pDeclaration)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	pDoc->LinkEndChild(pDeclaration);

	TiXmlElement *pRoot = new TiXmlElement(xml_lpszNode_Library);
	if (!pRoot)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	pDoc->LinkEndChild(pRoot);

	// �����
	pNode = new TiXmlElement(xml_lpszNode_Key);
	if (!pNode)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	pRoot->LinkEndChild(pNode);
	pText = new TiXmlText(lpszAppKey);
	if (!pText)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	pNode->LinkEndChild(pText);

	// �汾�ţ��Ķΰ汾�ţ���ȡʱ������飩
	pNode = new TiXmlElement(xml_lpszNode_Version);
	if (pNode)
	{
		pRoot->LinkEndChild(pNode);
		_sntprintf(szVersion, 64, _T("%d.%d.%d.%d"), FILEVERSION_MAJOR, FILEVERSION_MINOR, FILEVERSION_PATCH, FILEVERSION_ADDITION);
		lpString = CTextManager::SwitchSave(szVersion, nEncode, NULL);
		if (lpString)
		{
			pText = new TiXmlText(lpString);
			free(lpString);
			if (pText)
			{
				pNode->LinkEndChild(pText);
			}
		}
	}

	// ������
	pNode = new TiXmlElement(xml_lpszNode_Title);
	if (!pNode)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	pRoot->LinkEndChild(pNode);
	lpString = CTextManager::SwitchSave(GetDescription(), nEncode, NULL);
	if (!lpString)
	{
		return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
	}
	pText = new TiXmlText(lpString);
	free(lpString);
	if (!pText)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	pNode->LinkEndChild(pText);

	// ������Ϣ
	LPCPAPERCOVERINFO ppc = GetCoverInfo();
	if (ppc->nType == PAPER_COVERTYPE_DEFAULT)
	{
		pNode = new TiXmlElement(xml_lpszNode_Cover);
		if (!pNode)
		{
			return Q_ERROR(ERROR_ALLOC_FAIL);
		}
		pRoot->LinkEndChild(pNode);

		pNode->SetAttribute(xml_lpszAttribute_Width, ppc->nWidth);
		pNode->SetAttribute(xml_lpszAttribute_Height, ppc->nHeight);

		char c[10] = {0};
		sprintf(c, "#%02X%02X%02X", GetRValue(ppc->clrBk), GetGValue(ppc->clrBk), GetBValue(ppc->clrBk));
		pNode->SetAttribute(xml_lpszAttribute_ColorBk, c);
		sprintf(c, "#%02X%02X%02X", GetRValue(ppc->clrText), GetGValue(ppc->clrText), GetBValue(ppc->clrText));
		pNode->SetAttribute(xml_lpszAttribute_ColorText, c);

		pNode->SetAttribute(xml_lpszAttribute_Quality, ppc->nQuality);

		lpString = CTextManager::SwitchSave(ppc->strTitle, nEncode, NULL);
		pText = new TiXmlText(lpString);
		free(lpString);
		if (!pText)
		{
			return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
		}
		pNode->LinkEndChild(pText);
	}
	
	// �Զ�����Ŀ��Ϣ
	CItem *pList = GetTypeList(TYPE_CUSTOMRIGHT);
	while(pList)
	{
		if (pList->IsUndoValid(&m_sUndoStep))
		{
			pNode = new TiXmlElement(xml_lpszNode_Custom);
			if (!pNode)
			{
				return Q_ERROR(ERROR_ALLOC_FAIL);
			}
			pRoot->LinkEndChild(pNode);
			pNode->SetAttribute(xml_lpszAttribute_Type, XMLGetTypeAttribute(pList->GetType()));
			pNode->SetAttribute(xml_lpszAttribute_Index, pList->GetCustomIndex());
			if (((CList*)pList)->GetQNumber())
			{
				pNode->SetAttribute(xml_lpszAttribute_QNum, ((CList*)pList)->GetQNumber());
			}
			
			lpString = CTextManager::SwitchSave(pList->GetDescription(), nEncode, NULL);
			if (!lpString)
			{
				return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
			}
			pText = new TiXmlText(lpString);
			free(lpString);
			if (!pText)
			{
				return Q_ERROR(ERROR_ALLOC_FAIL);
			}
			pNode->LinkEndChild(pText);
		}

		pList = pList->GetNext();
	}

	// ��Ŀ���ݣ�������Ŀ����Ŀ�飩
	CItem *pItem = m_pItemList->GetHead();
	while(pItem)
	{
		if (pItem->IsUndoValid(&m_sUndoStep))
		{
			if (!pItem->WriteXML(pRoot, nEncode, FALSE))
			{
				return Q_ERROR(ERROR_INNER);
			}
		}
		
		pItem = pItem->GetNext();
	}

	// �����ѡ��������
	// ����Ҫ����Ŀ�鰴grp��qstn��������
	if (m_bMaxCompatible)
	{
		pItem = m_pItemList->GetHead();
		while(pItem)
		{
			if (pItem->IsUndoValid(&m_sUndoStep) &&
				pItem->GetType() == TYPE_GROUP)
			{
				if (!pItem->WriteXML(pRoot, nEncode, TRUE))
				{
					return Q_ERROR(ERROR_INNER);
				}
			}

			pItem = pItem->GetNext();
		}
	}

	return Q_TRUE;
}

void CPaper::LoadSave(LPCTSTR strFilePath)
{
	FILE *fp = NULL;

	if (strFilePath)
	{
		fp = _tfopen(strFilePath, _T("rb"));
	}
	else
	{
		TCHAR strSaveFile[MAX_PATH] = {0};
		GetSaveFilePath(strSaveFile);
		
		fp = _tfopen(strSaveFile, _T("rb"));
	}
	if (!fp)
	{
		return;
	}

	char Head[15] = {0};
	// �����ʼ��һ��QuickQuiz_Save����
	if (!fread(Head, 14 * sizeof(char), 1, fp))
	{
		fclose(fp);
		return;
	}
	if (strncmp(Head, "QuickQuiz_Save", 14))
	{
		fclose(fp);
		return;
	}
	
	CItem *pItem = m_pItemList->GetHead();
	while(pItem)
	{
		if (pItem->IsUndoValid(&m_sUndoStep))
		{
			if (!pItem->LoadStore(fp))
			{
				break;
			}
		}
		
		pItem = pItem->GetNext();
	}
	
	fclose(fp);
	SaveStoreQuestions(NULL);
}

void CPaper::SaveSave()
{
	TCHAR strSaveFile[MAX_PATH] = {0};
	GetSaveFilePath(strSaveFile);

	if (!m_pSaveList->GetItemCount())
	{
		_tremove(strSaveFile);
		return;
	}

	FILE *fp = _tfopen(strSaveFile, _T("wb"));
	if (!fp)
	{
		return;
	}

	if (!fwrite(lpszSaveKey, strlen(lpszSaveKey) * sizeof(char), 1, fp))
	{
		fclose(fp);
		return;
	}

	CItem *pItem = m_pItemList->GetHead();
	while(pItem)
	{
		if (pItem->IsUndoValid(&m_sUndoStep))
		{
			if (!pItem->SaveStore(fp))
			{
				break;
			}
		}

		pItem = pItem->GetNext();
	}

	fclose(fp);
}
