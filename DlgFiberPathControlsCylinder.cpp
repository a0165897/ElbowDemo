// DlgFiberPathControlsCylinder.cpp: 实现文件
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "DlgFiberPathControlsCylinder.h"


// CDlgFiberPathControlsCylinder 对话框

CDlgFiberPathControlsCylinder::CDlgFiberPathControlsCylinder(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_FIBER_PATH_CONTROL_CYLINDER, pParent)
{
	m_dlg_cylinder_winding_angle = 0;
	m_dlg_cylinder_band_width = 0;
	m_dlg_cylinder_band_thickness = 0;
	m_dlg_cylinder_cut_num = 0;
}

CDlgFiberPathControlsCylinder::~CDlgFiberPathControlsCylinder()
{
}

void CDlgFiberPathControlsCylinder::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_CYLINDER_WINDING_ANGLE, m_dlg_cylinder_winding_angle);
	DDX_Text(pDX, IDC_CYLINDER_BAND_WIDTH, m_dlg_cylinder_band_width);
	DDX_Text(pDX, IDC_CYLINDER_CUT_NUM, m_dlg_cylinder_cut_num);
	DDX_Text(pDX, IDC_CYLINDER_BAND_THICKNESS, m_dlg_cylinder_band_thickness);

	CDialog::DoDataExchange(pDX);
}

BOOL CDlgFiberPathControlsCylinder::OnInitDialog() {
	CDialog::OnInitDialog();
	m_dlg_cylinder_winding_angle = 25.0f;
	m_dlg_cylinder_band_width = 5.0f;
	m_dlg_cylinder_cut_num = 7;
	m_dlg_cylinder_band_thickness = 0.05f;
	UpdateData(FALSE);
	return TRUE;
}

// CDlgFiberPathControlsCylinder 消息处理程序
