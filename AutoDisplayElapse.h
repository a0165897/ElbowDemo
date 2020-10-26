#if !defined(AFX_AUTODISPLAYELAPSE_H__F8E48C56_BDE3_11D3_AD14_0000E83E71C7__INCLUDED_)
#define AFX_AUTODISPLAYELAPSE_H_F8_E48_C56_BDE3_11D3_AD14_0000_E83_E71_C7_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoDisplayElapse.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoDisplayElapse dialog

class CAutoDisplayElapse : public CDialog
{
// Construction
public:
	CAutoDisplayElapse(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoDisplayElapse)
	enum { IDD = IDD_DISPLAY_ELAPSE };
	UINT	m_auto_display_elapse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoDisplayElapse)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoDisplayElapse)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTODISPLAYELAPSE_H__F8E48C56_BDE3_11D3_AD14_0000E83E71C7__INCLUDED_)
