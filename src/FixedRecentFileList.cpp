// FixedRecentFileList.cpp: implementation of the CFixedRecentFileList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FixedRecentFileList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFixedRecentFileList::CFixedRecentFileList(UINT nStart, LPCTSTR lpszSection,
										   LPCTSTR lpszEntryFormat, int nSize,
										   int nMaxDispLen /* = AFX_ABBREV_FILENAME_LEN */) :
	CRecentFileList(nStart, lpszSection, lpszEntryFormat, nSize, nMaxDispLen)
{

}

CFixedRecentFileList::~CFixedRecentFileList()
{

}

// 摘自FileList.cpp
// 解决当最近文件无法在弹出菜单中显示的问题
void CFixedRecentFileList::UpdateMenu(CCmdUI* pCmdUI)
{
	ASSERT(m_arrNames != NULL);
	
	CMenu* pMenu = pCmdUI->m_pMenu;
	if (m_strOriginal.IsEmpty() && pMenu != NULL)
		pMenu->GetMenuString(pCmdUI->m_nID, m_strOriginal, MF_BYCOMMAND);
	
	if (m_arrNames[0].IsEmpty())
	{
		// 把除了第一个的列表都清空
		for (int iMRU = 1; iMRU < m_nSize; iMRU++)
			pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);

		// no MRU files
		if (!m_strOriginal.IsEmpty())
			pCmdUI->SetText(m_strOriginal);
		pCmdUI->Enable(FALSE);
		return;
	}
	
	if (pCmdUI->m_pMenu == NULL)
		return;

	// 如果是弹出式菜单，就跳过
	// 防止无法在弹出菜单中显示最近文件
	if (pCmdUI->m_pSubMenu)
		return;
	
	// 删除全部菜单项
	for (int iMRU = 0; iMRU < m_nSize; iMRU++)
		pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
	
	TCHAR szCurDir[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, szCurDir);
	int nCurDir = lstrlen(szCurDir);
	ASSERT(nCurDir >= 0);
	szCurDir[nCurDir] = '\\';
	szCurDir[++nCurDir] = '\0';
	
	CString strName;
	CString strTemp;
	for (iMRU = 0; iMRU < m_nSize; iMRU++)
	{
		if (!GetDisplayName(strName, iMRU, szCurDir, nCurDir))
			break;
		
		// double up any '&' characters so they are not underlined
		LPCTSTR lpszSrc = strName;
		LPTSTR lpszDest = strTemp.GetBuffer(strName.GetLength()*2);
		while (*lpszSrc != 0)
		{
			if (*lpszSrc == '&')
				*lpszDest++ = '&';
			if (_istlead(*lpszSrc))
				*lpszDest++ = *lpszSrc++;
			*lpszDest++ = *lpszSrc++;
		}
		*lpszDest = 0;
		strTemp.ReleaseBuffer();
		
		// insert mnemonic + the file name
		TCHAR buf[10];
		_sntprintf(buf, 10, _T("&%d "), (iMRU+1+m_nStart) % 10);
		pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++,
			MF_STRING | MF_BYPOSITION, pCmdUI->m_nID++,
			CString(buf) + strTemp);
	}
	
	// update end menu count
	pCmdUI->m_nIndex--; // point to last menu added
	pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();
	
	pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
}

void CFixedRecentFileList::ClearList()
{
	int i;
	for (i = 0; i < m_nSize; i++)
	{
		m_arrNames[i].Empty();
	}
}