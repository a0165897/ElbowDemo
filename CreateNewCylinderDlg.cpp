// CreateNewCylinderDlg.cpp: 实现文件
//

#include "Stdafx.h"
#include "mfcogl1.h"
#include "CreateNewCylinderDlg.h"
#include "afxdialogex.h"


// CCreateNewCylinderDlg 对话框

CCreateNewCylinderDlg::CCreateNewCylinderDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CREATE_NEW_CYLINDER, pParent)
{
	m_dlg_cylinder_middle_length = 0;
	m_dlg_cylinder_middle_radius = 0;
	m_dlg_cylinder_left_length = 0;
	m_dlg_cylinder_left_radius = 0;
	m_dlg_cylinder_right_length = 0;
	m_dlg_cylinder_right_radius = 0;
}

CCreateNewCylinderDlg::~CCreateNewCylinderDlg()
{
}

void CCreateNewCylinderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CYLINDER_MIDDLE_LENGTH, m_dlg_cylinder_middle_length);
	DDV_MinMaxFloat(pDX, m_dlg_cylinder_middle_length, 0.f, 1000.f);
	DDX_Text(pDX, IDC_CYLINDER_MIDDLE_RADIUS, m_dlg_cylinder_middle_radius);
	DDV_MinMaxFloat(pDX, m_dlg_cylinder_middle_radius, 0.f, 1000.f);
	DDX_Text(pDX, IDC_CYLINDER_LEFT_LENGTH, m_dlg_cylinder_left_length);
	DDV_MinMaxFloat(pDX, m_dlg_cylinder_left_length, 0.f, 1000.f);
	DDX_Text(pDX, IDC_CYLINDER_LEFT_RADIUS, m_dlg_cylinder_left_radius);
	DDV_MinMaxFloat(pDX, m_dlg_cylinder_left_radius, 0.f, 1000.f);
	DDX_Text(pDX, IDC_CYLINDER_RIGHT_LENGTH, m_dlg_cylinder_right_length);
	DDV_MinMaxFloat(pDX, m_dlg_cylinder_right_length, 0.f, 1000.f);
	DDX_Text(pDX, IDC_CYLINDER_RIGHT_RADIUS, m_dlg_cylinder_right_radius);
	DDV_MinMaxFloat(pDX, m_dlg_cylinder_right_radius, 0.f, 1000.f);
}

// CCreateNewCylinderDlg 消息处理程序
BOOL CCreateNewCylinderDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	m_dlg_cylinder_middle_length = 100;
	m_dlg_cylinder_middle_radius = 20;
	m_dlg_cylinder_left_length = 15;
	m_dlg_cylinder_left_radius = 5;
	m_dlg_cylinder_right_length = 20;
	m_dlg_cylinder_right_radius = 6;
	UpdateData(FALSE);
	return TRUE;
}BEGIN_MESSAGE_MAP(CCreateNewCylinderDlg, CDialog)
ON_EN_CHANGE(IDC_CYLINDER_LEFT_RADIUS, &CCreateNewCylinderDlg::OnEnChangeCylinderLeftRadius)
END_MESSAGE_MAP()


void CCreateNewCylinderDlg::OnEnChangeCylinderLeftRadius()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
