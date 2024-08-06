/*---------------------------------------------------------------------
 *
 *  Program: AC_FTP.EXE Asynch Ftp Client (TCP)
 *
 *  filename: FTP_CTRL.C
 *
 *  copyright by Bob Quinn, 1995
 *   
 *  Description:
 *    Client application that uses "file transfer protocol" (ftp)
 *    service as described by RFC 959.  
 *
 *    This module contains the functions that deal with the FTP
 *    control connection (for sending commands to FTP server, 
 *    receiving replies from FTP server, and processing them.
 *
 *  This software is not subject to any  export  provision  of
 *  the  United  States  Department  of  Commerce,  and may be
 *  exported to any country or planet.
 *
 *  Permission is granted to anyone to use this  software  for any  
 *  purpose  on  any computer system, and to alter it and redistribute 
 *  it freely, subject to the following  restrictions:
 *
 *  1. The author is not responsible for the consequences of
 *     use of this software, no matter how awful, even if they
 *     arise from flaws in it.
 *
 *  2. The origin of this software must not be misrepresented,
 *     either by explicit claim or by omission.  Since few users
 *     ever read sources, credits must appear in the documentation.
 *
 *  3. Altered versions must be plainly marked as such, and
 *     must not be misrepresented as being the original software.
 *     Since few users ever read sources, credits must appear in
 *     the documentation.
 *
 *  4. This notice may not be removed or altered.
 *	 
 ---------------------------------------------------------------------*/
#include "..\wsa_xtra.h" 
#include <windows.h>
#include <windowsx.h>
#include "..\winsockx.h"

#include <string.h>    /* for _fmemcpy() & _fmemset() */
#include <winsock.h>
#include "resource.h"
#include <direct.h>    /* for Microsoft find file structure */

#include "ac_ftp.h"
/*---------------------------------------------------------------
 * Function: InitCtrlConn()
 *
 * Description: get a TCP socket, register for async notification,
 *   and then connect to Ftp server
 */
SOCKET InitCtrlConn(PSOCKADDR_IN pstName, HWND hDlg, u_int nAsyncMsg) 
{
  int nRet;
  SOCKET hCtrlSock;
                     
  if (bDebug) 
    OutputDebugString("InitCtrlConn()\n");
    
  /* Get a TCP socket for control connection */
  hCtrlSock = socket (AF_INET, SOCK_STREAM, 0);
  if (hCtrlSock == INVALID_SOCKET)  {
    WSAperror(WSAGetLastError(), "socket()", hInst);
  } else {
                 
    /* Request async notification for most events */
    nRet = WSAAsyncSelect(hCtrlSock, hDlg, nAsyncMsg, 
           (FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE));
    if (nRet == SOCKET_ERROR) {
      WSAperror(WSAGetLastError(), "WSAAsyncSelect()", hInst);
      closesocket(hCtrlSock);
      hCtrlSock = INVALID_SOCKET;
    } else {
                   
      /* Initiate non-blocking connect to server */
      pstName->sin_family = PF_INET;
      pstName->sin_port   = htons(IPPORT_FTP);
      nRet = connect(hCtrlSock,(LPSOCKADDR)pstName,SOCKADDR_LEN);
      if (nRet == SOCKET_ERROR) {
	    int WSAErr = WSAGetLastError();
		               
	    /* Anything but "would block" error is bad */
	    if (WSAErr != WSAEWOULDBLOCK) {
	      /* Report error and clean up */
	      WSAperror(WSAErr, "connect()", hInst);
	      closesocket(hCtrlSock);
	      hCtrlSock = INVALID_SOCKET;
	    }
      }
    }
  }
  return (hCtrlSock);
} /* end InitCtrlConn() */

/*--------------------------------------------------------------
 * Function: SendFtpCmd()
 *
 * Description: Format and send an FTP command to the server
 */
