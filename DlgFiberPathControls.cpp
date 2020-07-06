// DlgFiberPathControls.cpp : implementation file
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "DlgFiberPathControls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFiberPathControls dialog


CDlgFiberPathControls::CDlgFiberPathControls(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFiberPathControls::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFiberPathControls)
	m_max_friction = 0.0f;
	m_reference_winding_angle = 0.0f;
	m_fiber_band_width = 0.0f;
	m_path_loops = 0;
	m_thickness = 0.0f;
	m_layer = 0;
	m_use_layer = FALSE;
	m_offset = 0.0f;
	m_eye_accelerate = 0.0f;
	m_eye_width = 0.0f;
	m_roller_width = 0.0f;
	m_payeye_offset = 0.0f;
	m_payout_layer = 0;
	m_data_format = FALSE;
	m_data_compress = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgFiberPathControls::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFiberPathControls)
	DDX_Text(pDX, IDC_MAX_FRICTION, m_max_friction);
	DDV_MinMaxFloat(pDX, m_max_friction, 0.f, 1.f);
	DDX_Text(pDX, IDC_REFERENCE_WINDING_ANGLE, m_reference_winding_angle);
	DDX_Text(pDX, IDC_FIBER_BAND_WIDTH, m_fiber_band_width);
	DDV_MinMaxFloat(pDX, m_fiber_band_width, 0.1f, 5.f);
	DDX_Text(pDX, IDC_PATH_LOOPS, m_path_loops);
	DDV_MinMaxInt(pDX, m_path_loops, 0, 1000);
	DDX_Text(pDX, IDC_FILAMENT_THICKNESS, m_thickness);
	DDV_MinMaxFloat(pDX, m_thickness, 1.e-002f, 10.f);
	DDX_Text(pDX, IDC_LAYER_NUMBER, m_layer);
	DDV_MinMaxInt(pDX, m_layer, 1, 10);
	DDX_Check(pDX, IDC_USE_LAYER, m_use_layer);
	DDX_Text(pDX, IDC_INITIAL_OFFSET, m_offset);
	DDV_MinMaxFloat(pDX, m_offset, 3.f, 15.f);
	DDX_Text(pDX, IDC_PAYEYE_ACCELERATE, m_eye_accelerate);
	DDV_MinMaxFloat(pDX, m_eye_accelerate, 0.2f, 15.f);
	DDX_Text(pDX, IDC_PAYEYE_WIDTH, m_eye_width);
	DDV_MinMaxFloat(pDX, m_eye_width, 0.2f, 15.f);
	DDX_Text(pDX, IDC_ROLLER_WIDTH, m_roller_width);
	DDV_MinMaxFloat(pDX, m_roller_width, 0.2f, 15.f);
	DDX_Text(pDX, IDC_PAYEYE_ANGLE, m_payeye_offset);
	DDV_MinMaxFloat(pDX, m_payeye_offset, 0.f, 8.f);
	DDX_Text(pDX, IDC_PAYOUT_LAYER, m_payout_layer);
	DDV_MinMaxInt(pDX, m_payout_layer, 0, 50);
	DDX_Check(pDX, IDC_DATA_FORMAT, m_data_format);
	DDX_Check(pDX, IDC_COMPRESS_DATA, m_data_compress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFiberPathControls, CDialog)
	//{{AFX_MSG_MAP(CDlgFiberPathControls)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFiberPathControls message handlers

BOOL CDlgFiberPathControls::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_layer=1;
	m_max_friction = 0.2f;
//	m_position_error_divisor = 0.0f;
//	m_reference_winding_angle = 60.0f;	
	m_fiber_band_width=2.0f;
	m_thickness=0.025f;
	m_use_layer=FALSE;
	m_offset=5.0;
	m_eye_accelerate=0.5;
	m_eye_width=6.0;
	m_roller_width=5.0;
	m_payeye_offset=4.5;
	m_payout_layer=1;
	m_data_format=FALSE;
	m_data_compress=FALSE;

/*	m_length_ratio_semi_to_half_part = 0.8f;
	m_semi_max_phi_inc = 12.0f;
	m_geo_max_phi_inc = 12.0f;
*/	m_path_loops=2;

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
