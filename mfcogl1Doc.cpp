// mfcogl1Doc.cpp : implementation of the CMfcogl1Doc class
//
#include "stdafx.h"
#include <process.h>
#include "mfcogl1.h"
#include "mfcogl1Doc.h"
#include "DlgFiberPathControls.h"
#include "mfcogl1View.h"

/*added by LMK*/
#include "DlgFiberPathControlsTube.h"
#include <deque>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define TEST_GEODESIC

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1Doc

IMPLEMENT_DYNCREATE(CMfcogl1Doc, CDocument)

BEGIN_MESSAGE_MAP(CMfcogl1Doc, CDocument)
	//{{AFX_MSG_MAP(CMfcogl1Doc)
	ON_COMMAND(IDM_FIBER_PATH_CONTROL_PARAMETERS, OnSwitchFiberPathControlDlg)
	ON_COMMAND(IDM_COMPUTE_FIBER_PATH, OnSwitchComputeFiberPath)//纤维路径 选择用那个算法
	ON_COMMAND(IDM_COMPUTE_PAYEYE_TRACK, OnSwitchComputePayeye)//机器路径 选择用那个算法
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1Doc construction/destruction

CMfcogl1Doc::CMfcogl1Doc()
{
	// TODO: add one-time construction code here

	m_WindingCount[0]=0;

	pPath=NULL;
	ptrack=NULL;
	psite=NULL;
	pPress=NULL;
	flag_add=NULL;
	phi_add=y_add=alpha_add=NULL;
	phi_turn=y_turn=alpha_turn=NULL;
	m_bCanComputing=m_bCanDisplayFiber=false;
	m_bPayeyeComplete=false;
	m_bUseLayer=data_compress=FALSE;
	m_bSavePath=m_bSavePayeye=m_bPressTest=FALSE;
	data_format=TRUE;
	lpOpenPathFile=lpOpenTrackFile=" ";
}

CMfcogl1Doc::~CMfcogl1Doc()
{
	if(pPath!=NULL)
	{
		delete []pPath;
		pPath=NULL;
	}
	if(psite!=NULL)
	{
		delete psite;
		psite=NULL;
	}
	if(pPress!=NULL)
	{
		delete []pPress;
		pPress=NULL;
	}
	if(ptrack!=NULL)
	{
		delete []ptrack;
		ptrack=NULL;
	}

	CMfcogl1Doc::CleanTubeMemory();
}

bool CMfcogl1Doc::IsEachPrime(int m1, int m2)
{	
	int r;
	r=m1%m2;
	while(r>1)
	{
		m1=m2;
		m2=r;
		r=m1%m2;
	}
	return (r==1)?true:false;
}


void CMfcogl1Doc::InitialAngle()
{ 
	  float c1;
	  c1=sqrt((m_sweep_radius-m_pipe_radius)/m_sweep_radius)*(m_sweep_radius-m_pipe_radius)/(m_sweep_radius+m_pipe_radius);
	  m_IniWndAngle=acos(c1)+0.12;
}

//应该是重置下参数之类的
void CMfcogl1Doc::ResetWndDesign()
{
	if(pPath!=NULL)
	{
		delete []pPath;
		pPath=NULL;
	}
	if(ptrack!=NULL)
	{
		delete []ptrack;
		ptrack=NULL;
	}
	friction_test=false;
	m_bCanComputing=false;
	m_bCanDisplayFiber=false;
	m_bPayeyeComplete=FALSE;
	m_bPressTest=FALSE;
	m_bSavePath=m_bSavePayeye=FALSE;
	lpOpenPathFile=lpOpenTrackFile=" ";

}

void CMfcogl1Doc::WriteTestData(unsigned short latitude, unsigned short long_elbow, unsigned short long_cylinder)
{
	CFileDialog fdlg(FALSE,"dat","press_test",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT,
		             "Pressure test file(*.dat)|*.dat|*.bmf|*.bmf|All file(*.*)|*.*||",NULL);
	if(fdlg.DoModal()==IDOK)
	{
		CFile f;
		CFileException fe;
	//	char *pFileName="press_test.dat";
		unsigned short i=2,stotal;
		float temp;
		f.Open(fdlg.GetPathName(),CFile::modeCreate|CFile::modeWrite,&fe);
		if(fe.m_cause==CFileException::diskFull)
		{
			AfxMessageBox(_T("Disk is full,please free some space and try again !"));
			return;
		}
		temp=PI-m_IniWndAngle;
		stotal=m_numlayer*latitude*(long_elbow+2*long_cylinder);
		f.Write(&i,sizeof(i));
		f.Write(&m_IniWndAngle,sizeof(float));
		f.Write(&temp,sizeof(float));
		f.Write(&pPress[(long_cylinder+2)*latitude].thickness_ave,sizeof(float));
		
		f.Write(&stotal,sizeof(stotal));
//		f.Write(&m_numlayer,sizeof(m_numlayer));
		f.Write(&latitude,sizeof(latitude));
		f.Write(&long_elbow,sizeof(long_elbow));
		f.Write(&long_cylinder,sizeof(long_cylinder));
			
		f.Write(&m_sweep_radius,sizeof(double));
		f.Write(&m_pipe_radius,sizeof(double));
		f.Write(&m_height,sizeof(double));
		f.Write(&m_span_angle,sizeof(double));

		for(i=0;i<stotal;i++)
		{
			f.Write(pPress+i,sizeof(Pressure));
		}
		f.Close();
	}
	delete []pPress;
	pPress=NULL;
	return;
}

