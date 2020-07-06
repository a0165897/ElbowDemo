// HintDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "HintDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHintDialog dialog


CHintDialog::CHintDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CHintDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHintDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHintDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHintDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHintDialog, CDialog)
	//{{AFX_MSG_MAP(CHintDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHintDialog message handlers
