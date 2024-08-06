// EBookView.cpp : implementation of the CEBookView class
//

#include "stdafx.h"
#include "EBook.h"

#include "EBookSet.h"
#include "EBookDoc.h"
#include "EBookView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEBookView

IMPLEMENT_DYNCREATE(CEBookView, CDaoRecordView)

BEGIN_MESSAGE_MAP(CEBookView, CDaoRecordView)
	//{{AFX_MSG_MAP(CEBookView)
	ON_WM_SIZE()
	ON_COMMAND(ID_RECORD_ADD, OnRecordAdd)
	ON_COMMAND(ID_RECORD_ADD_SUB, OnRecordAddSub)
	ON_COMMAND(ID_RECORD_EDIT, OnRecordEdit)
	ON_COMMAND(ID_REDORD_DELETE, OnRedordDelete)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnRclickTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE, OnEndlabeleditTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_COMMAND(ID_LEFTMARGIN_INCREASE, OnLeftmarginIncrease)
	ON_COMMAND(ID_LEFTMARGIN_DECREASE, OnLeftmarginDecrease)
	ON_COMMAND(ID_ASIZEINCREASE, OnAsizeincrease)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(NM_CLICK, IDC_TREE, OnClickTree)
	ON_EN_CHANGE(IDC_CONTENT, OnChangeContent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEBookView construction/destruction

CEBookView::CEBookView()
	: CDaoRecordView(CEBookView::IDD)
{
	//{{AFX_DATA_INIT(CEBookView)
	m_pSet = NULL;
	m_Content = _T("");
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_tree=NULL;
	m_edit=NULL;
    IsEditing=FALSE;//Is tree item editing?
	m_bAdding=FALSE; //No new item add to tree
	IsDataOpen=FALSE; //m_pSet can not use now
	m_treeWidth=100; //tree list width
	TempWidth=100;
	b_IsDraging=FALSE; //is drag?
}

CEBookView::~CEBookView()
{
}

void CEBookView::DoDataExchange(CDataExchange* pDX)
{
	CDaoRecordView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEBookView)
	DDX_Text(pDX, IDC_CONTENT, m_Content);
	//}}AFX_DATA_MAP
}

BOOL CEBookView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CDaoRecordView::PreCreateWindow(cs);
}

