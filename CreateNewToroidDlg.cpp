#include "stdafx.h"
#include "CreateNewToroidDlg.h"
#include "mfcogl1.h"

CCreateNewToroidDlg::CCreateNewToroidDlg(CWnd* pParent /*=nullptr*/): CDialog(IDD_CREATE_NEW_TOROID, pParent){
	m_dlg_toroid_R = 0;
	m_dlg_toroid_r = 0;
}
CCreateNewToroidDlg::~CCreateNewToroidDlg()
{
}
void CCreateNewToroidDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TOROID_BIGR, m_dlg_toroid_R);
	DDX_Text(pDX, IDC_TOROID_SMALLR, m_dlg_toroid_r);
}

BOOL CCreateNewToroidDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	m_dlg_toroid_R = 100;
	m_dlg_toroid_r = 25;
	UpdateData(FALSE);
	return TRUE;
}