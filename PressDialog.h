#if !defined(AFX_PRESSDIALOG_H__54F5E2E0_4E14_11D4_BF94_CF410A4B2DEA__INCLUDED_)
#define AFX_PRESSDIALOG_H__54F5E2E0_4E14_11D4_BF94_CF410A4B2DEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PressDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPressDialog dialog

class CPressDialog : public CDialog
{
// Construction
public:
	CPressDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPressDialog)
	enum { IDD = IDD_PRESS_DIALOG };
	short	m_long_cylinder;
	short	m_long_elbow;
	short	m_latitude;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPressDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPressDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRESSDIALOG_H__54F5E2E0_4E14_11D4_BF94_CF410A4B2DEA__INCLUDED_)
