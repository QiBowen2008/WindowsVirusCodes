// FS.h : main header file for the FS application
//

#if !defined(AFX_FS_H__E22B6FC6_E492_11D0_BC56_F118154FCB62__INCLUDED_)
#define AFX_FS_H__E22B6FC6_E492_11D0_BC56_F118154FCB62__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFSApp:
// See FS.cpp for the implementation of this class
//

class CFSApp : public CWinApp
{
public:
	CFSApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFSApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFSApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FS_H__E22B6FC6_E492_11D0_BC56_F118154FCB62__INCLUDED_)
