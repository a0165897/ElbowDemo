// CreateNewTubeDlg.cpp: 实现文件
//

#include "StdAfx.h"
#include "mfcogl1.h"
#include "CreateNewTubeDlg.h"



// CCreateNewTubeDlg 对话框



CCreateNewTubeDlg::CCreateNewTubeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CREATE_NEW_TUBE, pParent)
{
	m_dlg_tube_width = 0;
	m_dlg_tube_height = 0;
	m_dlg_tube_length = 0;
	m_dlg_tube_r = 0;
}

CCreateNewTubeDlg::~CCreateNewTubeDlg()
{
}

void CCreateNewTubeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TUBE_WIDTH, m_dlg_tube_width);
	DDV_MinMaxFloat(pDX, m_dlg_tube_width, 0.f, 1000.f);
	DDX_Text(pDX, IDC_TUBE_HEIGHT, m_dlg_tube_height);
	DDV_MinMaxFloat(pDX, m_dlg_tube_height, 0.f, 1000.f);
	DDX_Text(pDX, IDC_TUBE_LENGTH, m_dlg_tube_length);
	DDV_MinMaxFloat(pDX, m_dlg_tube_length, 0.f, 1000.f);
	DDX_Text(pDX, IDC_TUBE_RADIUS, m_dlg_tube_r);
	//make sure the maximum of radius smaller than half width. 
	DDV_MinMaxFloat(pDX, m_dlg_tube_r, 0.f, min(m_dlg_tube_width/2, m_dlg_tube_height/2));
	m_dlg_tube_a=m_dlg_tube_width/2 -  m_dlg_tube_r;
	m_dlg_tube_b = m_dlg_tube_height/2 - m_dlg_tube_r;

}

BOOL CCreateNewTubeDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	m_dlg_tube_width = 50;
	m_dlg_tube_height = 30;
	m_dlg_tube_length = 100;
	m_dlg_tube_r = 5;
	UpdateData(FALSE);
	return TRUE;
}


// CCreateNewTubeDlg 消息处理程序
