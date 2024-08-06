// EBookSet.cpp : implementation of the CEBookSet class
//

#include "stdafx.h"
#include "EBook.h"
#include "EBookSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEBookSet implementation

IMPLEMENT_DYNAMIC(CEBookSet, CDaoRecordset)

CEBookSet::CEBookSet(CDaoDatabase* pdb)
	: CDaoRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CEBookSet)
	m_BookID = 0;
	m_ParentID = 0;
	m_title = _T("");
	m_Content = _T("");
	m_nFields = 4;
	//}}AFX_FIELD_INIT
	m_nDefaultType = dbOpenDynaset;
}

CString CEBookSet::GetDefaultDBName()
{
	return _T("ebook.dat");
}


CString CEBookSet::GetDefaultSQL()
{
	return _T("[Book]");
}

void CEBookSet::DoFieldExchange(CDaoFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CEBookSet)
	pFX->SetFieldType(CDaoFieldExchange::outputColumn);
	DFX_Long(pFX, _T("[BookID]"), m_BookID);
	DFX_Long(pFX, _T("[ParentID]"), m_ParentID);
	DFX_Text(pFX, _T("[title]"), m_title);
	DFX_Text(pFX, _T("[Content]"), m_Content);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CEBookSet diagnostics

#ifdef _DEBUG
void CEBookSet::AssertValid() const
{
	CDaoRecordset::AssertValid();
}

void CEBookSet::Dump(CDumpContext& dc) const
{
	CDaoRecordset::Dump(dc);
}
#endif //_DEBUG
