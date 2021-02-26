#pragma once
// CDlgFiberPathControlsToroid 对话框

class CDlgFiberPathControlsToroid : public CDialog
{
public:
	CDlgFiberPathControlsToroid(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFiberPathControlsToroid();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FIBER_PATH_CONTROL_TOROID };
#endif
	float m_dlg_toroid_band_width;
	float m_dlg_toroid_winding_angle;
	float m_dlg_toroid_slippage_coefficient;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
};
