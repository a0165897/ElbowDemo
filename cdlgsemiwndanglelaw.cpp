// cdlgsemiwndanglelaw.cpp : implementation file
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "cdlgsemiwndanglelaw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cdlgsemiwndanglelaw dialog


cdlgsemiwndanglelaw::cdlgsemiwndanglelaw(CWnd* pParent /*=NULL*/)
	: CDialog(cdlgsemiwndanglelaw::IDD, pParent)
{
	//{{AFX_DATA_INIT(cdlgsemiwndanglelaw)
	m_dlg_alpha_end = 0.0f;
	m_dlg_alph0 = 0.0f;
	m_dlg_lamda = 0.0f;
	m_dlg_phi_inc = 0.0f;
	m_dlg_phi0 = 0.0f;
	m_dlg_wnd_angle_quadrant = 0;
	m_dlg_theta0 = 0.0f;
	//}}AFX_DATA_INIT
}


void cdlgsemiwndanglelaw::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cdlgsemiwndanglelaw)
	DDX_Text(pDX, IDC_ALPHA_END, m_dlg_alpha_end);
	DDV_MinMaxFloat(pDX, m_dlg_alpha_end, 0.f, 90.f);
	DDX_Text(pDX, IDC_ALPHA0, m_dlg_alph0);
	DDV_MinMaxFloat(pDX, m_dlg_alph0, 0.f, 90.f);
	DDX_Text(pDX, IDC_lamda, m_dlg_lamda);
	DDX_Text(pDX, IDC_PHI_INC, m_dlg_phi_inc);
	DDX_Text(pDX, IDC_PHI0, m_dlg_phi0);
	DDX_Text(pDX, IDC_WND_ANGLE_QUADRANT, m_dlg_wnd_angle_quadrant);
	DDV_MinMaxUInt(pDX, m_dlg_wnd_angle_quadrant, 1, 4);
	DDX_Text(pDX, IDC_THETA0, m_dlg_theta0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cdlgsemiwndanglelaw, CDialog)
	//{{AFX_MSG_MAP(cdlgsemiwndanglelaw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cdlgsemiwndanglelaw message handlers

BOOL cdlgsemiwndanglelaw::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_dlg_alph0=90;
	m_dlg_alpha_end=60;
	m_dlg_lamda=0.2;
	m_dlg_phi0=0;
	m_dlg_phi_inc=-360;
	m_dlg_wnd_angle_quadrant=3;

	UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
