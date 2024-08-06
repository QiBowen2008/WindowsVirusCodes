// Copyright (c) 1999 Lee Patterson
// leepatterson@home.com
//
// Demonstrats: 
//
//	o Internet Server that accepts telnet connections.
//	o Using multiple threads to handle more then one connection. 
//	o BlockingSocket class for using sockets. 
//	  (I'd recomend this class over the MS CSocket and such, as they still have 
//     some 16-bit stuff in them, and this class ports to Unix a lot easier.
//	   That was the main goal of my program.)
//
//	Also included:
//
//	o Simple linked list class that can be used anywhere (unix/mfc/console).
//    (Note: The linked list class is not thread safe.)
//
// I'd be happy to field any questions about the workings
// of the server, or how I set things up, but I'm not interested in fixing 
// up this server, or fixing any of the "chess game" bugs. Any bugs
// found in the socket classes I would be interested in, although they are 
// throughly tested.
//
// I built this server one night as a demo for a company in hopes
// of getting a job there. Didn't know a damn thing about sockets
// so I used, and built upon a blockingsocket class that was found 
// in Inside Visual C++ 5.0 by MS PRESS.
//
// There are no rules to either game, so players can move whatever 
// piece to whatever square. There isn't even any error checking,
// and you can't backspace. This is a very limited server in that
// respect.

/* The layout of the server is:

		1. Create an Accept thread to accept connections on port 23 (telnet)
		2. Get a connection, create a new accept thread to be able to 
		   handle more connections.
		3. Log this person in, getting user name.
		4. Create a lobby of sorts where you can do simple chat, create a new
		   game, join a game, or watch a game.
		5. This lobby reads a line of input from the socket, and then checks 
		   what the command the user typed was via ParsCmd. 
		6. When the person creates a game, we add the game object to a pending 
		   list, and wait for someone to join the game.
		7. When a person JOIN's a game, we check for the game name on the pending
		   list, and assign that person as the second player on that game object.
		8. When you WATCH a game, you are added to a list of watches on that
		   game object.

  The game objects:

	The base class is the BoardGame class. Chess and Checkers games are
	derived from BoardGame. This is to make a consistant interface
	for adding players to a board game, and drawing the board. 

*/

//
// NOTE:
//
//	 There is probably a bit of an issue with people disconnecting,
//	 and things not being cleaned up. I never bothered to finish that 
//	 off, as it wasn't important. 
// 

#include <assert.h>
#include <memory.h>
#include <process.h>    /* _beginthread, _endthread */
#include <iostream.h>
#include <winsock.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>		//strstr
#include <llist.h>
#include "blockingsocket.h"
#include "gameob.h"

int repeat=1;

#define ASSERT assert
#define VERIFY assert

#define BUFFERSIZE 1000
CLList m_games;		//list of Chess Game Objects (NOTE: This isn't thread safe!)
CLList m_pending;	//list of games that are waiting for second player (NOTE: This isn't thread safe!)
CLList m_connections;		//list of all the people connected

void log(const char* s)
{
	printf("%s\n",s);
	OutputDebugString(s);
	OutputDebugString("\n");
}

//This is the main game loop. It reads in users moves, and prints out the board
//contents to all people in a single game.
void GameLoop(void* pParam)
{
	log("GameLoop start");

	BoardGame* pgame=(BoardGame*)pParam;
	char* help= "Enter the column & row of the piece you are moving from & to\r\n"
				"in the format FFTT. (ie: moving whites queen pawn ahead one from \r\n"
				"column 3, row 6 to column 3, row 5 would be entered 3635.)\r\n";
	char* entermove="What is your move? (?=help) ";
	char* whitesmove="It's whites move.";
	char* blacksmove="It's blacks move.";
	bool bAbort=false;
	bool bMadeMove=false;
	char inbuf[10];
	try
	{
		do
		{
			//handle whites move
			pgame->Draw();
			pgame->m_black->Print(whitesmove);
			bMadeMove=false;
			do
			{
				pgame->m_white->Write(entermove,strlen(entermove));
				pgame->m_white->ReadLine(inbuf,sizeof inbuf,300);
				if(!stricmp(inbuf,"quit"))
				{
					bAbort=true;
					pgame->m_white->Print("Aborting game");
					pgame->m_black->Print("White aborted game");
				}
				else if(!strcmp(inbuf,"?"))
				{
					pgame->m_white->Print(help);
				}
				else
				{
					if(strlen(inbuf)!=4)
						pgame->m_white->Print("Syntax error");
					else
					{
						if(pgame->ValidMove(inbuf[0],inbuf[1],inbuf[2],inbuf[3]))
						{
							bMadeMove=true;
							pgame->Move(inbuf[0],inbuf[1],inbuf[2],inbuf[3]);
						}
						else 
							pgame->m_white->Print("not a valid move");
					}
				}
			} while(!bMadeMove && !bAbort);
			//handle blacks move
			if(!bAbort)
			{
				pgame->Draw();
				pgame->m_white->Print(blacksmove);
				do
				{
					pgame->m_black->Write(entermove,strlen(entermove));
					pgame->m_black->ReadLine(inbuf,sizeof inbuf,300);
					if(!stricmp(inbuf,"quit"))
					{
						bAbort=true;
						pgame->m_black->Print("Aborting game");
						pgame->m_white->Print("Black aborted game");
					}
					else if(!strcmp(inbuf,"?"))
					{
						 pgame->m_black->Print(help);
					}
					else
					{
						if(strlen(inbuf)!=4)
							pgame->m_white->Print("Syntax error");
						else
						{
							bMadeMove=true;
							pgame->Move(inbuf[0],inbuf[1],inbuf[2],inbuf[3]);
						}
					}
				} while(!bMadeMove && !bAbort);
			}
		} while(!bAbort);
	}
	catch(const char* e)
	{
		log(e);
	}
	CLList* plist=m_games.FindItem(pgame);
	m_games.RemoveItem(plist);
//	delete pgame;

	log("GameLoop end");
}

