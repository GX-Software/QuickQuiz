// Test.cpp: implementation of the CQView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuickQuiz.h"

#include "MainFrm.h"
#include "Doc.h"
#include "View.h"

#include "Blank.h"

#include "TestSetDlg.h"
#include "TestFrame.h"
#include "TestMaker.h"
#include "BlkTxtCheckDlg.h"

#include "ViewDlg.h"

#include "TxtOutDlg.h"
#include "PaperMaker.h"
#include "TestCompetitiveDlg.h"

#include "ChooseTypeDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CQView::OnTestBegin() 
{
	CDoc *pDoc = GetDocument();
	if (!pDoc)
	{
		return;
	}

	CTestFrame *pTF = new CTestFrame;
	if (!pTF)
	{
		return;
	}
	if (!pTF->StartTest(pDoc))
	{
		delete pTF;
	}
}

void CQView::OnTestView() 
{
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc);
	ASSERT(pDoc->GetSelectedPaper());
	
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	
	CViewDlg Dlg(pDoc->GetSelectedPaper());
	Dlg.DoModal();
	
	pDoc->Store(NULL);
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
}

void CQView::OnTestMake() 
{
	CTMessageBox Box;
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc);
	ASSERT(pDoc->GetSelectedPaper());
	
	CTestSetDlg SetDlg(IDS_TEST_PAPERSET, pDoc->GetAllPapersList(), pDoc->GetSelectedPaper());
	if (IDOK != SetDlg.DoModal())
	{
		return;
	}
	
	CList List;
	CTestPara *pCPara = SetDlg.GetTestPara();
	PTESTPARA pPara = pCPara->GetTestPara();
	// 没有题目
	if (!pCPara->GetTotalCount())
	{
		return;
	}
	
	if (!pPara->pPaper->CreateTestList(pCPara, &List))
	{
		Box.SuperMessageBox(NULL, IDS_TEST_CREATETESTFAIL, IDS_MSGCAPTION_ERROR,
			MB_OK | MB_ICONERROR, 0, 0);
		return;
	}

	// 设定导出试卷的类型
	CTxtOutDlg TxtOutDlg(pPara->pPaper);
	if (IDOK != TxtOutDlg.DoModal())
	{
		return;
	}
	
	// 生成试卷
	CPaperMaker Maker;
	if (!Maker.PaperOut(&List, pCPara,
		TxtOutDlg.m_sFileFullPath, TxtOutDlg.m_nSel,
		TxtOutDlg.m_nAswStyle, TxtOutDlg.m_bWithRes,
		TxtOutDlg.m_sAnsiFont, TxtOutDlg.m_sMainFont, TxtOutDlg.m_sPreFont))
	{
		Box.SuperMessageBox(NULL, IDS_TEST_CREATETESTFAIL, IDS_MSGCAPTION_ERROR,
			MB_OK | MB_ICONERROR, 0, 0);
		return;
	}

	switch(TxtOutDlg.m_nSel)
	{
	case PAPERTYPE_TEXTONLY:
		ShellExecute(NULL, _T("open"), _T("notepad.exe"), TxtOutDlg.m_sFileFullPath,
			NULL, SW_SHOWNORMAL);
		break;

	case PAPERTYPE_HTMLONLY:
	case PAPERTYPE_HTMLPLUS:
		ShellExecute(NULL, _T("open"), TxtOutDlg.m_sFileFullPath, NULL,
			NULL, SW_SHOWNORMAL);
		break;
	}
}

void CQView::OnTestCompetitive() 
{
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc);
	ASSERT(pDoc->GetSelectedPaper());
	
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	
	CTestCompetitiveDlg Dlg(pDoc->GetSelectedPaper());
	Dlg.DoModal();
	
	pDoc->Store(NULL);
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
}