void CMfcogl1Doc::SaveDataToDisk(LPCTSTR lpszFilter)
{
	char *pDefExt="wdf";
	if(!m_bSavePayeye&&m_bSavePath)
	{
		AfxMessageBox(_T("Path data has been saved, please use the \
\nsame name to save payeye track data !"));
		pDefExt="mdf";
	}
	if(!m_bSavePath&&m_bSavePayeye)AfxMessageBox(_T("Payeye track data has been saved, please \
use the \nsame name to save path data data !"));
	CFileDialog fdlg(FALSE,pDefExt,"path_data",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT,
		             lpszFilter,NULL);
	if(fdlg.DoModal()==IDOK)
	{
		CWaitCursor wait;
		CFile f;
		CFileException fe;
		unsigned long i;
		if(fdlg.GetFileExt()=="wdf")
		{
			f.Open(fdlg.GetPathName(),CFile::modeCreate|CFile::modeWrite,&fe);
			if(fe.m_cause==CFileException::diskFull)
			{
				AfxMessageBox(_T("Disk is full,please free some space and try again !"));
				return;
			}
	
			f.Write(m_WindingCount,sizeof(unsigned long));
			f.Write(&m_numlayer,sizeof(m_numlayer));
			f.Write(&m_sweep_radius,sizeof(m_sweep_radius));
			f.Write(&m_pipe_radius,sizeof(m_pipe_radius));
//			for(i=0;i<m_numlayer;i++)f.Write((cylinder_radius+i),sizeof(float));
			f.Write(&m_height,sizeof(m_height));
			f.Write(&m_span_angle,sizeof(m_span_angle));
			f.Write(&m_FilamentWidth,sizeof(float));
			f.Write(&circuit_step,sizeof(int));
//			For display parameter.
			f.Write(&m_IniWndAngle,sizeof(float));
			f.Write(&m_FilamentThickness,sizeof(float));
			f.Write(&m_friction,sizeof(float));
//			For Finite element analysis.
			f.Write(&m,sizeof(m));
			f.Write(&k,sizeof(k));
			f.Write(&nocycle,sizeof(nocycle));
			f.Write(&large,sizeof(large));
//			Save fiber path.
			for(i=0;i<=m_WindingCount[0];i++)f.Write((pPath+i),sizeof(PathPoint));
			f.Close();

			m_bSavePath=true;
		}
		else
		{
			if(fdlg.GetFileExt()=="mdf"||fdlg.GetFileExt()=="txt")
			{
					
				if(!data_format)
				{
					f.Open(fdlg.GetPathName(),CFile::modeCreate|CFile::modeWrite,&fe);
					if(fe.m_cause==CFileException::diskFull)
					{
						AfxMessageBox(_T("Disk is full,please free some space and try again !"));
						return;
					}
					
					f.Write(m_WindingCount,sizeof(unsigned long));
					for(i=0;i<=m_WindingCount[0];i++)f.Write((ptrack+i),sizeof(Track));
					f.Close();
				}
				else
				{
				char pTemp[80];
				short int j;
				float x,z,S_angle,P_angle,spanDeg;
				CStdioFile sf;
				spanDeg=6.0*PI/180.0;
				sf.Open(fdlg.GetPathName(),CFile::modeCreate|CFile::modeWrite);
				sprintf(pTemp,"The total number of trajectory is %u, and the height of carriage is %f.\n",\
m_WindingCount[0]+1,-11.8);
				sf.WriteString(pTemp);
				sprintf(pTemp,"      x  ,      z ,   SwingAngle, SpindleAngle\n");
				sf.WriteString(pTemp);
				if(!data_compress)
				{
				for(i=0;i<=m_WindingCount[0];i++)
				{
					x=ptrack[i].x*10.0;
					z=ptrack[i].z*10.0;
					P_angle=ptrack[i].swingAngle*180.0/PI;
					S_angle=-ptrack[i].spindleAngle*180.0/PI;
					sprintf(pTemp,"Y %f Z %f U %f X %f\n",x,z,P_angle,S_angle);
					sf.WriteString(pTemp);
					if(fabs(ptrack[i].z-ptrack[i-1].z)<tolerance)
					{
						j=i;
						while((ptrack[j+1].spindleAngle-ptrack[i].spindleAngle)<spanDeg)j++;
						if(j>i) i=j-1;
					}
				}
				}
				else
				{
				for(i=0;i<=m_WindingCount[0];i++)
				{
					x=ptrack[i].x*10.0;
					z=ptrack[i].z*10.0;
					P_angle=ptrack[i].swingAngle*180.0/PI;
					S_angle=-ptrack[i].spindleAngle*180.0/PI;
					sprintf(pTemp,"Y %f Z %f U %f X %f\n",x,z,P_angle,S_angle);
					sf.WriteString(pTemp);
					if(i<m_WindingCount[0])
					{
					j=i;
					while((ptrack[j+1].spindleAngle-ptrack[i].spindleAngle)<spanDeg)
					{
						j++;
						if(j>=m_WindingCount[0]) break;
					}
					if(j>i) i=j-1;
					}
				}
				}
				sf.Close();
				}
				m_bSavePayeye=true;
			}
			else AfxMessageBox(_T("Unknown file format !"));
		}
	}
}