int SendFtpCmd(void)
{
  int nRet, nLen, nBytesSent = 0;
  int nFtpCmd = astFtpCmd[1].nFtpCmd;
  
  if (bDebug) {
    wsprintf(achTempBuf, 
      "SendFtpCmd()    Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, State:%d\n", 
      nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd, 
      astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, nAppState);
    OutputDebugString (achTempBuf);
  }

  /* Create a command string (if we don't already have one) */
  if (szFtpCmd[0] == 0) {
    
    switch (nFtpCmd) {
      case PORT:
        wsprintf (szFtpCmd, "PORT %d,%d,%d,%d,%d,%d\r\n", 
          stDLclName.sin_addr.S_un.S_un_b.s_b1,  /* local addr */
          stDLclName.sin_addr.S_un.S_un_b.s_b2,
          stDLclName.sin_addr.S_un.S_un_b.s_b3,
          stDLclName.sin_addr.S_un.S_un_b.s_b4,
          stDLclName.sin_port & 0xFF,            /* local port */
         (stDLclName.sin_port & 0xFF00)>>8);
        break;
      case CWD:
      case DELE:
      case PASS:
      case RETR:
      case STOR:
      case TYPE:
      case USER:
        /* Ftp commmand and parameter */
        wsprintf (szFtpCmd, "%s %s\r\n", 
          aszFtpCmd[nFtpCmd], &(astFtpCmd[1].szFtpParm));
        break;
      case ABOR:
      case LIST:
      case PWD:
      case QUIT:
        /* Solitary Ftp command string (no parameter) */
        wsprintf (szFtpCmd, "%s\r\n", aszFtpCmd[nFtpCmd]);
        break;
      default: 
        return (0);  /* we have a bogus command! */
    }
  }
  nLen = strlen(szFtpCmd);
  
  if (hCtrlSock != INVALID_SOCKET) {         
    /* Send the ftp command to control socket */
    while (nBytesSent < nLen) {
      nRet = send(hCtrlSock, (LPSTR)szFtpCmd, nLen-nBytesSent, 0);
      if (nRet == SOCKET_ERROR) {
        int WSAErr = WSAGetLastError();
      
        /* If "would block" error we'll pickup again with async
         *  FD_WRITE notification, but any other error is bad news */
        if (WSAErr != WSAEWOULDBLOCK) 
          WSAperror(WSAErr, "SendFtpCmd()", hInst);
        break;
      }
      nBytesSent += nRet;
    }
  }
  /* if we sent it all, update our status and move everything up 
   *  in command queue */
  if (nBytesSent == nLen) {
    int i;

    if (nFtpCmd == PASS)                 /* hide password */
      memset (szFtpCmd+5, 'x', 10);
        
    if (bLogFile)                         /* log command */
      _lwrite (hLogFile, szFtpCmd, strlen(szFtpCmd));

    GetDlgItemText (hWinMain, IDC_REPLY,  /* display command */
      achRplyBuf, RPLY_SIZE-strlen(szFtpCmd));
    wsprintf (achTempBuf, "%s%s", szFtpCmd, achRplyBuf);
    SetDlgItemText (hWinMain, IDC_REPLY, achTempBuf);

    szFtpCmd[0] = 0;  /* disable Ftp command string */
    
    /* move everything up in the command queue */
    for (i=0; i < nQLen; i++) {
      astFtpCmd[i].nFtpCmd = astFtpCmd[i+1].nFtpCmd;
      astFtpCmd[i+1].nFtpCmd = 0;        /* reset old command */
      if (*(astFtpCmd[i+1].szFtpParm)) {
        memcpy (astFtpCmd[i].szFtpParm, astFtpCmd[i+1].szFtpParm, CMD_SIZE);
        *(astFtpCmd[i+1].szFtpParm) = 0; /* terminate old string */
      } else {
        *(astFtpCmd[i].szFtpParm) = 0;   /* terminate unused string */
      }
    }
    nQLen--;          /* decrement the queue length */
    
    switch (nFtpCmd) {
      case (USER):
        SetDlgItemText (hWinMain, IDC_STATUS,"Status: connecting");
        break;
      case (STOR):
        SetDlgItemText (hWinMain, IDC_STATUS,"Status: sending a file");
        break;
      case (RETR):
        SetDlgItemText (hWinMain, IDC_STATUS,"Status: receiving a file");
        break;
      case (LIST):
        SetDlgItemText (hWinMain, IDC_STATUS,"Status: receiving directory");
        break;
      case (QUIT):
        SetDlgItemText (hWinMain, IDC_SERVER, "Server: none");
        SetDlgItemText (hWinMain, IDC_STATUS, "Status: not connected");
        break;
    }
  }
  return (nBytesSent);
} /* end SendFtpCmd() */
                     