void CEBookView::OnInitialUpdate()
{
	m_pSet = &GetDocument()->m_eBookSet;
	CDaoRecordView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
//	ResizeParentToFit();

    m_tree=(CTreeCtrl*)GetDlgItem(IDC_TREE);
    m_pSet->MoveFirst();
	CString s;
    
	HTREEITEM hInsertAfter;
	if(m_tree)
	{
		hNewParent=TVI_ROOT;
	   	s=m_pSet->m_title ;
		hInsertAfter=TVI_FIRST;
		hRoot=AddTreeItem(s,hNewParent,hInsertAfter);
        m_treelist.AddItem (hRoot,m_pSet->m_BookID);
	}
	m_pSet->MoveNext();
	while(!m_pSet->IsEOF())
	{
	 hNewParent=m_treelist.FindItem (m_pSet->m_ParentID );
     if(hNewParent) 
	 {
		 s=m_pSet->m_title;
		 hInsertAfter=AddTreeItem(s,hNewParent,TVI_LAST);
		 m_treelist.AddItem (hInsertAfter,m_pSet->m_BookID );
	 }
	 m_pSet->MoveNext();
  }
	m_pSet->MoveFirst ();
	m_Content=m_pSet->m_Content ;
	UpdateData(FALSE);
	IsDataOpen=TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CEBookView diagnostics

#ifdef _DEBUG
void CEBookView::AssertValid() const
{
	CDaoRecordView::AssertValid();
}

void CEBookView::Dump(CDumpContext& dc) const
{
	CDaoRecordView::Dump(dc);
}

CEBookDoc* CEBookView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEBookDoc)));
	return (CEBookDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEBookView database support
CDaoRecordset* CEBookView::OnGetRecordset()
{
	return m_pSet;
}


/////////////////////////////////////////////////////////////////////////////
// CEBookView message handlers

void CEBookView::OnSize(UINT nType, int cx, int cy) 
{
	CDaoRecordView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_tree=(CTreeCtrl*)GetDlgItem(IDC_TREE);
	m_edit=(CEdit*)GetDlgItem(IDC_CONTENT);
	if((m_tree!=NULL)&(m_edit!=NULL))
	{
	RECT rect;
    GetParent()->GetClientRect(&rect);
	ShowWindow(SW_NORMAL);
	rect.top+=20;
	rect.bottom -=18;
	MoveWindow(&rect);
	rect.top-=20;
	m_tree->ShowWindow(SW_NORMAL);
	m_tree->MoveWindow (rect.left ,rect.top ,rect.left+m_treeWidth-2 ,rect.bottom-25 );
    m_edit->ShowWindow(SW_NORMAL);
	m_edit->MoveWindow (rect.left+m_treeWidth+3,rect.top ,rect.right-rect.left-m_treeWidth-5,rect.bottom-rect.top-25 );	
	}
}

void CEBookView::OnRecordAdd() 
{
	// TODO: Add your command handler code here
	CString s;

	HTREEITEM hInsertAfter;
	m_tree=(CTreeCtrl*)GetDlgItem(IDC_TREE);
	if(m_tree->GetCount()<=0)
	{
		hNewParent=TVI_ROOT;
		s="EBook2000";
		hInsertAfter=TVI_FIRST;
		hRoot=AddTreeItem(s,hNewParent,hInsertAfter);
		return;
	}
	else if(m_tree->GetCount()==1)
	{
        hInsertAfter=TVI_LAST;
		hNewParent=hRoot;
	}
	else{
		hNewParent=m_tree->GetSelectedItem ();
		if(!hNewParent)
		{
			MessageBox("请选定加入标题的位置!");
			return;
		}
		if(hNewParent!=hRoot)
		{
		hNewParent=m_tree->GetParentItem(hNewParent);
		hInsertAfter=TVI_LAST;
     	}
	}
    s="新标题";
    hNewItem=AddTreeItem(s,hNewParent,hInsertAfter);
	CEdit *edit=(CEdit*)GetDlgItem(IDC_CONTENT);
	edit->SetWindowText("");
    // m_tree->GetEditControl 
//	m_tree->SetFocus (); 
	 m_tree->EnsureVisible(hNewItem); 
     m_bAdding=TRUE;
	 m_tree->SelectItem (hNewItem);
      m_tree->EditLabel (hNewItem);
	  
	  
}

HTREEITEM CEBookView::AddTreeItem(CString s, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
 TV_ITEM tvItem; 
	 int Length;
     Length=s.GetLength ();
	 char *ch;
		 ch=new char[Length+1];
    	 for(int i=0;i<Length;i++)
		     ch[i]=s.GetAt (i);
	      ch[i]='\0';
		  
     tvItem.mask=TVIF_TEXT;
	 tvItem.pszText=ch;
	 tvItem.cchTextMax = Length;
     TV_INSERTSTRUCT tvInsert;
	 tvInsert.hParent = hParent;
	 tvInsert.hInsertAfter=hInsertAfter;
	 tvInsert.item =tvItem;
    	HTREEITEM h= m_tree->InsertItem(&tvInsert);
	delete ch;
	return h;
	}

void CEBookView::OnRecordAddSub() 
{
	// TODO: Add your command handler code here
     CString s;
 	HTREEITEM hInsertAfter;
	m_tree=(CTreeCtrl*)GetDlgItem(IDC_TREE);
	if(m_tree->GetCount()<=0)
	{
		hNewParent=TVI_ROOT;
		s="EBook";
		hInsertAfter=TVI_FIRST;
		hRoot=AddTreeItem(s,hNewParent,hInsertAfter);
		return;
	}
	else if(m_tree->GetCount()==1)
	{
        hInsertAfter=TVI_LAST;
		hNewParent=hRoot;
	}
	else{
		hNewParent=m_tree->GetSelectedItem();
		if(!hNewParent)
		{
			MessageBox("请选定加入标题的位置!");
			return;
		}
		
	   	hInsertAfter=TVI_LAST;
     	}
    s="新标题";
    hNewItem=AddTreeItem(s,hNewParent,hInsertAfter);
    m_bAdding=TRUE;
	CEdit *edit=(CEdit*)GetDlgItem(IDC_CONTENT);
    edit->SetWindowText("");
    m_tree->EnsureVisible(hNewItem); 
    m_tree->SelectItem (hNewItem);
	m_tree->EditLabel (hNewItem);
	
//	m_tree->SetFocus();
}

void CEBookView::OnRecordEdit() 
{
	// TODO: Add your command handler code here
   m_tree=(CTreeCtrl*)GetDlgItem(IDC_TREE);
   m_EditItem=m_tree->GetSelectedItem();
   if(m_EditItem==hRoot)return;
   if(m_EditItem)
   {
	   m_tree->EditLabel (m_EditItem);
	   IsEditing=TRUE;
   }
}

void CEBookView::OnRedordDelete() 
{
	// TODO: Add your command handler code here
	 m_tree=(CTreeCtrl*)GetDlgItem(IDC_TREE);
     HTREEITEM hDeleteItem, hItem; //delete and it's parent item
     m_tree->SetFocus ();
	 hDeleteItem=m_tree->GetSelectedItem();
     if(hDeleteItem==hRoot)return; //roo titem can not be deleted
     if(hDeleteItem)
	 {
	   if(m_tree->ItemHasChildren (hDeleteItem))
	   { MessageBox("请先删除子标题!");
	      return;
	   }  
		
		 hItem=m_tree->GetParentItem(hDeleteItem);
	if(	m_tree->DeleteItem (hDeleteItem)) 
		 //hItem=m_tree->GetSelectedItem ();
	if(DeleteItemFromDataBase(hDeleteItem)) //delete item from database and tree list
	     //delete item from tree
		 // find item ID from tree list
	{
		 LONG ID;
		 ID=m_treelist.FindID(hItem);
		 //find item in data base
          m_pSet->MoveFirst();
  	    while(!m_pSet->IsEOF()) //search item from data base
		{
			if(m_pSet->m_BookID==ID)
			    break;
			   m_pSet->MoveNext();
		}
		 m_Content=m_pSet->m_Content;
		 UpdateData(FALSE);
         m_tree->SelectItem(hItem); 
	 }
	 }
}

void CEBookView::OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
	HTREEITEM hItem;
    m_tree=(CTreeCtrl*)GetDlgItem(IDC_TREE);
	hItem=m_tree->GetSelectedItem();
	if(hItem==NULL)return;
    RECT rect;
     m_tree->GetItemRect(hItem,&rect,TRUE);
    CMenu Menu;
	Menu.LoadMenu(IDR_MENU_TREE);
	pMenu=Menu.GetSubMenu(0);
	ASSERT(pMenu!=0);
	CPoint p;
    p.x=rect.left;
	p.y=rect.top;
	ClientToScreen(&p);
	pMenu->TrackPopupMenu(0,p.x,p.y,this);
}