void CMfcogl1Doc::ReadDataFromDisk(LPCTSTR lpFileName, CString lpFileExtName)
{
		
		if(lpFileExtName=="wdf")
		{
				ResetWndDesign();
				unsigned long i;
				CFile f;
				CFileException fe;
				if(!f.Open(lpFileName,CFile::modeRead,&fe))
				{
					AfxMessageBox("You input the wrong parameters, input again please!");
					return;
				}
				if(fe.m_cause==CFileException::diskFull)
				{
					AfxMessageBox(_T("Disk is full,please free some space and try again !"));
					return;
				}
				if(pPath!=NULL)
				{
					delete []pPath;
					pPath=NULL;
				}
				f.Read(m_WindingCount,sizeof(unsigned long));
				f.Read(&m_numlayer,sizeof(m_numlayer));
				f.Read(&m_sweep_radius,sizeof(m_sweep_radius));				
				f.Read(&m_pipe_radius,sizeof(m_pipe_radius));
//				for(i=0;i<m_numlayer;i++)f.Read((cylinder_radius+i),sizeof(float));
				m_pipe_radius=cylinder_radius[0];
				f.Read(&m_height,sizeof(m_height));
				f.Read(&m_span_angle,sizeof(m_span_angle));
				f.Read(&m_FilamentWidth,sizeof(float));
				f.Read(&circuit_step,sizeof(int));
//				For display parameter.
				f.Read(&m_IniWndAngle,sizeof(float));
				f.Read(&m_FilamentThickness,sizeof(float));
				f.Read(&m_friction,sizeof(float));
//				For Finite element analysis.	
				f.Read(&m,sizeof(m));
				f.Read(&k,sizeof(k));
				f.Read(&nocycle,sizeof(nocycle));
				f.Read(&large,sizeof(large));

//				Read fiber path.
				pPath=new PathPoint[m_WindingCount[0]+1];
				if(pPath==NULL)
				{
					AfxMessageBox(_T("Memory allocate failed, close other program and try again !"));
					return;
				}
				for(i=0;i<=m_WindingCount[0];i++)f.Read((pPath+i),sizeof(PathPoint));
				f.Close();
				m_bCanDisplayFiber=true;
				m_bSavePath=true;
				lpOpenPathFile=lpFileName;
				UpdateAllViews(NULL,0l,NULL);
		}
		else
		{
			if(lpFileExtName=="mdf")
			{
				if(m_bPayeyeComplete)
				{
					AfxMessageBox(_T("You are trying to open file incompatible with \
\npath data file !"));
					return;
				}
				if(lpOpenPathFile==" ")
				{
					AfxMessageBox(_T("Please compute payeye track by using menu !"));
					return;
				}
				CFile f;
				CFileException fe;
				unsigned long i;
				if(!f.Open(lpFileName,CFile::modeRead,&fe))
				{
					AfxMessageBox("You input the wrong parameters, input again please!");
					return;
				}
				if(fe.m_cause==CFileException::diskFull)
				{
					AfxMessageBox(_T("Disk is full,please free some space and try again !"));
					return;
				}
				if(ptrack!=NULL)
				{
					delete []ptrack;
					ptrack=NULL;
				}

				f.Read(m_WindingCount,sizeof(unsigned long));
				ptrack=new Track[m_WindingCount[0]+1];
				if(ptrack==NULL)
				{
					AfxMessageBox(_T("Memory allocate failed, close other program and try again !"));
					return;
				}
				for(i=0;i<=m_WindingCount[0];i++)f.Read((ptrack+i),sizeof(Track));
				f.Close();
				m_bPayeyeComplete=true;
				m_bSavePayeye=true;
				lpOpenTrackFile=lpFileName;
			}
			else AfxMessageBox(_T("Unknown file format !"));
		}
}

void CMfcogl1Doc::OnFileSave() 
{
	if(m_bCanDisplayFiber)
	{
		if(m_bPayeyeComplete)
		{
			if(m_bSavePath)
			{
				if(m_bSavePayeye)AfxMessageBox(_T("There is no new data to be saved !"));
				else SaveDataToDisk("Payeye track data file(*.mdf)|*.mdf|\
Text data file for machine(*.txt)|*.txt||");
			}
			else 
			{
				if(m_bSavePayeye)SaveDataToDisk("Winding path data file(*.wdf)|*.wdf||");
				else SaveDataToDisk("Winding path data file(*.wdf)|*.wdf|Payeye track data \
file(*.mdf)|*.mdf||");
			}
		}
		else
		{
			if(m_bSavePath)AfxMessageBox(_T("There's no new data to be saved !"));
			else SaveDataToDisk("Winding path data file(*.wdf)|*.wdf||");
		}
	}
	else AfxMessageBox(_T("There's no new data to be saved !"));
	// TODO: Add your command handler code here
	
}

void CMfcogl1Doc::OnFileOpen() 
{
	if((!m_bSavePath&&m_bCanDisplayFiber)||(!m_bSavePayeye&&m_bPayeyeComplete))
	{

		CHintDialog hdlg;
		if(hdlg.DoModal()==IDOK)return;
		else ResetWndDesign();
	}
//	ReadDataFromDisk();
	// TODO: Add your command handler code here
	
}

/*added by LMK*/
//Since now we have two mandrels(elbow,tube),choose right dialog to get parameters.
void CMfcogl1Doc::OnSwitchFiberPathControlDlg() {
	if (m_isShowing == 1) {//tube
		OnOpenFiberPathControlTubeParametersDlg();
	}
	else if(m_isShowing == 2) {//elbow
		//OnOpenFiberPathControlParametersDlg();
	}
}

