#pragma once


// CCreateNewTubeDlg 对话框

class CCreateNewTubeDlg : public CDialog{
public:
	CCreateNewTubeDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCreateNewTubeDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_NEW_TUBE };
#endif
	float m_dlg_tube_a;
	float m_dlg_tube_b;
	float m_dlg_tube_c;
	float m_dlg_tube_r;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
};
