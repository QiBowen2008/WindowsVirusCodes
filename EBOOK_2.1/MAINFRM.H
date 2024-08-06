// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__95725311_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)
#define AFX_MAINFRM_H__95725311_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	void MoveMyWindow();
	CMenu* pMenu;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	RECT rect;
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDowneword();
	afx_msg void OnEmail();
    afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
    afx_msg void OnAppExit();
	afx_msg void OnLiben(WPARAM wParam,LPARAM lParam);
	afx_msg void OnShowMyWindow();
	afx_msg void OnSupport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__95725311_DF79_11D2_8BE2_E78C9ECEAB18__INCLUDED_)
