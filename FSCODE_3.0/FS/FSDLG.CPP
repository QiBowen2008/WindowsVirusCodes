// FSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FS.h"
#include "FSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFSDlg dialog

CFSDlg::CFSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFSDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFSDlg)
	m_Function = -1;
	m_FileName = _T("");
	m_BlockSize = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFSDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Radio(pDX, IDC_SPLIT_RADIO, m_Function);
	DDX_Text(pDX, IDC_FILESELECT_EDIT, m_FileName);
	DDX_Text(pDX, IDC_BLOCKSIZE_EDIT, m_BlockSize);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFSDlg, CDialog)
	//{{AFX_MSG_MAP(CFSDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SPLIT_RADIO, OnSplitRadio)
	ON_BN_CLICKED(IDC_MERGE_RADIO, OnMergeRadio)
	ON_BN_CLICKED(IDC_SELECTFILE_BUTTON, OnSelectfileButton)
	ON_BN_CLICKED(IDC_START_BUTTON, OnStartButton)
	ON_EN_CHANGE(IDC_BLOCKSIZE_EDIT, OnChangeBlocksizeEdit)
	ON_EN_CHANGE(IDC_FILESELECT_EDIT, OnChangeFileselectEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFSDlg message handlers

BOOL CFSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_Function=0;
	m_BlockSize='1';
	m_PreBlockSize='1';
	nBlockSize=1;
	UpdateData(FALSE);
	GotoDlgCtrl(GetDlgItem(IDC_FILESELECT_EDIT));
	return FALSE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFSDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFSDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFSDlg::OnSplitRadio() 
{
	// TODO: Add your control notification handler code here
	if (m_Function==0)
		return ;
	UpdateData(TRUE);
	GetDlgItem(IDC_BLOCKSIZE_EDIT)->EnableWindow(TRUE);
	m_MergeFileName=m_FileName;
	m_FileName=m_SplitFileName;
	UpdateData(FALSE);
}

void CFSDlg::OnMergeRadio() 
{
	// TODO: Add your control notification handler code here
	if (m_Function==1)
		return ;
	UpdateData(TRUE);
	GetDlgItem(IDC_BLOCKSIZE_EDIT)->EnableWindow(FALSE);
	m_SplitFileName=m_FileName;
	m_FileName=m_MergeFileName;
	UpdateData(FALSE);
}

void CFSDlg::OnSelectfileButton() 
{
	// TODO: Add your control notification handler code here
	CString strOpenFileType;
	if (m_Function==0)
		strOpenFileType="All files (*.*)|*.*||";
	else
		strOpenFileType="Information files (*.fsi)|*.fsi||";
	CFileDialog OpenFile(TRUE,NULL,NULL,
		OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
		strOpenFileType);
	if (OpenFile.DoModal()==IDOK)
	{
		m_FileName=OpenFile.GetPathName();
		UpdateData(FALSE);
	}
}

void CFSDlg::OnStartButton() 
{
	//TODO: Add your control notification handler code here
	//Get input information
	if (m_Function==0)//Split file
	{
		if (nBlockSize<=0)
		{
			MessageBox("Block size must be larger than 0.",
				"File Split",
				MB_ICONINFORMATION|MB_OK);
			return;
		}

		//Split option starts here
		GetDlgItem(IDC_START_BUTTON)->EnableWindow(FALSE);
		Split();
		
		//After return
		GetDlgItem(IDC_START_BUTTON)->EnableWindow(TRUE);
		m_Progress.SetPos(0);
	}
	else//Merge files
	{
		//Merge option starts here
		GetDlgItem(IDC_START_BUTTON)->EnableWindow(FALSE);
		Merge();

		//After return
		GetDlgItem(IDC_START_BUTTON)->EnableWindow(TRUE);
		m_Progress.SetPos(0);
	}
}

int CFSDlg::GetStringLength(char * InputString)
//This function get the length of the input string.
//'\0' is included when count the length.
{
	int length=1;
	while (*InputString!='\0')
	{
		length++;
		InputString++;
	}
	return length;
}

void CFSDlg::Split()
{
	CFile SourceFile,TargetFile;
	CString m_NoPathFileName,m_FileTitle,m_FilePath;
	CString TargetFileName,TargetFileExName;
	char Buff[1024],NumberString[25];
	int CurrentBlock_KBytes,nBlockCounter;
	int nFileTitleLength;
	long int FileLength,CopiedLength;
	UINT BuffLength,nReadLength;

	//Open source file
	if (SourceFile.Open(m_FileName,
		CFile::modeRead|CFile::typeBinary)
		==FALSE)
	{
		MessageBox("Can't open source file.",
			"File Split",
			MB_ICONINFORMATION|MB_OK);
		return;
	}

	//Get source file information
	FileLength=(long int)SourceFile.GetLength();
	if ((FileLength/1024)<nBlockSize)
	{
		MessageBox("You needn't split this file.",
			"File Split",
			MB_ICONINFORMATION|MB_OK);
		SourceFile.Close();
		return;
	}
	m_NoPathFileName=SourceFile.GetFileName();
	nFileTitleLength=m_NoPathFileName.ReverseFind('.');
	if (nFileTitleLength!=-1)
		m_FileTitle=m_NoPathFileName.Left(nFileTitleLength);
	else
		m_FileTitle=m_NoPathFileName;
	m_FilePath=m_FileName.Left
		(m_FileName.GetLength()-m_NoPathFileName.GetLength());

	//Create the first target file.It is also the first block.
	TargetFileName=m_FilePath+m_FileTitle+".FSI";
	if (TargetFile.Open(TargetFileName,
		CFile::modeCreate|CFile::modeReadWrite|CFile::typeBinary)
		==FALSE)
	{
		MessageBox("Can't create target file.",
			"File Split",
			MB_ICONINFORMATION|MB_OK);
		SourceFile.Close();
		return;
	}

	//Write FSI file header
	TargetFile.Write("FSI",3);
	TargetFile.Write(m_NoPathFileName+"\0",m_NoPathFileName.GetLength()+1);
	itoa(FileLength,NumberString,10);
	TargetFile.Write(NumberString,GetStringLength(NumberString));

	//Write blocks
	nBlockCounter=0;
	CurrentBlock_KBytes=0;
	CopiedLength=0;
	FileLength/=1024;
	BuffLength=1020-m_NoPathFileName.GetLength()
		-GetStringLength(NumberString);
	while ((nReadLength=SourceFile.Read(Buff,BuffLength))!=0)
	{
		if (CurrentBlock_KBytes==nBlockSize)
		{
			TargetFile.Close();

			//Generate next file name
			nBlockCounter++;
			if (nBlockCounter>999)
			{
				MessageBox("Too many file blocks.",
					"File Split",
					MB_ICONINFORMATION|MB_OK);
				return;
			}
			itoa(nBlockCounter,NumberString,10);
			TargetFileExName=(CString)"00"+NumberString;
			TargetFileExName=TargetFileExName.Right(3);
			TargetFileName=m_FilePath+m_FileTitle+'.'+TargetFileExName;

			//Create next block
			if (TargetFile.Open(TargetFileName,
				CFile::modeCreate|CFile::modeReadWrite|CFile::typeBinary)
				==FALSE)
			{
				MessageBox("Can't create block files.",
					"File Split",
					MB_ICONINFORMATION|MB_OK);
				SourceFile.Close();
				return;
			}
			CurrentBlock_KBytes=0;
		}
		TargetFile.Write(Buff,nReadLength);
		BuffLength=1024;
		CurrentBlock_KBytes++;
		CopiedLength++;
		m_Progress.SetPos((int)(100*CopiedLength/FileLength));
	}

	TargetFile.Close();
	return;
}

void CFSDlg::Merge()
{
	CFile SourceFile,TargetFile;
	CString m_NoPathFileName,m_FileTitle,m_FilePath;
	CString DriveLetter,SourceFileName,SourceFileExName;
	CString TargetFileName;
	CFileStatus status;
	char Buff[1024];
	char *BuffPointer;
	char NumberString[5];
	int nFileTitleLength;
	int nBlockCounter,StringLength;
	int FileKBytes,CopiedKBytes;
	int MessageBoxReturn;
	long int FileLength,CopiedLength;
	UINT BuffLength,nReadLength;

	//Open FSI file
	if (SourceFile.Open(m_FileName,
		CFile::modeRead|CFile::typeBinary)
		==FALSE)
	{
		MessageBox("Can't open source file.",
			"File Split",
			MB_ICONINFORMATION|MB_OK);
		return;
	}
	m_NoPathFileName=SourceFile.GetFileName();
	nFileTitleLength=m_NoPathFileName.ReverseFind('.');
	if (nFileTitleLength!=-1)
		m_FileTitle=m_NoPathFileName.Left(nFileTitleLength);
	else
		m_FileTitle=m_NoPathFileName;
	m_FilePath=m_FileName.Left
		(m_FileName.GetLength()-m_NoPathFileName.GetLength());

	//Verify FSI file
	SourceFile.Read(Buff,1024);
	if((Buff[0]!='F')||(Buff[1]!='S')||(Buff[2]!='I'))
	{
		MessageBox("Incorrect file format.",
			"File Split",
			MB_ICONINFORMATION|MB_OK);
		return;
	}

	//Get file head information
	BuffPointer=Buff;
	for(int i=1;i<=3;i++)
		BuffPointer++;
	TargetFileName=m_FilePath+BuffPointer;
	StringLength=GetStringLength(BuffPointer);
	for(i=1;i<=StringLength;i++)
		BuffPointer++;
	FileLength=atoi(BuffPointer);
	FileKBytes=FileLength/1024;
	BuffLength=1021-StringLength;
	StringLength=GetStringLength(BuffPointer);
	for(i=1;i<=StringLength;i++)
		BuffPointer++;
	BuffLength-=StringLength;

	//Create target file
	DriveLetter=m_FileName;
	DriveLetter=DriveLetter.Left(2);
	DriveLetter.MakeUpper();
	if(CFile::GetStatus(TargetFileName,status))
	{
		if (status.m_attribute&0x01)
		{
			MessageBoxReturn=MessageBox
				((CString)"Over write read only file "+TargetFileName+'?',
				"File Split",
				MB_ICONQUESTION|MB_YESNOCANCEL);
			if(MessageBoxReturn==IDYES)
			{
				status.m_attribute=0x00;
				CFile::SetStatus(TargetFileName,status);
			}
		}
		else
			MessageBoxReturn=MessageBox
				((CString)"Over write "+TargetFileName+'?',
				"File Split",
				MB_ICONQUESTION|MB_YESNOCANCEL);
		if(MessageBoxReturn==IDCANCEL)
		{
			SourceFile.Close();
			return;
		}
	}
	if((DriveLetter.Compare("A:")==0)||(MessageBoxReturn==IDNO))
	{
		CFileDialog SaveFile(FALSE,NULL,TargetFileName,
			OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,
			"All files (*.*)|*.*||");
		if (SaveFile.DoModal()==IDOK)
		{
			TargetFileName=SaveFile.GetPathName();
			if (CFile::GetStatus(TargetFileName,status))
			{
				status.m_attribute=0x00;
				CFile::SetStatus(TargetFileName,status);
			}
		}
		else
		{
			SourceFile.Close();
			return;
		}
	}
	if (TargetFile.Open(TargetFileName,
		CFile::modeCreate|CFile::modeReadWrite|CFile::typeBinary)
		==FALSE)
	{
		MessageBox("Can't create target file.",
			"File Split",
			MB_ICONINFORMATION|MB_OK);
		SourceFile.Close();
		return;
	}

	//Restore target file
	CopiedLength=0;
	nBlockCounter=0;
	CopiedKBytes=0;
	while(TRUE)
	{
		TargetFile.Write(BuffPointer,BuffLength);
		BuffPointer=Buff;
		CopiedLength+=BuffLength;
		CopiedKBytes++;
		m_Progress.SetPos((int)100*CopiedKBytes/FileKBytes);
		if (FileLength<=CopiedLength)
			break;

		//Read from block
		if ((nReadLength=SourceFile.Read(Buff,1024))==0)
		{
			SourceFile.Close();

			//Generate next file name
			nBlockCounter++;
			itoa(nBlockCounter,NumberString,10);
			SourceFileExName=(CString)"00"+NumberString;
			SourceFileExName=SourceFileExName.Right(3);
			SourceFileName=m_FilePath+m_FileTitle+'.'+SourceFileExName;

			//Open next block
			if (SourceFile.Open(SourceFileName,
				CFile::modeRead|CFile::typeBinary)
				==FALSE)
			{
				if (DriveLetter.Compare("A:")==0)
				{
					do
					{
						if(MessageBox
							("Insert next disk("+SourceFileName+").",
							"File Split",
							MB_ICONINFORMATION|MB_OKCANCEL)
							==IDCANCEL)
						{
							MessageBox("Can't open block files.",
								"File Split",
								MB_ICONINFORMATION|MB_OK);
							TargetFile.Close();		 
							return;
						}
					}
					while(SourceFile.Open(SourceFileName,
						CFile::modeRead|CFile::typeBinary)
						==FALSE);
				}
				else
				{
					MessageBox("Can't open block files.",
						"File Split",
						MB_ICONINFORMATION|MB_OK);
					TargetFile.Close();
					return;
				}
			}

			//Read from new block
			if ((nReadLength=SourceFile.Read(Buff,1024))==0)
			{
				MessageBox("Can't read from block files.",
					"File Split",
					MB_ICONINFORMATION|MB_OK);
				TargetFile.Close();
				return;
			}
		}
		BuffLength=nReadLength;
	}

	//Verify target file
	nReadLength=SourceFile.Read(Buff,1024);
	SourceFile.Close();
	if (((TargetFile.GetLength())!=((UINT)FileLength))
		||(nReadLength!=0))
	{
		MessageBox("Unexpected error!",
			"File Split",
			MB_ICONINFORMATION|MB_OK);
		TargetFile.Close();
		return;
	}

	TargetFile.Close();
	return;
}

void CFSDlg::OnChangeBlocksizeEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	int nTempBlockSize=0;
	UpdateData(TRUE);
	if (m_BlockSize.GetLength()>6)
	{
		m_BlockSize=m_PreBlockSize;
		MessageBox("Blocksize too larger.",
			"File Split",
			MB_ICONINFORMATION|MB_OK);
		UpdateData(FALSE);
		return;
	}
	for(int i=0;i<m_BlockSize.GetLength();i++)
	{
		if((m_BlockSize[i]<'0')||(m_BlockSize[i]>'9'))
		{
			m_BlockSize=m_PreBlockSize;
			MessageBox("You must input an integer as blocksize.",
				"File Split",
				MB_ICONINFORMATION|MB_OK);
			UpdateData(FALSE);
			return;
		}
		nTempBlockSize=nTempBlockSize*10+((int)m_BlockSize[i]-0x30);
	}
	m_PreBlockSize=m_BlockSize;
	nBlockSize=nTempBlockSize;
	// TODO: Add your control notification handler code here
	
}

void CFSDlg::OnChangeFileselectEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here

}
