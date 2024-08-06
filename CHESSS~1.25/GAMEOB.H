// Copyright (c) 1999 Lee Patterson
// leepatterson@home.com

class CPersonInstance : public CTelnetSocket
{
public:
	CPersonInstance();
	~CPersonInstance();

public:
	enum Color {WHITE,BLACK};
	enum Location {LOBBY,CHESS,CHECKERS};

protected:
	int m_color;
	int m_location;
	int m_channel;
};


class BoardSpace
{
public:
	BoardSpace();

public:
	enum { EMPTY,WPAWN,WKNIGHT,WBISHOP,WROOK,WQUEEN,WKING,BPAWN,BKNIGHT,BBISHOP,BROOK,BQUEEN,BKING };
	enum { EMPTYCHECKER, WSQUARE, BSQUARE, WCHECKER, BCHECKER };

		
public:
	int m_id;
	int m_type;
};

class BoardGame
{
public:
	BoardGame();
	void AddPerson(CPersonInstance* pInstance);
	void RemovePerson(CPersonInstance* pInstance);
	int Left(int v);
	int Right(int v);
	UINT Type() {return m_nBoardType;}
	virtual bool HasAllPlayers()=0;
	virtual void AddBlack(CPersonInstance* pInstance)=0;
	virtual void AddWatcher(CPersonInstance* pInstance)=0;

	//functions that must be defined in derived class
	virtual bool ValidMove(int from0,int from1,int to0,int to1)=0;
	virtual void Move(int from0,int from1,int to0,int to1)=0;
	virtual void Draw()=0;			//draw the board on all connections to this board game
	CLList m_connections;			//list of PersonInstances
	CPersonInstance* m_white;
	CPersonInstance* m_black;
	char m_gamename[30];

	enum {BTYPE_UNDEFINED, BTYPE_CHECKERS, BTYPE_CHESS};
protected:
	BoardSpace m_spaces[8][8];
	UINT m_nBoardType;
};

class ChessBoard : public BoardGame
{
public:
	ChessBoard();
	 bool ValidMove(int from0,int from1,int to0,int to1);
	virtual void Move(int from0,int from1,int to0,int to1);
	virtual void FillBoard();
	virtual void Draw();
	virtual bool HasAllPlayers()=0;
	virtual void AddBlack(CPersonInstance* pInstance)=0;
	virtual void AddWatcher(CPersonInstance* pInstance)=0;

protected:
	void DrawBoard(CPersonInstance* psocket);
	char GetPiece(int type);
};


class ChessGameOb : public ChessBoard
{
public:
	ChessGameOb(CPersonInstance* pInstanceWhite,const char* pGameName);
	~ChessGameOb();
	void AddWatcher(CPersonInstance* pInstance);
	void AddBlack(CPersonInstance* pInstance);
	bool HasAllPlayers();

public:
	CLList m_watchers;
	WORD m_wFlags;
	enum tagFlags {WAITINGFORPLAYER,INPROGRESS};

};

class CheckerBoard : public BoardGame
{
public: 
	CheckerBoard();
	 bool ValidMove(int from0,int from1,int to0,int to1);
	virtual void Move(int from0,int from1,int to0,int to1);
	virtual void FillBoard();
	virtual void Draw();
	virtual bool HasAllPlayers()=0;
	virtual void AddBlack(CPersonInstance* pInstance)=0;
	virtual void AddWatcher(CPersonInstance* pInstance)=0;

protected:
	void DrawBoard(CPersonInstance* psocket);
	char GetPiece(int type);
};

class CheckerGameOb : public CheckerBoard
{
public:
	CheckerGameOb(CPersonInstance* pInstanceWhite,const char* pGameName);
	~CheckerGameOb();
	void AddWatcher(CPersonInstance* pInstance);
	void AddBlack(CPersonInstance* pInstance);
	bool HasAllPlayers();
	bool HasBothPlayers();

public:
	CLList m_watchers;
	WORD m_wFlags;
	enum tagFlags {WAITINGFORPLAYER,INPROGRESS};
};
