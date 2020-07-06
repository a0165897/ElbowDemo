// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__27D94B2E_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_)
#define AFX_MAINFRM_H__27D94B2E_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define WM_MYMESSAGE WM_USER+100

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	CProgressCtrl m_wndProgress;
	CStatusBar  m_wndStatusBar;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar    m_wndToolBar;
	// Generated message map functions
protected:
//	CMfcogl1View* GetActiveView();
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowMandrel();
	afx_msg void OnWindowWndAngel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
//	afx_msg void OnShowCircuit();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__27D94B2E_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_)
