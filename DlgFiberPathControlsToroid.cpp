// DlgFiberPathControlsToroid.cpp: 实现文件
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "DlgFiberPathControlsToroid.h"

// CDlgFiberPathControlsToroid 对话框

CDlgFiberPathControlsToroid::CDlgFiberPathControlsToroid(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_FIBER_PATH_CONTROL_TOROID, pParent)
{
	m_dlg_toroid_band_width = 0;
	m_dlg_toroid_winding_angle = 0;
}

CDlgFiberPathControlsToroid::~CDlgFiberPathControlsToroid()
{
}

void CDlgFiberPathControlsToroid::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_TOROID_BAND_WIDTH, m_dlg_toroid_band_width);
	DDX_Text(pDX, IDC_TOROID_SLIPPAGE_COEFFICIENT, m_dlg_toroid_slippage_coefficient);
	DDX_Text(pDX, IDC_TOROID_WINDING_ANGLE, m_dlg_toroid_winding_angle);

	CDialog::DoDataExchange(pDX);
}

BOOL CDlgFiberPathControlsToroid::OnInitDialog() {
	CDialog::OnInitDialog();
	m_dlg_toroid_band_width = 5.0f;
	m_dlg_toroid_winding_angle = 50.0f;
	m_dlg_toroid_slippage_coefficient = 0.35f;
	UpdateData(FALSE);
	return TRUE;
}

// CDlgFiberPathControlsToroid 消息处理程序