/*--------------------------------------------------------------
 * Function: QueueFtpCmd()
 *
 * Description: Put FTP command in command queue for sending after we
 *  receive responses to pending commands or now if nothing is pending
 */
BOOL QueueFtpCmd(int nFtpCmd, LPSTR szFtpParm) {
   
  if (bDebug) {
    wsprintf(achTempBuf, 
      "QueueFtpCmd()    Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, pend:%d\n", 
      nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd,
      astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, 
      recv(hCtrlSock, achTempBuf, BUF_SIZE, MSG_PEEK));
    OutputDebugString (achTempBuf);    
  }
  if ((nFtpCmd == ABOR) || (nFtpCmd == QUIT)) {
    AbortFtpCmd();
    if (hCtrlSock != INVALID_SOCKET)
      SetDlgItemText (hWinMain, IDC_STATUS,"Status: connected");
  } else if (nQLen == MAX_CMDS) {
    /* Notify user if they can't fit in the queue */
    MessageBox (hWinMain, "Ftp command queue is full, try again later", 
       "Can't Queue Command", MB_OK | MB_ICONASTERISK);
    return (FALSE);            /* not queued */
  }
  nQLen++;  /* increment Ftp command counter */

  /* Save command vitals */  
  astFtpCmd[nQLen].nFtpCmd = nFtpCmd;
  if (szFtpParm)
    lstrcpy (astFtpCmd[nQLen].szFtpParm, szFtpParm);

  if (!(astFtpCmd[0].nFtpCmd) && astFtpCmd[1].nFtpCmd) {
    /* If nothing pending reply, then send the next command */
    SendFtpCmd();
  }
  return (TRUE);   /* queued! */
}  /* end QueueFtpCmd() */

/*--------------------------------------------------------------
 * Function: AbortFtpCmd()
 *
 * Description: Clean up routine to abort a pending FTP command and
 * clear the command queue
 */
void AbortFtpCmd(void) {
  int i;

  if (hLstnSock != INVALID_SOCKET) {/* Close listen socket */
    closesocket(hLstnSock);
    hLstnSock = INVALID_SOCKET;
  }
  if (hDataSock != INVALID_SOCKET){ /* Close data socket */
    CloseFtpConn(&hDataSock,  
      (astFtpCmd[0].nFtpCmd != STOR) ? achInBuf : (PSTR)0, 
      INPUT_SIZE, hWinMain);
    EndData();
  }
  for (i=0;i<MAX_CMDS;i++)          /* Clear command queue */
    astFtpCmd[i].nFtpCmd = 0;         
  nQLen = 0;
} /* end AbortFtpCmd() */

/*--------------------------------------------------------------
 * Function: RecvFtpRply()
 *
 * Description: Read the FTP reply from server (and log it)
 */
int RecvFtpRply(SOCKET hCtrlSock, LPSTR szFtpRply, int nLen)
{
  int nRet=0;

  if (bDebug) {
    wsprintf(achTempBuf, 
      "RecvFtpRply()    Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, State:%d\n", 
      nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd,
      astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, nAppState);
    OutputDebugString (achTempBuf);
  }
  if (hCtrlSock != INVALID_SOCKET) {
    memset(szFtpRply,0,nLen);   /* Init receive buffer */
  
    /* Read as much as we can */
    nRet = recv(hCtrlSock,(LPSTR)szFtpRply,nLen,0);
    
    if (nRet == SOCKET_ERROR) {
      int WSAErr = WSAGetLastError();
      if (WSAErr != WSAEWOULDBLOCK) {
        WSAperror (WSAErr, "RecvFtpRply()", hInst);
      }
    } else if (bLogFile)   /* log reply */
      _lwrite (hLogFile, szFtpRply, nRet);     
  }
  return (nRet);  
} /* end RecvFtpReply() */

