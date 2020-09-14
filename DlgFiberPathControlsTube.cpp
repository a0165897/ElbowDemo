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
}

BOOL CDlgFiberPathControlsTube::OnInitDialog() {
	CDialog::OnInitDialog();
	m_dlg_tube_winding_angle = 30.0f;
	m_dlg_tube_band_width = 5.0f;
	m_dlg_tube_cut_num = 7;
	UpdateData(FALSE);
	return TRUE;
}


// CDlgFiberPathControlsTube 消息处理程序