//read in the persons user name, and make sure they are a valid user
bool Login(CTelnetSocket* psocket)
{
	char* login="Enter your login name (only \"guest\" works): ";
	char line[1000];
	
	psocket->Write(login,strlen(login));
	psocket->ReadLine(line,sizeof line);

	if(stricmp(line,"guest"))
		return false;

	psocket->m_wFlags |= CTelnetSocket::FLAG_VALIDATED;
	return true;		
		
}


ChessGameOb* FindGame(const char* pgamename)
{
	log("Findgame");
	ChessGameOb* o;
	CLList* plist=m_games.GetHead();
	while(plist)
	{
		o=(ChessGameOb*)plist->GetItem();
		log(o->m_gamename);
		if(!stricmp(o->m_gamename,pgamename))
		{
			//found the game name
			return o;
		}
		plist=plist->GetNext();
	}
	return NULL;
}

ChessGameOb* FindPendingGame(const char* pgamename)
{
	log("Find pending game");
	ChessGameOb* o;
	CLList* plist=m_pending.GetHead();
	while(plist)
	{
		o=(ChessGameOb*)plist->GetItem();
		log(o->m_gamename);
		if(!stricmp(o->m_gamename,pgamename))
		{
			//found the game name
			return o;
		}
		plist=plist->GetNext();
	}
	return NULL;
}


enum
{
	CREATE=1,
	JOIN,
	WATCH,
	QUIT,
	PLAY,
	COLOR
};
char* cmds[]={"create","join","watch","quit","play","color"};
int FindToken(const char* buf)
{
	for(int i=0; i<sizeof cmds/sizeof cmds[0]; i++)
	{
		if(strstr(buf,cmds[i]))
			return i+1;
	}
	return 0;
}

//send the chat in buf to all connections
void Chat(CPersonInstance* pInstance, char* buf)
{
	CPersonInstance* o;
	CLList* plist=m_connections.GetHead();
	while(plist)
	{
		o=(CPersonInstance*)plist->GetItem();
		if(o!=pInstance)
			o->Print(buf);

		plist=plist->GetNext();
	}
}

//Look at what the user typed, and act upon it.
bool ParsCmd(char* buf,CPersonInstance* pInstance)
{
	char* SyntaxErrorMsg="Syntax error. 'create chess|checkers name' " ;
	int itoken=FindToken(buf);
	char cmd[30],parm1[30],parm2[30];
	BoardGame* pgame;
	CLList* plist;

	switch(itoken)
	{
		case WATCH:
			//user typed "watch game"
			sscanf(buf,"%s %s",cmd,parm1);
			pgame=FindGame(parm1);
			if(!pgame)
				pInstance->Print("No game found");
			else
			{
				if(!pgame->HasAllPlayers())
					pInstance->Print("Both players have to be playing befor watchin is aloud");
				else
				{
					pgame->AddWatcher(pInstance);
					pInstance->Print("You will see the board when the next person moves");
				}
				return true;
			}
			break;

		case CREATE:
			//user typed "create chess game"
			if(3!=sscanf(buf,"%s %s %s",cmd,parm1,parm2))	//get game name
			{
				pInstance->Print(SyntaxErrorMsg);
			}
			else
			{
				if(!stricmp(parm1,"checkers"))
				{
					pgame=new CheckerGameOb(pInstance,parm2);
					pInstance->Print("Created new checkers game");
				}
				else if(!stricmp(parm1,"chess"))
				{
					pInstance->Print("Created new chess game");
					pgame=new ChessGameOb(pInstance,parm2);
				}
				else
				{
					pInstance->Print(SyntaxErrorMsg);
					return false;
				}

				m_pending.AddTail(pgame);
	#if 1
				log("Pending games:");
				plist=m_pending.GetHead();
				while(plist)
				{
					pgame=(BoardGame*)plist->GetItem();
					log(pgame->m_gamename);
					plist=plist->GetNext();
				}
	#endif
				pInstance->Print("You must now wait till someone joins your game");
				return true;
			}
			break;

		case JOIN:
			//user typed "join game"
			sscanf(buf,"%s %s",cmd,parm1);
			pgame=FindPendingGame(parm1);
			if(pgame)
			{
				pgame->AddBlack(pInstance);
				plist=m_pending.FindItem(pgame);
				if(plist)
					m_pending.RemoveItem(plist);
				m_games.AddTail(pgame);
				_beginthread(GameLoop,0,pgame);
				return true;
			}
			else
			{
				pInstance->Print("No game found");
			}
			break;

		case QUIT:
			//user typed "quit"
			pInstance->Print("Good bye");
			pInstance->Close();
			delete pInstance;
			return true;

		default:
			//LBP (01/08/99): Print this chat to all players in the lobby.
			Chat(pInstance,buf);
			break;
	}
	return false;
}

