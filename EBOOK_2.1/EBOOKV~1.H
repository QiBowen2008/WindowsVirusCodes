// EBookView.h : interface of the CEBookView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBOOKVIEW_H__95725315_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)
#define AFX_EBOOKVIEW_H__95725315_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_

#include "TreeList.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEBookSet;

class CEBookView : public CDaoRecordView
{
protected: // create from serialization only
	CEBookView();
	DECLARE_DYNCREATE(CEBookView)

public:
	//{{AFX_DATA(CEBookView)
	enum { IDD = IDD_EBOOK_FORM };
	CEdit	*m_edit;
	CEBookSet* m_pSet;
	CString	m_Content;
	//}}AFX_DATA

// Attributes
public:
	CEBookDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEBookView)
	public:
	virtual CDaoRecordset* OnGetRecordset();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	int Min(int i1,int i2,int i3);
	int TempWidth;
	BOOL b_IsDraging;
	BOOL DeleteItemFromDataBase(HTREEITEM hDeleteItem);
	void AddItemToDataBase(HTREEITEM hNewParent,HTREEITEM hNewItem,CString title,CString s);
	CTreeList m_treelist;
	HTREEITEM AddTreeItem(CString s,HTREEITEM hParent,HTREEITEM hInsertAtfer);
	virtual ~CEBookView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	CMenu* pMenu;
	HTREEITEM hRoot;
	HTREEITEM hNewParent;
	HTREEITEM hNewItem;
	//{{AFX_MSG(CEBookView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRecordAdd();
	afx_msg void OnRecordAddSub();
	afx_msg void OnRecordEdit();
	afx_msg void OnRedordDelete();
	afx_msg void OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLeftmarginIncrease();
	afx_msg void OnLeftmarginDecrease();
	afx_msg void OnAsizeincrease();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeContent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_ChangeTitle;
	int m_treeWidth;
	BOOL IsDataOpen;
	BOOL m_bAdding;
	HTREEITEM m_EditItem;
	BOOL IsEditing;
	CTreeCtrl* m_tree;
};

#ifndef _DEBUG  // debug version in EBookView.cpp
inline CEBookDoc* CEBookView::GetDocument()
   { return (CEBookDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EBOOKVIEW_H__95725315_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)
