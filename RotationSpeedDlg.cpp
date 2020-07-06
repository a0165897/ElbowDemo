// RotationSpeedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "RotationSpeedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRotationSpeedDlg dialog


CRotationSpeedDlg::CRotationSpeedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRotationSpeedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRotationSpeedDlg)
	m_rotation_speed = 0.0f;
	//}}AFX_DATA_INIT
}


void CRotationSpeedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRotationSpeedDlg)
	DDX_Text(pDX, IDC_EDIT1, m_rotation_speed);
	DDV_MinMaxFloat(pDX, m_rotation_speed, -1000.f, 1000.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRotationSpeedDlg, CDialog)
	//{{AFX_MSG_MAP(CRotationSpeedDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotationSpeedDlg message handlers

BOOL CRotationSpeedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_rotation_speed=20;	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
