// mfcogl1View.cpp : implementation of the CMfcogl1View class
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "rotationspeeddlg.h"
#include "CreateNewElbowDlg.h"
#include "AutoDisplayElapse.h"
#include "mfcogl1Doc.h"
#include "mfcogl1View.h"
#include <math.h>


/*added by LMK */
#include "CreateNewTubeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1View

IMPLEMENT_DYNCREATE(CMfcogl1View, CView)

BEGIN_MESSAGE_MAP(CMfcogl1View, CView)
	//{{AFX_MSG_MAP(CMfcogl1View)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CHAR()
	ON_COMMAND(IDM_ROTATE_WITH_X, OnRotateAroundXAxis)
	ON_COMMAND(IDM_ROTATE_WITH_Y, OnRotateAroundYAxis)
	ON_COMMAND(IDM_ROTATE_WITH_Z, OnRotateAroundZAxis)
	ON_COMMAND(IDM_TRANSLATE_X, OnTranslateX)
	ON_COMMAND(IDM_TRANSLATE_Y, OnTranslateY)
	ON_COMMAND(IDM_TRANSLATE_Z, OnTranslateZ)
	ON_COMMAND(IDM_VIEWING_DIRECTION, OnToggleViewingDirection)
	ON_COMMAND(IDM_CREATE_NEW_ELBOW_MANDREL, OnCreateNewElbowMandrel)
	ON_WM_SIZE()
	ON_COMMAND(IDM_FILL_POLYGON, OnFillPolygonMode)
	ON_COMMAND(IDM_LINE_POLYGON, OnLinePolygonMode)
	ON_UPDATE_COMMAND_UI(IDM_FILL_POLYGON, OnUpdateFillPolygonMode)
	ON_UPDATE_COMMAND_UI(IDM_LINE_POLYGON, OnUpdateLinePolygonMode)
	ON_COMMAND(IDM_AUTO_ROTATE, OnAutoRotate)
	ON_UPDATE_COMMAND_UI(IDM_AUTO_ROTATE, OnUpdateAutoRotate)
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(IDM_ROTATION_SPEED, OnSetRotationSpeed)
	ON_COMMAND(IDM_DRAW_FIBER_PATH, OnDrawFiberPath)
	ON_COMMAND(IDM_DISPLAY_ALL, OnDisplayAll)
	ON_COMMAND(IDM_VIEW_DISABLE_MANDREL_DISPLAY, OnViewDisableMandrelDisplay)
	ON_COMMAND(IDM_DRAW_FIBER_TAPE, OnPayeyeSimulation)
	ON_COMMAND(IDM_DRAW_MANDREL, OnDrawMandrel)
	ON_COMMAND(IDM_DISPLAY_WINDING_SEQUENCE, OnDisplayWindingSequence)//设置动态路径
	ON_COMMAND(IDM_RESET_DISPLAY_STEP_BY_STEP, OnResetDisplayProcessToStart)
	ON_COMMAND(IDM_DISPLAY_NEXT_POINT, OnDisplayNextPointOfWindingProcess)
	ON_COMMAND(IDM_AUTO_DISPLAY_WINDING_PROCESS, OnAutoDisplayNextPoint)//实时仿真路径
	ON_COMMAND(IDM_ADJUST_DISPLAY_ELAPSE, OnAdjustDisplayElapse)
	ON_COMMAND(ID_VIEW_DISPLAY_MANDREL, OnViewDisplayMandrel)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPLAY_MANDREL, OnUpdateViewDisplayMandrel)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

	/*added by LMK*/
	ON_COMMAND(IDM_CREATE_NEW_TUBE_MANDREL, OnCreateNewTubeMandrel)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1View construction/destruction

CMfcogl1View::CMfcogl1View()
{
	// TODO: add construction code here
	step_number=20;
	m_bCanDisplayPayeye=false;
	m_bCanDisplayPath=false;
	pDoc=NULL;
	m_bHaveCylinder=false;
	m_bCanDisplayMandrel=bDynamicFlag=true;


	m_cview_pPathPoint=NULL;
	m_cview_total_path_point_number=0;
	m_cview_auto_draw_two_points=true;
	m_auto_rotate=false;

	m_hrc=NULL;
	m_oldRect=NULL;
    m_pdc=NULL;
	pFrame=NULL;

	m_Xangle=0;
	m_Yangle=-90;//rotate the mandrel to center with z axis
	m_Zangle=0;


	m_view_pipe_radius=15; //mm
	m_view_sweep_radius=45;
	m_view_span_angle=90;
	m_view_cylinder_height=15;

	m_elbow_updated=TRUE;
	m_elbow_cnt=0;
	m_polygon_line_mode=FALSE;
	m_auto_rotate=FALSE;
	m_auto_rotate_angle=0;
	m_view_rotation_speed=50;
	m_angle_increment=0.1*m_view_rotation_speed*360/60;
	m_cview_disable_mandrel_display=false;
	m_cview_display_winding_in_sequence=false;	
	m_cview_enable_tape_display=true;
	m_cview_display_elapse=100;
}

CMfcogl1View::~CMfcogl1View()
{	
	if(m_cview_pPathPoint!=NULL)
	delete 	m_cview_pPathPoint;
}

BOOL CMfcogl1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	cs.cx=600;
	cs.cy=500;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1View drawing

void CMfcogl1View::OnDraw(CDC* pDC)
{

//  Transfer mandrel data to Document whatever it is default or new inputed values
    if(!wglMakeCurrent(m_pdc->GetSafeHdc(),m_hrc) )
	{
		MessageBox("Make the rendering context current failed");
		//return 1;
	}
	glClearDepth(1.0f);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LEQUAL); //without apparent effect
	glEnable(GL_DEPTH_TEST);
	/*changed by LMK*/
	//It was a GL_POSITION setting,but didn't work.
	GLfloat light0Pos[4] = { 0.3F, 0.3F, 0.3F, 0.0F };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0Pos);
	//glLighti(GL_LIGHT0,GL_SPOT_CUTOFF,5);
	//CreateMandrelDisplayList();
	//CreateFiberPathList();
	if(m_elbow_cnt==0)
	{	
		InitFrustum();
		m_elbow_cnt++;
	}
	else if(m_elbow_updated) //Reset translate and rotation accumulated value accordingly
	{
 		InitFrustum();
		m_Xangle=0;
		m_Yangle=0;
		m_Zangle=0;
		m_elbow_updated=FALSE;
	}

//	glClearColor(0.0f,0.0f,0.0f,1.0f); //black
//	glClearColor(0.5f,0.0f,0.80f,1.0f);
	glClearColor(0.98f,0.98f,0.98f,1.0f);//white
//	glClearDepth(100.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(m_X_translate,m_Y_translate,m_Z_translate);

	if(m_auto_rotate)
	{
//		glPushMatrix();
		glTranslatef(0,0,+(m_view_pipe_radius+m_view_sweep_radius)/2);
		glRotatef(m_auto_rotate_angle,1,0,0);
		glTranslatef(0,0,-(m_view_pipe_radius+m_view_sweep_radius)/2);
//		glPopMatrix();
	}
	glRotatef(m_Xangle,1.0f,0.0f,0.0f);
	glRotatef(m_Yangle,0.0f,1.0f,0.0f);
	glRotatef(m_Zangle,0.0f,0.0f,1.0f);
	glColor3f(1.0f,0.0f,1.0f);
	if(m_polygon_line_mode)
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}
	else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

