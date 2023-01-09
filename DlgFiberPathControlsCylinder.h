#pragma once


// CDlgFiberPathControlsCylinder 对话框

class CDlgFiberPathControlsCylinder : public CDialog
{
public:
	CDlgFiberPathControlsCylinder(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFiberPathControlsCylinder();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FIBER_PATH_CONTROL_CYLINDER };
#endif
	float m_dlg_cylinder_winding_angle;
	float m_dlg_cylinder_band_width;
	float m_dlg_cylinder_band_thickness;
	float m_dlg_cylinder_cut_num;
	float m_dlg_cylinder_slippage_coefficient;
	CString m_dlg_cylinder_winding_algorithm;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	CComboBox m_combo;
};
