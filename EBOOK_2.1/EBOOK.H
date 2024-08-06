// EBook.h : main header file for the EBOOK application
//

#if !defined(AFX_EBOOK_H__9572530D_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)
#define AFX_EBOOK_H__9572530D_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CEBookApp:
// See EBook.cpp for the implementation of this class
//

class CEBookApp : public CWinApp
{
public:
	CEBookApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEBookApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CEBookApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EBOOK_H__9572530D_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)
