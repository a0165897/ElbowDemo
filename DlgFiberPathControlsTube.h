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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
};
