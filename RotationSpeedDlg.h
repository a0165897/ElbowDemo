#if !defined(AFX_ROTATIONSPEEDDLG_H__48B08136_EC12_11D2_AB98_0000E83E71C7__INCLUDED_)
#define AFX_ROTATIONSPEEDDLG_H__48B08136_EC12_11D2_AB98_0000E83E71C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RotationSpeedDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRotationSpeedDlg dialog

class CRotationSpeedDlg : public CDialog
{
// Construction
public:
	CRotationSpeedDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRotationSpeedDlg)
	enum { IDD = IDD_ROTATION_SPEED };
	float	m_rotation_speed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRotationSpeedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRotationSpeedDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROTATIONSPEEDDLG_H__48B08136_EC12_11D2_AB98_0000E83E71C7__INCLUDED_)
