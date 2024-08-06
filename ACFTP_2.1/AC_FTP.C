/*---------------------------------------------------------------------
 *
 *  Program: AC_FTP.EXE Asynch Ftp Client (TCP)
 *
 *  filename: ac_ftp.c
 *
 *  copyright by Bob Quinn, 1995
 *   
 *  Description:
 *    Client application that uses "file transfer protocol" (ftp)
 *    service as described by RFC 959.  This application demonstrates
 *    bulk data transfer techniques, and reading variable length records 
 *    from a byte stream (TCP socket).  The user interface is minimized
 *    in order to emphasize the network code.
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
#define STRICT
#include "..\wsa_xtra.h"
#include <windows.h>
#include <windowsx.h>

#include <string.h>    /* for _fmemcpy() & _fmemset() */
#include <winsock.h>
#include "resource.h"
#include <time.h>
#include <stdio.h>
#include <direct.h>    /* for getcwd() */
#include "..\winsockx.h"

#include "ac_ftp.h"

/* Ftp commmand strings (indexed by our command macro values) */
LPSTR aszFtpCmd[] = {
  "",    "CWD", "DELE", "PASS","PORT","RETR","STOR",
  "TYPE","USER","ABOR","LIST","PWD",  "QUIT"
};
char szAppName[] = "AC_FTP";

BOOL nAppState=NOT_CONNECTED;    /* Application State */
                                   
BOOL bToNul  =FALSE;             /* Get to NUL device file */
BOOL bFromNul=FALSE;             /* Put from NUL device file */
BOOL bIOBeep =FALSE;             /* Beep on FD_READ, FD_WRITE */
BOOL bDebug  =FALSE;             /* Debug output to WinDebug */
BOOL bReAsync=TRUE;              /* Call WSAAsyncSelect after accept() */
BOOL bLogFile=TRUE;              /* Write Cmds and Replies to logfile */

SOCKET hCtrlSock=INVALID_SOCKET; /* Ftp control socket */
SOCKET hLstnSock=INVALID_SOCKET; /* Listening data socket */
SOCKET hDataSock=INVALID_SOCKET; /* Connected data socket */

char szHost[MAXHOSTNAME]={0};    /* Remote host name or address */
char szUser[MAXUSERNAME]={0};    /* User ID */
char szPWrd[MAXPASSWORD]={0};    /* User password */

SOCKADDR_IN stCLclName;          /* Control socket name (local client) */
SOCKADDR_IN stCRmtName;          /*                     (remote server) */
SOCKADDR_IN stDLclName;          /* Data socket name (local client)*/          
SOCKADDR_IN stDRmtName;          /*                  (remote server) */
                              
char achInBuf  [INPUT_SIZE];     /* Network input data buffer */
char achOutBuf [INPUT_SIZE];     /* Network output buffer */
char szFtpRply [RPLY_SIZE]={0};  /* Ftp reply (input) buffer */
char szDataFile[MAXFILENAME]={0};/* Filename */
char szFtpCmd  [CMD_SIZE]={0};   /* Ftp command buffer */
char achRplyBuf[BUF_SIZE];       /* Reply display buffer */

/* first one (index=0) is awaiting a reply
 * second (index=1) is next to be sent, etcetera */ 
FTPCMD astFtpCmd[MAX_CMDS];      /* Ftp command queue */
int nQLen;                       /* Number of entries in Ftp cmd queue */
 
int nFtpRplyCode;                /* Ftp reply code from server */
int iNextRply;                   /* Index to next reply string */
int iLastRply;

HFILE hDataFile=HFILE_ERROR;     /* File handle for open data file */
LONG lStartTime;                 /* Start time for data transfer */
LONG lByteCount;

char szLogFile[] = "ac_ftp.log"; /* Ftp command and reply log file */
HFILE hLogFile=HFILE_ERROR;

/*--------------------------------------------------------------------
 *  Function: WinMain()
 *
 *  Description: 
 *     initialize WinSock and open main dialog box
 *
 */
