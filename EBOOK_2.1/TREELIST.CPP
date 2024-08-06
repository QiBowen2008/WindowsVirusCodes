// TreeList.cpp: implementation of the CTreeList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EBook.h"
#include "TreeList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTreeList::CTreeList()
{

}

CTreeList::~CTreeList()
{
 while(!List.IsEmpty())
 {
	 pNode=(Node*)List.RemoveHead();
	 delete pNode;
}
}
void CTreeList::AddItem(HTREEITEM hItem, LONG Id)
{
 pNode = new Node;
 pNode->ID=Id;
 pNode->item =hItem;
 List.AddTail (pNode);	
 }

HTREEITEM CTreeList::FindItem(LONG id)
{
  POSITION pos=List.GetHeadPosition();
  while(pos!=NULL)
  {
	  pNode=(Node*)List.GetNext(pos);
	  if(pNode->ID==id)
		  return(pNode->item);
  }
 return(NULL);
}

LONG CTreeList::FindID(HTREEITEM hItem)
{
POSITION pos=List.GetHeadPosition();
  while(pos!=NULL)
  {
	  pNode=(Node*)List.GetNext(pos);
	  if(pNode->item ==hItem)
		  return(pNode->ID);
  }
return (-1);
}

LONG CTreeList::DeleteItem(HTREEITEM hDeleteItem)
{
 POSITION pos=List.GetHeadPosition();
 POSITION DeletePos;
 LONG id;
  while(pos!=NULL)
  {
	  DeletePos=pos;
	  pNode=(Node*)List.GetNext(pos);
	  if(pNode->item==hDeleteItem)
	  {
		  id=pNode->ID;
		  if(pos==NULL){
		  pNode=(Node*)List.RemoveTail();
		  }
		  else
		  {
           List.RemoveAt(pos);
		  }
		  ASSERT(pNode!=NULL);
		  delete pNode;
		  return(id);
	   }
  }
  return(-1);
}
