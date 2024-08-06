; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CEBookView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "EBook.h"
LastPage=0

ClassCount=7
Class1=CEBookApp
Class2=CEBookDoc
Class3=CEBookView
Class4=CMainFrame
Class6=CAboutDlg

ResourceCount=6
Resource1=IDD_ABOUTBOX
Resource2=IDR_MENU_TREE
Class5=CEBookSet
Resource3=IDD_EBOOK_FORM
Resource4=IDD_FLASHDLG
Class7=CFlashDlg
Resource5=IDR_BEGINMENU
Resource6=IDR_MAINFRAME

[CLS:CEBookApp]
Type=0
HeaderFile=EBook.h
ImplementationFile=EBook.cpp
Filter=N
LastObject=CEBookApp

[CLS:CEBookDoc]
Type=0
HeaderFile=EBookDoc.h
ImplementationFile=EBookDoc.cpp
Filter=N

[CLS:CEBookView]
Type=0
HeaderFile=EBookView.h
ImplementationFile=EBookView.cpp
Filter=D
BaseClass=CDaoRecordView
VirtualFilter=XRVWC
LastObject=IDC_CONTENT


[CLS:CEBookSet]
Type=0
HeaderFile=EBookSet.h
ImplementationFile=EBookSet.cpp
Filter=N

[DB:CEBookSet]
DB=1
DBType=DAO
ColumnCount=5
Column1=[BookID], 4, 4
Column2=[ID], 4, 4
Column3=[ParentID], 4, 4
Column4=[title], 12, 100
Column5=[Content], -1, 0


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CFrameWnd
VirtualFilter=fWC




[CLS:CAboutDlg]
Type=0
HeaderFile=EBook.cpp
ImplementationFile=EBook.cpp
Filter=D
LastObject=IDC_WELCOMEPAGE
BaseClass=CDialog
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=7
Control1=IDC_STATIC,static,1342308480
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342177294
Control4=IDC_HTTP,button,1342275584
Control5=IDC_EMAIL,button,1342275584
Control6=IDC_DOWNEWORD,button,1342275584
Control7=IDC_WELCOMEPAGE,button,1342210048

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_APP_EXIT
Command2=ID_RECORD_ADD
Command3=ID_RECORD_ADD_SUB
Command4=ID_RECORD_EDIT
Command5=ID_REDORD_DELETE
Command6=ID_APP_ABOUT
CommandCount=6

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_UNDO
Command2=ID_EDIT_CUT
Command3=ID_EDIT_COPY
Command4=ID_EDIT_PASTE
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_NEXT_PANE
Command10=ID_PREV_PANE
Command11=ID_CONTEXT_HELP
Command12=ID_HELP
CommandCount=12

[DLG:IDD_EBOOK_FORM]
Type=1
Class=CEBookView
ControlCount=2
Control1=IDC_TREE,SysTreeView32,1350634031
Control2=IDC_CONTENT,edit,1352732740

[MNU:IDR_MENU_TREE]
Type=1
Class=CEBookView
CommandCount=0

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_ASIZEINCREASE
Command2=ID_ASIZEDECREASE
Command3=ID_LEFTMARGIN_INCREASE
Command4=ID_LEFTMARGIN_DECREASE
Command5=ID_BUTTON32780
CommandCount=5

[DLG:IDD_FLASHDLG]
Type=1
Class=CFlashDlg
ControlCount=2
Control1=IDC_WELCOMEPAGE,button,1342278400
Control2=IDC_STATIC,static,1342179342

[CLS:CFlashDlg]
Type=0
HeaderFile=FlashDlg.h
ImplementationFile=FlashDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CFlashDlg
VirtualFilter=dWC

[MNU:IDR_BEGINMENU]
Type=1
Class=CMainFrame
CommandCount=0

