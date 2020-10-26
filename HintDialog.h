#if !defined(AFX_HINTDIALOG_H__6937E2C1_67EC_11D4_BF96_CF0FCA31DF33__INCLUDED_)
#define AFX_HINTDIALOG_H__6937E2C1_67EC_11D4_BF96_CF0FCA31DF33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HintDialog.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CHintDialog dialog

class CHintDialog : public CDialog
{
// Construction
public:
	CHintDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHintDialog)
	enum { IDD = IDD_HINT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHintDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHintDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HINTDIALOG_H__6937E2C1_67EC_11D4_BF96_CF0FCA31DF33__INCLUDED_)
