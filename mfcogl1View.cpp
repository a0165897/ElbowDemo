// mfcogl1View.cpp : implementation of the CMfcogl1View class
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "RotationSpeedDlg.h"
#include "CreateNewCylinderDlg.h"
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
int a22333;
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
	ON_COMMAND(IDM_DRAW_FIBER_TAPE, OnPayeyeSimulation)//设置丝嘴仿真
	ON_COMMAND(IDM_DISPLAY_WINDING_SEQUENCE, OnDisplayWindingSequence)//设置动态路径
	ON_COMMAND(IDM_RESET_DISPLAY_STEP_BY_STEP, OnResetDisplayProcessToStart)
	ON_COMMAND(IDM_DISPLAY_NEXT_POINT, OnDisplayNextPointOfWindingProcess)
	ON_COMMAND(IDM_AUTO_DISPLAY_WINDING_PROCESS, OnAutoDisplayNextPoint)//实时仿真路径
	ON_COMMAND(IDM_ADJUST_DISPLAY_ELAPSE, OnAdjustDisplayElapse)
	ON_COMMAND(ID_VIEW_DISPLAY_MANDREL, OnViewDisplayMandrel)//显示芯模
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPLAY_MANDREL, OnUpdateViewDisplayMandrel)
	//}}AFX_MSG_MAP

	/*added by LMK*/
	ON_COMMAND(IDM_CREATE_NEW_TUBE_MANDREL, OnCreateNewTubeMandrel)
	ON_COMMAND(IDM_CREATE_NEW_CYLINDER_MANDREL, OnCreateNewCylinderMandrel)
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

	//instead of gluLootAt
	m_Xangle=32;
	m_Yangle=-25;//rotate the mandrel to center with z axis
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
	if(m_cview_pPathPoint!=NULL){ delete 	m_cview_pPathPoint; }

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
	}

	glClearDepth(1.0F);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LEQUAL); //without apparent effect
	glEnable(GL_DEPTH_TEST);
	//光源
	GLfloat light0Pos[4] = { 0.3F, 0.3F, 0.3F,1.0F };
	GLfloat light0Spec[4] = {1.0F, 1.0F, 1.0F,1.0F };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0Pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Pos);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0Spec);
	//设置透视矩阵
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

	glClearColor(0.8,0.8f,0.8f,1.0f);//white
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//定义World2View矩阵
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(m_X_translate,m_Y_translate,m_Z_translate);
	glRotatef(m_Xangle,1.0f,0.0f,0.0f);
	glRotatef(m_Yangle,0.0f,1.0f,0.0f);
	glRotatef(m_Zangle,0.0f,0.0f,1.0f);
	//线框模式选择
	if(m_polygon_line_mode)
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}
	else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	//画坐标轴
	glLineWidth(3.0);
	glCallList(GLOBAL_LIST);
	glLineWidth(1.0);
	//旋转只影响芯模和纤维，不影响坐标轴和机器路径
	if (m_auto_rotate){
		glRotatef(m_auto_rotate_angle, 0,0,1);
	}
	//当允许显示芯模时
	if(!m_cview_disable_mandrel_display&&m_bCanDisplayMandrel){
		glCallList(MANDREL_DISPLAY_LIST);
	}
	//Material property of fiber for lighting
