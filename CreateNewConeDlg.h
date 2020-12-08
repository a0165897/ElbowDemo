#pragma once


// CCreateNewConeDlg 对话框

class CCreateNewConeDlg : public CDialog{
public:
	CCreateNewConeDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCreateNewConeDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_NEW_TUBE };
#endif
	float m_dlg_cone_length;
	float m_dlg_cone_lradius;
	float m_dlg_cone_rradius;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
};
