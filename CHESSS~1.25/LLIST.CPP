//*******************
//*
//* Filename: llist.cpp
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

#include <stdlib.h>
#include <malloc.h>
#include "llist.h"

//uncomment the following line if this will be compiled using a windows mfc application
//used only when debugging
//#define WINDEBUGGING

#ifdef WINDEBUGGING
#include "windows.h"
#include <stdio.h>
#endif

CLList::CLList()
{
	m_iNumEntries = 0;
	m_uniqueID=1;
	m_pHead = NULL;
	m_pTail = NULL;
	m_pNext = NULL;
	m_pPrev = NULL;
}  

CLList::~CLList()
{
	RemoveAll();
}

void CLList::RemoveAll()
{
	if(m_iNumEntries)
	{
		CLList* pList = GetHead();
		CLList* pNext;
		while (pList)
		{
			m_iNumEntries--;
			pNext = pList->m_pNext;
			//if(m_iNumEntries>0) //this line removed as a remove all wouldn't remove the last item.
				delete pList;
			pList = pNext;
		}
		m_pHead=NULL;
		m_pTail=NULL;
		m_iNumEntries=0;
	}
}

void CLList::RemoveAllItems()
{
	if(m_iNumEntries)
	{
		CLList* pList = GetHead();
		CLList* pNext;
		while (pList)
		{
			m_iNumEntries--;
			pNext = pList->m_pNext;
			delete pList->m_pvItem;
			//if(m_iNumEntries>0)
				delete pList;
			pList = pNext;	
		}
		m_pHead=NULL;
		m_pTail=NULL;
		m_iNumEntries=0;
	}
}

void CLList::AddHead (void* pvItem)
{
	CLList *pHead = GetHead ();
	CLList *pItem = new CLList;
#ifdef WINDEBUGGING
	char buf[80];
	sprintf(buf,"CLList::AddHead new %X\n",pItem);
	OutputDebugString(buf);
#endif

	pItem->m_pvItem = pvItem;
	pItem->m_id=m_uniqueID++;
	
	//setup next & prev for new item
	pItem->m_pPrev = NULL;
	pItem->m_pNext = pHead;

	m_pHead = pItem;		//new head item
	if(!m_pTail)
		m_pTail=m_pHead;	//LBP (06/02/99): new item, so tail has to be the same
	
	//setup next & prev for adjacent items

	if(pHead)
		pHead->m_pPrev = pItem;
	
	m_iNumEntries++;
}

void CLList::AddTail (void* pvItem)
{
		CLList *pTail = GetTail ();
		CLList *pItem = new CLList;
#ifdef WINDEBUGGING
	char buf[80];
	sprintf(buf,"CLList::AddTail new %X\n",pItem);
	OutputDebugString(buf);
#endif

		pItem->m_pvItem = pvItem;
		pItem->m_id=m_uniqueID++;
		pItem->m_pPrev = pTail;

		if(!pTail)
			m_pHead=pItem;
		else
			pTail->m_pNext = pItem;

		m_pTail = pItem;
		m_iNumEntries++;
}


void CLList::RemoveItem (CLList* pList)
{
	if(!pList)
		return;

	m_iNumEntries--;
	if(!m_iNumEntries)
	{
		m_pHead=NULL;
		m_pTail=NULL;
		m_pvItem=NULL;
		delete pList;
		return;
	}

	if (pList->m_pPrev)
	{
		pList->m_pPrev->m_pNext = pList->m_pNext;
		if (pList->m_pNext)
			pList->m_pNext->m_pPrev = pList->m_pPrev;
		else
			m_pTail=pList->m_pPrev;		//this was the last item in the list
	}
	else 
	{
		m_pHead=pList->m_pNext;		//this was the first item in the list
		if(pList->m_pNext)
			pList->m_pNext->m_pPrev = NULL;
	}
	delete pList;
}

void CLList::RemoveTail ()
{
	CLList *pList = GetTail ();
	if(pList)
	{
		if(pList->m_pPrev)
			pList->m_pPrev->m_pNext=NULL;
		m_pTail=pList->m_pPrev;
		if(m_pTail==NULL)
			m_pHead=NULL;		//nothing left on the list
		
		delete pList;
		m_iNumEntries--;
	}
}

void CLList::RemoveHead ()
{
	CLList *pList = GetHead ();
	if(pList)
	{
		if(pList->m_pNext)
			pList->m_pNext->m_pPrev=NULL;
		m_pHead=pList->m_pNext;
		if(m_pHead==NULL)
			m_pTail=NULL;		//nothing left on the list

		delete pList;
		m_iNumEntries--;
	}
}

void CLList::InsertAfter (CLList* pList, void* pvItem)
{
	if (!pList->m_pNext)
	{
		AddTail (pvItem);
	}
	else
	{
		CLList* pItem = new CLList;

		pItem->m_pvItem = pvItem;
		pItem->m_id=m_uniqueID++;

		//setup next & prev for new item
		pItem->m_pPrev = pList;
		pItem->m_pNext = pList->m_pNext;
		
		//setup next & prev for adjacent items
		pList->m_pNext->m_pPrev = pItem;
		pList->m_pNext = pItem;

		m_iNumEntries++;
	}
}

void CLList::InsertBefore (CLList* pList, void* pvItem)
{
	if (!pList->m_pPrev)
	{
		AddTail (pvItem);
	}
	else
	{
		CLList* pItem = new CLList;

		pItem->m_pvItem = pvItem;
		pItem->m_id=m_uniqueID++;
		
		//setup next & prev for new item
		pItem->m_pNext = pList;
		pItem->m_pPrev = pList->m_pPrev;
		
		//setup next & prev for adjacent items
		pList->m_pPrev->m_pNext = pItem;
		pList->m_pPrev = pItem;
		
		m_iNumEntries++;
	}
}

CLList* CLList::FindItem(void* pvItem)
{
	CLList* pList = GetHead();
	while(pList)
	{
		if(pList->GetItem()==pvItem)
			return pList;
		pList=pList->GetNext();
	}
	return NULL;
}

CLList* CLList::FindID(int id)
{
	CLList* pList = GetHead();
	while(pList)
	{
		if(pList->GetID()==id)
			return pList;
		pList=pList->GetNext();
	}
	return NULL;
}

CLList* CLList::GetIndex(int iIndex)
{
	CLList* plist=GetHead();
	if(plist)
		for(int i=1; i<iIndex && plist; i++,plist=plist->GetNext()) {}
	return plist;
}
