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
	//width = 2a+2r  a is half width of the flat area.
	float m_dlg_tube_width;
	//height = 2b+2r  b is half height of the flat area.
	float m_dlg_tube_height;
	float m_dlg_tube_length;
	float m_dlg_tube_a;
	float m_dlg_tube_b;
	float m_dlg_tube_r;
	float m_dlg_tube_redundance;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
};