int WINAPI WinMain
  (HINSTANCE hInstance,
   HINSTANCE hPrevInstance,
   LPSTR  lpszCmdLine,
   int    nCmdShow)
{
    MSG msg;
    int nRet;

    lpszCmdLine   = lpszCmdLine;   /* avoid warning */
    hPrevInstance = hPrevInstance;
    nCmdShow      = nCmdShow;
                      
    hInst = hInstance;	/* save instance handle */
                                                                
    /*-------------initialize WinSock DLL------------*/
    nRet = WSAStartup(WSA_VERSION, &stWSAData);
    /* WSAStartup() returns error value if failed (0 on success) */
    if (nRet != 0) {    
      WSAperror(nRet, "WSAStartup()", hInst);
      /* No sense continuing if we can't use WinSock */
    } else {
          
      DialogBox (hInst, MAKEINTRESOURCE(AC_FTP), NULL, Dlg_Main);
    
      /*---------------release WinSock DLL--------------*/
      nRet = WSACleanup();
      if (nRet == SOCKET_ERROR)
        WSAperror(WSAGetLastError(), "WSACleanup()", hInst);
    }    
        
    return msg.wParam;
} /* end WinMain() */

/*--------------------------------------------------------------------
 * Function: Dlg_Main()
 *
 * Description: Do all the message processing for the main dialog box
 */
