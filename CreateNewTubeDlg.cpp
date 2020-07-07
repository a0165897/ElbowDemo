// CreateNewTubeDlg.cpp: 实现文件
//

#include "StdAfx.h"
#include "mfcogl1.h"
#include "CreateNewTubeDlg.h"



// CCreateNewTubeDlg 对话框



CCreateNewTubeDlg::CCreateNewTubeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CREATE_NEW_TUBE, pParent)
{

}

CCreateNewTubeDlg::~CCreateNewTubeDlg()
{
}

void CCreateNewTubeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LENGTH_A, m_dlg_tube_a);
	DDX_Text(pDX, IDC_LENGTH_B, m_dlg_tube_b);
	DDX_Text(pDX, IDC_LENGTH_C, m_dlg_tube_c);
	DDX_Text(pDX, IDC_LENGTH_R, m_dlg_tube_r);
}

BOOL CCreateNewTubeDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	m_dlg_tube_a = 16;
	m_dlg_tube_b = 8;
	m_dlg_tube_c = 60;
	m_dlg_tube_r = 10;
	UpdateData(FALSE);
	return TRUE;
}


// CCreateNewTubeDlg 消息处理程序
