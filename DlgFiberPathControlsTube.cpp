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
	m_dlg_tube_pm_distance = 0 ;//吐丝嘴距芯模最内缘距离
	m_dlg_tube_pm_left_distance = 0 ;//吐丝嘴距芯模左侧距离
	m_dlg_tube_mandrel_redundancy = 0 ;//芯模左右侧冗余
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
	DDX_Text(pDX, IDC_TUBE_MANDREL_REDUNDANCY, m_dlg_tube_mandrel_redundancy);

}

BOOL CDlgFiberPathControlsTube::OnInitDialog() {
	CDialog::OnInitDialog();
	m_dlg_tube_winding_angle = 30.0f;
	m_dlg_tube_band_width = 5.0f;
	m_dlg_tube_cut_num = 7;
	m_dlg_tube_band_thickness = 0.05f;

	m_dlg_tube_mandrel_speed = 0.1;//芯模转速
	m_dlg_tube_pm_distance = 5.0f;//吐丝嘴距芯模最内缘距离
	m_dlg_tube_pm_left_distance = 5.0f;//吐丝嘴距芯模左侧距离
	m_dlg_tube_mandrel_redundancy = 5.0f;//芯模左右侧冗余
	UpdateData(FALSE);
	return TRUE;
}


// CDlgFiberPathControlsTube 消息处理程序