/*--------------------------------------------------------------
 * Function: ProcessFtpRply()
 *
 * Description: Figure out what happened, and what to do next.
 */
void ProcessFtpRply (LPSTR szRply, int nBufLen) 
{
  LPSTR szFtpRply;
  int nPendingFtpCmd, i;
  
  if (bDebug) {
    wsprintf(achTempBuf, 
      "ProcessFtpRply() Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, State:%d\n", 
      nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd,
      astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, nAppState);
    OutputDebugString (achTempBuf);    
  }

  /* Skip continuation lines (denoted by a dash after reply code
   *  or with a blank reply code and no dash) */
  szFtpRply = szRply;
  while ((*(szFtpRply+3) == '-') || 
        ((*(szFtpRply)==' ')&&(*(szFtpRply+1)==' ')&&(*(szFtpRply+2)==' '))) {
    /* find end of reply line */
    for (i=0;*szFtpRply!=0x0a && *szFtpRply && i<nBufLen-3; szFtpRply++,i++);
    szFtpRply++;       /* go to beginning of next reply */
    if (!(*szFtpRply)) /* quit if end of string */
      return;
  }
  
  *szFtpCmd  = 0;                        /* Disable old command string */
  nPendingFtpCmd = astFtpCmd[0].nFtpCmd; /* Save last Ftp Cmd */
  if ((*szFtpRply != '1') && 
      (nPendingFtpCmd != LIST) &&
      (nPendingFtpCmd != STOR) &&
      (nPendingFtpCmd != RETR))
    /* For any but preliminary reply, clear old command */    
    astFtpCmd[0].nFtpCmd = 0;

  /* First digit in 3-digit Ftp reply code is the most significant */
  switch (*szFtpRply) {
    case ('1'):  /* Positive preliminary reply */
      break;
    case ('2'):  /* Positive completion reply */
      switch(nPendingFtpCmd) {
        case 0:  
          /* Check for "220 Service ready for new user" reply, and
           *  send user command to login if login message found */
          if ((*(szFtpRply+1)=='2') && (*(szFtpRply+2)=='0'))
            QueueFtpCmd(USER, szUser);
          break;
        case CWD:
        case USER:
        case PASS:
          /* We're logged in!  Get remote working directory */
          QueueFtpCmd(PWD, 0);
          break;
        case PWD:
          /* Display remote working directory */
          SetDlgItemText (hWinMain, IDC_RPWD, &szFtpRply[4]);
          break;
        case TYPE:
        case PORT:
          /* Send next command (it's already queued) */
          SendFtpCmd();
          break;
        case ABOR:
          /* Close the data socket */
          if (hDataSock != INVALID_SOCKET)
            CloseFtpConn(&hDataSock, (PSTR)0, 0, hWinMain);
          break;
        case QUIT:
          /* Close the control socket */
          if (hCtrlSock != INVALID_SOCKET)
            CloseFtpConn(&hCtrlSock, (PSTR)0, 0, hWinMain);
          break;
        default:
          break; /* Nothing to do after most replies */
      }
      break;     
    case ('3'):  /* Positive intermediate reply */
      if (nPendingFtpCmd == USER)
        QueueFtpCmd(PASS, szPWrd);
      break;
    case ('4'):  /* Transient negative completion reply */
    case ('5'):  /* Permenant negative completion reply */
      /* If Port failed, forget about queued commands */
      if (nPendingFtpCmd != ABOR)
        QueueFtpCmd(ABOR, 0);
      break;
  }
} /* end ProcessFtpRply() */