void CEBookView::OnEndlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
 //focus to new item
     	m_tree=(CTreeCtrl*)GetDlgItem(IDC_TREE);
             CString s;
			 m_EditItem=m_tree->GetSelectedItem ();
			 m_edit=m_tree->GetEditControl ();
			 m_edit->GetWindowText (s);
			 if(m_pSet->m_title !=s)
			 {
             m_pSet->Edit ();
		     m_pSet->m_title =s;
		     if(m_pSet->CanUpdate ())
		      m_pSet->Update();
		     m_pSet->Edit();
			 }
	*pResult = TRUE;
}

void CEBookView::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_tree=(CTreeCtrl*)GetDlgItem(IDC_TREE);
	m_edit=(CEdit*)GetDlgItem(IDC_CONTENT);
	CString s;
if(IsDataOpen)
		 {
		   			
		HTREEITEM hItem =m_tree->GetSelectedItem ();
		LONG ID;
		ID=m_treelist.FindID(hItem);
		m_pSet->MoveFirst ();
		while(!m_pSet->IsEOF())
		{
			if(m_pSet->m_BookID==ID)
			    break;
			   m_pSet->MoveNext();
		}
		m_Content=m_pSet->m_Content ;
         UpdateData(FALSE);
		m_tree->EnsureVisible (hItem);
	    
	}
	if(IsEditing)
	   {
	    m_tree->SetFocus ();
		m_tree->SelectItem (m_EditItem);
		s=m_tree->GetItemText(m_EditItem); 
     	LONG ID;
		ID=m_treelist.FindID(m_EditItem);
		ASSERT(ID>0);
		m_pSet->MoveFirst ();
		while(!m_pSet->IsEOF())
		{
			if(m_pSet->m_BookID==ID)
			    break;
			   m_pSet->MoveNext();
		}
		 CString Content;
		 int i;
		 Content=m_pSet->m_Content;
          if((s=="新标题")&(Content.GetLength()>5))
		  {
			  i=Min(Content.Find ("\r\n"),Content.Find(','),
				  Content.Find(". "));
			  i=Min(i,Content.Find("。"),Content.Find(';'));
			  if(i>99) i=50;
			  s=Content.Left (i);
			  m_tree->SetItemText (m_EditItem,s);
		  }
			  
         m_pSet->Edit ();
		     m_pSet->m_title =s;
		     if(m_pSet->CanUpdate ())
		      m_pSet->Update();
		     m_pSet->Edit();
		IsEditing=FALSE; //Complete edit tree item
	}
	else if(m_bAdding)
		{
        m_tree->SetFocus ();
		m_tree->SelectItem (hNewItem);
        m_edit->GetWindowText(s); 
		AddItemToDataBase(hNewParent,hNewItem,
			m_tree->GetItemText(hNewItem),	s);
		    //m_pSet->AddNew ();
			//m_pSet->m_
	    m_bAdding=FALSE;
		}
	      
	*pResult = 0;
}

