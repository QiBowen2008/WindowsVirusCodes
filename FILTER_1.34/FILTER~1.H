// FilterDlg.h : header file
//

#if !defined(AFX_FILTERDLG_H__08E648B9_BCF5_11D3_B1BC_0080C84821E0__INCLUDED_)
#define AFX_FILTERDLG_H__08E648B9_BCF5_11D3_B1BC_0080C84821E0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg dialog

class CFilterDlg : public CDialog
{
// Construction
public:
	void UpdateStaticBox(bool status);
	void CalWav3();
	void CalWav4();
	void CalWav2();
	void CalWav1();
	void DrawOldWave(CPaintDC& dc);
	CFilterDlg(CWnd* pParent = NULL);	// standard constructor
	double y[501],y2[501],y4[14][501],v[2][501];
	int CurrentWav;

// Dialog Data
	//{{AFX_DATA(CFilterDlg)
	enum { IDD = IDD_FILTER_DIALOG };
	CEdit	m_Edit3;
	CEdit	m_Edit2;
	CSpinButtonCtrl	m_Spin3;
	CSpinButtonCtrl	m_Spin2;
	CEdit	m_ZoominEdit;
	CSpinButtonCtrl	m_ZoominSpin;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableRegulate(bool enable);
	bool DataSuccess;
	HICON m_hIcon;
	int m_Zoomin;
	// Generated message map functions
	//{{AFX_MSG(CFilterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnWav1();
	afx_msg void OnUpdateEdit1();
	afx_msg void OnWav2();
	afx_msg void OnUpdateEdit2();
	afx_msg void OnUpdateEdit3();
	afx_msg void OnWav4();
	afx_msg void OnWav3();
	afx_msg void OnStatic1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERDLG_H__08E648B9_BCF5_11D3_B1BC_0080C84821E0__INCLUDED_)
