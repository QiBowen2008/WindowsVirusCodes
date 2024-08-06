// TreeList.h: interface for the CTreeList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREELIST_H__9572533B_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)
#define AFX_TREELIST_H__9572533B_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTreeList  
{  
	struct Node{
	LONG ID;
	HTREEITEM item;
	};
	Node *pNode;
public:
	LONG DeleteItem(HTREEITEM hDeleteItem);
	LONG FindID(HTREEITEM hItem);
	HTREEITEM FindItem(LONG id);
	void AddItem(HTREEITEM hItem,LONG Id);
	CTreeList();
	virtual ~CTreeList();
    CPtrList List;
private:

};

#endif // !defined(AFX_TREELIST_H__9572533B_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)