void CMfcogl1Doc::OnOpenFiberPathControlTubeParametersDlg() {

	CDlgFiberPathControlsTube fpTubeDlg;
	if (IDOK == fpTubeDlg.DoModal()) {
		ResetWndDesign();
		m_doc_tube_winding_angle = fpTubeDlg.m_dlg_tube_winding_angle * PI / 180;
		m_doc_tube_band_width = fpTubeDlg.m_dlg_tube_band_width;
		m_doc_tube_cut_num = fpTubeDlg.m_dlg_tube_cut_num;

		payeye.mandrel_redundancy = fpTubeDlg.m_dlg_tube_mandrel_redundancy;
		payeye.mandrel_speed = fpTubeDlg.m_dlg_tube_mandrel_speed;
		payeye.pm_distance = fpTubeDlg.m_dlg_tube_pm_distance;
		payeye.pm_left_distance = fpTubeDlg.m_dlg_tube_pm_left_distance;

		m_bCanComputing = true;
		angleStep = (payeye.mandrel_speed * 2 * PI) / 10;// 1/10秒 转过多少角度

		CMfcogl1View* pView;
		POSITION pos = GetFirstViewPosition();
		pView = (CMfcogl1View*)GetNextView(pos);

		model.a = pView->m_view_tube_a;
		model.b = pView->m_view_tube_b;
		model.length = pView->m_view_tube_length;
		model.r = pView->m_view_tube_r;
		model.angle = m_doc_tube_winding_angle;
		model.width = m_doc_tube_band_width;

		//计算合适的切点数(缠绕一周出发点的总量)，跳跃数(两次出发点序号的差值)
		float round = 2 * PI * model.r + 4 * model.a + 4 * model.b;//周长
		float real_width = model.width / abs(cos(model.angle));//实际切面宽
		M0 = ceil(round / real_width);//实际剖分段数
		interval = round / M0;//剖分步长
		cutNum = m_doc_tube_cut_num;

		while ((M0 + 1) % cutNum != 0) {//缩小纤维半径以满足切点数约束
			model.width -= 0.05;
			real_width = model.width / abs(cos(model.angle));
			M0 = ceil(round / real_width);
			interval = round / M0;
		}
		jumpNum = (M0 + 1) / cutNum;

		CString STemp;
		STemp.Format(_T("纱宽(修正) = %.3f mm\n等分数=%d\n缠绕角 = %.3f degree\n切点数=%d\n跳跃数=%d"), real_width, M0, fpTubeDlg.m_dlg_tube_winding_angle, cutNum, jumpNum);
		AfxMessageBox(STemp);
		if (jumpNum == -1) {
			OnOpenFiberPathControlTubeParametersDlg();
		}
	}
}
//纤维束
void CMfcogl1Doc::OnSwitchComputeFiberPath() {
	if (m_isShowing == 1) {//tube
		OnComputeFiberPathTube();
	}
	else if (m_isShowing == 3) {//cylinder
		//cylidner is doing
		debug_show("cylinder is doing...");
	}
}
void CMfcogl1Doc::OnSwitchComputePayeye(){
	if(m_isShowing == 1){//tube
		OnComputePayeyeTube();
	}
	if(m_isShowing == 3){
		debug_show("cylinder is doing...");
	}
}

void CMfcogl1Doc::CleanTubeMemory() {
	if (TubePointList != NULL) {
		delete TubePointList;
		TubePointList = NULL;
	}
	if (TubeTrackListTime != NULL) {
		delete TubeTrackListTime;
		TubeTrackListTime = NULL;
	}
	if (TubePointListTime != NULL) {
		delete TubePointListTime;
		TubePointListTime = NULL;
	}
	if (distanceE != NULL) {
		delete distanceE;
		distanceE = NULL;
	}
	TubeStartList.clear();
}