//    Draw mandrel through triangle strip
//	glCallList(COMPOSITE_LIST);

	if(!m_cview_disable_mandrel_display&&m_bCanDisplayMandrel)
    {
		glPushMatrix();
		glCallList(MANDRELDISPLAYLIST);
		glPopMatrix();
	}
	//Material property of fiber for lighting
////////////////////////////////////////////////////////////////////////////
//draw fiber
	if(!m_cview_display_winding_in_sequence)	
	{
		if(!m_cview_enable_tape_display)
		{
			glPushMatrix();
			glCallList(FIBER_PATH_LIST);
			glPopMatrix();
		}
	}
	else
	{
		if(m_cview_pnt_num_displayed_to==m_cview_total_path_point_number||
		   m_payeye_to==m_cview_total_path_point_number)
 
		{
//			bDynamicFlag=TRUE;
			if(pFrame==NULL)pFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd; //pFrame=GetParentOwner();
			pFrame->m_wndStatusBar.SetPaneText(2,"   ");
			pFrame->m_wndProgress.ShowWindow(SW_HIDE); //PostMessage(WM_TIMER);
		}
		else		
		{
			if(m_payeye_to==m_cview_pnt_num_displayed_to)
				scircuit=(float)m_payeye_to/(float)m_cview_total_path_point_number*100;
			else scircuit=(float)m_cview_pnt_num_displayed_to/(float)m_cview_total_path_point_number*100;
			astring.Format("%d%%",scircuit);
			if(pFrame==NULL)pFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd; //GetParentOwner();
			pFrame->m_wndProgress.ShowWindow(SW_SHOW); //PostMessage(WM_TIMER);
//			pFrame->m_wndProgress.GetClientRect(&arect);
//			CClientDC tempDC(&pFrame->m_wndProgress);
//			tempDC.DrawText(astring,&arect,DT_RIGHT);
			pFrame->m_wndStatusBar.SetPaneText(2,astring);
			pFrame->m_wndProgress.SetPos(scircuit);
//			bDynamicFlag=FALSE;
		}
		if(m_bCanDisplayPath)DisplaytheWindingProcess();
		if(m_bCanDisplayPayeye)
		{
			glPushMatrix();
			glRotatef(90,0.0,1.0,0.0);
			glLineWidth(3.0f);
			DisplayPayeyeProcess();
			glLineWidth(1.0f);
			glPopMatrix();
		}

	}

	//CreateFiberPathList();
///////////////////////////////////////////////////////////////////////////
	glPopMatrix();
	glFlush();
//	SwapBuffers(m_pdc->GetSafeHdc());
	//    if(!wglMakeCurrent(m_pdc->GetSafeHdc(),NULL) )
	//{
	//	MessageBox("Release associate HDC failed");
	//	return 1;
	//}

	SwapBuffers(wglGetCurrentDC());

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1View printing

BOOL CMfcogl1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMfcogl1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMfcogl1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1View diagnostics

#ifdef _DEBUG
void CMfcogl1View::AssertValid() const
{
	CView::AssertValid();
}

void CMfcogl1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMfcogl1Doc* CMfcogl1View::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMfcogl1Doc)));
	return (CMfcogl1Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1View message handlers

int CMfcogl1View::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
     
	// TODO: Add your specialized creation code here
	PIXELFORMATDESCRIPTOR pfd={
		sizeof(PIXELFORMATDESCRIPTOR), //size
			1,                         //version 
			PFD_DRAW_TO_WINDOW|        //Flags
			PFD_SUPPORT_OPENGL|
			PFD_DOUBLEBUFFER,        
			PFD_TYPE_RGBA,
			24,                         //PixelType
			0,0,0,0,0,0,                //RGB Bits and shif 
            0,                          //Alphabits
			0,                          //Alphshifs
			0,                          //Accumbits, accum buffer
			0,0,0,                      //RGB Accumbits
			0,                          //Accum Alphabits 
            32,							//Depthbits, z buffer
			0,							//Stencilbits
			0,							//Auxbuffer
			PFD_MAIN_PLANE,             //Layertype
			0,							//Reserved
			0,                          //LayerMask
			0,							//VisibleMask
			0							//DamageMask
	};

	int iPixelFormatIndex;
	if((m_pdc= new CClientDC(this))==NULL)
    return 1;  
	if((iPixelFormatIndex=ChoosePixelFormat(m_pdc->GetSafeHdc(), &pfd))==0)
	{
		MessageBox("Chose Pixel Format Failed!");
		return 1;
	}
	if((SetPixelFormat(m_pdc->GetSafeHdc(),iPixelFormatIndex,&pfd))==FALSE)
	{
		MessageBox("Set Pixel Format Failed");
		return 1;
	}
	if((m_hrc=wglCreateContext(m_pdc->GetSafeHdc()))==NULL)
	{
		MessageBox("Create rendering context failed");
		return 1;
	}

	pFrame=(CMainFrame* )AfxGetMainWnd();
	pFrame->m_wndStatusBar.GetItemRect(1,&arect);
	pFrame->m_wndProgress.MoveWindow(&arect);

	return 0;
}

void CMfcogl1View::OnDestroy() 
{
	if(m_hrc==wglGetCurrentContext())
	{
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(m_hrc);
	}
    if(m_pdc) delete m_pdc;
 
	CView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CMfcogl1View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	BOOL bErase=FALSE;
	CView::OnChar(nChar, nRepCnt, nFlags);
	//ToUper
	switch (nChar)
	{
		case 'x':  {
						m_Xangle+=15;
						//bErase=TRUE;		
				   }
		case 'y': {
					m_Yangle+=15;
						//bErase=TRUE;		
				  }
		case 'z': {
					m_Zangle+=15;
					//bErase=TRUE;		
				  }
		case '=':{
					step_number+=10;
					break;
				 }
		case '-':{
					if(step_number>10)step_number-=10;
					break;
				 }
					
	}
	Invalidate(bErase);
}

void CMfcogl1View::OnRotateAroundXAxis() 
{
		BOOL bErase=FALSE;
		m_Xangle+=m_rotate_step*m_viewing_sign;
		Invalidate(bErase);
}

void CMfcogl1View::OnRotateAroundYAxis() 
{
	    m_Yangle+=m_rotate_step*m_viewing_sign;
		BOOL bErase=FALSE; //Keep background unErased
		Invalidate(bErase); //Erase only foreground
}

void CMfcogl1View::OnRotateAroundZAxis() 
{
		m_Zangle+=m_rotate_step*m_viewing_sign;	
		BOOL bErase=FALSE; //Keep background unErased
		Invalidate(bErase); //Erase only foreground

}

