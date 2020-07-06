// PressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "PressDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPressDialog dialog


CPressDialog::CPressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPressDialog)
	m_long_cylinder = 10;
	m_long_elbow = 50;
	m_latitude = 36;
	//}}AFX_DATA_INIT
}


void CPressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPressDialog)
	DDX_Text(pDX, IDC_CYLINDER_LONGITUDE, m_long_cylinder);
	DDV_MinMaxInt(pDX, m_long_cylinder, 5, 15);
	DDX_Text(pDX, IDC_ELBOW_LONGITUDE, m_long_elbow);
	DDV_MinMaxInt(pDX, m_long_elbow, 20, 90);
	DDX_Text(pDX, IDC_LATITUDE_NUMBER, m_latitude);
	DDV_MinMaxInt(pDX, m_latitude, 20, 72);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPressDialog, CDialog)
	//{{AFX_MSG_MAP(CPressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPressDialog message handlers

BOOL CPressDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_latitude=36;
	m_long_elbow=50;
	m_long_cylinder=5;
	UpdateData(FALSE);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