void CEBookView::AddItemToDataBase(HTREEITEM hNewParent, HTREEITEM hNewItem, CString title, CString s)
{
  LONG ParentID;
  ParentID=m_treelist.FindID(hNewParent);
  if(ParentID<0)return;
  LONG NewBookID;
  m_pSet->MoveLast();
  NewBookID=m_pSet->m_BookID+1; 
  m_pSet->AddNew();
  m_pSet->m_BookID =NewBookID;
  m_pSet->m_ParentID =ParentID;
  m_pSet->m_title =title;
  m_pSet->m_Content =s;
  m_pSet->Update ();
  m_pSet->MoveLast();
  m_treelist.AddItem (hNewItem,NewBookID);
}

BOOL CEBookView::DeleteItemFromDataBase(HTREEITEM hDeleteItem)
{
  LONG ID;
  ID=m_treelist.DeleteItem(hDeleteItem);//delete item from tree list
 // ASSERT(ID>=0);
  if(ID<0)return(FALSE); //if can't delete from list return;
  m_pSet->MoveFirst();
  	while(!m_pSet->IsEOF()) //search item from data base
		{
			if(m_pSet->m_BookID==ID)
			    break;
			   m_pSet->MoveNext();
		}
	m_pSet->Delete (); //delete item from data base 
	return(TRUE);
}

BOOL CEBookView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDaoRecordView::PreTranslateMessage(pMsg);
}

void CEBookView::OnLeftmarginIncrease() 
{
	// TODO: Add your command handler code here
	m_edit=(CEdit*)GetDlgItem(IDC_CONTENT);
	DWORD margins;
	margins=m_edit->GetMargins ();
	m_edit->SetMargins ((WPARAM)margins+10,10);
    m_edit->Invalidate ();
}

void CEBookView::OnLeftmarginDecrease() 
{
	// TODO: Add your command handler code here
	m_edit=(CEdit*)GetDlgItem(IDC_CONTENT);
	DWORD margins;
	margins=m_edit->GetMargins ();
	if((WPARAM)margins>15){
	m_edit->SetMargins ((WPARAM)margins-10,10);
    m_edit->Invalidate ();}
}

void CEBookView::OnAsizeincrease() 
{

	MessageBox("在后续版本中将增加该功能");
	// TODO: Add your command handler code here
/*	m_edit=(CEdit*)GetDlgItem(IDC_CONTENT);
//	m_edit->GetDe

    LOGFONT logFont;
    logFont.lfHeight = 8;
    logFont.lfWidth = 0;
    logFont.lfEscapement = 0;
    logFont.lfOrientation = 0;
    logFont.lfWeight = FW_NORMAL;
    logFont.lfItalic = 0;
    logFont.lfUnderline = 0;
    logFont.lfStrikeOut = 0;
    logFont.lfCharSet = ANSI_CHARSET;
    logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logFont.lfQuality = PROOF_QUALITY;
    logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
    strcpy(logFont.lfFaceName, "Times New Roman");
    // Initialize the position of text in the window.
       // Create and display eight example fonts.
           // Set the new font's height.
         logFont.lfHeight =8*4;
         // Create a new font and select it into the DC.
         CFont font;
         font.CreateFontIndirect(&logFont);
		 CDC *pDC=m_edit->GetDC ();
         CFont* oldFont = pDC->SelectObject(&font);
          m_edit->Invalidate ();
		  */
   }

