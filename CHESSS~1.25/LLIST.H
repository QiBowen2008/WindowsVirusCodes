#ifndef LLIST_H
#define LLIST_H

//*************************************
//*
//* Notes: 
//*
//*		Linked list class for handing any type of object.
//*
//***************************
//*
//* Date Created: 05/14/98
//* Author: Lee Patterson
//*
//* Copyright (c) 1999, Lee Patterson
//*
//*************************************

class CLList
{
public:
	CLList ();
	~CLList ();

	//adding and removing from the list
	void AddTail (void*);
	void AddHead (void*);
	void InsertAfter (CLList* pList, void* pvItem);
	void InsertBefore (CLList* pList, void* pvItem);
	void RemoveItem (CLList*);
	void RemoveHead ();
	void RemoveTail ();
	void RemoveAll();
	void RemoveAllItems();

	//retrieving items
	int NumEntries(){return m_iNumEntries;}
	void* GetItem (CLList* pList) {return pList->m_pvItem;}
	void* GetItem () {return m_pvItem;}
	int GetID () {return m_id;}
	CLList* GetHead () {return m_pHead;}
	CLList* GetTail () {return m_pTail;}
	CLList* GetNext (CLList* pList) {return pList->m_pNext;}
	CLList* GetPrev (CLList* pList) {return pList->m_pPrev;}
	CLList* GetNext () {return m_pNext;}
	CLList* GetPrev () {return m_pPrev;}

	//finding items
	CLList* FindItem(void* pvItem);
	CLList* FindID(int id);
	CLList* GetIndex(int iIndex);

protected:
	int m_iNumEntries;
	int m_uniqueID;
	int m_id;

	void* m_pvItem;

	CLList* m_pHead;
	CLList* m_pTail;
	CLList* m_pPrev;
	CLList* m_pNext;
};

#endif