BOOL CALLBACK Dlg_Main 
  (HWND hDlg,
   UINT msg,
   UINT wParam,
   LPARAM lParam)
{                      
    int nAddrSize = sizeof(SOCKADDR);
    WORD WSAEvent, WSAErr;
    SOCKET hSock;
    BOOL bOk, bRet = FALSE;
    int  nRet;
    LONG lRet;
   
    switch (msg) {
      case WSA_ASYNC+1:
        /*------------------------------------------------- 
         * Data socket async notification message handlers 
         *-------------------------------------------------*/ 
        hSock = (SOCKET)wParam;                 /* socket */
        WSAEvent = WSAGETSELECTEVENT (lParam);  /* extract event */
        WSAErr   = WSAGETSELECTERROR (lParam);  /* extract error */
        /* if error, display to user (listen socket should not have
         *  an error, but some WinSocks incorrectly post it) */
        if ((WSAErr) && (hSock == hDataSock))  {
          int i,j;
          for (i=0, j=WSAEvent; j; i++, j>>=1); /* convert bit to index */
            WSAperror(WSAErr,aszWSAEvent[i], hInst);
          /* fall-through to call reenabling function for this event */
        }
        switch (WSAEvent) {
          case FD_READ:
            if (bIOBeep)
              MessageBeep(0xFFFF);
            if (hDataSock != INVALID_SOCKET) {
              /* Receive file data or directory list */
              RecvData(hDataSock, hDataFile, achInBuf, INPUT_SIZE);
            }
            break;
          case FD_ACCEPT:
            if (hLstnSock != INVALID_SOCKET) {
              /* Accept the incoming data connection request */
              hDataSock = 
                AcceptDataConn(hLstnSock, &stDRmtName);
              nAppState |= DATACONNECTED;
              /* Close the Listening Socket */
              closesocket(hLstnSock);
              hLstnSock = INVALID_SOCKET;
              lStartTime = GetTickCount();
              /* Data transfer should begin with FD_WRITE or FD_READ.  We 
               * fall through to jumpstart sends since FD_WRITE is not 
               * always implemented correctly */
            }
          case FD_WRITE:
             /* Send file data */
            if (astFtpCmd[0].nFtpCmd == STOR) {
              lRet = SendData(&hDataSock, hDataFile, MTU_SIZE);
            }
            break;
          case FD_CLOSE:                    /* Data connection closed */
            if (hSock == hDataSock) {
              /* Read any remaining data into buffer and close connection */  
              CloseFtpConn(&hDataSock, 
                (astFtpCmd[0].nFtpCmd != STOR) ? achInBuf : (LPSTR)0, 
                INPUT_SIZE, hDlg);
              EndData ();
            }
            break;
          default:
             break;
        } /* end switch(WSAEvent) */
        break;
        
      case WSA_ASYNC: 
        /*----------------------------------------------------- 
         * Control socket async notification message handlers 
         *-----------------------------------------------------*/ 
        WSAEvent = WSAGETSELECTEVENT (lParam);  /* extract event */
        WSAErr   = WSAGETSELECTERROR (lParam);  /* extract error */
        if (WSAErr) { /* if error, display to user */
          int i,j;
          for (i=0, j=WSAEvent; j; i++, j>>=1); /* convert bit to index */
          WSAperror(WSAErr,aszWSAEvent[i], hInst);
          /* fall-through to call reenabling function for this event */
        }
        hSock = (SOCKET)wParam;                  
        switch (WSAEvent) {
          case FD_READ:
             if (!iNextRply) {
                /* Receive reply from server */
                iLastRply = RecvFtpRply(hCtrlSock, szFtpRply, RPLY_SIZE);
             }
            if (iLastRply && (iLastRply != SOCKET_ERROR)) {
                /* Display the reply Message */
                GetDlgItemText (hWinMain, IDC_REPLY, achRplyBuf, 
                  RPLY_SIZE-strlen(szFtpRply));
                wsprintf (achTempBuf, "%s%s", szFtpRply, achRplyBuf);
                SetDlgItemText (hWinMain, IDC_REPLY, achTempBuf);

		        /* Save index to next reply (if there is one) */
                nRet = strlen(szFtpRply);
                if (iLastRply > nRet+2) {
                  iNextRply = nRet+3;
                  /* Adjust if reply only had LF (no CR) */
                  if (szFtpRply[nRet+2])
                    iNextRply = nRet+2;
                }
             }

            /* Figure out what to do with reply based on last command */
            ProcessFtpRply (szFtpRply, RPLY_SIZE);
            break;
          case FD_WRITE:
            /* Send command to server */
            if (astFtpCmd[1].nFtpCmd)
              SendFtpCmd();
            break;
          case FD_CONNECT:
            /* Control connected at TCP level */
            nAppState = CTRLCONNECTED;
            wsprintf(achTempBuf, "Server: %s", szHost);
            SetDlgItemText (hDlg, IDC_SERVER, achTempBuf);
            SetDlgItemText (hDlg, IDC_STATUS, "Status: connected");
            break;
          case FD_CLOSE:
            if (nAppState & CTRLCONNECTED) {
              nAppState = NOT_CONNECTED;        /* Reset app state */
              AbortFtpCmd();
              if (hCtrlSock != INVALID_SOCKET)  /* Close control socket */
                CloseFtpConn(&hCtrlSock, (PSTR)0, 0, hDlg);
              SetDlgItemText (hDlg, IDC_SERVER, "Server: none");
              SetDlgItemText (hDlg, IDC_STATUS, "Status: not connected");
            }
            break;
          default:
             break;
        } /* end switch(WSAEvent) */
        break;

      case WM_COMMAND:
        switch (wParam) {
          case IDC_CONNECT:
             /* If we already have a socket open, tell user to close it */
             if (nAppState & CTRLCONNECTED) {
                 MessageBox (hDlg,"Close the active connection first",
                    "Can't Connect", MB_OK | MB_ICONASTERISK);
             } else {
             
               /* Prompt user for server and login user information */
               bOk = DialogBox (hInst, MAKEINTRESOURCE(IDD_SERVER),
                      hDlg, Dlg_Login);
    
               if (bOk) {
                 /* Check the destination address and resolve if necessary */
                 stCRmtName.sin_addr.s_addr = GetAddr(szHost);
                 if (stCRmtName.sin_addr.s_addr == INADDR_ANY) {
               
                   /* Tell user to enter a host */
                   wsprintf(achTempBuf, 
                     "Sorry, server %s is invalid.  Try again", szHost);
                   MessageBox (hDlg, achTempBuf,
                     "Can't connect!", MB_OK | MB_ICONASTERISK);
                 } else {
                 
                   /* Initiate connect attempt to server */
                   hCtrlSock = 
                     InitCtrlConn(&stCRmtName, hDlg, WSA_ASYNC);
                 }
               }
             }
             break;
                     
           case IDC_CLOSE:
             if (nAppState & CTRLCONNECTED) {
               /* Set application state so nothing else is processed */
               nAppState = NOT_CONNECTED;
               
               /* If we're listening, stop now */
               if (hLstnSock != INVALID_SOCKET) {
                 closesocket(hLstnSock);
                 hLstnSock = INVALID_SOCKET;
               }
               /* If there is a data connection, then abort it */
               if (hDataSock != INVALID_SOCKET)
                 QueueFtpCmd (ABOR, 0);
               /* Quit the control connection */
               if (hCtrlSock != INVALID_SOCKET)
                 QueueFtpCmd (QUIT, 0);
               SetDlgItemText (hDlg, IDC_SERVER, "Server: none");
               SetDlgItemText (hDlg, IDC_STATUS, "Status: not connected");
             }
             break;
                     
           case IDC_RETR:
             /* Prompt for name of remote file to get */
             if (nAppState & CTRLCONNECTED) {
               bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME),
                 hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);
                 
               if (bOk && szDataFile[0]) {
                 if (!bToNul) {
                   /* If user provided a filename open same name here for write.
                    *  Truncate the filename to 8 chars plus 3, if necessary */
                   hDataFile = CreateLclFile (szDataFile);
                 }
                 if (hDataFile != HFILE_ERROR || bToNul) {
                   /* Tell the Server where to connect back to us */
                   hLstnSock =
                     InitDataConn(&stDLclName, hDlg, WSA_ASYNC+1);
                   if (hLstnSock != INVALID_SOCKET) {
                      /* Queue PORT, TYPE and RETR cmds */
                     if (QueueFtpCmd(PORT, 0)) {
                       if (QueueFtpCmd (TYPE, "I"))  
                         QueueFtpCmd(RETR, szDataFile);
                     }
                   }
                 }
               }
             } else
               not_connected();
             break;
            
           case IDC_STOR:
             /* Prompt for name of local file to send */
             if (nAppState & CTRLCONNECTED) {
               bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME),
                 hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);
                                                                   
               if (bOk && szDataFile[0]) {
                 if (!bFromNul) {
                   /* If user provided filename, try to open same name locally */
                   hDataFile = _lopen(szDataFile, 0);
                   if (hDataFile == HFILE_ERROR) {
                     wsprintf(achTempBuf, "Unable to open file: %s", szDataFile);
                     MessageBox (hWinMain, (LPSTR)achTempBuf,"File Error",  
                       MB_OK | MB_ICONASTERISK);
                   }
                 }
                 if (hDataFile != HFILE_ERROR || bFromNul) {
                   /* Tell the Server where to connect back to us */
                   hLstnSock =
                     InitDataConn(&stDLclName, hDlg, WSA_ASYNC+1);
                   if (hLstnSock != INVALID_SOCKET) {
                     /* Queue PORT, TYPE and STOR cmds */
                     if (QueueFtpCmd (PORT, 0)) {
                       if (QueueFtpCmd (TYPE, "I"))
                         QueueFtpCmd (STOR, szDataFile);
                     }                     
                   }
                 }
               }
             } else
               not_connected();
             break;
             
           case IDC_ABOR:
             if (hCtrlSock != INVALID_SOCKET)
               /* Abort the pending ftp command */
               QueueFtpCmd (ABOR, 0);
             break;
             
           case IDC_LCWD:
             /* Prompt for directory name, and move to it on local system. */
             bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME), 
                     hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);
             
             if (bOk && szDataFile[0]) {
               if (!(_chdir (szDataFile))) {
                 getcwd (szDataFile, MAXFILENAME-1);
                 SetDlgItemText (hDlg, IDC_LPWD, szDataFile);
               }
             }
             break;
             
           case IDC_LDEL:
             /* Prompt for filename, and delete it from local system */
             bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME), 
                     hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);

             if (bOk && szDataFile[0]) {
               /* If user provided filename, then delete it */
               remove (szDataFile);
             }
             break;
             
           case IDC_LDIR:
             /* Get local file directory, and display in notepad */
             if (GetLclDir(szTempFile)) {
               wsprintf (achTempBuf, "notepad %s", szTempFile);
               WinExec (achTempBuf, SW_SHOW);
             }             
             break;
        
           case IDC_RCWD:
             /* Prompt for directory name, and move to it on remote system. */
             if (nAppState & CTRLCONNECTED) {
               szDataFile[0] = 0;
               bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME), 
                 hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);
             
               if (bOk && szDataFile[0]) {
                 QueueFtpCmd (CWD, szDataFile);
               }
             } else
               not_connected();
             break;
             
           case IDC_RDEL:
             /* Prompt for filename, and delete it from remote system */
             if (nAppState & CTRLCONNECTED) {
               szDataFile[0] = 0;
               bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME), 
                 hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);
             
               if (bOk && szDataFile[0]) {
                 /* If user provided a filename, send command to delete it */
                 QueueFtpCmd (DELE, szDataFile);
               }
             } else
               not_connected();
             break;
                                                   
           case IDC_RDIR:
             /* Get remote file directory, and display in notepad file */
             if (nAppState & CTRLCONNECTED) {
               hDataFile = CreateLclFile (szTempFile);
               if (hDataFile != HFILE_ERROR) {
                 /* Prepare to receive the incoming connection from server */
                 hLstnSock =
                   InitDataConn(&stDLclName, hDlg, WSA_ASYNC+1);
                 if (hLstnSock != INVALID_SOCKET) {
                   if (QueueFtpCmd (PORT, 0))          /* Queue PORT, TYPE and LIST cmds */
                     if (QueueFtpCmd (TYPE, "A"))
                       QueueFtpCmd (LIST, 0);
                 }
               }
             } else 
               not_connected();
             break;
             
           case IDC_OPTIONS:
             DialogBox (hInst, MAKEINTRESOURCE(IDD_OPTIONS), 
                 hDlg, Dlg_Options);
             break;
             
           case IDABOUT:
             DialogBox (hInst, MAKEINTRESOURCE(IDD_ABOUT), hDlg, Dlg_About);
             break;

	       case WM_DESTROY:
           case IDC_EXIT:
             SendMessage (hDlg, WM_COMMAND, IDC_CLOSE, 0L);
             if (hLogFile != HFILE_ERROR) 
               _lclose(hLogFile);
             EndDialog(hDlg, msg);
             bRet = TRUE;
             break;
                       
           default:
              break;
         } /* end case WM_COMMAND: */
         break;
           
      case WM_INITDIALOG:
        hWinMain = hDlg;	/* save our main window handle */
        
        /* Assign an icon to dialog box */