////////////////////////////////////////////////////////////////////////////
//draw fiber 当直接显示纤维路径时 这个时候以display list方式输出
	if(!m_cview_display_winding_in_sequence&&!m_cview_enable_tape_display){
		glCallList(FIBER_PATH_LIST);
	}
	else{//动态显示路径
		//进度条
		if(m_cview_pnt_num_displayed_to==m_cview_total_path_point_number|| m_payeye_to==m_cview_total_path_point_number){
			if(pFrame==NULL)pFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd; //pFrame=GetParentOwner();
			pFrame->m_wndStatusBar.SetPaneText(2,"   ");
			pFrame->m_wndProgress.ShowWindow(SW_HIDE); //PostMessage(WM_TIMER);
		}
		else{
			//进度条
			if(m_payeye_to==m_cview_pnt_num_displayed_to)
				scircuit=(float)m_payeye_to/(float)m_cview_total_path_point_number*100;
			else scircuit=(float)m_cview_pnt_num_displayed_to/(float)m_cview_total_path_point_number*100;
			astring.Format("%d%%",scircuit);
			if(pFrame==NULL)pFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd; //GetParentOwner();
			pFrame->m_wndProgress.ShowWindow(SW_SHOW); //PostMessage(WM_TIMER);
			pFrame->m_wndStatusBar.SetPaneText(2,astring);
			pFrame->m_wndProgress.SetPos(scircuit);
//			bDynamicFlag=FALSE;
		}
		if (m_bCanDisplayPath) { //动态的纤维路径
			DisplaytheWindingProcess(); 
		}
		if(m_bCanDisplayPayeye){//动态的机器路径
			glPushMatrix();
			glRotatef(90,0.0,1.0,0.0);
			glLineWidth(3.0f);
			DisplayPayeyeProcess();
			glLineWidth(1.0f);
			glPopMatrix();
		}
	}//动态展示部分到此结束
	glPopMatrix();
	glFlush();

	SwapBuffers(wglGetCurrentDC());

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

	//清理所有内存
	KillTimer(0);
	KillTimer(1);
	CMfcogl1Doc* pDoc;
	pDoc = (CMfcogl1Doc*)GetDocument();
	pDoc->TubePointList->clear();
	pDoc->TubePointListTime->clear();
	pDoc->TubeTrackListTime->clear();

	
	CView::OnDestroy();
}

void CMfcogl1View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	BOOL bErase=FALSE;
	CView::OnChar(nChar, nRepCnt, nFlags);
	//ToUper
	switch (nChar)
	{
		case '3':{
			//DEBUG
			int fuck = 1;
		}
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
	fNearPlane=0.1F;
	fFarPlane=10000.0F;
    fovy=15.0F; //the bigger fovy is, the smaller the image is , fovy++, size--
	//定义一下透视矩阵
	gluPerspective(fovy, fAspect, fNearPlane, fFarPlane);
	glMatrixMode(GL_MODELVIEW);
	
	m_viewing_sign=1;
	m_translate_step=h/20;
	m_rotate_step=10.0F;//in degree
	m_X_translate=0.0F;
	m_Y_translate=0.0F;

	DrawAxis();
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
	Invalidate(FALSE); //Erase only foreground
}  

void CMfcogl1View::OnLinePolygonMode() 
{
	// TODO: Add your command handler code here
		m_polygon_line_mode=TRUE;
		Invalidate(FALSE); //Erase only foreground
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
	if (m_auto_rotate) {//rotating now
		KillTimer(0);
		Invalidate(false);
	}
	else {//not rotating now
		SetTimer(0, 10, NULL);
	}
	m_auto_rotate=!m_auto_rotate;
	m_angle_increment=0.01*m_view_rotation_speed*360/60;
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
	if(m_auto_rotate)
	{
		m_auto_rotate_angle+=m_angle_increment;
	}
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
	}	
	Invalidate(FALSE);
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

//显示机器路径
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
//显示纤维路径
int CMfcogl1View::DisplaytheWindingProcess()
{
	 if (pDoc->m_bCanDisplayFiber && pDoc->m_isShowing == 1) {
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
			float a[3] = { (*iter).x,(*iter).y,(*iter).z };
			glVertex3fv(a);
			iter++;
		}
		glEnd();
	}
	return  0;
}
//路径仿真的按钮
void CMfcogl1View::OnDrawFiberPath() 
{
	ASSERT(pDoc!=NULL);
	if (pDoc->m_isShowing == 1) {
		return;
	}
	if(pDoc->m_bCanDisplayFiber)
	{
	//	CreateFiberPathList();
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
	glCallList(MANDREL_DISPLAY_LIST);
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
		SetTimer(2,elapse/10,NULL);
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
//方管的芯模
void CMfcogl1View::CreateTubeDisplayList(){
	GLfloat length_step = m_view_tube_length;
	int i = 0, flagx = 1, flagy = 1, degree, angle_step = 10;
	glNewList(MANDREL_DISPLAY_LIST, GL_COMPILE);

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
			glVertex3f(flagx * m_view_tube_a + m_view_tube_r * cos(degree * PI / 180), flagy * m_view_tube_b + m_view_tube_r * sin(degree * PI / 180), length_step * (i++ % 2));
			glVertex3f(flagx * m_view_tube_a + m_view_tube_r * cos(degree * PI / 180), flagy * m_view_tube_b + m_view_tube_r * sin(degree * PI / 180), length_step * (i++ % 2));
		}
		flagx = (degree >= 90 && degree < 270) ? -1 : 1;
		flagy = (degree >= 180 && degree < 360) ? -1 : 1;
		glNormal3f(cos(degree * PI / 180), sin(degree * PI / 180), 0.0f);
		glVertex3f(flagx * m_view_tube_a + m_view_tube_r * cos(degree * PI / 180), flagy * m_view_tube_b + m_view_tube_r * sin(degree * PI / 180), length_step * (i++ % 2));
		glVertex3f(flagx * m_view_tube_a + m_view_tube_r * cos(degree * PI / 180), flagy * m_view_tube_b + m_view_tube_r * sin(degree * PI / 180), length_step * (i++ % 2 ));
	}
	glEnd();
	glEndList();
}

