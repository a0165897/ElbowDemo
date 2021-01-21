#pragma once


// CDlgFiberPathControlsTube 对话框

class CDlgFiberPathControlsTube : public CDialog
{
public:
	CDlgFiberPathControlsTube(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFiberPathControlsTube();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FIBER_PATH_CONTROL_TUBE };
#endif
	float m_dlg_tube_winding_angle;
	float m_dlg_tube_band_width;
	float m_dlg_tube_band_thickness;
	float m_dlg_tube_cut_num;

	float m_dlg_tube_mandrel_speed;//芯模转速
	float m_dlg_tube_pm_distance;//吐丝嘴-芯模转轴距离
	float m_dlg_tube_pm_left_distance;//吐丝嘴-钉圈平面距离

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
};
