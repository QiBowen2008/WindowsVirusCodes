/*---------------------------------------------------------------------
 *
 * Program: AC_FTP.EXE Asynch Ftp Client (TCP)
 *
 * filename: ac_ftp.h
 *
 * copyright by Bob Quinn, 1995
 *   
 *  Description:
 *    Common declarations.
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
#include <winsock.h>
#include "..\winsockx.h"

#ifndef IDC_STATIC
#define IDC_STATIC -1
#endif

#define CMD_SIZE    128
#define RPLY_SIZE   MTU_SIZE
#define MAXNULPUT   1048576

/* Ftp Commands that take arguments (subset) */
#define CWD  1
#define DELE 2
#define PASS 3
#define PORT 4
#define RETR 5
#define STOR 6
#define TYPE 7 
#define USER 8 

/* Ftp commands without arguments (subset) */
#define ABOR 9
#define LIST 10     
#define PWD  11
#define QUIT 12

/* Ftp commmand strings */
extern LPSTR aszFtpCmd[13];

/*----------- Application states -----------*/
#define NOT_CONNECTED    0
#define CTRLCONNECTED    2
#define DATACONNECTED    4

/*------------ Global variables ------------*/

extern char szAppName[];

extern BOOL nAppState;             /* Application State */
                                   
extern BOOL bToNul;                /* Get to NUL device file */
extern BOOL bFromNul;              /* Put from NUL device file */
extern BOOL bIOBeep;               /* Beep on FD_READ, FD_WRITE */
extern BOOL bDebug;                /* Debug output to WinDebug */
extern BOOL bReAsync;              /* Call WSAAsyncSelect after accept() */
extern BOOL bLogFile;              /* Write Cmds and Replies to logfile */

extern SOCKET hCtrlSock;           /* Ftp control socket */
extern SOCKET hLstnSock;           /* Listening data socket */
extern SOCKET hDataSock;           /* Connected data socket */

extern char szHost[MAXHOSTNAME];   /* Remote host name or address */
extern char szUser[MAXUSERNAME];   /* User ID */
extern char szPWrd[MAXPASSWORD];   /* User password */

extern SOCKADDR_IN stCLclName;     /* Control socket name (local client) */
extern SOCKADDR_IN stCRmtName;     /*                     (remote server) */
extern SOCKADDR_IN stDLclName;     /* Data socket name (local client)*/          
extern SOCKADDR_IN stDRmtName;     /*                  (remote server) */
                              
extern char achInBuf  [INPUT_SIZE];/* Network input data buffer */
extern char achOutBuf [INPUT_SIZE];/* Network output buffer */
extern char szFtpRply [RPLY_SIZE]; /* Ftp reply (input) buffer */
extern char szDataFile[MAXFILENAME];/* Filename */
extern char szFtpCmd  [CMD_SIZE];  /* Ftp command buffer */
extern char achRplyBuf[BUF_SIZE];  /* Reply display buffer */

typedef struct stFtpCmd {
  int   nFtpCmd;                   /* Ftp command value */
  char  szFtpParm[CMD_SIZE];       /* Ftp parameter string */
} FTPCMD;

#define MAX_CMDS 4
/* first one (index=0) is awaiting a reply
 * second (index=1) is next to be sent, etcetera */ 
extern FTPCMD astFtpCmd[MAX_CMDS]; /* Ftp command queue */
extern int nQLen;                  /* Number of entries in Ftp cmd queue */
 
extern int nFtpRplyCode;           /* Ftp reply code from server */
extern int iNextRply;              /* Index to next reply string */
extern int iLastRply;

extern HFILE hDataFile;            /* File handle for open data file */
extern LONG lStartTime;            /* Start time for data transfer */
extern LONG lByteCount;

extern char szLogFile[];           /* Ftp command and reply log file */
extern HFILE hLogFile;

/*------------- Function prototypes --------------*/
BOOL CALLBACK Dlg_Main   (HWND,UINT,UINT,LPARAM); /* Dialog procedures */
BOOL CALLBACK Dlg_Login  (HWND,UINT,UINT,LPARAM);
BOOL CALLBACK Dlg_File   (HWND,UINT,UINT,LPARAM);
BOOL CALLBACK Dlg_Options(HWND,UINT,UINT,LPARAM);

SOCKET InitCtrlConn(PSOCKADDR_IN, HWND, u_int);  /* Control connection */
BOOL QueueFtpCmd(int, LPSTR);
int  SendFtpCmd(void);
void AbortFtpCmd(void);
int  RecvFtpRply(SOCKET, LPSTR, int);
void ProcessFtpRply(LPSTR, int);

SOCKET InitDataConn(PSOCKADDR_IN, HWND, u_int);  /* Data connection */
SOCKET AcceptDataConn(SOCKET, PSOCKADDR_IN);
long SendData(SOCKET*, HFILE, int);
int  RecvData(SOCKET, HFILE, LPSTR, int);
void EndData(void);
void not_connected(void);                        /* Utility functions */
int  CloseFtpConn(SOCKET*, LPSTR, int, HWND); 

