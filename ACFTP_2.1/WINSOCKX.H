/*---------------------------------------------------------------------
 *
 * filename: WINSOCKX.H  WinSock subroutine library (static)
 *
 * copyright by Bob Quinn, 1995
 *   
 *  Description:
 *    Specifics to our winsockx.lib library (macros, externs, prototypes)
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
#include <windows.h>

#define MTU_SIZE    1460
#define INPUT_SIZE  8192
#define BUF_SIZE    1024
#define ERR_SIZE    512

#define MAXUSERNAME  64
#define MAXPASSWORD  32
#define MAXFILENAME  64

/* our asynch notification message */                             
#define WSA_ASYNC WM_USER+1
                                                      
#define IDC_COMPILEDATE                 1050
#define IDS_DLLVER                      1055
#define IDS_DLLHIVER                    1056
#define IDS_MAXSOCKS                    1059
#define IDS_MAXUDP                      1060
#define IDS_DESCRIP                     1065
#define IDS_STATUS                      1066
extern WSADATA stWSAData;    /* WinSock DLL Info */

#define IDD_FILENAME                    102
#define IDC_FILE                        1029

extern char *aszWSAEvent[7];     /* for error messages */
extern char achTempBuf[BUF_SIZE];/* Screen I/O data buffer and such */
extern char szTempFile[10];      /* Temporary work filename */

extern HWND hWinMain;        /* Main window (or dialog) handle */
extern HINSTANCE hInst;      /* Instance handle */

/*---- Library Function Prototypes ----*/
void    CenterWnd(HWND, HWND, BOOL);
int     CloseConn(SOCKET, LPSTR, int, HWND);
HFILE   CreateLclFile (LPSTR);
BOOL CALLBACK Dlg_About (HWND, UINT, UINT, LPARAM);
BOOL CALLBACK Dlg_File  (HWND, UINT, UINT, LPARAM);
u_long  GetAddr(LPSTR);
int     GetBuf(SOCKET, int, int);
LONG    GetHostID(void);
BOOL    GetLclDir(LPSTR szTempFile);
u_short GetPort (LPSTR);
void    WSAperror (int, LPSTR, HANDLE);
int     WSAErrStr (int, LPSTR, HANDLE);