void CMfcogl1View::OnTranslateX() 
{
	// TODO: Add your command handler code here

		m_X_translate+=m_translate_step*m_viewing_sign;	
		BOOL bErase=FALSE; //Keep background unErased
		Invalidate(bErase); //Erase only foreground
}

void CMfcogl1View::OnTranslateY() 
{
	// TODO: Add your command handler code here
		m_Y_translate+=m_translate_step*m_viewing_sign;

		BOOL bErase=FALSE; //Keep background unErased
		Invalidate(bErase); //Erase only foreground
}

void CMfcogl1View::OnTranslateZ() 
{
	// TODO: Add your command handler code here
		m_Z_translate+=m_translate_step*m_viewing_sign;

		BOOL bErase=FALSE; //Keep background unErased
		Invalidate(bErase); //Erase only foreground
}

void CMfcogl1View::OnToggleViewingDirection() 
{
	m_viewing_sign=(m_viewing_sign>0) ? -1:1;
}

void CMfcogl1View::OnCreateNewElbowMandrel() 
{

	CMfcogl1Doc *pDoc;
	pDoc=(CMfcogl1Doc*)GetDocument();
	if((!pDoc->m_bSavePath&&pDoc->m_bCanDisplayFiber)||(!pDoc->m_bSavePayeye&&pDoc->m_bPayeyeComplete))
	{
		CHintDialog hdlg;
		if(hdlg.DoModal()==IDOK)return;
	}
	CCreateNewElbowDlg new_elb_dlg;
	int choice=new_elb_dlg.DoModal();
	if(choice==IDOK)
	{
		pDoc->ResetWndDesign();		
		m_view_pipe_radius=	new_elb_dlg.m_dlg_pipe_diameter/2.;
		m_view_sweep_radius=new_elb_dlg.m_dlg_sweep_radius;
		m_view_span_angle=new_elb_dlg.m_dlg_span_angle;
		m_view_cylinder_height=new_elb_dlg.m_cylinder_height;
		m_bHaveCylinder=new_elb_dlg.m_have_cylinder;
		m_elbow_updated=TRUE;
		m_auto_rotate=FALSE;
		m_cview_display_winding_in_sequence=FALSE;
		m_cview_enable_tape_display=TRUE;
		m_bCanDisplayPayeye=FALSE;
		m_bCanDisplayPath=FALSE;
		CreateMandrelDisplayList();
		++m_elbow_cnt;
		pDoc->ModifyMandrelParameters(m_view_sweep_radius,m_view_pipe_radius,m_view_span_angle,m_view_cylinder_height);
		BOOL bErase=FALSE; //Keep background unErased
		Invalidate(bErase); //Erase only foreground
		/*added by LMK*/
		pDoc->m_isShowing = 2;
	}

}

void CMfcogl1View::InitFrustum()
{
	GLfloat     fMaxObjSize, fAspect;
	GLfloat     fNearPlane, fFarPlane;
	GLfloat     bound_radius,h,fovy;
//	fovy=70.0F*PI/180;//feild of view angle in degree

	bound_radius=m_view_pipe_radius+m_view_sweep_radius;
	h=2*bound_radius;
	fNearPlane =bound_radius;
	GetClientRect(&m_oldRect);
	if (m_oldRect.bottom)
		fAspect = (GLfloat)m_oldRect.right/m_oldRect.bottom;
	else    // don't divide by zero, not that we should ever run into that...
	fAspect = 1.0f;

	fMaxObjSize = float (2*bound_radius*sin(m_view_span_angle/2) );

	m_fRadius = bound_radius+fNearPlane + fMaxObjSize / 2.0f;
	fFarPlane = fNearPlane+30*fMaxObjSize;
	m_Z_translate=-5*m_fRadius;
    fovy=180/PI*2*atan(fMaxObjSize /2/abs(m_Z_translate)); 

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//平截头裁剪面
	fNearPlane=1.0F;
	fFarPlane=10000.0F;
    fovy=15.0F; //the bigger fovy is, the smaller the image is , fovy++, size--
	gluPerspective(fovy, fAspect, fNearPlane, fFarPlane);
	glMatrixMode(GL_MODELVIEW);
	
	m_viewing_sign=1;
	m_translate_step=h/20;
	m_rotate_step=10.0F;//in degree
	m_X_translate=0.0F;
	m_Y_translate=0.0F;
}

void CMfcogl1View::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	m_elbow_updated=FALSE;
	Invalidate(FALSE); //Erase only foreground
	
}

void CMfcogl1View::OnFillPolygonMode() 
{
	// TODO: Add your command handler code here
	m_polygon_line_mode=FALSE;
}  

void CMfcogl1View::OnLinePolygonMode() 
{
	// TODO: Add your command handler code here
		m_polygon_line_mode=TRUE;
}

void CMfcogl1View::OnUpdateFillPolygonMode(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(!m_polygon_line_mode);
}

void CMfcogl1View::OnUpdateLinePolygonMode(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_polygon_line_mode);
}

void CMfcogl1View::OnAutoRotate() 
{
	m_auto_rotate=TRUE;
	m_cview_auto_draw_two_points= !m_auto_rotate;
	SetTimer(0,10,NULL);
	m_angle_increment=0.01*m_view_rotation_speed*360/60;
	m_Xangle=0;
	//m_Yangle=-45;//rotate the mandrel to center with z axis
//	m_Yangle=-(90-m_view_span_angle/2)*2;
	m_Yangle=-90;//Rotate the center of mandrel from x axis to z axis

	m_Zangle=0;
}

void CMfcogl1View::OnUpdateAutoRotate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_auto_rotate);	
}

void CMfcogl1View::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_auto_rotate)
	{
		//KillTimer(0);	
		CView::OnLButtonDown(nFlags, point);
		//m_auto_rotate=FALSE;
	}
	SetCapture();
	m_mouse_down_point=point;
}

void CMfcogl1View::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (m_auto_rotate)
	{
		//KillTimer(0);
		CView::OnMButtonDown(nFlags, point);
		//m_auto_rotate = FALSE;
	}
	SetCapture();
	m_mouse_down_point = point;
}

void CMfcogl1View::OnTimer(UINT nIDEvent) 
{
	//m_Xangle+=4;
	//m_Yangle+=10;
	//m_Zangle+=5;
	if(m_auto_rotate)
	{
		m_auto_rotate_angle+=m_angle_increment;
		Invalidate(FALSE);	
	}
	else
	if(m_cview_auto_draw_two_points)
	{
		if((m_cview_pnt_num_displayed_to==m_cview_total_path_point_number)||
			(m_payeye_to==m_cview_total_path_point_number))KillTimer(2);
		if(m_cview_total_path_point_number>=(m_cview_pnt_num_displayed_to+step_number))
		m_cview_pnt_num_displayed_to=m_cview_pnt_num_displayed_to+step_number;
		else
		if(m_cview_total_path_point_number>=(m_cview_pnt_num_displayed_to+1))
			m_cview_pnt_num_displayed_to=m_cview_total_path_point_number;
		if(m_bCanDisplayPayeye)
		{
			if(m_cview_total_path_point_number>=(m_payeye_to+step_number))
					m_payeye_to+=step_number;
			else
			if(m_cview_total_path_point_number>=(m_payeye_to+1))
					m_payeye_to=m_cview_total_path_point_number;
		}
		Invalidate(FALSE);
	}	
	CView::OnTimer(nIDEvent);
}