#ifdef WIN32
        SetClassLong (hDlg, GCL_HICON, (LONG)        
          LoadIcon((HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE),
          __TEXT("AC_FTP")));
#else
        SetClassWord(hDlg,GCW_HICON,(WORD)LoadIcon(hInst,MAKEINTRESOURCE(AC_FTP)));
#endif

        /* Initialize FTP command structure array */
        memset (astFtpCmd, 0, (sizeof(struct stFtpCmd))*MAX_CMDS);
        
        /* Display current working directory */
        getcwd (szDataFile, MAXFILENAME-1);
        SetDlgItemText (hDlg, IDC_LPWD, szDataFile);
        
        /* Open logfile, if logging enabled */
        if (bLogFile) {
          hLogFile = _lcreat (szLogFile, 0);
          if (hLogFile == HFILE_ERROR) { 
            MessageBox (hWinMain, "Unable to open logfile",
              "File Error", MB_OK | MB_ICONASTERISK);
            bLogFile = FALSE;
          }
        }
        /* Center dialog box */
        CenterWnd (hDlg, NULL, TRUE);
        break;
             
      default:
        break;
  } /* end switch (msg) */

  return (bRet);
} /* end Dlg_Main() */

/*---------------------------------------------------------------------
 * Function: Dlg_Login
 *
 * Description: Prompt for destination host, user name, and password
 */                                        
