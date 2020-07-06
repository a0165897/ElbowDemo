#if !defined(AFX_CREATENEWELBOWDLG_H__1F2D1373_E809_11D2_AB91_0000E83E71C7__INCLUDED_)
#define AFX_CREATENEWELBOWDLG_H__1F2D1373_E809_11D2_AB91_0000E83E71C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreateNewElbowDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreateNewElbowDlg dialog

class CCreateNewElbowDlg : public CDialog
{
// Construction
public:
	CCreateNewElbowDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCreateNewElbowDlg)
	enum { IDD = IDD_CREATE_NEW_ELBOW };
	float	m_dlg_span_angle;
	float	m_dlg_sweep_radius;
	float	m_dlg_pipe_diameter;
	float	m_cylinder_height;
	BOOL	m_have_cylinder;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateNewElbowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateNewElbowDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATENEWELBOWDLG_H__1F2D1373_E809_11D2_AB91_0000E83E71C7__INCLUDED_)
