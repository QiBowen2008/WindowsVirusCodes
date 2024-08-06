// Copyright (c) 1999 Lee Patterson
// leepatterson@home.com

#include <winsock.h>
#include <stdio.h>
#include <llist.h>
#include "blockingsocket.h"
#include "gameob.h"

//**********
//*
//*	PERSON INSTANCE OBJECT
//*
//**********************
CPersonInstance::CPersonInstance()
{
	m_color=WHITE;
	m_location=CHESS;
	m_channel=0; 
}

CPersonInstance::~CPersonInstance()
{
}

//**********
//*
//*	BOARD SPACE OBJECT
//*
//**********************
BoardSpace::BoardSpace()
{
	static int icount=0;
	m_id=icount++;
	m_type=EMPTY;
}

//**********
//*
//*	BOARD GAME OBJECT
//*
//**********************
BoardGame::BoardGame()
{
	m_white=NULL;
	m_black=NULL;
	m_nBoardType=BTYPE_UNDEFINED;
	memset(m_gamename,0,sizeof m_gamename);
}

void BoardGame::AddPerson(CPersonInstance* pInstance)
{
	m_connections.AddTail(pInstance);
}

void BoardGame::RemovePerson(CPersonInstance* pInstance)
{
	CLList* plist=m_connections.FindItem(pInstance);
	if(plist)
		m_connections.RemoveItem(plist);
}

int BoardGame::Left(int v)
{
	return (v&0xf0)>>4;
}

int BoardGame::Right(int v)
{
	return v&0xf;
}

//**********
//*
//*	CHESS BOARD OBJECT
//*
//**********************
ChessBoard::ChessBoard()
{
	FillBoard();
}

bool ChessBoard::ValidMove(int from0,int from1,int to0,int to1)
{
	return true;	//all moves are cool
}

void ChessBoard::Move(int from0,int from1,int to0,int to1)
{

	m_spaces[to1-'0'][to0-'0'].m_type=m_spaces[from1-'0'][from0-'0'].m_type;
	m_spaces[from1-'0'][from0-'0'].m_type=BoardSpace::EMPTY;
}

void ChessBoard::FillBoard()
{
	int blackbackrow[]={BoardSpace::BROOK,BoardSpace::BKNIGHT,BoardSpace::BBISHOP,BoardSpace::BQUEEN,BoardSpace::BKING,BoardSpace::BBISHOP,BoardSpace::BKNIGHT,BoardSpace::BROOK};
	int whitebackrow[]={BoardSpace::WROOK,BoardSpace::WKNIGHT,BoardSpace::WBISHOP,BoardSpace::WQUEEN,BoardSpace::WKING,BoardSpace::WBISHOP,BoardSpace::WKNIGHT,BoardSpace::WROOK};
	int i;
	int j;

	//fill back rows
	for(i=0; i<8; i++)
	{
		m_spaces[0][i].m_type=blackbackrow[i];
		m_spaces[7][i].m_type=whitebackrow[i];
	}

	//fill pawn row
	for(i=0; i<8; i++)
	{
		m_spaces[1][i].m_type=BoardSpace::BPAWN;
		m_spaces[6][i].m_type=BoardSpace::WPAWN;
	}

	//fill in the rest of the empty spaces
	for(i=2; i<6; i++)
	{
		for(j=0; j<8; j++)
			m_spaces[i][j].m_type=BoardSpace::EMPTY;
	}
}

void ChessBoard::Draw()
{
	CPersonInstance* pInstance;
	CLList* plist=m_connections.GetHead();
	while(plist)
	{
		pInstance=(CPersonInstance*)plist->GetItem();
		DrawBoard(pInstance);
		plist=plist->GetNext();
	}
}