BOOL CALLBACK Dlg_Login (
  HWND hDlg,
  UINT msg,
  UINT wParam,
  LPARAM lParam)
{
   BOOL bRet = FALSE;
   lParam = lParam;  /* avoid warning */

   switch (msg) {
     case WM_INITDIALOG:
       SetDlgItemText (hDlg, IDC_SERVER, szHost);
       SetDlgItemText (hDlg, IDC_USERNAME, szUser);
       SetDlgItemText (hDlg, IDC_PASSWORD, szPWrd);
       CenterWnd (hDlg, hWinMain, TRUE);
       break;
     case WM_COMMAND:
       switch (wParam) {
         case IDOK:
           GetDlgItemText (hDlg, IDC_SERVER, szHost, MAXHOSTNAME);
           GetDlgItemText (hDlg, IDC_USERNAME, szUser, MAXUSERNAME);
           GetDlgItemText (hDlg, IDC_PASSWORD, szPWrd, MAXPASSWORD);
           EndDialog (hDlg, TRUE);
           bRet = TRUE;
           break;
         case IDCANCEL:
           EndDialog (hDlg, FALSE);
           bRet = FALSE;
           break;
         default:
           break;
       }
   }        
   return(bRet);
} /* end Dlg_Login */

/*---------------------------------------------------------------------
 * Function: Dlg_Options()
 *
 * Description: Allow user to change a number of run-time parameters
 *  that affect the operation, to allow experimentation and debugging.
 */                                        
