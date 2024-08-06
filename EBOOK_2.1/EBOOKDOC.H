// EBookDoc.h : interface of the CEBookDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBOOKDOC_H__95725313_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)
#define AFX_EBOOKDOC_H__95725313_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "EBookSet.h"


class CEBookDoc : public CDocument
{
protected: // create from serialization only
	CEBookDoc();
	DECLARE_DYNCREATE(CEBookDoc)

// Attributes
public:
	CEBookSet m_eBookSet;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEBookDoc)
	public:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEBookDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEBookDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EBOOKDOC_H__95725313_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)