void CEBookView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	HCURSOR hCursor;
	if(b_IsDraging)
	{
	 RECT rect;
	  if(TempWidth<point.x)
		  m_treeWidth=point.x-5;
		  else 
		  m_treeWidth=point.x+4;
		    GetParent()->GetClientRect(&rect);
     OnSize(SIZE_RESTORED , rect.right -rect.left , rect.bottom-rect.top );
	TempWidth=m_treeWidth;
	}
	else if(abs(point.x -m_treeWidth)<=3)
	{
		
		hCursor=::LoadCursor (NULL,IDC_SIZEWE);
		::SetCursor (hCursor);
	}
	else if((abs(point.x-m_treeWidth)>3)&(b_IsDraging==FALSE))
	{
        hCursor=::LoadCursor (NULL,IDC_ARROW);
		::SetCursor (hCursor);
     	}

	else if((abs(point.x-m_treeWidth)>3)&(b_IsDraging))
	{
        hCursor=::LoadCursor (NULL,IDC_ARROW);
		::SetCursor (hCursor);
        b_IsDraging=FALSE;
	}
   
    
	CDaoRecordView::OnMouseMove(nFlags, point);
}

void CEBookView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(abs(point.x -m_treeWidth)<=2)
	{
		HCURSOR hCursor;
		hCursor=::LoadCursor (NULL,IDC_SIZEWE);
		::SetCursor (hCursor);
		b_IsDraging=!b_IsDraging;
	}
	CDaoRecordView::OnLButtonDown(nFlags, point);
}




void CEBookView::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
		b_IsDraging=FALSE;
	*pResult = 0;
}

int CEBookView::Min(int i1, int i2, int i3)
{
   int i=i1;
   if(i1==-1)i=30;
	if((i>i2)&(i2>0))
		i=i2;
	if((i>i3)&(i3>0))
		i=i3;
	return(i);
}

void CEBookView::OnChangeContent() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDaoRecordView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
//	if(!m_bAdding)return;  
	CString Content;
		 int i;
		 CString s;
		 m_tree=(CTreeCtrl*)GetDlgItem(IDC_TREE);
		 m_EditItem=m_tree->GetSelectedItem ();
		 s=m_tree->GetItemText (m_EditItem);
		 m_edit=(CEdit*)GetDlgItem(IDC_CONTENT);
		 m_edit->GetWindowText (Content);
          if((s=="新标题")&(Content.GetLength()>10))
		  {
			  i=Min(Content.Find ("\r\n"),Content.Find(','),
				  Content.Find(". "));
			  i=Min(i,Content.Find("。"),Content.Find(';'));
			  i=Min(i,Content.Find ("---"),Content.Find("?"));
			  if(i>99) i=50;
			  int j=0;
			  while(Content.GetAt (j)==' ')
			  {j++;}
			  m_ChangeTitle=Content.Left (i);
			  m_ChangeTitle=m_ChangeTitle.Right(i-j);
			  m_tree->SetItemText (m_EditItem,m_ChangeTitle);
		  }
	UpdateData(TRUE);
            m_pSet->Edit ();
			 if((m_pSet->m_title =="新标题")&(m_ChangeTitle.GetLength ()>1))
			 {
			 if((m_ChangeTitle!=m_pSet->m_title)&(m_ChangeTitle!="新标题"))
		     m_pSet->m_title=m_ChangeTitle; 
			 }
			 m_pSet->m_Content =m_Content;
		     if(m_pSet->CanUpdate ())
		      m_pSet->Update();
		     m_pSet->Edit();
}
/*
void CEBookView::OnBegindragTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

    m_dragItem = pNMTreeView->itemNew.hItem;
    if( m_tree.GetParentItem( m_dragItem ) != NULL )   
	{
        CImageList* pDragImage;
        pDragImage = m_tree.CreateDragImage( m_dragItem );
        m_tree.SelectItem( m_dragItem );
        pDragImage->BeginDrag( 0, CPoint(0,0) );
        pDragImage->DragEnter( &m_tree, pNMTreeView->ptDrag );
        SetCapture();     
		m_bIsDragging = TRUE;   
		delete pDragImage;
    }   
	*pResult = 0;
}
*/
