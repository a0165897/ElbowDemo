// WaglView1.cpp : implementation file
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "WaglView1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaglView

IMPLEMENT_DYNCREATE(CWaglView, CView)

CWaglView::CWaglView()
{
}

CWaglView::~CWaglView()
{
}


BEGIN_MESSAGE_MAP(CWaglView, CView)
	//{{AFX_MSG_MAP(CWaglView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaglView drawing

void CWaglView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
	CRect clientRect;
	CString *pStr;
	CSize size;
	float phi0, wagl0;

	int i=0,x, y, scale_length, Nscale=10;//刻度的长度是窗口大小CSize对象成员cx, cy的1/n

	//如何保证数据是有效的？
	//确定窗口大小：分别找自变量和因变量数据的有符号的最大和最小值，然后求差DX和DY
	//确定原点根据有符号的最大和最小值m_WndAngleArray
     
/*	span_angle=pDoc->m_span_angle;
	phi_min=pDoc->m_WndAngleArray[i].phi;
	phi_max=pDoc->m_WndAngleArray[i].phi;
    wagl_min=pDoc->m_WndAngleArray[i].alpha;
	wagl_max=pDoc->m_WndAngleArray[i].alpha;

	for(i=1;i<pDoc->m_Wagl_View_pnt_num;i++)
	{
		if(phi_max< (pDoc->m_WndAngleArray[i].phi))
			phi_max=pDoc->m_WndAngleArray[i].phi;
		if(wagl_max<(pDoc->m_WndAngleArray[i].alpha))
		   wagl_max=pDoc->m_WndAngleArray[i].alpha;
	}

	for(i=1;i<pDoc->m_Wagl_View_pnt_num;i++)
	{
		if(phi_min > (pDoc->m_WndAngleArray[i].phi))
			phi_min=pDoc->m_WndAngleArray[i].phi;
		if(wagl_min> (pDoc->m_WndAngleArray[i].alpha))
		   wagl_min=pDoc->m_WndAngleArray[i].alpha;
	}

	DX=phi_max-phi_min;
	DY=wagl_max-wagl_min;


	GetClientRect(clientRect);
	//x=clientRect.Width();
	//y=clientRect.Height();
	pDC->SetMapMode(MM_ANISOTROPIC);
	size=pDC->SetViewportExt(clientRect.right,clientRect.bottom);
	//size=pDC->SetViewportExt(x,y);

	pDC->SetWindowExt(DX,DY);
	pDC->SetWindowOrg( int(fabs(phi_min)),int(fabs(wagl_max)));

	pDC->SetViewportOrg(clientRect.right/2, clientRect.bottom/2);
	
/*	//Draw two axes and the scales
	pDC->MoveTo(,);
	pDC->LineTo(,);//axis X 
	*pStr="CAgl";
	x=;
	y=;
	pDC->TextOut(x,y, pStr, pStr->GetLength());//Draw Cenagl--central angle 
    
	x_scale0=;
	for( i=1;i<=Nscale;i++)
	{
		x=;
		y=;
		pDC->MoveTo(x,-y);
		pDC->LineTo(x,y);

        x_scale=x_scale0+i*x_scale_step;
		//Converst number to string 
		pStr->Format("%d", x_scale);
		x=;
		y=;
		pDC->TextOut(x,y, pStr, pStr->GetLength());//Draw Cenagl--central angle 

	}//axis X scale

	pDC->MoveTo(,);
	pDC->LineTo(,);//axis Y
	pDC->TextOut();//Draw Wagl--winding angle

	for( i=1;i<=Nscale;i++)
	{
		x=;
		y=;
		pDC->MoveTo(-x,y);
		pDC->LineTo( x,y);
		pDC->TextOut();//Draw the denotation
	}//axis X scale

	pDC->MoveTo(m_WndAngleArray[i].phi,m_WndAngleArray[i].alpha);//i=0, move to the first point
	//Do loop to draw the winding angle to central angle
    //The angle uints are degree
	for( i=1;i<=m_path_point_number;i++)
	{
			x=m_WndAngleArray[i].phi;
			y=m_WndAngleArray[i].alpha;
		    pDC->LineTo(x,y);
			pDC->MoveTo(x,y);//Update the start point
	}
*/
}

/////////////////////////////////////////////////////////////////////////////
// CWaglView diagnostics

#ifdef _DEBUG
void CWaglView::AssertValid() const
{
	CView::AssertValid();
}

void CWaglView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWaglView message handlers
