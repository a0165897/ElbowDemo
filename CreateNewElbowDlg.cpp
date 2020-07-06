// CreateNewElbowDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "CreateNewElbowDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreateNewElbowDlg dialog


CCreateNewElbowDlg::CCreateNewElbowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateNewElbowDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateNewElbowDlg)
	m_dlg_span_angle = 0.0f;
	m_dlg_sweep_radius = 0.0f;
	m_dlg_pipe_diameter = 0.0f;
	m_cylinder_height = 0.0f;
	m_have_cylinder = TRUE;
	//}}AFX_DATA_INIT
}


void CCreateNewElbowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateNewElbowDlg)
	DDX_Text(pDX, IDC_SPAN_ANGLE, m_dlg_span_angle);
	DDV_MinMaxFloat(pDX, m_dlg_span_angle, 0.f, 360.f);
	DDX_Text(pDX, IDC_SWEEP_RADIUS, m_dlg_sweep_radius);
	DDV_MinMaxFloat(pDX, m_dlg_sweep_radius, 0.f, 4000.f);
	DDX_Text(pDX, IDC_PIPE_RADIUS, m_dlg_pipe_diameter);
	DDV_MinMaxFloat(pDX, m_dlg_pipe_diameter, 0.f, 2000.f);
	DDX_Text(pDX, IDC_CYLINDER_HEIGHT, m_cylinder_height);
	DDV_MinMaxFloat(pDX, m_cylinder_height, 0.f, 1000.f);
	DDX_Check(pDX, IDC_HAVE_CYLINDER, m_have_cylinder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateNewElbowDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateNewElbowDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateNewElbowDlg message handlers

BOOL CCreateNewElbowDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_cylinder_height=8.5;
	m_dlg_pipe_diameter=31.5;
	m_dlg_span_angle=90;
	m_dlg_sweep_radius=45.75;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
