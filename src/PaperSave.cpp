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
	// 这里首先清理错误信息，因为在读取过程中可能有不影响读取但有错误的情况
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

	// 读取开头的JPEG部分
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
			// 这里是初始化，故不涉及撤销列表问题
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

	// 如果带有封面却实际没保存为带封面的格式，则此处生成封面
	if (m_sCover.nType != PAPER_COVERTYPE_NONE && !m_sCover.ulDataSize)
	{
		if (!MakeJpegCover(NULL, &m_sCover))
		{
			return Q_ERROR(ERROR_INNER);
		}
	}

	// 重新生成类别列表
	ResetTypeList();

	// 读取收藏题目
	LoadSave(NULL);
	
	m_bModify = FALSE;
	m_bCanEdit = FALSE;

	// 这里返回TRUE，是为了不清理有错误但不影响读取的问题
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
		// 当保存为兼容模式时，必然退出加密
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
	
	// 保存错题本信息
	SaveSave();
	
	// 保存之后保存序列号
	m_sUndoStep.nSaveIndex = m_sUndoStep.nCurStep;
	
	if (strFilePath)
	{
		_tcsncpy(m_strFilePath, strFilePath, MAX_PATH);
	}
	
	return Q_TRUE;
}

/*
功能：	读取2.x标准题库格式（.qux格式）
返回值：成功返回TRUE，否则返回FALSE
备注：	不能用来读取1.x标准题库格式（.quiz格式）
*/
EBOOL CPaper::LoadPaperM(LPBYTE p, DWORD dwSize, password funcPass)
{
	if (!p || !dwSize)
	{
		return Q_ERROR(ERROR_INNER);
	}

	DWORD dwReadSize = 0, dwRead = 0;

	// 题库起始是一个QuickQuiz字样
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

	// 然后是四个字节的版本信息
	// 所有新版格式均视为4，后续不再更改
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

	// 题库是否被锁定
	dwReadSize = sizeof(BYTE);
	if (dwRead + dwReadSize > dwSize)
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}
	m_nEncryption = (int)*p;
	dwRead += dwReadSize;
	p += dwReadSize;

	// 如果题库被锁定了
	if (IsLocked())
	{
		// 4个字节的MD5密码
		dwReadSize = sizeof(m_uMD5);
		if (dwRead + dwReadSize > dwSize)
		{
			return Q_ERROR(ERROR_FILE_INCOMPLETE);
		}
		memcpy(m_uMD5, p, sizeof(m_uMD5));
		dwRead += dwReadSize;
		p += dwReadSize;

		// 调用接口函数获得密码字串
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

	// 一字节压缩方式
	// 0为不压缩，1为zlib压缩
	// 不压缩的情况是压缩后文件大小比不压缩还大
	dwReadSize = sizeof(BYTE);
	if (dwRead + dwReadSize > dwSize)
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}
	m_nCompressType = (BOOL)*p;
	dwRead += dwReadSize;
	p += dwReadSize;

	// 四字节后续数据块大小
	// 原始数据块的长度是包括末尾0的
	ULONG ulLenData = 0, ulLenRaw = 0;
	dwReadSize = sizeof(ULONG);
	if (dwRead + dwReadSize > dwSize)
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}
	memcpy(&ulLenData, p, dwReadSize);
	dwRead += dwReadSize;
	p += dwReadSize;

	// 长度超出内存
	if (dwRead + ulLenData > dwSize)
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}

	// 如果是压缩题库
	if (PAPER_COMPRESS_ZLIB == m_nCompressType)
	{
		// 四字节压缩前长度
		dwReadSize = sizeof(ULONG);
		if (dwRead + dwReadSize > dwSize)
		{
			return Q_ERROR(ERROR_FILE_INCOMPLETE);
		}
		memcpy(&ulLenRaw, p, dwReadSize);
		dwRead += dwReadSize;
		p += dwReadSize;
	}

	// 解密题库内容
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

	// 如果为zlib压缩，解压缩
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

	// 解压后的字符以XML解析
	int ret = LoadXML((char*)n);
	free(n);
	if (!ret)
	{
		return Q_ERROR(ERROR_INNER);
	}
	
	return Q_RET(TRUE);
}

