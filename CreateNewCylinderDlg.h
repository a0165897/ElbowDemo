#pragma once


// CCreateNewCylinderDlg 对话框

class CCreateNewCylinderDlg : public CDialog{
public:
	CCreateNewCylinderDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCreateNewCylinderDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_NEW_CYLINDER };
#endif
	float m_dlg_cylinder_middle_length;
	float m_dlg_cylinder_middle_radius;
	float m_dlg_cylinder_left_length;
	float m_dlg_cylinder_left_radius;
	float m_dlg_cylinder_right_length;
	float m_dlg_cylinder_right_radius;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnEnChangeCylinderLeftRadius();
};