void CMfcogl1View::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(GetCapture()==this&&nFlags==MK_LBUTTON)
	{
		m_Xangle+=(GLfloat)(point.y-m_mouse_down_point.y)/2;
		m_Yangle+=(GLfloat)(point.x-m_mouse_down_point.x)/2;
		Invalidate(FALSE);
		m_mouse_down_point=point;
	}
	if (GetCapture() == this && nFlags == MK_MBUTTON)//鼠标中键
	{
		m_Y_translate += -(GLfloat)(point.y - m_mouse_down_point.y) / 2;
		m_X_translate += (GLfloat)(point.x - m_mouse_down_point.x) / 2;
		Invalidate(FALSE);
		m_mouse_down_point = point;
	}
	CView::OnMouseMove(nFlags, point);
}

BOOL CMfcogl1View::OnMouseWheel(UINT   nFlags, short   zDelta, CPoint   pt) {
	m_Z_translate += zDelta;
	Invalidate(FALSE);
	CView::OnMouseWheel(nFlags, zDelta, pt);
	return true;
}

void CMfcogl1View::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();	
	CView::OnLButtonUp(nFlags, point);
}

void CMfcogl1View::OnMButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	CView::OnMButtonUp(nFlags, point);
}

void CMfcogl1View::OnSetRotationSpeed() 
{
	CRotationSpeedDlg spddlg;
	if(IDOK==spddlg.DoModal())
	{
		m_view_rotation_speed=spddlg.m_rotation_speed;
		m_auto_rotate=FALSE;
		KillTimer(0);
	}
	//OnAutoRotate(); //CALL message handler directly in order to update speed right after the
	//dialog is closed
}



int CMfcogl1View::ReadDataFromDLL(char *pFileName)
{
	CFile f;
	CFileException e;
	CString str;
	int byte_size,i, rt;
	long int last, current, pBuffer[8];
    bool last_data_valid=true, current_data_valid=true;
	m_cview_dll_data_valid=true;

	rt=f.Open(pFileName,CFile::modeReadWrite,&e);
	if(!rt)
	{
		m_cview_dll_vanish=true;
		return 0;
	}
	if(rt)
	{
		m_cview_dll_vanish=false;
		byte_size=sizeof(long int);
		f.Seek(-8*sizeof(long int), CFile::end );
		f.Read(m_cview_dll_pData,8*sizeof(long int));
		last=m_cview_dll_pData[0];
		for(i=1;i<4;i++)
		{
			if(m_cview_dll_pData[i]!=last)
			{
				last_data_valid=false;
				break;
			}
		}
		current=m_cview_dll_pData[4];
		for(i=5;i<8;i++)
		{
			if(m_cview_dll_pData[i]!=current)
			{
				current_data_valid=false;
				break;
			}
		}

		if(last_data_valid&&current_data_valid)
		{
			if((last>0)&&(last==current+1))
			{
				last=current;
				current=current-1;
			}
			else
			{
				last=-1;
				current=-10;
				m_cview_dll_data_valid=false;
			}
			for(i=0;i<4;i++)
			{
				pBuffer[i]=last;
			}
			for(i=4;i<8;i++)
			{
				pBuffer[i]=current;
			}
			f.Seek(-8*sizeof(long int), CFile::end );
			f.Write(pBuffer,8*sizeof(long int));	
			f.Close();
			str.Format("Last=%d, current=%d", last, current);
			//AfxMessageBox(str,MB_OK||MB_APPLMODAL); 
		}
		else m_cview_dll_data_valid=false;
		return 1;
	}
}

int CMfcogl1View::SetDataInDLL(char *pFileName)
{
	CFile f;
	CFileException e;
	CString str;
	int byte_size,i,rt;
	long int last, current;
    bool last_data_valid=true, current_data_valid=true;

	rt=f.Open(pFileName,CFile::modeWrite,&e);
	if(!rt)
	{
		m_cview_dll_vanish=true;
		return 0;
	}
	if(rt)
	{
		m_cview_dll_vanish=false;
		byte_size=sizeof(long int);
		last=50;
		current=49;
		for(i=0;i<4;i++)
		{
			m_cview_dll_pData[i]=last;
		}

		for(i=4;i<8;i++)
		{
			m_cview_dll_pData[i]=current;
		}

		f.Seek(-8*sizeof(long int), CFile::end );
		f.Write(m_cview_dll_pData,8*sizeof(long int));

		f.Close();
		return 1;
	}
}

void CMfcogl1View::CreateMandrelDisplayList()

