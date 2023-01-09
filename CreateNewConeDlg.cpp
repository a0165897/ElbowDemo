// CreateNewConeDlg.cpp: 实现文件
//

#include "StdAfx.h"
#include "mfcogl1.h"
#include "CreateNewConeDlg.h"
// CCreateNewConeDlg 对话框
CCreateNewConeDlg::CCreateNewConeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CREATE_NEW_CONE, pParent)
{
	m_dlg_cone_length = 0;
	m_dlg_cone_lradius = 0;
	m_dlg_cone_rradius = 0;
}

CCreateNewConeDlg::~CCreateNewConeDlg()
{
}

void CCreateNewConeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CONE_LENGTH, m_dlg_cone_length);
	DDV_MinMaxFloat(pDX, m_dlg_cone_length, 0.f, 10000.f);
	DDX_Text(pDX, IDC_CONE_LRADIUS, m_dlg_cone_lradius);
	DDV_MinMaxFloat(pDX, m_dlg_cone_length, 0.f, 10000.f);
	DDX_Text(pDX, IDC_CONE_RRADIUS, m_dlg_cone_rradius);
}

BOOL CCreateNewConeDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	m_dlg_cone_length = 100;
	m_dlg_cone_lradius = 10;
	m_dlg_cone_rradius = 20;
	UpdateData(FALSE);
	return TRUE;
}

