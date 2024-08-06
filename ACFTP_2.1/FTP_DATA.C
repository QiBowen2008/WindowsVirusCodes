/*---------------------------------------------------------------------
 *
 *  Program: AC_FTP.EXE Asynch Ftp Client (TCP)
 *
 *  filename: FTP_DATA.C
 *
 *  copyright by Bob Quinn, 1995
 *   
 *  Description:
 *    Client application that uses "file transfer protocol" (ftp)
 *    service as described by RFC 959.  
 *
 *    This module contains the functions that deal with the FTP
 *    data connection (for sending and receiving data between 
 *    client and server to service LIST, RECV and STOR commands). 
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
#include <stdlib.h>    /* for _ltoa() */
#include <winsock.h>
#include "resource.h"
#include <direct.h>    /* for Microsoft find file structure */
#include "ac_ftp.h"

/*--------------------------------------------------------------
 * Function: InitDataConn()
 *
 * Description: Set up a listening socket for a data connection
 */
SOCKET InitDataConn(PSOCKADDR_IN lpstName, HWND hDlg, u_int nAsyncMsg)
{
  int nRet;
  SOCKET hLstnSock;
  int nLen = SOCKADDR_LEN;
  
  if (bDebug) {
    wsprintf(achTempBuf, 
      "InitDataConn()   Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, State:%d\n", 
      nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd,
      astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, nAppState);
    OutputDebugString (achTempBuf);    
  }
  lByteCount = 0;  /* init byte counter */
  
  /* Get a TCP socket to use for data connection listen*/
  hLstnSock = socket (AF_INET, SOCK_STREAM, 0);
  if (hLstnSock == INVALID_SOCKET)  {
    WSAperror(WSAGetLastError(), "socket()", hInst);
  } else {
    /* Request async notification for most events */
    nRet = WSAAsyncSelect(hLstnSock, hDlg, nAsyncMsg, 
           (FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE));
    if (nRet == SOCKET_ERROR) {
      WSAperror(WSAGetLastError(), "WSAAsyncSelect()", hInst);
    } else {
                   
      /* Name the local socket with bind() */
      lpstName->sin_family = PF_INET;
      lpstName->sin_port   = 0;  /* any port will do */
      nRet = bind(hLstnSock,(LPSOCKADDR)lpstName,SOCKADDR_LEN);
      if (nRet == SOCKET_ERROR) {
	    WSAperror(WSAGetLastError(), "bind()", hInst);
      } else {
        
        /* Get local port number assigned by bind() */
        nRet = getsockname(hLstnSock,(LPSOCKADDR)lpstName, 
            (int FAR *)&nLen);
        if (nRet == SOCKET_ERROR) {
	      WSAperror(WSAGetLastError(), "getsockname()", hInst);
        } else {

          /* Listen for incoming connection requests */
          nRet = listen(hLstnSock, 5);
          if (nRet == SOCKET_ERROR) {
	        WSAperror(WSAGetLastError(), "listen()", hInst);
          }
        }
      }
    }
    /* If we haven't had an error but we still don't know the local 
     *  IP address, then we need to try to get it before we return */
    if (!lpstName->sin_addr.s_addr) {
      lpstName->sin_addr.s_addr = GetHostID();
      if (!lpstName->sin_addr.s_addr) {
        MessageBox (hDlg, "Can't get local IP address", 
          "InitDataConn() Failed", MB_OK | MB_ICONASTERISK);
        nRet = SOCKET_ERROR;
      }
    }
    /* If we had an error or we still don't know our IP address, 
     *  then we have a problem.  Clean up */
    if (nRet == SOCKET_ERROR) {
	  closesocket(hLstnSock);
	  hLstnSock = INVALID_SOCKET;
    }
  }
  return (hLstnSock);
} /* end InitDataConn() */

/*--------------------------------------------------------------
 * Function: AcceptDataConn()
 *
 * Description: Accept an incoming data connection
 */
