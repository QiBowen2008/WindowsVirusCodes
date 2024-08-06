#ifndef _GSocket_Class
#define _GSocket_Class
#include "windows.h"
#include "winsock.h"
#define ECreate "无法建立套接字对象"
#define GSOCKET_ERROR SOCKET_ERROR
#define GSOCKET_TIMEOUT 0
#define GSOCKET_OK 1
typedef unsigned long ulong;
class GSocket
{
public:
	GSocket();
	virtual	~GSocket();
	virtual bool Create();
	virtual int Send(char*,int,int TimeOut);
	virtual int Receive(char*,int,int TimeOut);
	virtual int Connect(char*Server,int Port);
	virtual int Close();
protected:
	virtual	bool	ServerToIp(char*);
	virtual int 	Select(bool isSend,int TimeOut);
private:
	SOCKET _GSocket;
	char*_Server;
	sockaddr_in _sockaddr;
	int _GSType,
		_Port;
	ulong	_ServerIp;
	hostent* _serverhost;
	int _StartUpTime;
	WSADATA _wsaData;
};
#endif