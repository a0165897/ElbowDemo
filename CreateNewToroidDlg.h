#pragma once
class CCreateNewToroidDlg:public CDialog{
public:
	CCreateNewToroidDlg(CWnd* pParent = nullptr);
	virtual ~CCreateNewToroidDlg();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_NEW_TOROID };
#endif
	float m_dlg_toroid_R;
	float m_dlg_toroid_r;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	virtual BOOL OnInitDialog();
public:
};