{
 	GLfloat ra,rb,span_angle; //default values
 	GLfloat theta,dtheta,phi,dphi; 
 	GLfloat point1[3],point2[3];
 	GLfloat m_Ne1[3], m_Ne2[3];
	GLdouble theta0;
 	GLint iCrossSegment=36, iLongitudinalSegment=40,iphi,itheta;
 	dphi=2*PI/iCrossSegment;

	rb=m_view_pipe_radius;
	rb=rb*0.96;
	ra=m_view_sweep_radius;
	//ra=ra-ra*0.05;
	span_angle=PI*m_view_span_angle/180;
	dtheta=span_angle/iLongitudinalSegment;

	theta0=(PI-span_angle)/2;

	if(glIsList(MANDRELDISPLAYLIST)==GL_TRUE)
 	glDeleteLists(MANDRELDISPLAYLIST, 1);
	glNewList(MANDRELDISPLAYLIST,GL_COMPILE);

	GLfloat matAmb[4] = { 0.01F, 0.01F, 0.01F, 1.00F };
    GLfloat matDiff[4] = { 0.7F, 0.8F, 0.85F, 0.60F };
    GLfloat matSpec[4] = { 0.20F, 0.20F, 0.20F, 1.00F };
    GLfloat matShine = 10.00F;
  
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialf(GL_FRONT, GL_SHININESS, matShine);

	glBegin(GL_TRIANGLE_STRIP);
		for(itheta=1;itheta<=iLongitudinalSegment;itheta++)
		{
			GLfloat fThetaLeft=(itheta-1)*dtheta+theta0;
			GLfloat fThetaRight=fThetaLeft+dtheta;

			GLfloat fCosLeftTheta=(float)cos(fThetaLeft);
			GLfloat fSinLeftTheta=(float)sin(fThetaLeft);

			GLfloat fCosRightTheta=(float)cos(fThetaRight);
			GLfloat fSinRightTheta=(float)sin(fThetaRight);

			for( iphi=1;iphi<=iCrossSegment+1;iphi++)
			{
				if(iphi==iCrossSegment)
					phi=0;
				else
					phi=(iphi-1)* dphi;
				GLfloat cosphi=(GLfloat)cos(phi);
				GLfloat sinphi=(GLfloat)sin(phi);
				GLfloat y=rb*sinphi;
				//left circumferential frame
				//Vertex coordinate
				GLfloat r=(ra+rb*cosphi);
				point1[0]=r*fSinLeftTheta;   // X coordinate
				point1[1]=y;					 // Y coordinate
				point1[2]=r*fCosLeftTheta;  // z coordinate
				//Vertex normal
				m_Ne1[0]=fSinLeftTheta*cosphi;
				m_Ne1[1]=sinphi;
				m_Ne1[2]=fCosLeftTheta*cosphi;
			
				glNormal3fv(m_Ne1);
				glColor3f(1.0f,1.0f,0.0f);
				glVertex3fv(point1);

				//right circumferential frame
				//Vertex coordinate
				point2[0]=r*fSinRightTheta; // X coordinate
				point2[1]=y;				 // Y coordinate
				point2[2]=r*fCosRightTheta; // z coordinate
				m_Ne2[0]=fSinRightTheta*cosphi;
				m_Ne2[1]=sinphi;
				m_Ne2[2]=fCosRightTheta*cosphi;

				glNormal3fv(m_Ne2);
				glColor3f(0.0f,0.5f,0.5f);
				glVertex3fv(point2);
			}
		}
	glEnd();

	if(m_bHaveCylinder)
	{  
		iLongitudinalSegment=5;
		dtheta=m_view_cylinder_height/iLongitudinalSegment;
		theta=0.0;
		matDiff[0]=0.1;
		matDiff[1]=0.8;
		matDiff[2]=0.2;
		glMaterialfv(GL_FRONT,GL_DIFFUSE,matDiff);
		glPushMatrix();
		glTranslatef(ra*cos(m_view_span_angle*PI/180.0/2.0),0.0,ra*sin(m_view_span_angle*PI/180.0/2.0));
		glRotatef(-m_view_span_angle/2.0,0.0,1.0,0.0);
		for(itheta=0;itheta<iLongitudinalSegment;itheta++)
		{
			phi=0.0;
			glBegin(GL_TRIANGLE_STRIP);
			for(iphi=0;iphi<=iCrossSegment;iphi++)
			{
				if(iphi==iCrossSegment)phi=0.0;
				point1[0]=rb*cos(phi);
				point1[1]=rb*sin(phi);
				point1[2]=theta;
				m_Ne1[0]=cos(phi);
				m_Ne1[1]=sin(phi);
				m_Ne1[2]=0.0;
				glNormal3fv(m_Ne1);
				glVertex3fv(point1);
				point1[2]=(itheta==(iLongitudinalSegment-1))?m_view_cylinder_height:(theta+dtheta);
				glVertex3fv(point1);
				phi+=dphi;
			}
			glEnd();
			theta+=dtheta;
		}
		glPopMatrix();
   
		glPushMatrix();
		glTranslatef(ra*cos(m_view_span_angle*PI/180.0/2.0),0.0,-ra*sin(m_view_span_angle*PI/180.0/2.0));
		glRotatef(m_view_span_angle/2.0,0.0,1.0,0.0);
		dtheta=-m_view_cylinder_height/iLongitudinalSegment;
		theta=0.0;
		for(itheta=0;itheta<iLongitudinalSegment;itheta++)
		{
			phi=0.0;
			glBegin(GL_TRIANGLE_STRIP);
			for(iphi=0;iphi<=iCrossSegment;iphi++)
			{
				if(iphi==iCrossSegment)phi=0.0;
				point1[0]=rb*cos(phi);
				point1[1]=rb*sin(phi);
				point1[2]=theta;
				m_Ne1[0]=cos(phi);
				m_Ne1[1]=sin(phi);
				m_Ne1[2]=0.0;
				glNormal3fv(m_Ne1);
				glVertex3fv(point1);
				point1[2]=(itheta==(iLongitudinalSegment-1))?(-m_view_cylinder_height):(theta+dtheta);
				glVertex3fv(point1);
				phi+=dphi;
			}
			glEnd();
			theta+=dtheta;
		}
	}
	glEndList();

}

int CMfcogl1View::DisplayPayeyeProcess()
{
 	if(pDoc->m_bPayeyeComplete)
	{
		unsigned long i;
		GLfloat point[3],deviation=pDoc->deviation;

		glBegin(GL_LINE_STRIP);
			GLfloat fiber_matAmb[4] = {0.30F, 0.30F, 0.0F, 1.00F};
			GLfloat fiber_matDiff[4] = {0.05F, 0.05F, 0.8F, 0.50F};
			GLfloat fiber_matSpec[4] = {0.40F, 0.40F, 0.00F, 1.00F};
			GLfloat fiber_matShine = 60.00F;
  
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fiber_matAmb);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fiber_matDiff);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fiber_matSpec);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, fiber_matShine);
        
			for(i=0;i<=m_payeye_to;i++)
			{
				if(i>=(m_payeye_to-2))
				{
					fiber_matDiff[0]=1.0f;
					fiber_matDiff[1]=1.0f;
					fiber_matDiff[2]=0.05f;
					glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,fiber_matDiff);
				}	
				point[0]=PT[i].x;
				point[1]=-11.8+30;  //m_view_sweep_radius+m_view_pipe_radius;
				point[2]=PT[i].z+deviation;
				glNormal3f(0.0,1.0,0.0);
				glVertex3fv(point);			

			}
		
		glEnd();
	}
	return  0;
}