/*
功能：	按1.x版本生成的题库进行读取
返回值：成功则返回TRUE，否则返回FALSE
备注：	只能读取到版本号3
*/
EBOOL CPaper::LoadPaperF(FILE *fp, password funcPass)
{
	ASSERT(fp);

	int i = 0, nType = TYPE_DEFAULT;
	CItem *pItem = NULL;
	LPWSTR ptr = NULL;
	char Head[10] = {0};
	int nCustomCount = 0;

	// 题库起始是一个QuickQuiz字样
	if (!fread(Head, 9 * sizeof(char), 1, fp))
	{
		return Q_ERROR(ERROR_READ_FILE_FAIL);
	}
	if (strncmp(Head, lpszAppKey, 9))
	{
		return Q_ERROR(ERROR_HEAD_NOT_FOUND);
	}

	// 然后是四个字节的版本信息
	if (!fread(&m_dwVersion, sizeof(DWORD), 1, fp))
	{
		return Q_ERROR(ERROR_READ_FILE_FAIL);
	}
	if (m_dwVersion > FILEVERSION_1X)
	{
		return Q_ERROR(ERROR_INVALID_VERSION);
	}

	// 先读取题库名称
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

	// 题库是否被锁定
	m_nEncryption = fgetc(fp);
	if (EOF == m_nEncryption)
	{
		m_nEncryption = ENCRYPT_NO;
		return Q_ERROR(ERROR_READ_FILE_FAIL);
	}
	// 如果题库被锁定了，则紧跟4个字节的MD5密码
	else if (IsLocked())
	{
		if (!fread(m_uMD5, sizeof(m_uMD5), 1, fp))
		{
			return Q_ERROR(ERROR_READ_FILE_FAIL);
		}

		// 调用接口函数获得密码字串
		if (!funcPass(m_cPassword, m_uMD5))
		{
			m_cPassword[0] = 0;
			return Q_ERROR_NOMSG;
		}
	}

	// VERSION2新增功能：自定义题目类型
	// 最大支持10个自定义题目类型
	if (m_dwVersion >= 2)
	{
		// 读入自定义类型数
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
			// 最大缓冲字节数
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
			// 依次读入自定义类型名
			for (i = 0; i < nCustomCount; i++)
			{
				// 读入题目类型
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
		// 读取题目类型
		// VERSION2以后类型占用4字节
		if (m_dwVersion == 1)
		{
			i = 0;
			if (!fread(&i, sizeof(BYTE), 1, fp))
			{
				if (feof(fp))
				{
					// 这里跳出循环，意味着正常结束
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
					// 这里跳出循环，意味着正常结束
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
		
		// 由于是序列化内容，故一旦读取失败，则直接返回
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

		// 计算一下CRC码
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

	// 检查编码
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
		// 这里无奈采用了Windows的方法
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

	// 检查根节点
	pLibrary = Doc.RootElement();
	if (!pLibrary || strcmp(pLibrary->Value(), xml_lpszNode_Library))
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}

	// 检查标识
	pNode = pLibrary->FirstChildElement(xml_lpszNode_Key);
	if (!pNode || strcmp(pNode->GetText(), lpszAppKey))
	{
		return Q_ERROR(ERROR_FILE_INCOMPLETE);
	}

	// 题库名称
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

	// 封面信息
	pNode = pLibrary->FirstChildElement(xml_lpszNode_Cover);
	if (pNode)
	{
		int r, g, b;

		// 这里是初始化，故不涉及撤销列表问题
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

	// 自定义题目类型
	// 为了兼容性，首先判断题库中是否有不带index属性的项
	// 必须所有项均包含index才按照index进行读取，否则不检查index
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
			// 未找到对应的自定义类型，就不找了
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

	// 读取题目
	// 在最新版的文件中，题目组也按照qstn进行存储
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
			++nGroup; // 统计qstn题目组
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

	// 重新更新自定义题目组序号
	// 防止xml文件中自定义题目组不从1开始
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

	// 为了对旧版进行兼容，当无qstn题目组时，
	// 将读取grp题目组，
	// 否则，将不会读取任何grp题目组
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
功能：	按2.x标准题库格式（.qux格式）保存
返回值：成功返回TRUE，否则返回FALSE
备注：	
*/
EBOOL CPaper::SavePaperM(FILE *fp)
{
	size_t size, xmlsize;

	// 题库头部
	if (!fwrite(lpszAppKey, 9 * sizeof(char), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// 4字节版本号
	m_dwVersion = FILEVERSION;
	if (!fwrite(&m_dwVersion, sizeof(DWORD), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// 题库是否被锁定
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

	// 转换为XML形式
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
	
	// 检查压缩后的大小
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

	// 如果压缩后的长度长于压缩前
	if (xmlsize < ulC)
	{
		// 丢弃压缩信息，直接保存为原数据
		// 保存时，包含末尾的0
		free(p);
		p = (LPBYTE)_tcsdup((LPCTSTR)Printer.CStr());
		if (!p)
		{
			return Q_ERROR(ERROR_ALLOC_FAIL);
		}
		size = xmlsize;
		m_nCompressType = PAPER_COMPRESS_NO;
	}
	// 如果压缩后长度不长于压缩前
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

	// 后续数据块大小
	// 仅保存加密前的大小
	// 加密过程中可能产生对齐，故实际数据块大小可能与此不同
	// 在读取时需要另外计算
	if (!fwrite(&size, sizeof(DWORD), 1, fp))
	{
		free(p);
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// 加密数据
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
		// 不足的部分用0xcd补全
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

	// 如果压缩格式为zlib
	if (PAPER_COMPRESS_ZLIB == m_nCompressType)
	{
		// 压缩前长度
		if (!fwrite(&xmlsize, sizeof(DWORD), 1, fp))
		{
			free(p);
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
	}

	// 压缩后内容
	if (!fwrite(p, size, 1, fp))
	{
		free(p);
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
	free(p);

	return Q_TRUE;
}

/*
功能：	按1.x标准题库格式（.quiz格式）保存
返回值：成功返回TRUE，否则返回FALSE
备注：	1.x最后一版的版本号为3
*/
EBOOL CPaper::SavePaperF(FILE *fp)
{
	ASSERT(fp);

	int i;

	// 题库头部
	if (!fwrite(lpszAppKey, 9 * sizeof(char), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// 写入版本信息
	i = FILEVERSION_1X;
	if (!fwrite(&i, sizeof(DWORD), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// 首先存储题库名称
	if (!CTextManager::SaveFileString(fp, GetDescription(), sizeof(WORD)))
	{
		return Q_ERROR(ERROR_INNER);
	}

	// 题库是否被锁定
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

	// VERSION2新增功能，自定义题目类型
	int nCustomCount = GetCustomTypeCount();
	if (m_dwVersion >= 2)
	{
		// 写入自定义类型数
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
			// 最大缓冲字节数
			if (!fwrite(&nMax, sizeof(DWORD), 1, fp))
			{
				return Q_ERROR(ERROR_WRITE_FILE_FAIL);
			}

			// 依次写入自定义类型
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
		// 这里无奈采用了Windows的方法
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

	// 题库标记
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

	// 版本号（四段版本号，读取时不做检查）
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

	// 题库标题
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

	// 封面信息
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
	
	// 自定义题目信息
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

	// 题目内容（包括题目和题目组）
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

	// 如果勾选了最大兼容
	// 则需要将题目组按grp和qstn保存两次
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
	// 题库起始是一个QuickQuiz_Save字样
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
