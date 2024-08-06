// FilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Filter.h"
#include "FilterDlg.h"
#include "calculate.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg dialog

CFilterDlg::CFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilterDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterDlg)
	DDX_Control(pDX, IDC_EDIT3, m_Edit3);
	DDX_Control(pDX, IDC_EDIT2, m_Edit2);
	DDX_Control(pDX, IDC_SPIN3, m_Spin3);
	DDX_Control(pDX, IDC_SPIN2, m_Spin2);
	DDX_Control(pDX, IDC_EDIT1, m_ZoominEdit);
	DDX_Control(pDX, IDC_SPIN1, m_ZoominSpin);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CFilterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_WAV1, OnWav1)
	ON_EN_UPDATE(IDC_EDIT1, OnUpdateEdit1)
	ON_BN_CLICKED(IDC_WAV2, OnWav2)
	ON_EN_UPDATE(IDC_EDIT2, OnUpdateEdit2)
	ON_EN_UPDATE(IDC_EDIT3, OnUpdateEdit3)
	ON_BN_CLICKED(IDC_WAV4, OnWav4)
	ON_BN_CLICKED(IDC_WAV3, OnWav3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg message handlers

BOOL CFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	/////////////////////////初始化/////////////////////
	CEdit* zoomin_edit=(CEdit*)GetDlgItem(IDC_EDIT1);
	m_ZoominSpin.SetBuddy(zoomin_edit);
	m_ZoominSpin.SetRange(1,200);
	m_ZoominSpin.SetPos(10);
	m_Zoomin=10;
	m_Spin2.SetBuddy(&m_Edit2);
	m_Spin3.SetBuddy(&m_Edit3);
	m_Spin2.SetRange(0,100);
	m_Spin3.SetRange(0,100);
	m_Spin2.SetPos(30);
	m_Spin3.SetPos(50);
	EnableRegulate(false);
	DataSuccess=false;
	// TODO: Add extra initialization here
	ShowWindow(SW_MAXIMIZE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFilterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFilterDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (IsIconic())
	{
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if(DataSuccess)
		   DrawOldWave(dc);
		CDialog::OnPaint();

	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFilterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFilterDlg::OnWav1() 
{
CalWav1();
UpdateStaticBox(1);
CRect rect(20,50,520,500);
InvalidateRect(rect);
EnableRegulate(false);
CurrentWav=1;
}

void CFilterDlg::OnWav2() 
{
	// TODO: Add your control notification handler code here
EnableRegulate(true);	
CalWav2();
UpdateStaticBox(1);
CRect rect(20,50,520,500);
InvalidateRect(rect);
CurrentWav=2;
}



void CFilterDlg::OnWav3() 
{
	// TODO: Add your control notification handler code here
EnableRegulate(false);	
CalWav3();
UpdateStaticBox(1);
CRect rect(20,50,520,500);
InvalidateRect(rect);	
CurrentWav=3;
}



void CFilterDlg::OnWav4() 
{
	// TODO: Add your control notification handler code here
EnableRegulate(false);	
CalWav4();
UpdateStaticBox(0);
CRect rect(20,50,520,500);
InvalidateRect(rect);
CurrentWav=4;
}


void CFilterDlg::CalWav1()
{
	int m,n;
	long seed;
	double fs,snr;
	double a[3]={1,1,1};
	double f[3]={10,20,50};
	double ph[3]={0,45,90};
	m=3;
	n=500;
	seed=13579l;
	fs=1000;
	snr=1000.0;
	sinwn(a,f,ph,m,fs,snr,seed,y,n);
    DataSuccess=true;
	/////////////////////滤波后的波形/////////////////
	double mu,pi,mean,sigma;
	double d[501],w[501];
	pi=4.0*atan(1.0);
	mean=0.0;
	sigma=1.0;
	seed=13579l;
	n=500;
	///////////////////////////////////////////////

	double a2[1]={1};
	double f2[1]={20};
	double ph2[1]={45};
	m=1;
	n=500;
	seed=13579l;
	fs=1000;
	snr=1000.0;
	sinwn(a2,f2,ph2,m,fs,snr,seed,d,n);

	///////////////////////////////////////////////
	m=50;
	mu=0.002;
	lms(y,d,y2,n,w,m,mu);
	///lms(y,d,w,n,y2,m,mu);

}

void CFilterDlg::CalWav2()
{
	int m,n,i;
	long seed;
	double fs,snr;
	double a[11];
	double f[11];
	double ph[11];
	
	m=10;
	n=500;
	seed=13579l;
	fs=800;
	snr=1000;
	for(i=0;i<10;i++)
	{
		a[i]=1;
		ph[i]=rand()*90;
		f[i]=10*i;
	}
	
	sinwn(a,f,ph,m,fs,snr,seed,y,n);
        /* for(k=0;k<n;k++)
	{
		y[k]=0.0;
		for(i=0;i<m;i++)
		{
			y[k]=y[k]+a[i]*sin(k*f[i]+ph[i]);
		}
		
	}

*/	/////////////////////滤波后的波形/////////////////
	double mu,pi,mean,sigma;
	double d[501],w[501];
	pi=4.0*atan(1.0);
	mean=0.0;
	sigma=1.0;
	seed=13579l;
	n=500;
	///////////////////////////////////////////////
    int fl,fh;
	CString text;
	m_Edit2.GetWindowText(text);
    fl=atoi(text);
	m_Edit3.GetWindowText(text);
	fh=atoi(text);
	m=(fh-fl)/10;
	for(i=0;i<=m;i++)
	{
		a[i]=1;
		ph[i]=0;
	    f[i]=i*10+fl;
	}

	//d[k]=d[k]+a[i]*sin(k*f[i]+ph[i]);
	
	//for(k=0;k<n;k++)
	//{
	//	d[k]=0;
    
	//}
//	a[0]=1;
//	f[0]=50;
//	ph[0]=90;
//	m=1;
	sinwn(a,f,ph,m,fs,snr,seed,d,n);
	///////////////////////////////////////////////
	m=500;
	mu=0.0002;
	lms(y,d,y2,n,w,m,mu);
    DataSuccess=true;
}


void CFilterDlg::CalWav3()
{
    int m,n,i;
	long seed;
	double fs,snr;
	n=500;
	seed=13579l;
	fs=1000;
	snr=1000.0;
	for(i=0;i<n;i++)
        y[i]=uniform(0,1,&seed);
	///sinwn(a,f,ph,m,fs,snr,seed,y,n);

	double mu,pi,mean,sigma;
	double x[501],w[501];
	pi=4.0*atan(1.0);
	mean=0.0;
	sigma=1.0;
	seed=13579l;
	n=500;
	///////////////////////////////////////////////

	double a2[1]={1};
	double f2[1]={20};
	double ph2[1]={45};
	m=1;
	n=500;
	seed=13579l;
	fs=1000;
	snr=1000.0;
	sinwn(a2,f2,ph2,m,fs,snr,seed,x,n);
    for(i=0;i<500;i++)
		y[i]=x[i]+y[i];
	//////////////////////////////滤波后的波形///////////////////////////////////////////////////////
	m=50;
	mu=0.002;
	double e[501];
	lms(y,y,y2,e,n,w,m,mu);
	DataSuccess=true;
}

/*
void CFilterDlg::CalWav4()
{
 double d[501],e[7]={37.0,23.8,13.6,6.3,1.9,0.78,0.49};
 int m=2,k=500,N=16,n=500;
 long seed=13579L;
 double mu=0.015;
 double pi=4.0*atan(1.0);
 /////////////////////////////////////// 
 int i,j;
 double Q=0.01;
 double w0=-2,a=cos(1/8*pi),b=sin(1/8*pi);
 for(i=0;i<500;i++)
 {
	w0+=0.02;
    for(j=0;j<7;j++)
	{
		y4[j*2][i]=(0-(a*w0+2*b)+sqrt((a*w0+2*b)*(a*w0+2*b)-4*(0.5+Q)*(2+w0*w0*(0.5+Q)-e[j])))/(2*(0.5+Q));
		y4[j*2+1][i]=(0-(a*w0+2*b)-sqrt((a*w0+2*b)*(a*w0+2*b)-4*(0.5+Q)*(2+w0*w0*(0.5+Q)-e[j])))/(2*(0.5+Q));
		//y4[j*2][i]=0-(a*w0-2*b+(sqrt((a*w0+2*b)*(a*w0+2*b)-(1+2*Q)*(4-2*e[j]+(1+2*Q)*w0*w0)))/(1+2*Q));
		//y4[j*2+1][i]=0-(a*w0-2*b-(sqrt((a*w0+2*b)*(a*w0+2*b)-(1+2*Q)*(4-2*e[j]+(1+2*Q)*w0*w0)))/(1+2*Q));
    }
 }

 /////////////////////////////////////////////////
 for(i=0;i<k;i++)
 {
	 y[i]=sin(2*pi*i/N)+uniform(0,1,&seed);
     d[i]=2*cos(2*pi*i/N);
 }
 lms2(y,d,y2,n,v[0],m,mu);
//////////////////////////////////////////////////

 DataSuccess=true;
}

*/
void CFilterDlg::CalWav4()
{
	double a[]={1,1,1,1,1,1,1,1,1,1,1},b[]={1,0,0,0,0,0,0,0,0,0,0};
	double x[501];
	gain(b,a,10,10,y,x,500,2);
	gain(b,a,10,10,x,y2,500,2);
	DataSuccess=true;
}

void CFilterDlg::DrawOldWave(CPaintDC& dc)
{
CBitmap bitmap;
CBitmap* pOldBitmap;
CDC MemDC;
CRect rect(0,0,500,180);
CBrush brush(COLORREF(RGB(0,0,0)));
CBrush* oldbrush;
int ny,t,starty=100;////starty:原点的Y坐标
double maxt=12.5;
double space=0.025;
CPen pen;
CPen* oldpen;
MemDC.CreateCompatibleDC(&dc); ///建立与显示设备兼容的内存设备场境
bitmap.CreateCompatibleBitmap(&dc,500,180);///建立与显示设备兼容的位图
pOldBitmap=MemDC.SelectObject(&bitmap); ///将位图选入内存场境
pen.CreatePen(PS_SOLID,1, RGB(255,255,255));
oldpen=MemDC.SelectObject(&pen);
oldbrush=MemDC.SelectObject(&brush);
MemDC.FillRect(rect,&brush);

///////////////////////////////
ny=(int)(y[0]*m_Zoomin);
MemDC.MoveTo(0,starty+ny);
for(t=1;t<(maxt/space);t++)
{ ///画波形
    ny=(int)(y[t]*m_Zoomin);
    MemDC.LineTo(t,starty+ny);
}

///////////////////////////
dc.BitBlt(20,80,500,180,&MemDC,0,0,SRCCOPY); ///显示原图形
MemDC.FillRect(rect,&brush);/////清除

ny=(int)(y2[0]*m_Zoomin);
MemDC.MoveTo(0,starty+ny);
for(t=1;t<(maxt/space);t++)
{ ///画波形
    ny=(int)(y2[t]*m_Zoomin);
    MemDC.LineTo(t,starty+ny);
}

dc.BitBlt(20,310,500,180,&MemDC,0,0,SRCCOPY); 
MemDC.SelectObject(oldpen);
pen.DeleteObject();
MemDC.SelectObject(oldbrush);
brush.DeleteObject();
MemDC.SelectObject(pOldBitmap);
}

void CFilterDlg::OnUpdateEdit1() 
{
if(IsWindowVisible())
{
  CString text="10";
  m_ZoominEdit.GetWindowText(text);
  m_Zoomin=atoi(text);
  CRect rect(20,50,520,500);
  InvalidateRect(rect);	
}
}


void CFilterDlg::OnUpdateEdit2() 
{
if(IsWindowVisible())
 if(m_Edit3.IsWindowEnabled())
  OnWav2();	
}

void CFilterDlg::OnUpdateEdit3() 
{
if(IsWindowVisible())
 if(m_Edit3.IsWindowEnabled())
  OnWav2();
}

void CFilterDlg::EnableRegulate(bool enable)
{
CEdit *edit2,*edit3;
edit2=(CEdit*)GetDlgItem(IDC_EDIT2);
edit3=(CEdit*)GetDlgItem(IDC_EDIT3);
if(edit2)
   edit2->EnableWindow(enable);
if(edit3)
   edit3->EnableWindow(enable);
m_Spin2.EnableWindow(enable);
m_Spin3.EnableWindow(enable);
}


void CFilterDlg::UpdateStaticBox(bool status)
{
 CStatic *static1,*static2;
 static1=(CStatic*)GetDlgItem(IDC_STATIC1);
 static2=(CStatic*)GetDlgItem(IDC_STATIC2);
 if(status)
 {
  static1->SetWindowText("输入信号波形");
  static2->SetWindowText("输出信号波形");
 }
 else
 {
  static1->SetWindowText("其它");
  static2->SetWindowText("误差");
 }
}
