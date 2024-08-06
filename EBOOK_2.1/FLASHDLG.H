#if !defined(AFX_FLASHDLG_H__CC101EC5_E1E3_11D2_8BE2_9AF2C8A72F19__INCLUDED_)
#define AFX_FLASHDLG_H__CC101EC5_E1E3_11D2_8BE2_9AF2C8A72F19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlashDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlashDlg dialog

class CFlashDlg : public CDialog
{
// Construction
public:
	CFlashDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFlashDlg)
	enum { IDD = IDD_FLASHDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlashDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFlashDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnWelcomepage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int Time;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLASHDLG_H__CC101EC5_E1E3_11D2_8BE2_9AF2C8A72F19__INCLUDED_)