int CMfcogl1View::DisplaytheWindingProcess()
{
	if(pDoc->m_bCanDisplayFiber && pDoc->m_isShowing==2)
	{
 		GLdouble ra,rb; //default values //,span_angle
 		int i; 
		int point_number; //circuit_step_view;
    
		float w,normal[3],vertex[3];//Supposed value, should be replaced by value inputed from dialog box
		float angle,x,z;
  		ra=pDoc->m_sweep_radius;
		rb=pDoc->m_pipe_radius;
		w=pDoc->m_FilamentWidth;

		angle=pDoc->m_span_angle/2.0;
		x=ra*cos(angle);
		z=ra*sin(angle);

//		circuit_step_view=pDoc->circuit_step;

		point_number=m_cview_pnt_num_displayed_to;

		//Draw fiber tape
			GLfloat fiber_matAmb[4] = { 0.30F, 0.30F, 0.6F, 1.00F };
			GLfloat fiber_matDiff[4] = { 0.05F, 0.05F, 0.5F, 0.50F };
			GLfloat fiber_matSpec[4] = { 0.20F, 0.20F, 0.50F, 1.00F };
			GLfloat fiber_matShine = 60.00F;
  
			glMaterialfv(GL_FRONT, GL_AMBIENT, fiber_matAmb);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, fiber_matDiff);
			glMaterialfv(GL_FRONT, GL_SPECULAR, fiber_matSpec);
			glMaterialf(GL_FRONT, GL_SHININESS, fiber_matShine);
        
			glBegin(GL_TRIANGLE_STRIP);
			
			for(i=0;i<=point_number;i++)
			{
/*				if((i/circuit_step_view+1)%2)
				{
					fiber_matDiff[1]=1.0;
					fiber_matDiff[0]=1.0;
					fiber_matDiff[2]=0.05;
					glMaterialfv(GL_FRONT, GL_DIFFUSE, fiber_matDiff);
				}
				else
				{
					fiber_matDiff[1]=0.05;
					fiber_matDiff[2]=0.5;
					fiber_matDiff[0]=0.05;
					glMaterialfv(GL_FRONT, GL_DIFFUSE, fiber_matDiff);
				}
*/				
				if(i==(point_number-2))
				{
					fiber_matDiff[0]=1.0f;
					fiber_matDiff[1]=1.0f;
					fiber_matDiff[2]=0.05f;
					glMaterialfv(GL_FRONT,GL_DIFFUSE,fiber_matDiff);
				}
				if(PE[i].flag==0)
				{		
					normal[0]=cos(PE[i].theta)*cos(PE[i].phi);
					normal[1]=sin(PE[i].phi);
					normal[2]=sin(PE[i].theta)*cos(PE[i].phi);
					vertex[0]=(ra+rb*cos(PE[i].phi))*cos(PE[i].theta);
					vertex[1]=rb*sin(PE[i].phi);
					vertex[2]=(ra+rb*cos(PE[i].phi))*sin(PE[i].theta);
					vertex[0]+=w/2.0*(sin(PE[i].theta)*sin(PE[i].alpha)-
						sin(PE[i].phi)*cos(PE[i].theta)*cos(PE[i].alpha));
					vertex[1]+=w/2.0*cos(PE[i].phi)*cos(PE[i].alpha);
					vertex[2]-=w/2.0*(cos(PE[i].theta)*sin(PE[i].alpha)+
						sin(PE[i].phi)*sin(PE[i].theta)*cos(PE[i].alpha));
					glNormal3fv(normal);
					glVertex3fv(vertex);
					vertex[0]-=w*(sin(PE[i].theta)*sin(PE[i].alpha)-
						sin(PE[i].phi)*cos(PE[i].theta)*cos(PE[i].alpha));
					vertex[1]-=w*cos(PE[i].phi)*cos(PE[i].alpha);
					vertex[2]+=w*(cos(PE[i].theta)*sin(PE[i].alpha)+
						sin(PE[i].phi)*sin(PE[i].theta)*cos(PE[i].alpha));
					glVertex3fv(vertex);
				}
				else
				{
					normal[0]=cos(PE[i].phi)*cos(PE[i].flag*angle);
					normal[1]=sin(PE[i].phi);
					normal[2]=cos(PE[i].phi)*sin(PE[i].flag*angle);
					vertex[0]=rb*cos(PE[i].phi)*cos(PE[i].flag*angle)-PE[i].flag*PE[i].theta
						*sin(PE[i].flag*angle)+x;
					vertex[1]=rb*sin(PE[i].phi);
					vertex[2]=PE[i].flag*PE[i].theta*cos(PE[i].flag*angle)+rb*cos(PE[i].phi)
						*sin(PE[i].flag*angle)+PE[i].flag*z;
					vertex[0]-=w/2.0*(sin(PE[i].phi)*cos(PE[i].alpha)*cos(PE[i].flag*angle)-
						sin(PE[i].alpha)*sin(PE[i].flag*angle));
					vertex[1]+=w/2.0*cos(PE[i].phi)*cos(PE[i].alpha);
					vertex[2]-=w/2.0*(sin(PE[i].alpha)*cos(PE[i].flag*angle)+sin(PE[i].phi)*
						cos(PE[i].alpha)*sin(PE[i].flag*angle));
					
					glNormal3fv(normal);
					glVertex3fv(vertex);
					vertex[0]+=w*(sin(PE[i].phi)*cos(PE[i].alpha)*cos(PE[i].flag*angle)-
						sin(PE[i].alpha)*sin(PE[i].flag*angle));
					vertex[1]-=w*cos(PE[i].phi)*cos(PE[i].alpha);
					vertex[2]+=w*(sin(PE[i].alpha)*cos(PE[i].flag*angle)+sin(PE[i].phi)*
						cos(PE[i].alpha)*sin(PE[i].flag*angle));
					glVertex3fv(vertex);
				}
			}
			glEnd();
	}
	else if (pDoc->m_bCanDisplayFiber && pDoc->m_isShowing == 1) {
		int i,point_number = m_cview_pnt_num_displayed_to;
		GLfloat matAmb[4] = { 0.8F, 0.5F, 0.3F, 1.00F };
		GLfloat matDiff[4] = { 0.8F, 0.5F, 0.3F, 0.80F };
		GLfloat matSpec[4] = { 0.30F, 0.30F, 0.30F, 0.30F };
		GLfloat matShine = 60.00F;
		glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
		glMaterialf(GL_FRONT, GL_SHININESS, matShine);
		glLineWidth(2);
		glBegin(GL_LINE_STRIP);
		auto iter = pDoc->TubePointList->begin();
		for (i = 0; i < point_number; i++) {
			float a[3] = { (*iter).x,(*iter).y,(*iter).z-pDoc->model.length/2 };
			glVertex3fv(a);
			iter++;
		}
		glEnd();
	}
	return  0;
}

