// AutoDisplayElapse.cpp : implementation file
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "AutoDisplayElapse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoDisplayElapse dialog


CAutoDisplayElapse::CAutoDisplayElapse(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoDisplayElapse::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoDisplayElapse)
	m_auto_display_elapse = 0;
	//}}AFX_DATA_INIT
}


void CAutoDisplayElapse::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoDisplayElapse)
	DDX_Text(pDX, IDC_AUTO_DISPLAY_ELAPSE, m_auto_display_elapse);
	DDV_MinMaxUInt(pDX, m_auto_display_elapse, 0, 500);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoDisplayElapse, CDialog)
	//{{AFX_MSG_MAP(CAutoDisplayElapse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoDisplayElapse message handlers

BOOL CAutoDisplayElapse::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_auto_display_elapse=50;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
