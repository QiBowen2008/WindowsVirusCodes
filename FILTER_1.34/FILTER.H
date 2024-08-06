// Filter.h : main header file for the FILTER application
//

#if !defined(AFX_FILTER_H__08E648B7_BCF5_11D3_B1BC_0080C84821E0__INCLUDED_)
#define AFX_FILTER_H__08E648B7_BCF5_11D3_B1BC_0080C84821E0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFilterApp:
// See Filter.cpp for the implementation of this class
//

class CFilterApp : public CWinApp
{
public:
	CFilterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFilterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTER_H__08E648B7_BCF5_11D3_B1BC_0080C84821E0__INCLUDED_)