int CMfcogl1View::CreateFiberPathList()
{
 	GLfloat ra,rb,rh,w; //default values //,span_angle
 	GLfloat angle,x,z;
 	GLfloat normal[3],vertex[3];
// 	int i,j,k,index=0,flag,quarter_elbow,total_cylinder; //iVsegments=36, iUsegments=40,
	unsigned long i,total_point_number;
//	int circuit_step_view;

    CMfcogl1Doc *pDoc;
	pDoc=GetDocument();
	ra=pDoc->m_sweep_radius;
	rb=pDoc->m_pipe_radius;
	rh=pDoc->m_height;
/*	quarter_elbow=pDoc->quarter_elbow;
	total_cylinder=pDoc->total_cylinder;
*/	angle=pDoc->m_span_angle/2.0;
	x=ra*cos(angle);
	z=ra*sin(angle);
	w=pDoc->m_FilamentWidth;
	i=pDoc->m_numlayer;
	total_point_number=pDoc->m_WindingCount[i-1];
//	circuit_step_view=pDoc->circuit_step;

	if(glIsList(FIBER_PATH_LIST)==GL_TRUE)glDeleteLists(FIBER_PATH_LIST, 1);
	glNewList(FIBER_PATH_LIST,GL_COMPILE);
	
		GLfloat fiber_matAmb[4] = { 0.30F, 0.30F, 0.6F, 1.00F };
	    GLfloat fiber_matDiff[4] = { 0.05F, 0.05F, 0.5F, 0.50F };
	    GLfloat fiber_matSpec[4] = { 0.20F, 0.20F, 0.50F, 1.00F };
	    GLfloat fiber_matShine = 60.00F;
  
	    glMaterialfv(GL_FRONT, GL_AMBIENT, fiber_matAmb);
	    glMaterialfv(GL_FRONT, GL_DIFFUSE, fiber_matDiff);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, fiber_matSpec);
	    glMaterialf(GL_FRONT, GL_SHININESS, fiber_matShine);
        

		glBegin(GL_TRIANGLE_STRIP);
		
		for(i=0;i<=total_point_number;i++)
		{
/*			if((i/circuit_step_view+1)%2)
			{
				fiber_matDiff[0]=1.0;
				fiber_matDiff[2]=0.05;
				fiber_matDiff[1]=1.0;
				glMaterialfv(GL_FRONT, GL_DIFFUSE, fiber_matDiff);
			}
			else
			{
				fiber_matDiff[0]=0.05;
				fiber_matDiff[2]=0.5;
				fiber_matDiff[1]=0.05;
				glMaterialfv(GL_FRONT, GL_DIFFUSE, fiber_matDiff);
			}
*/
			if(PE[i].flag==0)
			{		
				normal[0]=cos(PE[i].theta)*cos(PE[i].phi);
				normal[1]=sin(PE[i].phi);
				normal[2]=sin(PE[i].theta)*cos(PE[i].phi);
				vertex[0]=(ra+rb*cos(PE[i].phi))*cos(PE[i].theta);
				vertex[1]=rb*sin(PE[i].phi);
				vertex[2]=(ra+rb*cos(PE[i].phi))*sin(PE[i].theta);
				vertex[0]+=w/2.0*(sin(PE[i].theta)*sin(PE[i].alpha)-
					sin(PE[i].phi)*cos(PE[i].theta)*cos(PE[i].alpha));
				vertex[1]+=w/2.0*cos(PE[i].phi)*cos(PE[i].alpha);
				vertex[2]-=w/2.0*(cos(PE[i].theta)*sin(PE[i].alpha)+
					sin(PE[i].phi)*sin(PE[i].theta)*cos(PE[i].alpha));
				glNormal3fv(normal);
				glVertex3fv(vertex);
				vertex[0]-=w*(sin(PE[i].theta)*sin(PE[i].alpha)-
					sin(PE[i].phi)*cos(PE[i].theta)*cos(PE[i].alpha));
				vertex[1]-=w*cos(PE[i].phi)*cos(PE[i].alpha);
				vertex[2]+=w*(cos(PE[i].theta)*sin(PE[i].alpha)+
					sin(PE[i].phi)*sin(PE[i].theta)*cos(PE[i].alpha));
				glVertex3fv(vertex);
			}
			else
			{
				normal[0]=cos(PE[i].phi)*cos(PE[i].flag*angle);
				normal[1]=sin(PE[i].phi);
				normal[2]=cos(PE[i].phi)*sin(PE[i].flag*angle);
				vertex[0]=rb*cos(PE[i].phi)*cos(PE[i].flag*angle)-PE[i].flag*PE[i].theta
					*sin(PE[i].flag*angle)+x;
				vertex[1]=rb*sin(PE[i].phi);
				vertex[2]=PE[i].flag*PE[i].theta*cos(PE[i].flag*angle)+rb*cos(PE[i].phi)
					*sin(PE[i].flag*angle)+PE[i].flag*z;
				vertex[0]-=w/2.0*(sin(PE[i].phi)*cos(PE[i].alpha)*cos(PE[i].flag*angle)-
					sin(PE[i].alpha)*sin(PE[i].flag*angle));
				vertex[1]+=w/2.0*cos(PE[i].phi)*cos(PE[i].alpha);
				vertex[2]-=w/2.0*(sin(PE[i].alpha)*cos(PE[i].flag*angle)+sin(PE[i].phi)*
					cos(PE[i].alpha)*sin(PE[i].flag*angle));
				
				glNormal3fv(normal);
				glVertex3fv(vertex);
				vertex[0]+=w*(sin(PE[i].phi)*cos(PE[i].alpha)*cos(PE[i].flag*angle)-
					sin(PE[i].alpha)*sin(PE[i].flag*angle));
				vertex[1]-=w*cos(PE[i].phi)*cos(PE[i].alpha);
				vertex[2]+=w*(sin(PE[i].alpha)*cos(PE[i].flag*angle)+sin(PE[i].phi)*
					cos(PE[i].alpha)*sin(PE[i].flag*angle));
				glVertex3fv(vertex);
			}
		}
		glEnd();
	glEndList();
	return  0;
}

void CMfcogl1View::OnDrawFiberPath() 
{
	CMfcogl1Doc *pDoc;
	pDoc=(CMfcogl1Doc*)GetDocument();
	ASSERT(pDoc!=NULL);
	if (pDoc->m_isShowing == 1) {
		return;
	}
	if(pDoc->m_bCanDisplayFiber)
	{
		CreateFiberPathList();
//	m_cview_display_winding_in_sequence=false;
		m_cview_enable_tape_display=false;
		Invalidate(false);
	}
	else AfxMessageBox(_T("Please compute fiber path or load existent file first !"));
}

void CMfcogl1View::OnDisplayAll() 
{
	if(glIsList(COMPOSITE_LIST)==GL_TRUE)
 	glDeleteLists(COMPOSITE_LIST, 1);
	glNewList(COMPOSITE_LIST,GL_COMPILE );
	glCallList(FIBER_PATH_LIST);
	glCallList(MANDRELDISPLAYLIST);
	glEndList();
}


void CMfcogl1View::OnViewDisableMandrelDisplay() 
{
	// TODO: Add your command handler code here
	m_cview_disable_mandrel_display=!m_cview_disable_mandrel_display;
}

void CMfcogl1View::OnPayeyeSimulation() 
{
	if(!m_cview_display_winding_in_sequence)pDoc=GetDocument();
	if(pDoc->m_bPayeyeComplete)
	{
		m_cview_display_winding_in_sequence=true;
		m_bCanDisplayPayeye=TRUE;
		m_auto_rotate=FALSE;
		m_payeye_to=1;
	}
	else
	{
		AfxMessageBox(_T("Please compute payeye track or load existent file first !"));
	}
}

void CMfcogl1View::OnDrawMandrel() 
{
}
//设置动态路径
void CMfcogl1View::OnDisplayWindingSequence() 
{
	if(!m_bCanDisplayPayeye)pDoc=GetDocument();
	if(pDoc->m_bCanDisplayFiber)
	{
		m_cview_display_winding_in_sequence=true;
		m_bCanDisplayPath=TRUE;
		m_auto_rotate=FALSE;
		if(pDoc->m_bUseLayer)
		m_cview_total_path_point_number=pDoc->m_WindingCount[pDoc->m_numlayer-1];
		else m_cview_total_path_point_number=pDoc->m_WindingCount[0];	
		m_cview_pnt_num_displayed_to=1;
		Invalidate(FALSE);
	}
	else AfxMessageBox(_T("Please compute fiber path or load existent file first !"));
}

void CMfcogl1View::OnResetDisplayProcessToStart() 
{
	m_cview_pnt_num_displayed_to=0;
}