SOCKET AcceptDataConn(SOCKET hLstnSock, PSOCKADDR_IN pstName)
{
  SOCKET hDataSock;
  int nRet, nLen = SOCKADDR_LEN, nOptval;
  
  if (bDebug) {
    wsprintf(achTempBuf, 
      "AcceptDataConn() Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, State:%d\n", 
      nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd,
      astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, nAppState);
    OutputDebugString (achTempBuf);    
  }

  hDataSock = accept (hLstnSock, (LPSOCKADDR)pstName, (LPINT)&nLen);
  if (hDataSock == SOCKET_ERROR) {
    int WSAErr = WSAGetLastError();
    if (WSAErr != WSAEWOULDBLOCK)
      WSAperror (WSAErr, "accept", hInst);
  } else if (bReAsync) {
    /* This SHOULD be unnecessary, since all new sockets are supposed
     *  to inherit properties of the listening socket (like all the
     *  asynch events registered but some WinSocks don't do this.
     * Request async notification for most events */
    nRet = WSAAsyncSelect(hDataSock, hWinMain, WSA_ASYNC+1, 
           (FD_READ | FD_WRITE | FD_CLOSE));
    if (nRet == SOCKET_ERROR) {
      WSAperror(WSAGetLastError(), "WSAAsyncSelect()", hInst);
    }
    /* Try to get lots of buffer space */
    nOptval = astFtpCmd[0].nFtpCmd==STOR ? SO_SNDBUF : SO_RCVBUF;
    GetBuf(hDataSock, INPUT_SIZE*2, nOptval);
  }
  return (hDataSock);
} /* end AcceptData() */

/*--------------------------------------------------------------
 * Function: SendData()
 *
 * Description: Open data file, read and send
 */
long SendData(SOCKET *hDataSock, HFILE hDataFile, int len)
{
  static int cbReadFromFile;         /* bytes read from file */
  static int cbSentToServer;         /* number of buffered bytes sent */
  static HFILE hLastFile;            /* handle of last file sent */
  long cbTotalSent = 0;              /* total bytes sent */
  int nRet, WSAErr, cbBytesToSend;

  /* Reset our counters when we access a new file */
  if (hLastFile != hDataFile) {
    cbReadFromFile = 0;
    cbSentToServer = 0;
    hLastFile = hDataFile;
  }
    
  /* Read data from file, and send it. */
  do {
    if (bIOBeep)
      MessageBeep(0xFFFF);

    /* calculate what's left to send */
    cbBytesToSend = cbReadFromFile - cbSentToServer; 
    if (cbBytesToSend <= 0) {
    
      /* read data from input file, if we need it */
      if (!bFromNul) {
        cbReadFromFile = _lread(hDataFile, achOutBuf, INPUT_SIZE);
        if (cbReadFromFile == HFILE_ERROR) {
          MessageBox (hWinMain, "Error reading data file", 
            "SendData() Failed", MB_OK | MB_ICONASTERISK);
          break;
        } else if (!cbReadFromFile){
          /* EOF: no more data to send */
          CloseFtpConn(hDataSock, (PSTR)0, 0, hWinMain);
          EndData();
          break;
        } else {
          cbBytesToSend = cbReadFromFile; /* send as much as we read */
        } 
      } else {
        /* just send whatever's in memory (up to our max) */
        if (lByteCount < MAXNULPUT) {
          cbBytesToSend = INPUT_SIZE;
        } else {
          CloseFtpConn(hDataSock, (PSTR)0, 0, hWinMain);
          EndData();
        }
      }
      cbSentToServer = 0;  /* reset tally */
    }
    /* Send data to server */
    nRet = send (*hDataSock, &(achOutBuf[cbSentToServer]), 
                ((len < cbBytesToSend) ? len : cbBytesToSend), 0);

    if (nRet == SOCKET_ERROR) {
      WSAErr = WSAGetLastError();
      /* Display significant errors */
      if (WSAErr != WSAEWOULDBLOCK)
        WSAperror(WSAErr, (LPSTR)"send()", hInst);
    } else {
      /* Update byte counter, and display. */
      lByteCount += nRet;
      _ltoa(lByteCount, achTempBuf, 10);
      SetDlgItemText(hWinMain, IDC_DATA_RATE, achTempBuf);
      cbSentToServer += nRet;/* tally bytes sent since last file read */
      cbTotalSent    += nRet;/* tally total bytes sent since we started */
    }
  } while (nRet != SOCKET_ERROR);

  return (cbTotalSent);
} /* end SendData() */

