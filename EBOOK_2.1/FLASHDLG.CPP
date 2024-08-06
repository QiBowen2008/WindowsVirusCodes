// FlashDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EBook.h"
#include "FlashDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlashDlg dialog


CFlashDlg::CFlashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlashDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFlashDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Time=0;
}


void CFlashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlashDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFlashDlg, CDialog)
	//{{AFX_MSG_MAP(CFlashDlg)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_WELCOMEPAGE, OnWelcomepage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlashDlg message handlers

int CFlashDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	SetTimer(0,1000,NULL);
	return 0;
}

void CFlashDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
		if(Time++>2)
		EndDialog(TRUE);
	CDialog::OnTimer(nIDEvent);
}

BOOL CFlashDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
		KillTimer(0);
	return CDialog::DestroyWindow();
}

void CFlashDlg::OnWelcomepage() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(::GetDesktopWindow(),"open","http://grwy.online.ha.cn/eword",
		NULL,NULL,SW_MAXIMIZE );
}
