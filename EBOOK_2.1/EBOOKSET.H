// EBookSet.h : interface of the CEBookSet class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBOOKSET_H__95725317_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)
#define AFX_EBOOKSET_H__95725317_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEBookSet : public CDaoRecordset
{
public:
	CEBookSet(CDaoDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CEBookSet)

// Field/Param Data
	//{{AFX_FIELD(CEBookSet, CDaoRecordset)
	long	m_BookID;
	long	m_ParentID;
	CString	m_title;
	CString	m_Content;
	//}}AFX_FIELD

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEBookSet)
	public:
	virtual CString GetDefaultDBName();		// REVIEW:  Get a comment here
	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CDaoFieldExchange* pFX);	// RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EBOOKSET_H__95725317_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)

