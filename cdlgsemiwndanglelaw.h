#if !defined(AFX_CDLGSEMIWNDANGLELAW_H__4DB95463_AFB9_11D3_925A_0000E823D18E__INCLUDED_)
#define AFX_CDLGSEMIWNDANGLELAW_H__4DB95463_AFB9_11D3_925A_0000E823D18E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// cdlgsemiwndanglelaw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cdlgsemiwndanglelaw dialog

class cdlgsemiwndanglelaw : public CDialog
{
// Construction
public:
	cdlgsemiwndanglelaw(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(cdlgsemiwndanglelaw)
	enum { IDD = IDD_SEMI_WND_ANGLE_LAW };
	float	m_dlg_alpha_end;
	float	m_dlg_alph0;
	float	m_dlg_lamda;
	float	m_dlg_phi_inc;
	float	m_dlg_phi0;
	UINT	m_dlg_wnd_angle_quadrant;
	float	m_dlg_theta0;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cdlgsemiwndanglelaw)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cdlgsemiwndanglelaw)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDLGSEMIWNDANGLELAW_H__4DB95463_AFB9_11D3_925A_0000E823D18E__INCLUDED_)
