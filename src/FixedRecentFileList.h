// FixedRecentFileList.h: interface for the CFixedRecentFileList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIXEDRECENTFILELIST_H__19A79829_C769_4C48_B824_0E1AC10DE3B6__INCLUDED_)
#define AFX_FIXEDRECENTFILELIST_H__19A79829_C769_4C48_B824_0E1AC10DE3B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFixedRecentFileList : public CRecentFileList  
{
public:
	CFixedRecentFileList(UINT nStart, LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat, int nSize,
		int nMaxDispLen = AFX_ABBREV_FILENAME_LEN);
	virtual ~CFixedRecentFileList();

	virtual void UpdateMenu(CCmdUI* pCmdUI);
	void ClearList();
};

#endif // !defined(AFX_FIXEDRECENTFILELIST_H__19A79829_C769_4C48_B824_0E1AC10DE3B6__INCLUDED_)