//生成起始点列表 在两个头上各绕一圈
int CMfcogl1Doc::OnGenerateTubeStartList(std::deque<struct position>& TubeStartList) {
	float halfInterval = interval / 2;
	int i = 0;
	struct position p;
	for (i = 0; i * interval < model.a; i++) {
		p.x = i * interval;
		p.y = model.b + model.r;
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeStartList.push_back(p);
		p.z = model.length;
		p.direction = -1;
		TubeStartList.push_back(p);
	}
	for (; i * interval < model.a + PI * model.r / 2; i++) {
		p.x = model.a + model.r * sin((i * interval - model.a) / model.r);
		p.y = model.b + model.r * cos((i * interval - model.a) / model.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeStartList.push_back(p);
		p.z = model.length;
		p.direction = -1;
		TubeStartList.push_back(p);
	}
	for (; i * interval < model.a + PI * model.r / 2 + 2 * model.b; i++) {
		p.x = model.a + model.r;
		p.y = model.b - (i * interval - model.a - PI * model.r / 2);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeStartList.push_back(p);
		p.z = model.length;
		p.direction = -1;
		TubeStartList.push_back(p);
	}
	for (; i * interval < model.a + PI * model.r + 2 * model.b; i++) {
		p.x = model.a + model.r * cos((i * interval - (model.a + PI * model.r / 2 + 2 * model.b)) / model.r);
		p.y = -model.b - model.r * sin((i * interval - (model.a + PI * model.r / 2 + 2 * model.b)) / model.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeStartList.push_back(p);
		p.z = model.length;
		p.direction = -1;
		TubeStartList.push_back(p);
	}
	for (; i * interval < 3 * model.a + PI * model.r + 2 * model.b; i++) {
		p.x = 2 * model.a - i * interval + PI * model.r + 2 * model.b;
		p.y = -model.b - model.r;
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeStartList.push_back(p);
		p.z = model.length;
		p.direction = -1;
		TubeStartList.push_back(p);
	}
	for (; i * interval < 3 * model.a + 1.5 * PI * model.r + 2 * model.b; i++) {
		p.x = -model.a - model.r * sin((i * interval - (3 * model.a + PI * model.r + 2 * model.b)) / model.r);
		p.y = -model.b - model.r * cos((i * interval - (3 * model.a + PI * model.r + 2 * model.b)) / model.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeStartList.push_back(p);
		p.z = model.length;
		p.direction = -1;
		TubeStartList.push_back(p);
	}
	for (; i * interval < 3 * model.a + 1.5 * PI * model.r + 4 * model.b; i++) {
		p.x = -model.a - model.r;
		p.y = -model.b + i * interval - 3 * model.a - 1.5 * PI * model.r - 2 * model.b;
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeStartList.push_back(p);
		p.z = model.length;
		p.direction = -1;
		TubeStartList.push_back(p);
	}
	for (; i * interval < 3 * model.a + 2 * PI * model.r + 4 * model.b; i++) {
		p.x = -model.a - model.r * cos((i * interval - (3 * model.a + 1.5 * PI * model.r + 4 * model.b)) / model.r);
		p.y = model.b + model.r * sin((i * interval - (3 * model.a + 1.5 * PI * model.r + 4 * model.b)) / model.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeStartList.push_back(p);
		p.z = model.length;
		p.direction = -1;
		TubeStartList.push_back(p);
	}
	for (; i * interval < 4 * model.a + 2 * PI * model.r + 4 * model.b - tiny; i++) {
		p.x = i * interval - (4 * model.a + 2 * PI * model.r + 4 * model.b);
		p.y = model.b + model.r;
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeStartList.push_back(p);
		p.z = model.length;
		p.direction = -1;
		TubeStartList.push_back(p);
	}

	return TubeStartList.size();
}

//方管纤维束
void CMfcogl1Doc::OnComputeFiberPathTube() {
	CMfcogl1Doc::CleanTubeMemory();
	if (m_bCanComputing == true)
	{
		/* algorithm:
			step1:according to origin angle and width, and model, draw a path from origin place to edge
			step2:according to algebraic pattern(M,	), decide which edge point to wind back
			step3:keep the winding angle, wind to another edge,back to step2,until winded M time.
		*/
		TubePointList = new std::deque<struct TubePoint>;
		TubeTrackListTime = new std::deque<struct Track>;
		TubePointListTime = new std::deque<struct TubePoint>;
		//debug
		distanceE = new std::deque<float>;

		windingPathStep = 0.05;//计算的步长
		currentAngle = 0;
		TubeCurrentStart[0] = TubeCurrentStart[1] = -1;

		//generate GL list
		glNewList(FIBER_PATH_LIST, GL_COMPILE); //FIBER_PATH_LIST     2

		GLfloat matAmb[4] = { 0.8F, 0.5F, 0.3F, 1.00F };
		GLfloat matDiff[4] = { 0.8F, 0.5F, 0.3F, 0.80F };
		GLfloat matSpec[4] = { 0.30F, 0.30F, 0.30F, 0.30F };
		GLfloat matShine = 60.00F;
		glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
		glMaterialf(GL_FRONT, GL_SHININESS, matShine);

		//生成所有路径的起点
		int totalNum = OnGenerateTubeStartList(TubeStartList);
		//计算循环
		while (TubeStartList.size() != 0  ) {
			
			OnGeneratePosition(TubeStartList);
			OnRenderSinglePath();//计算本纤维路径的所有顶点位置并输入glList和TubePointList
			OnComputeStartAngle();
			OnComputePayeyeTube();			//计算一条纤维带

			AfxMessageBox("Payeye Track Computation Finished.");

		}
		glEndList();
		m_WindingCount[0] = TubePointList->size();
		AfxMessageBox("纤维路径计算完毕\ntube Track Computation Finished.");
 		m_bCanDisplayFiber = true;

		CMfcogl1View* pView;
		POSITION pos = GetFirstViewPosition();
		pView = (CMfcogl1View*)GetNextView(pos);
		pView->m_cview_enable_tape_display = true;
		pView->Invalidate(false);
	}
	else{
		AfxMessageBox(_T("请先设置纤维参数\nPlease setup parameter first."));
	}
}

//根据当前位置和跳跃数算出本条路径的起点
void CMfcogl1Doc::OnGeneratePosition(std::deque<struct position>&TubeStartList) {

	vector trackStart = { cos(-currentAngle), sin(-currentAngle),0 };

	float angle = -1;
	float cross, localAngle;
	auto tmpPosition = TubeStartList.begin();
	int currentStart = (currentPosition.direction == -1) ?  TubeCurrentStart[0]: TubeCurrentStart[1];

	if (currentStart != -1) {
		auto i = TubeStartList.begin();
		for (; i < TubeStartList.end(); i++) {//遍历 找到下一个合适的节点 
			if (currentPosition.direction != (*i).direction && (*i).index == (currentStart + jumpNum) % M0) {
				tmpPosition = i;
				break;
			}
		}
		if (i == TubeStartList.end()) {
			CString STemp;
			STemp.Format("错误：OnGeneratePosition 找不到合适的节点 ");
			AfxMessageBox(STemp);
		}
	}

	Track lastTrack;
	lastTrack.x = model.a + model.r + payeye.pm_distance;
	lastTrack.z = (*tmpPosition).direction > 0 ? 0 : model.length;
	lastTrack.spindleAngle = currentAngle + acos(angle);
	lastTrack.swingAngle = (*tmpPosition).direction > 0 ? -PI / 2 : PI / 2;


	currentPosition = *tmpPosition;

	if (currentPosition.direction ==1) {
		TubeCurrentStart[0] = (*tmpPosition).index;
	}
	else {
		TubeCurrentStart[1]=(*tmpPosition).index;
	}
	TubeStartList.erase(tmpPosition);
}

//算出单根纤维路径
void CMfcogl1Doc::OnRenderSinglePath() {
	glBegin(GL_LINE_STRIP);
	//initial OpenGL
	//判断当前position处于芯模的哪个面，从而选中合适的入口。
	//类似于状态机，本质上是对两个OnRender不同状态的调用
	//缠绕到面的边缘时，会在OnRender中递归调用下一个面的OnRender，直到缠绕到芯模边缘退出。
	if (-model.a <= currentPosition.x && currentPosition.x < model.a && currentPosition.y>0) {
		OnRenderLinePart(1);
	}
	else if (model.a <= currentPosition.x && currentPosition.x < model.a + model.r && currentPosition.y > 0) {
		OnRenderCurvePart(2);
	}
	else if (-model.b < currentPosition.y && currentPosition.y <= model.b && currentPosition.x>0) {
		OnRenderLinePart(3);
	}
	else if (-model.b - model.r < currentPosition.y && currentPosition.y <= -model.b && currentPosition.x>0) {
		OnRenderCurvePart(4);
	}
	else if (-model.a < currentPosition.x && currentPosition.x <= model.a && currentPosition.y < 0) {
		OnRenderLinePart(5);
	}
	else if (-model.a - model.r < currentPosition.x && currentPosition.x <= -model.a && currentPosition.y < 0) {
		OnRenderCurvePart(6);
	}
	else if (-model.b <= currentPosition.y && currentPosition.y < model.b && currentPosition.x < 0) {
		OnRenderLinePart(7);
	}
	else if (-model.a - model.r <= currentPosition.x && currentPosition.x < -model.a && currentPosition.y >0) {
		OnRenderCurvePart(8);
	}
	glEnd();
}

void CMfcogl1Doc::updatePosition(float* nextPoint) {
	currentPosition.x = nextPoint[0];	currentPosition.y = nextPoint[1]; currentPosition.z = nextPoint[2];
}

int CMfcogl1Doc::outTubeEdge() {
	return (currentPosition.z < 0) ? -1 : (currentPosition.z >= model.length ? 1 : 0);
}

void CMfcogl1Doc::insertPoint(float* nextPoint) {
	glVertex3fv(nextPoint);
}

int CMfcogl1Doc::OnRenderLinePart(int state) {
	float nextPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	float startPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	model.stepLength = 0;
	int direction = currentPosition.direction;
	switch (state) {
	case 1:

		//glNormal3f(cos((360-2*segmentw)*3.14/180),sin((360-2*segmentw)*3.14/180),0.0f);		

		while (nextPoint[0] < model.a && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			insertPoint(nextPoint);

			nextPoint[0] = startPoint[0] + model.stepLength * sin(model.angle);
			nextPoint[1] = startPoint[1];
			nextPoint[2] = startPoint[2] + model.stepLength * cos(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal_radian = PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			tempTubePoint.direction = currentPosition.direction;
			tempTubePoint.index = currentPosition.index;
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}

		break;
	case 3:

		//glNormal3f(cos((360-2*segmentw)*3.14/180),sin((360-2*segmentw)*3.14/180),0.0f);		

		while (-model.b < nextPoint[1] && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			insertPoint(nextPoint);
			nextPoint[0] = startPoint[0];
			nextPoint[1] = startPoint[1] - model.stepLength * sin(model.angle);
			nextPoint[2] = startPoint[2] + model.stepLength * cos(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal_radian = 0;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			tempTubePoint.direction = currentPosition.direction;
			tempTubePoint.index = currentPosition.index;
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}

		break;
	case 5:

		//glNormal3f(cos((360-2*segmentw)*3.14/180),sin((360-2*segmentw)*3.14/180),0.0f);		

		while (-model.a < nextPoint[0] && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			insertPoint(nextPoint);

			nextPoint[0] = startPoint[0] - model.stepLength * sin(model.angle);
			nextPoint[1] = startPoint[1];
			nextPoint[2] = startPoint[2] + model.stepLength * cos(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal_radian = -PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			tempTubePoint.direction = currentPosition.direction;
			tempTubePoint.index = currentPosition.index;
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}

		break;
	case 7:

		//glNormal3f(cos((360-2*segmentw)*3.14/180),sin((360-2*segmentw)*3.14/180),0.0f);		

		while (nextPoint[1] < model.b && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			insertPoint(nextPoint);

			nextPoint[0] = startPoint[0];
			nextPoint[1] = startPoint[1] + model.stepLength * sin(model.angle);
			nextPoint[2] = startPoint[2] + model.stepLength * cos(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal_radian = -PI;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			tempTubePoint.direction = currentPosition.direction;
			tempTubePoint.index = currentPosition.index;
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}

		break;
	default:
		break;
	}

	updatePosition(nextPoint);
	if (outTubeEdge() == 0) {//from one part move into another neighbor part.   some how like a LSM.
		OnRenderCurvePart(state + 1);
	}
	else {
		return outTubeEdge();
	}
}

int CMfcogl1Doc::OnRenderCurvePart(int state) {
	float startPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	float nextPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	float phase = 0;
	model.stepLength = 0;
	int direction = currentPosition.direction;
	//此时，为了便于计算，将stepLength定义为在该分面经过的弧度（而非曲线弧长）。为了保持曲线弧长增量不变，需要计算其在本分面上对应Δ弧度。
	float adjust = model.r / sin(model.angle);
	switch (state) {
	case 2:
		phase = atan((currentPosition.x - model.a) / (currentPosition.y - model.b));
		model.stepLength = phase;
		startPoint[0] = model.a;
		startPoint[1] = model.b;

		//圆柱面上测地线 对弧面的三条边约束
		while (model.stepLength <= PI / 2 && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			insertPoint(nextPoint);

			//0:横向（2a）1:高（2b）2:纵向（C）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			nextPoint[0] = startPoint[0] + model.r * sin(model.stepLength);
			nextPoint[1] = startPoint[1] + model.r * cos(model.stepLength);
			nextPoint[2] = startPoint[2] + model.r * (model.stepLength - phase) / tan(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal_radian = -model.stepLength + PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.z = cos(model.angle) * direction;
			tempTubePoint.direction = currentPosition.direction;
			tempTubePoint.index = currentPosition.index;
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep/adjust;
		}

		break;
	case 4:
		phase = PI / 2 + atan((-model.b - currentPosition.y) / (currentPosition.x - model.a));
		model.stepLength = phase;
		startPoint[0] = model.a;
		startPoint[1] = -model.b;

		//圆柱面上测地线
		while (model.stepLength <= PI && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			insertPoint(nextPoint);

			//0:横向（2a）1:高（2b）2:纵向（C）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			nextPoint[0] = startPoint[0] + model.r * sin(model.stepLength);
			nextPoint[1] = startPoint[1] + model.r * cos(model.stepLength);
			nextPoint[2] = startPoint[2] + model.r * (model.stepLength - phase) / tan(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal_radian = -model.stepLength + PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			tempTubePoint.direction = currentPosition.direction;
			tempTubePoint.index = currentPosition.index;
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep/adjust;
		}

		break;
	case 6:
		phase = PI + atan((-currentPosition.x - model.a) / (-currentPosition.y - model.b));
		model.stepLength = phase;
		startPoint[0] = -model.a;
		startPoint[1] = -model.b;

		//圆柱面上测地线
		while (model.stepLength <= 1.5 * PI && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			insertPoint(nextPoint);

			//0:横向（2a）1:高（2b）2:纵向（C）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			nextPoint[0] = startPoint[0] + model.r * sin(model.stepLength);
			nextPoint[1] = startPoint[1] + model.r * cos(model.stepLength);
			nextPoint[2] = startPoint[2] + model.r * (model.stepLength - phase) / tan(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal_radian = -model.stepLength + PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			tempTubePoint.direction = currentPosition.direction;
			tempTubePoint.index = currentPosition.index;
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep/adjust;
		}

		break;
	case 8:
		phase = 1.5 * PI + atan((currentPosition.y - model.b) / (-currentPosition.x - model.a));
		model.stepLength = phase;
		startPoint[0] = -model.a;
		startPoint[1] = model.b;

		//圆柱面上测地线
		while (model.stepLength <= 2 * PI && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			//0:横向（2a）1:高（2b）2:纵向（c）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			insertPoint(nextPoint);

			nextPoint[0] = startPoint[0] + model.r * sin(model.stepLength);
			nextPoint[1] = startPoint[1] + model.r * cos(model.stepLength);
			nextPoint[2] = startPoint[2] + model.r * (model.stepLength - phase) / tan(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal_radian = -model.stepLength + PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal_radian - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			tempTubePoint.direction = currentPosition.direction;
			tempTubePoint.index = currentPosition.index;
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep/adjust;
		}

		break;
	default:
		break;
	}

	updatePosition(nextPoint);
	if (outTubeEdge() == 0) {
		OnRenderLinePart(state % 8 + 1);
	}
	else {
		return outTubeEdge();
	}
}

void CMfcogl1Doc::OnComputeStartAngle() {
	auto i = *TubePointList->begin();
	float x, y;
	if (abs(i.tangent.x) < 0.001) {
		x = i.x;
		float R = model.a + model.r + payeye.pm_distance;
		y = sqrt(R * R - x * x);
		if (i.tangent.y < 0) y = -y;
	}
	else if (abs(i.tangent.y) < 0.001) {
		y = i.y;
		float R = model.a + model.r + payeye.pm_distance;
		x = sqrt(R * R - y * y);
		if (i.tangent.x < 0)x = -x;
	}
	else {
		float A = i.tangent.y / i.tangent.x;
		float B = i.y - A * i.x;
		float R = model.a + model.r + payeye.pm_distance;
		float delta = sqrt(A * A * B * B - (A * A + 1) * (B * B - R * R));
		float x1 = (-A * B + delta) / (A * A + 1);
		float x2 = (-A * B - delta) / (A * A + 1);
		float t1 = (x1 - i.x) / (i.tangent.x);
		float t2 = (x2 - i.x) / (i.tangent.x);
		float t = max(t1, t2);
		x = i.x + i.tangent.x * t;
		y = i.y + i.tangent.y * t;
	}
	while (x * sin(-currentAngle) - cos(-currentAngle) * y >= 0) {
		currentAngle += angleStep;
	}
}

/*
条件1.已取得纤维路径 2.芯模以一定角速度旋转
step1.通过对路径坐标矩阵旋转找到实际坐标和导数
step2.通过射线相交找到对应的实际缠绕点和吐丝嘴坐标，转角
step3.芯模旋转角度更新，转step1
*/
void CMfcogl1Doc::OnComputePayeyeTube() {
	//初始化

	auto tmpPoint = TubePointList->begin();
	while (tmpPoint < TubePointList->end()) {
		auto before = tmpPoint;
		tmpPoint = updateTubeTrackListTime(tmpPoint);//计算本纤维带上的一个点，结果给TubeTrackListTime和TubePointListTime存数据
		currentAngle += angleStep;
	}

	//测试输出
	//glNewList(FIBER_TRACK_LIST, GL_COMPILE);

	//GLfloat matAmb[4] = { 1.0F, 0.0F, 0.0F, 1.00F };
	//GLfloat matDiff[4] = { 1.0F, 0.0F, 0.0F, 0.80F };
	//GLfloat matSpec[4] = { 1.0F, 0.0F, 0.0F, 0.30F };
	//GLfloat matShine = 60.00F;

	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	//glMaterialf(GL_FRONT, GL_SHININESS, matShine);
	//glPointSize(3);
	glBegin(GL_LINES);
	//把缠绕点序列 以射线形式画出来
	for (auto i = TubePointListTime->begin(); i < TubePointListTime->end(); i++) {//从TubePointListTime取数据
		float a[3] = { (*i).x,(*i).y,(*i).z };
		float b[3] = { (*i).x + (*i).tangent.x * (*i).tPoint,(*i).y + (*i).tangent.y * (*i).tPoint,(*i).z + (*i).tangent.z * (*i).tPoint };
		GLfloat matAmb[4] = { 0.0F, 1.0F, 0.8F, 1.00F };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmb);
		glVertex3fv(a);
		glVertex3fv(b);
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	//吐丝嘴轨道
	for (auto j = TubeTrackListTime->begin(); j < TubeTrackListTime->end(); j++) {
		float pointInPayeyeTrack[3] = { (model.a + model.r + payeye.pm_distance) * cos(-(*j).spindleAngle), (model.a + model.r + payeye.pm_distance) * sin(-(*j).spindleAngle),0 };
		//float a[3] = { pointInPayeyeTrack[0],pointInPayeyeTrack[1],(*j).z -20 };
		float b[3] = { pointInPayeyeTrack[0],pointInPayeyeTrack[1],(*j).z };
		GLfloat matAmb[4] = { 1.0F, 0.0F, 0.0F, 1.00F };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmb);
		//glVertex3fv(a);
		glVertex3fv(b);
	}
	glEnd();
	//glEndList();

	CMfcogl1View* pView;
	POSITION pos = GetFirstViewPosition();
	pView = (CMfcogl1View*)GetNextView(pos);
	pView->Invalidate(false);


}

std::deque<TubePoint>::iterator CMfcogl1Doc::updateTubeTrackListTime(std::deque<TubePoint>::iterator currentPoint) {
	std::deque<TubePoint>::iterator startPoint = currentPoint;
	vector trackStart = { (model.a + model.r + payeye.pm_distance) * cos(-currentAngle), (model.a + model.r + payeye.pm_distance) * sin(-currentAngle),0 };//相对运动 所以是-
	vector trackDirection = { 0,0,1 };
	struct bestPoint bestPoint = { INFINITY };
	float round = 4 * model.a + 4 * model.b + 2 * PI * model.r;
	while (currentPoint < TubePointList->end() && abs((*currentPoint).z - (*startPoint).z) < (0.5 * round / (tan(model.angle)))) {
		//track = y1 = d1*t1+p1  point = y2 = d2*t2+p2  
		vector crossTrackPoint = { -(*currentPoint).tangent.y,(*currentPoint).tangent.x,0 };//track * point  d1 * d2
		vector trackToPoint = { (*currentPoint).x - trackStart.x,(*currentPoint).y - trackStart.y ,(*currentPoint).z - trackStart.z };//point - track    p2 - p1
		float lengthCrossTrackPoint = sqrt(crossTrackPoint.x * crossTrackPoint.x + crossTrackPoint.y * crossTrackPoint.y + crossTrackPoint.z * crossTrackPoint.z);
		float distance = abs((trackToPoint.x * crossTrackPoint.x + trackToPoint.y * crossTrackPoint.y + trackToPoint.z * crossTrackPoint.z) / lengthCrossTrackPoint);
		if (distance <= bestPoint.distance + 0.01) {
			vector crossTrackToPointTrack = { trackToPoint.y,-trackToPoint.x,0 };//TrackToPoint * track
			float tPoint = (crossTrackToPointTrack.x * crossTrackPoint.x + crossTrackToPointTrack.y * crossTrackPoint.y + crossTrackToPointTrack.z * crossTrackPoint.z) / (lengthCrossTrackPoint * lengthCrossTrackPoint);
			if (tPoint > 0) {//else:虽然比inf小，但是因为射线方向不对，所以是不会更新的，所以最后输出的还是inf
				bestPoint.pointNum = currentPoint;
				(*bestPoint.pointNum).tPoint = tPoint;
				bestPoint.distance = distance;
				vector crossTrackToPointPoint = {
					trackToPoint.y * (*currentPoint).tangent.z - trackToPoint.z * (*currentPoint).tangent.y,
					trackToPoint.z * (*currentPoint).tangent.x - trackToPoint.x * (*currentPoint).tangent.z,
					trackToPoint.x * (*currentPoint).tangent.y - trackToPoint.y * (*currentPoint).tangent.x
				};
				float tTrack = (crossTrackToPointPoint.x * crossTrackPoint.x + crossTrackToPointPoint.y * crossTrackPoint.y + crossTrackToPointPoint.z * crossTrackPoint.z) / (lengthCrossTrackPoint * lengthCrossTrackPoint);
				bestPoint.track.x = model.a + model.r + payeye.pm_distance;
				bestPoint.track.z = trackStart.z + tTrack;
				bestPoint.track.spindleAngle = currentAngle;
				bestPoint.track.swingAngle = atan((*currentPoint).tangent.y / (*currentPoint).tangent.z);
			}
		}
		currentPoint++;
	}
	if (bestPoint.distance > 2) {
		testStop = 0;
	}
	if (bestPoint.distance > 1) {
		distanceE->push_back(bestPoint.distance);
	}

	TubeTrackListTime->push_back(bestPoint.track);
	TubePointListTime->push_back(*bestPoint.pointNum);//iterator所指的tubepoint
	return ++bestPoint.pointNum;


}

