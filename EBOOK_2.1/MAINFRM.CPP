// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "EBook.h"
#include "FlashDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define WM_LIBEN WM_USER+101
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()

	ON_MESSAGE(WM_LIBEN,OnLiben)
    ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_EBOOK,OnShowMyWindow)
    ON_COMMAND(ID_SUPPORT, OnSupport)
	ON_WM_SYSCOMMAND() 
	ON_BN_CLICKED(IDC_DOWNEWORD, OnDowneword)
	ON_BN_CLICKED(IDC_EMAIL, OnEmail)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
		CFlashDlg dlg;
	    dlg.DoModal();
    rect.left =100;
	rect.top=100;
	rect.right=700;
	rect.bottom=500;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
     NOTIFYICONDATA tnd;
    tnd.cbSize=sizeof(NOTIFYICONDATA);

     tnd.hWnd=this->m_hWnd;

     tnd.uID=IDR_MAINFRAME;

    tnd.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;

     tnd.uCallbackMessage=WM_LIBEN;

     tnd.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));

    strcpy(tnd.szTip,"EBOOK2000");

    Shell_NotifyIcon(NIM_ADD,&tnd);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
          cs.style=WS_POPUP;
          cs.dwExStyle |=WS_EX_TOOLWINDOW;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers




void CMainFrame::OnDowneword() 
{
	// TODO: Add your control notification handler code here

}

void CMainFrame::OnEmail() 
{
	// TODO: Add your control notification handler code here

}
void CMainFrame::OnAppExit() 
{
	// TODO: Add your command handler code here
	NOTIFYICONDATA tnid;

tnid.cbSize=sizeof(NOTIFYICONDATA);

tnid.hWnd=this->m_hWnd;

tnid.uID=IDR_MAINFRAME;//保证删除的是我们的图标

Shell_NotifyIcon(NIM_DELETE,&tnid);

AfxPostQuitMessage(0);

}
void CMainFrame::OnLiben(WPARAM wParam,LPARAM lParam)

{

UINT uID;//发出该消息的图标的ID

UINT uMouseMsg;//鼠标动作

POINT pt;

uID=(UINT) wParam;

uMouseMsg=(UINT) lParam;
CMenu menu;
if(uMouseMsg==WM_RBUTTONDOWN)//如果是单击右键

{

   switch(uID)

   {

    case IDR_MAINFRAME://如果是我们的图标

     GetCursorPos(&pt);//取得鼠标位置
     //执行相应操作
    menu.LoadMenu(IDR_BEGINMENU);
    pMenu=menu.GetSubMenu(0);
    ASSERT(pMenu!=0);
    pMenu->TrackPopupMenu (0,pt.x,pt.y,this);
       break;
   default:
    	break;
   }
}
   else if(uMouseMsg==WM_LBUTTONDOWN)//left button db click
   {
	   switch(uID){
       case IDR_MAINFRAME:
              OnShowMyWindow();
              break;
       default:
    	      break;
   }
   }
return; 

}

void CMainFrame::OnShowMyWindow()
{
 
    ModifyStyle(WS_POPUP,WS_OVERLAPPEDWINDOW); 
	ModifyStyleEx(WS_EX_TOOLWINDOW,WS_EX_TOPMOST);
   	ShowWindow(SW_NORMAL);
	MoveWindow(&rect,TRUE);
    }

void CMainFrame::OnSysCommand( UINT nID, LPARAM lParam )
{
  switch(nID)
  {
  case SC_RESTORE:   
        MoveMyWindow();
	     break;
  case  SC_MINIMIZE:
	     GetWindowRect(&rect);
         ShowWindow(SW_HIDE);
	     ModifyStyle(WS_CAPTION|FWS_PREFIXTITLE|WS_SYSMENU|
	     WS_MINIMIZEBOX|WS_MAXIMIZEBOX,WS_POPUP);
	     ModifyStyleEx(WS_EX_TOPMOST,WS_EX_TOOLWINDOW);
	break;
	     default:
		 Default();
         break;
   
  }
}
void CMainFrame::MoveMyWindow()
{
    ShowWindow(SW_NORMAL);
	MoveWindow(&rect,TRUE);
	UpdateWindow();
}

void CMainFrame::OnSupport()
{
   	ShellExecute(::GetDesktopWindow(),"open","http://grwy.online.ha.cn/eword",
		NULL,NULL,SW_MAXIMIZE );
}