/*added by LMK*/
//生成开口压力容器
void CMfcogl1View::OnCreateNewCylinderMandrel() {
	//初始化数值
	m_cview_display_winding_in_sequence = false;
	m_bCanDisplayPath = false;
	KillTimer(2);
	if (glIsList(FIBER_PATH_LIST) == GL_TRUE)
		glDeleteLists(FIBER_PATH_LIST, 1);

	CMfcogl1Doc* pDoc = GetDocument();
	pDoc->m_isShowing = 3;
	CCreateNewCylinderDlg new_cylinder_dlg;
	int choice = new_cylinder_dlg.DoModal();
	if (choice == IDOK) {
		pDoc->ResetWndDesign();
		m_view_cylinder_middle_length = new_cylinder_dlg.m_dlg_cylinder_middle_length;
		m_view_cylinder_middle_radius = new_cylinder_dlg.m_dlg_cylinder_middle_radius;
		m_view_cylinder_left_length = new_cylinder_dlg.m_dlg_cylinder_left_length;
	    m_view_cylinder_left_radius = new_cylinder_dlg.m_dlg_cylinder_left_radius;
		m_view_cylinder_right_length = new_cylinder_dlg.m_dlg_cylinder_right_length;
		m_view_cylinder_right_radius = new_cylinder_dlg.m_dlg_cylinder_right_radius;
	}
	CreateCylinderDisplayList();
	//to refresh screen
	Invalidate(false);
}
//计算开口压力容器的芯模 gl list输出
void CMfcogl1View::CreateCylinderDisplayList() {
	glNewList(MANDREL_DISPLAY_LIST, GL_COMPILE);

	//light and color setting.
	GLfloat matDiff[4] = { 0.4F, 0.6F, 0.6F, 1.00F };
	GLfloat matSpec[4] = { 0.1F, 0.1F, 0.1F, 1.00F };
	GLfloat matShine = 5.00F;

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, matShine);
	//compute the mandrel.
	glBegin(GL_TRIANGLE_STRIP);
	float left = 0,step = 1,r_step=10;
	for (; left < m_view_cylinder_left_length; left += step) {
		for (float r = 0; r <= 360; r+=r_step) {
			float x1 = left;
			float gx1 = sqrt(pow(m_view_cylinder_middle_radius, 2) - (pow(m_view_cylinder_middle_radius, 2) - pow(m_view_cylinder_left_radius, 2)) * pow(x1- m_view_cylinder_left_length, 2) / pow(m_view_cylinder_left_length, 2));
			float y1 = gx1 * sin(2 * PI * r / 360);
			float z1 = gx1 * cos(2 * PI * r / 360);

			float x2 = x1 + step;
			float gx2 = sqrt(pow(m_view_cylinder_middle_radius, 2) - (pow(m_view_cylinder_middle_radius, 2) - pow(m_view_cylinder_left_radius, 2)) * pow(x2- m_view_cylinder_left_length, 2) / pow(m_view_cylinder_left_length, 2));
			float y2 = gx2 * sin(2 * PI * r / 360);
			float z2 = gx2 * cos(2 * PI * r / 360);

			float nx = x1- m_view_cylinder_left_length, ny = y1, nz = z1;
			float normalize = sqrt(nx * nx + ny * ny + nz * nz);
			glNormal3f(nx / normalize, ny / normalize, nz / normalize);
			glVertex3f(x1, y1, z1);
			nx = x2 - m_view_cylinder_left_length, ny = y2, nz = z2;
			normalize = sqrt(nx * nx + ny * ny + nz * nz);
			glNormal3f(nx / normalize, ny / normalize, nz / normalize);
			glVertex3f(x2, y2, z2);
		}
	}
	float middle = m_view_cylinder_left_length; 
	for (; middle < m_view_cylinder_left_length+ m_view_cylinder_middle_length; middle += step) {
		for (float r = 0; r <= 360; r += r_step) {
			float x = middle;
			float y = m_view_cylinder_middle_radius * sin(2*PI * r / 360);
			float z = m_view_cylinder_middle_radius * cos(2*PI * r / 360);
			glNormal3f(0, sin(r * PI / 180), cos(r * PI / 180));
			glVertex3f(x, y, z);
			x += step;
			glNormal3f(0, sin(r * PI / 180), cos(r * PI / 180));
			glVertex3f(x, y, z);
		}
	}
	float right = m_view_cylinder_left_length+m_view_cylinder_middle_length;
	for (; right < m_view_cylinder_left_length+ m_view_cylinder_middle_length + m_view_cylinder_right_length; right += step) {
		for (float r = 0; r <= 360; r += r_step) {
			float x1 = right;
			float gx1 = sqrt(pow(m_view_cylinder_middle_radius, 2) - (pow(m_view_cylinder_middle_radius, 2) - pow(m_view_cylinder_right_radius, 2)) * pow(x1- m_view_cylinder_left_length -m_view_cylinder_middle_length, 2) / pow(m_view_cylinder_right_length, 2));
			float y1 = gx1 * sin(2 * PI * r / 360);
			float z1 = gx1 * cos(2 * PI * r / 360);

			float x2 = x1 + step;
			float gx2 = sqrt(pow(m_view_cylinder_middle_radius, 2) - (pow(m_view_cylinder_middle_radius, 2) - pow(m_view_cylinder_right_radius, 2)) * pow(x2 - m_view_cylinder_left_length - m_view_cylinder_middle_length, 2) / pow(m_view_cylinder_right_length, 2));
			float y2 = gx2 * sin(2 * PI * r / 360);
			float z2 = gx2 * cos(2 * PI * r / 360);

			float nx = x1 - m_view_cylinder_left_length- m_view_cylinder_middle_length, ny = y1, nz = z1;
			float normalize = sqrt(nx * nx + ny * ny + nz * nz);
			glNormal3f(nx / normalize, ny / normalize, nz / normalize);
			glVertex3f(x1, y1, z1);
			nx = x2 - m_view_cylinder_left_length- m_view_cylinder_middle_length, ny = y2, nz = z2;
			normalize = sqrt(nx * nx + ny * ny + nz * nz);
			glNormal3f(nx / normalize, ny / normalize, nz / normalize);
			glVertex3f(x2, y2, z2);
		}
	}
	
	glEnd();
	glEndList();
}

