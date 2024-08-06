// FSDlg.h : header file
//

#if !defined(AFX_FSDLG_H__E22B6FC8_E492_11D0_BC56_F118154FCB62__INCLUDED_)
#define AFX_FSDLG_H__E22B6FC8_E492_11D0_BC56_F118154FCB62__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CFSDlg dialog

class CFSDlg : public CDialog
{
// Construction
public:
	CFSDlg(CWnd* pParent = NULL);	// standard constructor
	CString m_SplitFileName;
	CString m_MergeFileName;
	CString m_PreBlockSize;
	int nBlockSize;

// Dialog Data
	//{{AFX_DATA(CFSDlg)
	enum { IDD = IDD_FS_DIALOG };
	CProgressCtrl	m_Progress;
	int		m_Function;
	CString	m_FileName;
	CString	m_BlockSize;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFSDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFSDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSplitRadio();
	afx_msg void OnMergeRadio();
	afx_msg void OnSelectfileButton();
	afx_msg void OnStartButton();
	afx_msg void OnChangeBlocksizeEdit();
	afx_msg void OnChangeFileselectEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Merge();
	void Split();
	int GetStringLength(char *InputString);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FSDLG_H__E22B6FC8_E492_11D0_BC56_F118154FCB62__INCLUDED_)
