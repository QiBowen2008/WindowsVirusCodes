// EBookDoc.cpp : implementation of the CEBookDoc class
//

#include "stdafx.h"
#include "EBook.h"

#include "EBookSet.h"
#include "EBookDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEBookDoc

IMPLEMENT_DYNCREATE(CEBookDoc, CDocument)

BEGIN_MESSAGE_MAP(CEBookDoc, CDocument)
	//{{AFX_MSG_MAP(CEBookDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEBookDoc construction/destruction

CEBookDoc::CEBookDoc()
{
	// TODO: add one-time construction code here

}

CEBookDoc::~CEBookDoc()
{
}

BOOL CEBookDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CEBookDoc diagnostics

#ifdef _DEBUG
void CEBookDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEBookDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEBookDoc commands
