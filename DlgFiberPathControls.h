#if !defined(AFX_DLGFIBERPATHCONTROLS_H__82D40D13_F306_11D2_ABA1_0000E83E71C7__INCLUDED_)
#define AFX_DLGFIBERPATHCONTROLS_H__82D40D13_F306_11D2_ABA1_0000E83E71C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFiberPathControls.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFiberPathControls dialog

class CDlgFiberPathControls : public CDialog
{
// Construction
public:
	CDlgFiberPathControls(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFiberPathControls)
	enum { IDD = IDD_FIBER_PATH_CONTROL };
	float	m_max_friction;
	float	m_reference_winding_angle;
	float	m_fiber_band_width;
	int		m_path_loops;
	float	m_thickness;
	short	m_layer;
	BOOL	m_use_layer;
	float	m_offset;
	float	m_eye_accelerate;
	float	m_eye_width;
	float	m_roller_width;
	float	m_payeye_offset;
	short	m_payout_layer;
	BOOL	m_data_format;
	BOOL	m_data_compress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFiberPathControls)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFiberPathControls)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFIBERPATHCONTROLS_H__82D40D13_F306_11D2_ABA1_0000E83E71C7__INCLUDED_)