/*--------------------------------------------------------------
 * Function: RecvData()
 *
 * Description: Receive data from net and write to open data file 
 */
int RecvData(SOCKET hDataSock, HFILE hDataFile, LPSTR achInBuf, int len)
{
  static HFILE hLastFile;          /* handle of last file sent */
  static int cbBytesBuffered;      /* total bytes received */
  int cbBytesRcvd = 0;
  int nRet=0, WSAErr;

   if (hDataFile != hLastFile) {
      hLastFile = hDataFile;
      cbBytesBuffered = 0; 
   }
                                                       
  /* Read as much as we can from server */
    while (cbBytesBuffered < len) {

      nRet = recv (hDataSock,&(achInBuf[cbBytesBuffered]), 
        len-cbBytesBuffered, 0);

      if (nRet == SOCKET_ERROR) {
        WSAErr = WSAGetLastError();
        /* Display significant errors */
        if (WSAErr != WSAEWOULDBLOCK)
          WSAperror(WSAErr, (LPSTR)"recv()", hInst);
        /* exit recv() loop on any error */
          goto recv_end;
      } else if (nRet == 0) { /* Other side closed socket */
        /* quit if server closed connection */
        goto recv_end;
          
      } else {
        /* Update byte counter, and display */
        lByteCount += nRet;
        _ltoa(lByteCount, achTempBuf, 10);
        SetDlgItemText(hWinMain, IDC_DATA_RATE, achTempBuf);
        cbBytesRcvd += nRet;     /* tally bytes read */
        cbBytesBuffered += nRet;
      }
    }
recv_end:
  if (!bToNul && 
      ((cbBytesBuffered > (len-MTU_SIZE)) || 
       ((nRet == SOCKET_ERROR) && WSAGetLastError() != WSAEWOULDBLOCK) || 
       (nRet == 0))) {
    /* If we have a lot buffered, write to data file */
    nRet = _lwrite(hDataFile, achInBuf, cbBytesBuffered);
    if (nRet == HFILE_ERROR)
      MessageBox (hWinMain, "Can't write to local file", 
        "RecvData() Failed", MB_OK | MB_ICONASTERISK);
    cbBytesBuffered = 0;
  } else if (bToNul)
    cbBytesBuffered = 0;
  return (cbBytesRcvd);
} /* end RecvData() */

/*--------------------------------------------------------------
 * Function: EndData()
 *
 * Description: Close up the data connection
 */
void EndData (void) {
  LONG dByteRate;
  LONG lMSecs;

  /* Calculate data transfer rate, and display */
  lMSecs = (LONG) GetTickCount() - lStartTime;
  if (lMSecs <= 55)
    lMSecs = 27;  /* about half of 55Msec PC clock resolution */
              
  /* Socket Check should not be necessary, but some WinSocks            
   *  mistakenly post FD_CLOSE to listen socket after close */
  nAppState &= ~(DATACONNECTED);
  SetDlgItemText (hWinMain, IDC_STATUS, "Status: connected");
            
  if (lByteCount > 0L) {
    dByteRate = (lByteCount/lMSecs); /* data rate (bytes/Msec) */
    wsprintf (achTempBuf,"%ld bytes %s in %ld.%ld seconds (%ld.%ld Kbytes/sec)",
      lByteCount, 
      ((astFtpCmd[0].nFtpCmd==STOR) ? "sent":"received"),
      lMSecs/1000, lMSecs%1000, 
      (dByteRate*1000)/1024, (dByteRate*1000)%1024);
    SetDlgItemText (hWinMain, IDC_DATA_RATE, achTempBuf);
    if (hLogFile != HFILE_ERROR)
      _lwrite (hLogFile, achTempBuf, strlen(achTempBuf));
  }
  lStartTime = 0L;
  if (hDataFile != HFILE_ERROR) {
    _lclose (hDataFile);
    hDataFile = HFILE_ERROR;
    if (astFtpCmd[0].nFtpCmd == LIST) {
      wsprintf (achTempBuf, "notepad %s", szTempFile);
      WinExec (achTempBuf, SW_SHOW);
    }
  }
  astFtpCmd[0].nFtpCmd = 0;  /* reset pending command */
} /* end EndData() */            