void ChessBoard::DrawBoard(CPersonInstance* pInstance)
{
	char buffer[80];	//buffer to store a line
	char* p=buffer;
	for(int y=0; y<8; y++)
	{
		sprintf(buffer,"%d ",y); 
		p=buffer+strlen(buffer);
		for(int x=0; x<8; x++)
		{
			*(p++)=GetPiece(m_spaces[y][x].m_type);
			*(p++)=' ';
		}
		*p='\0';
		pInstance->Print(buffer);
	}
	pInstance->Print("  0 1 2 3 4 5 6 7");
}


char ChessBoard::GetPiece(int type)
{
	char id;

	switch(type)
	{

		case BoardSpace::EMPTY:
			id='.';
			break;

		case BoardSpace::WPAWN:
			id='p';
			break;

		case BoardSpace::WBISHOP:
			id='b';
			break;

		case BoardSpace::WKNIGHT:
			id='n';
			break;

		case BoardSpace::WROOK:
			id='r';
			break;

		case BoardSpace::WQUEEN:
			id='q';
			break;

		case BoardSpace::WKING:
			id='k';
			break;

		case BoardSpace::BPAWN:
			id='P';
			break;

		case BoardSpace::BBISHOP:
			id='B';
			break;

		case BoardSpace::BKNIGHT:
			id='N';
			break;

		case BoardSpace::BROOK:
			id='R';
			break;

		case BoardSpace::BQUEEN:
			id='Q';
			break;

		case BoardSpace::BKING:
			id='K';
			break;

		default:
			id='?';
			break;
	}

	return id;
}

//**********
//*
//*	CHESS GAME OBJECT
//*
//**********************

ChessGameOb::ChessGameOb(CPersonInstance* pInstanceWhite,const char* pGameName)
{
	m_white = pInstanceWhite;
	strcpy(m_gamename,pGameName);
	m_connections.AddTail(pInstanceWhite);
	m_black=NULL;
}

ChessGameOb::~ChessGameOb()
{
	m_white->Cleanup();
	m_black->Cleanup();

	delete m_white;
	delete m_black;
}

void ChessGameOb::AddWatcher(CPersonInstance* pInstance)
{
	m_connections.AddTail(pInstance);
}

void ChessGameOb::AddBlack(CPersonInstance* pInstance)
{
	m_black = pInstance;
	m_connections.AddTail(pInstance);
}

bool ChessGameOb::HasAllPlayers()
{
	if(m_white && m_black)
		return true;

	return false;
}


//**********
//*
//*	CHECKER BOARD OBJECT
//*
//**********************

CheckerBoard::CheckerBoard()
{
	FillBoard();
}

bool CheckerBoard::ValidMove(int from0,int from1,int to0,int to1)
{
	return true;	//all moves are cool
}

void CheckerBoard::Move(int from0,int from1,int to0,int to1)
{

	m_spaces[to1-'0'][to0-'0'].m_type=m_spaces[from1-'0'][from0-'0'].m_type;
	m_spaces[from1-'0'][from0-'0'].m_type=BoardSpace::EMPTY;
}