void CMfcogl1View::OnDisplayNextPointOfWindingProcess() 
{
	if(m_cview_total_path_point_number>=(m_cview_pnt_num_displayed_to+2))
	m_cview_pnt_num_displayed_to=m_cview_pnt_num_displayed_to+2;
	else
		if(m_cview_total_path_point_number>=(m_cview_pnt_num_displayed_to+1))
		m_cview_pnt_num_displayed_to=m_cview_pnt_num_displayed_to+1;

	Invalidate(FALSE);
}
//实时仿真路径
void CMfcogl1View::OnAutoDisplayNextPoint() 
{
	if(m_bCanDisplayPath||m_bCanDisplayPayeye)
	{
		unsigned short elapse;
		if(pDoc->m_bUseLayer)
			m_cview_total_path_point_number=pDoc->m_WindingCount[pDoc->m_numlayer-1];
		else m_cview_total_path_point_number=pDoc->m_WindingCount[0];	

		m_cview_auto_draw_two_points=TRUE;
		m_auto_rotate=!m_cview_auto_draw_two_points;

		elapse=m_cview_display_elapse;	
		SetTimer(2,elapse/100,NULL);
	}
}

void CMfcogl1View::OnAdjustDisplayElapse() 
{
	CAutoDisplayElapse dlg;
	if(IDOK==(dlg.DoModal()))
	{
		m_cview_display_elapse=dlg.m_auto_display_elapse;
	}
}


void CMfcogl1View::OnViewDisplayMandrel() 
{
/*	CMainFrame:: SetCircuitValue(1,2,5);
	pFrame=GetParentOwner();
	pFrame->PostMessage(WM_MYMESSAGE);
*/	m_bCanDisplayMandrel=!m_bCanDisplayMandrel;
	Invalidate(FALSE);
	// TODO: Add your command handler code here
}

void CMfcogl1View::OnUpdateViewDisplayMandrel(CCmdUI* pCmdUI) 
{
	if(m_bCanDisplayMandrel)pCmdUI->SetText(_T("隐藏芯模(&Hide mandrel)"));
	else pCmdUI->SetText(_T("显示芯模(&Show mandrel)"));
	// TODO: Add your command update UI handler code here
	
}

void CMfcogl1View::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CMfcogl1Doc *pDoc;
	pDoc=GetDocument();
	m_view_pipe_radius=	pDoc->m_pipe_radius;
	m_view_sweep_radius=pDoc->m_sweep_radius;
	m_view_span_angle=pDoc->m_span_angle*180.0/PI;
	m_view_cylinder_height=pDoc->m_height;
	m_bHaveCylinder=TRUE;
	m_elbow_updated=TRUE;
	m_auto_rotate=FALSE;
	m_cview_disable_mandrel_display=FALSE;
	m_cview_display_winding_in_sequence=FALSE;
	m_cview_enable_tape_display=TRUE;
	m_bCanDisplayPayeye=FALSE;
	m_bCanDisplayPath=FALSE;
	//CreateTubeDisplayList();
	Invalidate(FALSE);
	// TODO: Add your specialized code here and/or call the base class
}

/*added by LMK*/
//生成方管
void CMfcogl1View::OnCreateNewTubeMandrel() {
	//初始化数值
	m_cview_display_winding_in_sequence = false;
	m_bCanDisplayPath = false;
	KillTimer(2);
	if (glIsList(FIBER_PATH_LIST) == GL_TRUE)
		glDeleteLists(FIBER_PATH_LIST, 1);

	CMfcogl1Doc* pDoc = GetDocument();
	pDoc->m_isShowing = 1;
	CCreateNewTubeDlg new_tube_dlg;
	int choice = new_tube_dlg.DoModal();
	if (choice == IDOK){
		pDoc->ResetWndDesign();
		m_view_tube_a = new_tube_dlg.m_dlg_tube_a;
		m_view_tube_b = new_tube_dlg.m_dlg_tube_b;
		m_view_tube_length = new_tube_dlg.m_dlg_tube_length;
		m_view_tube_r = new_tube_dlg.m_dlg_tube_r;
	}
	CreateTubeDisplayList();
	//to refresh screen
	Invalidate(false);
}

void CMfcogl1View::CreateTubeDisplayList(){
	GLfloat length_step = m_view_tube_length;
	int i = 0, flagx = 1, flagy = 1, degree, angle_step = 10;
	glNewList(MANDRELDISPLAYLIST, GL_COMPILE);

	myDrawAxis();

	//light and color setting.
	GLfloat matDiff[4] = { 0.4F, 0.6F, 0.6F, 1.00F };
	GLfloat matSpec[4] = { 0.1F, 0.1F, 0.1F, 1.00F };
	GLfloat matShine = 5.00F;

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, matShine);

	//compute the mandrel.
	glBegin(GL_TRIANGLE_STRIP);
	for (degree = 0; degree <=360; degree += angle_step) {
		if (degree % 90 == 0) {
			glVertex3f(flagx * m_view_tube_a + m_view_tube_r * cos(degree * PI / 180), flagy * m_view_tube_b + m_view_tube_r * sin(degree * PI / 180), length_step * (i++ % 2 - 0.5));
			glVertex3f(flagx * m_view_tube_a + m_view_tube_r * cos(degree * PI / 180), flagy * m_view_tube_b + m_view_tube_r * sin(degree * PI / 180), length_step * (i++ % 2 - 0.5));
		}
		flagx = (degree >= 90 && degree < 270) ? -1 : 1;
		flagy = (degree >= 180 && degree < 360) ? -1 : 1;
		glNormal3f(cos(degree * PI / 180), sin(degree * PI / 180), 0.0f);
		glVertex3f(flagx * m_view_tube_a + m_view_tube_r * cos(degree * PI / 180), flagy * m_view_tube_b + m_view_tube_r * sin(degree * PI / 180), length_step * (i++ % 2 - 0.5));
		glVertex3f(flagx * m_view_tube_a + m_view_tube_r * cos(degree * PI / 180), flagy * m_view_tube_b + m_view_tube_r * sin(degree * PI / 180), length_step * (i++ % 2 - 0.5));
	}
	glEnd();
	glEndList();
}

void CMfcogl1View::myDrawAxis() {
	glLineWidth(1.0);
	glEnable(GL_LINE_SMOOTH);
	glBegin(GL_LINES);
		GLfloat matDiff[4] = { 1.0F, 0.0F, 0.0F, 1.00F };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff);
		glVertex3f(0, 0, 0 - m_view_tube_length / 2);
		glVertex3f(10000, 0,0 - m_view_tube_length / 2);
		GLfloat matDiff2[4] = { 0.0F, 1.0F, 0.0F, 1.00F };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff2);
		glVertex3f(0, 0, 0 - m_view_tube_length / 2);
		glVertex3f(0, 0,10000 - m_view_tube_length / 2);
		GLfloat matDiff3[4] = { 0.0F, 0.0F, 1.0F, 1.00F };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff3);
		glVertex3f(0, 0, 0 - m_view_tube_length / 2);
		glVertex3f(0, 10000, 0 - m_view_tube_length / 2);
	glEnd();
	glBegin(GL_LINE_STRIP);
		
	glEnd(); 
	glDisable(GL_LINE_SMOOTH);
}