//LBP (01/08/99): this is the lobby of sorts.
void TalkProc(void* pParam)
{
	log("TalkProc start");
	char* msg[]=
	{
		"Type 'create name' to make a game where 'name' is the game name you choose.\r\n" 	//0
		"Type 'join name' to join a player that created a game.\r\n"
		"Type 'watch name' to watch a game in progress",
		"Good bye.",						//1
		"Syntax Error"						//2
	};
	CPersonInstance* pInstance=(CPersonInstance*)pParam;
	char inbuf[CTelnetSocket::nSizeRecv];
	bool bAbort=false;

	m_connections.AddTail(pInstance);

	try
	{
		pInstance->Print(msg[0]);
		do
		{
			pInstance->Write("% ",2);
			pInstance->ReadLine(inbuf,sizeof inbuf,1800);
			if(ParsCmd(inbuf,pInstance))
			{
				bAbort=true;
			}
		} while(!bAbort);
	}
	catch(const char* e)
	{
		log(e);
		pInstance->Cleanup();
		delete pInstance;
	}
	log("TalkProc end");
	_endthread();
}

void AcceptProc(void* pParam)
{
	CBlockingSocket* psockListen=(CBlockingSocket*)pParam;
	CSockAddr saClient;
	CPersonInstance sConnect;
	char* buffer;

	try 
	{
		char* welcome="Welcome to the club.";
		char* nonuser="Sorry, members only";

		buffer=(char*)calloc(1,1000);
		assert(buffer);
		if(!psockListen->Accept(sConnect,saClient))
			_endthread();
		_beginthread(AcceptProc,0,pParam);		//get ready for another connection

		log("Connection accepted");
		
		if(!Login(&sConnect))
		{
			sConnect.Print(nonuser);
			sConnect.Close();
		}
		else
		{
			sConnect.Print(welcome);

			//LBP (01/08/99): start up a lobby for this new connection
			CPersonInstance* newinstance=new CPersonInstance(sConnect);
			_beginthread(TalkProc,0,newinstance);
		}
	}
	catch(const char* e)
	{
		log(e);
		sConnect.Cleanup();
	}
	_endthread();
}

void CheckKey(void* dummy)
{
	int a;
	cin >> a;
	repeat=0;
}

void ProcessGames()
{
	_beginthread(CheckKey,0,NULL);

	do
	{
	} while(repeat);
}

void main(void)
{
	CBlockingSocket sockListen;
	WSADATA wsd;
	if(WSAStartup(0x0101,&wsd)!=0)
	{
		cout << "Unable to start socket\n";
	}
	else
	{
		cout << "Chess Server Demo by Lee Patterson\n";
		cout << "leepatterson@home.com\n\n";
		cout << "Demonstrates an object oriented server that can handle\n";
		cout << "any number of chess games with 2 players and any number of\n";
		cout << "watchers per game.\n\n";
		cout << "Use telnet as a client connect to this server (ie: telnet localhost).\n";
		cout << "Server uses port 23 (telnet default).\n";
		cout << "Ctrl+c exits server";
		try 
		{
			CSockAddr saServer(INADDR_ANY,23);
			sockListen.Create();
			sockListen.Bind(saServer);
			sockListen.Listen();
			_beginthread(AcceptProc,0,&sockListen);
		}
		catch(const char* e)
		{
			sockListen.Cleanup();
			cout << e << "\n";
		}

		ProcessGames();

		try
		{
			sockListen.Close();
			Sleep(300);
			WSACleanup();
		}
		catch(const char* e)
		{
			cout << e << "\n";
		}
	}
}

