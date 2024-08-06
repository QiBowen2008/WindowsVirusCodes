/*GSocket是一个很简单的SOCKET类，写这个类的目的是为了写极速寻呼！不久以后我会写个更强大的SOCKET类，大家可以去小弟的主页上看看！*/
#include "GSocket.h"
//-----------------------------------------
GSocket::GSocket()
{
_GSocket=INVALID_SOCKET;
_StartUpTime=0;
}
GSocket::~GSocket()
{
Close();
	while(_StartUpTime)
	{
		WSACleanup();
		_StartUpTime--;
	}
}
bool GSocket::Create()
{
	ZeroMemory(&_wsaData,sizeof(_wsaData));
	if(_GSocket!=INVALID_SOCKET)return false;
	if(WSAStartup(0x101,&_wsaData))return 0;
	_StartUpTime++;
	_GSocket=socket(AF_INET,SOCK_STREAM,0);
	if(_GSocket==INVALID_SOCKET)
		return 0;
	return 1;
}
bool GSocket::ServerToIp(char*_GServer)
{
if((_ServerIp=inet_addr(_GServer))==INADDR_NONE)
	{
		_serverhost=gethostbyname(_GServer);
		if(_serverhost)
			memcpy((void*)&_ServerIp,(void*)_serverhost->h_addr,_serverhost->h_length);
		else
			return false;
	}
return true;
}
int GSocket::Connect(char*Server,int Port)
{
ZeroMemory(&_sockaddr,sizeof(sockaddr));
_Port=Port;
if(!ServerToIp(Server))return SOCKET_ERROR;
_sockaddr.sin_family=AF_INET;
_sockaddr.sin_port=htons(Port);
_sockaddr.sin_addr.S_un.S_addr =_ServerIp;
return connect(_GSocket,(sockaddr*)&_sockaddr,sizeof(sockaddr));
}
int GSocket::Send(char*szSendBuf,int iSendLength,int TimeOut)
{
int ret;
ret=Select(true,TimeOut);
if(ret==GSOCKET_ERROR)return GSOCKET_ERROR;
if(ret==GSOCKET_TIMEOUT)return GSOCKET_TIMEOUT;
return send(_GSocket,szSendBuf,iSendLength,0);
}
int GSocket::Receive(char*szReceiveBuf,int iReceiveLength,int TimeOut)
{
int ret;
ret=Select(false,TimeOut);
if(ret==GSOCKET_ERROR)return GSOCKET_ERROR;
if(ret==GSOCKET_TIMEOUT)return GSOCKET_TIMEOUT;
return recv(_GSocket,szReceiveBuf,iReceiveLength,0);
}
int GSocket::Close()
{
 int ret;
 if(_GSocket!=INVALID_SOCKET)
 {

	ret=closesocket(_GSocket);
	if(ret!=SOCKET_ERROR)_GSocket=INVALID_SOCKET;
}
 return ret;
}
int GSocket::Select(bool isSend,int TimeOut)
{
fd_set _fd_set;
timeval _timeOut;
FD_ZERO(&_fd_set);
FD_SET(_GSocket,&_fd_set);
if(TimeOut<0)TimeOut=0;
_timeOut.tv_sec=TimeOut;
_timeOut.tv_usec=0;
if(!isSend)
	return select(0,&_fd_set,NULL,NULL,&_timeOut);
else
	return select(0,NULL,&_fd_set,NULL,&_timeOut);
}