void CMfcogl1View::DrawAxis() {
	if (glIsList(GLOBAL_LIST) == GL_TRUE) { glDeleteLists(GLOBAL_LIST, 1); }
	glNewList(GLOBAL_LIST, GL_COMPILE);
	//light and color setting.
	GLfloat matDiff[4] = { 0.4F, 0.6F, 0.6F, 1.00F };
	GLfloat matSpec[4] = { 0.1F, 0.1F, 0.1F, 1.00F };
	GLfloat matShine = 5.00F;

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, matShine);

	glBegin(GL_LINES);
		GLfloat axisDiff[4] = { 1.0F, 0.0F, 0.0F, 1.00F };
		glNormal3f(1,0, 0);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, axisDiff);
		glVertex3f(0, 0, 0);
		glVertex3f(10000, 0,0);
		GLfloat axisDiff2[4] = { 0.0F, 1.0F, 0.0F, 1.00F };
		glNormal3f(0, 1, 0);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, axisDiff2);
		glVertex3f(0, 0, 0 );
		glVertex3f(0, 10000,0 );
		GLfloat axisDiff3[4] = { 0.0F, 0.0F, 1.0F, 1.00F };
		glNormal3f(0, 0, 1);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, axisDiff3);
		glVertex3f(0, 0, 0 );
		glVertex3f(0, 0, 10000);
	glEnd();
	glEndList();
}