BOOL CALLBACK Dlg_Options (
  HWND hDlg,
  UINT msg,
  UINT wParam,
  LPARAM lParam)
{
   BOOL bRet = FALSE;
   lParam = lParam;  /* avoid warning */

   switch (msg) {
     case WM_INITDIALOG:
       CheckDlgButton (hDlg, IDC_TO_NUL,  bToNul);
       CheckDlgButton (hDlg, IDC_FROM_NUL,bFromNul);
       CheckDlgButton (hDlg, IDC_LOGFILE, bLogFile);
       CheckDlgButton (hDlg, IDC_IOBEEP,  bIOBeep);
       CheckDlgButton (hDlg, IDC_REASYNC, bReAsync);
       CenterWnd (hDlg, hWinMain, TRUE);
       break;
       
     case WM_COMMAND:
       switch (wParam) {
         case IDC_TO_NUL:
           bToNul = !bToNul;
           break;
         case IDC_FROM_NUL:
           bFromNul = !bFromNul;
           break;
         case IDC_LOGFILE:
           bLogFile = !bLogFile;
           break;
         case IDC_IOBEEP:
           bIOBeep = !bIOBeep;
           break;
         case IDC_REASYNC:
           bReAsync = !bReAsync;
           break;
         case IDOK:
           if (bLogFile && hLogFile == HFILE_ERROR) {
             bLogFile = FALSE;
           } else if (!bLogFile && hLogFile != HFILE_ERROR) {
             _lclose(hLogFile);
             hLogFile = HFILE_ERROR;
           } else if (bLogFile && hLogFile == HFILE_ERROR) {
             hLogFile = _lcreat (szLogFile, 0);
             if (hLogFile == HFILE_ERROR) { 
               MessageBox (hWinMain, "Unable to open logfile",
                 "File Error", MB_OK | MB_ICONASTERISK);
               bLogFile = FALSE;
             }
           }
           EndDialog (hDlg, TRUE);
           bRet = TRUE;
           break;
       }
   }        
   return(bRet);
} /* end Dlg_Options() */

/*---------------------------------------------------------------
 * Function: not_connected()
 *
 * Description: tell the user that the client needs a connection.
 */
void not_connected(void) {
  MessageBox (hWinMain, "You need to connect to an FTP Server",
   "Not Connected",  MB_OK | MB_ICONASTERISK);
}

/*--------------------------------------------------------------
 * Function: CloseFtpConn()
 *
 * Description: Close the connection gracefully and robustly,
 *  reading all remaining data into buffer before closure.
 */
int CloseFtpConn(SOCKET *hSock, LPSTR achInBuf, int len, HWND hWnd)
{
  char achDiscard[BUF_SIZE];
  int nRet=0;
  
  if (*hSock != INVALID_SOCKET) {
    /* disable asynchronous notification if window handle provided */
    if (hWnd) {
      nRet = WSAAsyncSelect(*hSock, hWnd, 0, 0);
      if (nRet == SOCKET_ERROR)
        WSAperror (WSAGetLastError(), "CloseFtpConn() WSAAsyncSelect()", hInst);
    }
  
    /* Half-close the connection to close neatly */
    nRet = shutdown (*hSock, 1);
    if (nRet == SOCKET_ERROR) {
      WSAperror (WSAGetLastError(), "shutdown()", hInst);
    } else {
      /* Read and discard remaining data (until EOF or error) */
      nRet = 1;
      while (nRet && (nRet != SOCKET_ERROR)) {
        if (achInBuf) 
          nRet = RecvData(*hSock, hDataFile, achInBuf, len);
        else
          nRet = recv (*hSock, (LPSTR)achDiscard, BUF_SIZE, 0);
      }
      /* close the socket, and ignore any error 
       *  (since we can't do much about them anyway */
      closesocket (*hSock);
    }
    *hSock = INVALID_SOCKET;  /* we always invalidate socket */
  }
  return (nRet);
} /* end CloseFtpConn() */

