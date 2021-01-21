// DlgFiberPathControlsTube.cpp: 实现文件
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "DlgFiberPathControlsTube.h"


// CDlgFiberPathControlsTube 对话框


CDlgFiberPathControlsTube::CDlgFiberPathControlsTube(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_FIBER_PATH_CONTROL_TUBE, pParent)
{
	m_dlg_tube_winding_angle = 0;
	m_dlg_tube_band_width = 0;
	m_dlg_tube_band_thickness = 0;
	m_dlg_tube_cut_num = 0 ;

	m_dlg_tube_mandrel_speed = 0 ;//芯模转速
	m_dlg_tube_pm_distance = 0 ;//吐丝嘴-芯模转轴距离
	m_dlg_tube_pm_left_distance = 0 ;//吐丝嘴-钉圈平面距离

}

CDlgFiberPathControlsTube::~CDlgFiberPathControlsTube()
{
}

void CDlgFiberPathControlsTube::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TUBE_WINDING_ANGLE, m_dlg_tube_winding_angle);
	DDX_Text(pDX, IDC_TUBE_BAND_WIDTH, m_dlg_tube_band_width);
	DDX_Text(pDX, IDC_TUBE_CUT_NUM, m_dlg_tube_cut_num);
	DDX_Text(pDX, IDC_TUBE_BAND_THICKNESS, m_dlg_tube_band_thickness);

	DDX_Text(pDX, IDC_TUBE_MANDREL_SPEED, m_dlg_tube_mandrel_speed);
	DDX_Text(pDX, IDC_TUBE_MANDREL_PAYEYE_DISTANCE, m_dlg_tube_pm_distance);
	DDX_Text(pDX, IDC_TUBE_MANDREL_PAYEYE_LEFT_DISTANCE, m_dlg_tube_pm_left_distance);
}

BOOL CDlgFiberPathControlsTube::OnInitDialog() {
	CDialog::OnInitDialog();
	m_dlg_tube_winding_angle = 15.0f;
	m_dlg_tube_band_width = 5.0f;
	m_dlg_tube_cut_num = 7;
	m_dlg_tube_band_thickness = 0.1f;

	m_dlg_tube_mandrel_speed = 36;//芯模转速
	m_dlg_tube_pm_distance = 50.0f;//吐丝嘴-芯模转轴距离
	m_dlg_tube_pm_left_distance = 0.0f;//吐丝嘴-钉圈平面距离

	UpdateData(FALSE);
	return TRUE;
}


// CDlgFiberPathControlsTube 消息处理程序