void CheckerBoard::FillBoard()
{
	int rows[]=
	{
		BoardSpace::WSQUARE,BoardSpace::BCHECKER,BoardSpace::WSQUARE,BoardSpace::BCHECKER,BoardSpace::WSQUARE,BoardSpace::BCHECKER,BoardSpace::WSQUARE,BoardSpace::BCHECKER,
		BoardSpace::BCHECKER,BoardSpace::WSQUARE,BoardSpace::BCHECKER,BoardSpace::WSQUARE,BoardSpace::BCHECKER,BoardSpace::WSQUARE,BoardSpace::BCHECKER,BoardSpace::WSQUARE,
		BoardSpace::WSQUARE,BoardSpace::BCHECKER,BoardSpace::WSQUARE,BoardSpace::BCHECKER,BoardSpace::WSQUARE,BoardSpace::BCHECKER,BoardSpace::WSQUARE,BoardSpace::BCHECKER,
		
		BoardSpace::BSQUARE,BoardSpace::WSQUARE,BoardSpace::BSQUARE,BoardSpace::WSQUARE,BoardSpace::BSQUARE,BoardSpace::WSQUARE,BoardSpace::BSQUARE,BoardSpace::WSQUARE,
		BoardSpace::WSQUARE,BoardSpace::BSQUARE,BoardSpace::WSQUARE,BoardSpace::BSQUARE,BoardSpace::WSQUARE,BoardSpace::BSQUARE,BoardSpace::WSQUARE,BoardSpace::BSQUARE,

		BoardSpace::WCHECKER,BoardSpace::WSQUARE,BoardSpace::WCHECKER,BoardSpace::WSQUARE,BoardSpace::WCHECKER,BoardSpace::WSQUARE,BoardSpace::WCHECKER,BoardSpace::WSQUARE,
		BoardSpace::WSQUARE,BoardSpace::WCHECKER,BoardSpace::WSQUARE,BoardSpace::WCHECKER,BoardSpace::WSQUARE,BoardSpace::WCHECKER,BoardSpace::WSQUARE,BoardSpace::WCHECKER,
		BoardSpace::WCHECKER,BoardSpace::WSQUARE,BoardSpace::WCHECKER,BoardSpace::WSQUARE,BoardSpace::WCHECKER,BoardSpace::WSQUARE,BoardSpace::WCHECKER,BoardSpace::WSQUARE,
	};
	int irow=0;
	for(int y=0; y<8; y++)
		for(int x=0; x<8; x++)
			m_spaces[y][x].m_type=rows[irow++];
}

void CheckerBoard::Draw()
{
	CPersonInstance* pInstance;
	CLList* plist=m_connections.GetHead();
	while(plist)
	{
		pInstance=(CPersonInstance*)plist->GetItem();
		DrawBoard(pInstance);
		plist=plist->GetNext();
	}
}

void CheckerBoard::DrawBoard(CPersonInstance* pInstance)
{
	char buffer[80];	//buffer to store a line
	char* p=buffer;
	pInstance->Print("    0   1   2   3   4   5   6   7");
	for(int y=0; y<8; y++)
	{
		pInstance->Print("   --- --- --- --- --- --- --- ---");
		sprintf(buffer,"%d |",y); 
		p=buffer+strlen(buffer);
		for(int x=0; x<8; x++)
		{
			*(p++)=' ';
			*(p++)=GetPiece(m_spaces[y][x].m_type);
			*(p++)=' ';
			*(p++)='|';
		}
		*(p++)=' ';
		*(p++)='0'+y;
		*p='\0';
		pInstance->Print(buffer);
	}
	pInstance->Print("   --- --- --- --- --- --- --- ---");
	pInstance->Print("    0   1   2   3   4   5   6   7");
}

char CheckerBoard::GetPiece(int type)
{
	char id;

	switch(type)
	{
		case BoardSpace::EMPTYCHECKER:
			id=' ';
			break;

		case BoardSpace::WSQUARE:
			id=' ';
			break;

		case BoardSpace::BSQUARE:
			id=' ';
			break;

		case BoardSpace::WCHECKER :
			id='W';
			break;

		case BoardSpace::BCHECKER:
			id='b';
			break;

		default: id='?'; break;
	}

	return id;
}

//**********
//*
//*	CHECKER GAME OBJECT
//*
//**********************

CheckerGameOb::CheckerGameOb(CPersonInstance* pInstanceWhite,const char* pGameName)
{
	m_white = pInstanceWhite;
	strcpy(m_gamename,pGameName);
	m_connections.AddTail(pInstanceWhite);
	m_black=NULL;
}

CheckerGameOb::~CheckerGameOb()
{
	m_white->Cleanup();
	m_black->Cleanup();

	delete m_white;
	delete m_black;
}

void CheckerGameOb::AddWatcher(CPersonInstance* pInstance)
{
	m_connections.AddTail(pInstance);
}

void CheckerGameOb::AddBlack(CPersonInstance* pInstance)
{
	m_black = pInstance;
	m_connections.AddTail(pInstance);
}

bool CheckerGameOb::HasAllPlayers()
{
	if(m_white && m_black)
		return true;

	return false;
}
