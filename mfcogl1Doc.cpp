// mfcogl1Doc.cpp : implementation of the CMfcogl1Doc class
//
/*
TODO：
1.add new path(1.different color along length 2. different color in different path(geo,non-geo,equal)).
2.able to calculate the complete path at least in equal.
*/
#include "stdafx.h"
#include <process.h>
#include "mfcogl1.h"
#include "mfcogl1Doc.h"
#include "DlgFiberPathControls.h"
#include "mfcogl1View.h"

/*added by LMK*/
#include "DlgFiberPathControlsTube.h"
#include "DlgFiberPathControlsCylinder.h"
#include <deque>
#include <vector>
#include <string>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define d_curve 0.1
//#define TEST_GEODESIC

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1Doc

IMPLEMENT_DYNCREATE(CMfcogl1Doc, CDocument)

BEGIN_MESSAGE_MAP(CMfcogl1Doc, CDocument)
	//{{AFX_MSG_MAP(CMfcogl1Doc)
	ON_COMMAND(IDM_FIBER_PATH_CONTROL_PARAMETERS, OnSwitchFiberPathControlDlg)//参数设置
	ON_COMMAND(IDM_COMPUTE_FIBER_PATH, OnSwitchComputeFiberPath)//纤维路径计算
	ON_COMMAND(IDM_COMPUTE_PAYEYE_TRACK, OnSwitchComputePayeye)//机器路径计算
	ON_COMMAND(IDM_SAVE_TRACK, OnSwitchSaveTrack)//输出机器路径
	//ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1Doc construction/destruction

CMfcogl1Doc::CMfcogl1Doc()
{
	// TODO: add one-time construction code here

	m_WindingCount[0]=0;
	psite=NULL;
	m_bCanComputing=m_bFiberPathComplete=false;
	m_bPayeyeComplete=false;
	m_bUseLayer=FALSE;

}

CMfcogl1Doc::~CMfcogl1Doc()
{

	if(psite!=NULL)
	{
		delete psite;
		psite=NULL;
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


//void CMfcogl1Doc::InitialAngle()
//{ 
//	  float c1;
//	  c1=sqrt((m_sweep_radius-m_pipe_radius)/m_sweep_radius)*(m_sweep_radius-m_pipe_radius)/(m_sweep_radius+m_pipe_radius);
//	  m_IniWndAngle=acos(c1)+0.12;
//}

//状态机归零
void CMfcogl1Doc::ResetWndDesign()
{
	m_bCanComputing=false;
	m_bFiberPathComplete=false;
	m_bPayeyeComplete=false;
}

//void CMfcogl1Doc::WriteTestData(unsigned short latitude, unsigned short long_elbow, unsigned short long_cylinder)
//{
//	CFileDialog fdlg(FALSE,"dat","press_test",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT,
//		             "Pressure test file(*.dat)|*.dat|*.bmf|*.bmf|All file(*.*)|*.*||",NULL);
//	if(fdlg.DoModal()==IDOK)
//	{
//		CFile f;
//		CFileException fe;
//	//	char *pFileName="press_test.dat";
//		unsigned short i=2,stotal;
//		float temp;
//		f.Open(fdlg.GetPathName(),CFile::modeCreate|CFile::modeWrite,&fe);
//		if(fe.m_cause==CFileException::diskFull)
//		{
//			AfxMessageBox(_T("Disk is full,please free some space and try again !"));
//			return;
//		}
//		temp=PI-m_IniWndAngle;
//		stotal=m_numlayer*latitude*(long_elbow+2*long_cylinder);
//		f.Write(&i,sizeof(i));
//		f.Write(&m_IniWndAngle,sizeof(float));
//		f.Write(&temp,sizeof(float));
//		f.Write(&pPress[(long_cylinder+2)*latitude].thickness_ave,sizeof(float));
//		
//		f.Write(&stotal,sizeof(stotal));
////		f.Write(&m_numlayer,sizeof(m_numlayer));
//		f.Write(&latitude,sizeof(latitude));
//		f.Write(&long_elbow,sizeof(long_elbow));
//		f.Write(&long_cylinder,sizeof(long_cylinder));
//			
//		f.Write(&m_sweep_radius,sizeof(float));
//		f.Write(&m_pipe_radius,sizeof(float));
//		f.Write(&m_height,sizeof(float));
//		f.Write(&m_span_angle,sizeof(float));
//
//		for(i=0;i<stotal;i++)
//		{
//			f.Write(pPress+i,sizeof(Pressure));
//		}
//		f.Close();
//	}
//	delete []pPress;
//	pPress=NULL;
//	return;
//}
//
//void CMfcogl1Doc::SaveDataToDisk(LPCTSTR lpszFilter)
//{
//	char *pDefExt="wdf";
//	if(!m_bSavePayeye&&m_bSavePath)
//	{
//		AfxMessageBox(_T("Path data has been saved, please use the \
//\nsame name to save payeye track data !"));
//		pDefExt="mdf";
//	}
//	if(!m_bSavePath&&m_bSavePayeye)AfxMessageBox(_T("Payeye track data has been saved, please \
//use the \nsame name to save path data data !"));
//	CFileDialog fdlg(FALSE,pDefExt,"path_data",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT,
//		             lpszFilter,NULL);
//	if(fdlg.DoModal()==IDOK)
//	{
//		CWaitCursor wait;
//		CFile f;
//		CFileException fe;
//		unsigned long i;
//		if(fdlg.GetFileExt()=="wdf")
//		{
//			f.Open(fdlg.GetPathName(),CFile::modeCreate|CFile::modeWrite,&fe);
//			if(fe.m_cause==CFileException::diskFull)
//			{
//				AfxMessageBox(_T("Disk is full,please free some space and try again !"));
//				return;
//			}
//	
//			f.Write(m_WindingCount,sizeof(unsigned long));
//			f.Write(&m_numlayer,sizeof(m_numlayer));
//			f.Write(&m_sweep_radius,sizeof(m_sweep_radius));
//			f.Write(&m_pipe_radius,sizeof(m_pipe_radius));
////			for(i=0;i<m_numlayer;i++)f.Write((cylinder_radius+i),sizeof(float));
//			f.Write(&m_height,sizeof(m_height));
//			f.Write(&m_span_angle,sizeof(m_span_angle));
//			f.Write(&m_FilamentWidth,sizeof(float));
//			f.Write(&circuit_step,sizeof(int));
////			For display parameter.
//			f.Write(&m_IniWndAngle,sizeof(float));
//			f.Write(&m_FilamentThickness,sizeof(float));
//			f.Write(&m_friction,sizeof(float));
////			For Finite element analysis.
//			f.Write(&m,sizeof(m));
//			f.Write(&k,sizeof(k));
//			f.Write(&nocycle,sizeof(nocycle));
//			f.Write(&large,sizeof(large));
////			Save fiber path.
//			for(i=0;i<=m_WindingCount[0];i++)f.Write((pPath+i),sizeof(PathPoint));
//			f.Close();
//
//			m_bSavePath=true;
//		}
//		else
//		{
//			if(fdlg.GetFileExt()=="mdf"||fdlg.GetFileExt()=="txt")
//			{
//					
//				if(!data_format)
//				{
//					f.Open(fdlg.GetPathName(),CFile::modeCreate|CFile::modeWrite,&fe);
//					if(fe.m_cause==CFileException::diskFull)
//					{
//						AfxMessageBox(_T("Disk is full,please free some space and try again !"));
//						return;
//					}
//					
//					f.Write(m_WindingCount,sizeof(unsigned long));
//					for(i=0;i<=m_WindingCount[0];i++)f.Write((ptrack+i),sizeof(track));
//					f.Close();
//				}
//				else
//				{
//				char pTemp[80];
//				short int j;
//				float x,z,S_angle,P_angle,spanDeg;
//				CStdioFile sf;
//				spanDeg=6.0*PI/180.0;
//				sf.Open(fdlg.GetPathName(),CFile::modeCreate|CFile::modeWrite);
//				sprintf(pTemp,"The total number of trajectory is %u, and the height of carriage is %f.\n",\
//m_WindingCount[0]+1,-11.8);
//				sf.WriteString(pTemp);
//				sprintf(pTemp,"      x  ,      z ,   SwingAngle, SpindleAngle\n");
//				sf.WriteString(pTemp);
//				if(!data_compress)
//				{
//				for(i=0;i<=m_WindingCount[0];i++)
//				{
//					x=ptrack[i].x*10.0;
//					z=ptrack[i].z*10.0;
//					P_angle=ptrack[i].swingAngle*180.0/PI;
//					S_angle=-ptrack[i].spindleAngle*180.0/PI;
//					sprintf(pTemp,"Y %f Z %f U %f X %f\n",x,z,P_angle,S_angle);
//					sf.WriteString(pTemp);
//					if(fabs(ptrack[i].z-ptrack[i-1].z)<tolerance)
//					{
//						j=i;
//						while((ptrack[j+1].spindleAngle-ptrack[i].spindleAngle)<spanDeg)j++;
//						if(j>i) i=j-1;
//					}
//				}
//				}
//				else
//				{
//				for(i=0;i<=m_WindingCount[0];i++)
//				{
//					x=ptrack[i].x*10.0;
//					z=ptrack[i].z*10.0;
//					P_angle=ptrack[i].swingAngle*180.0/PI;
//					S_angle=-ptrack[i].spindleAngle*180.0/PI;
//					sprintf(pTemp,"Y %f Z %f U %f X %f\n",x,z,P_angle,S_angle);
//					sf.WriteString(pTemp);
//					if(i<m_WindingCount[0])
//					{
//					j=i;
//					while((ptrack[j+1].spindleAngle-ptrack[i].spindleAngle)<spanDeg)
//					{
//						j++;
//						if(j>=m_WindingCount[0]) break;
//					}
//					if(j>i) i=j-1;
//					}
//				}
//				}
//				sf.Close();
//				}
//				m_bSavePayeye=true;
//			}
//			else AfxMessageBox(_T("Unknown file format !"));
//		}
//	}
//}
//
//void CMfcogl1Doc::ReadDataFromDisk(LPCTSTR lpFileName, CString lpFileExtName)
//{
//		
//		if(lpFileExtName=="wdf")
//		{
//				ResetWndDesign();
//				unsigned long i;
//				CFile f;
//				CFileException fe;
//				if(!f.Open(lpFileName,CFile::modeRead,&fe))
//				{
//					AfxMessageBox("You input the wrong parameters, input again please!");
//					return;
//				}
//				if(fe.m_cause==CFileException::diskFull)
//				{
//					AfxMessageBox(_T("Disk is full,please free some space and try again !"));
//					return;
//				}
//				if(pPath!=NULL)
//				{
//					delete []pPath;
//					pPath=NULL;
//				}
//				f.Read(m_WindingCount,sizeof(unsigned long));
//				f.Read(&m_numlayer,sizeof(m_numlayer));
//				f.Read(&m_sweep_radius,sizeof(m_sweep_radius));				
//				f.Read(&m_pipe_radius,sizeof(m_pipe_radius));
////				for(i=0;i<m_numlayer;i++)f.Read((cylinder_radius+i),sizeof(float));
//				m_pipe_radius=cylinder_radius[0];
//				f.Read(&m_height,sizeof(m_height));
//				f.Read(&m_span_angle,sizeof(m_span_angle));
//				f.Read(&m_FilamentWidth,sizeof(float));
//				f.Read(&circuit_step,sizeof(int));
////				For display parameter.
//				f.Read(&m_IniWndAngle,sizeof(float));
//				f.Read(&m_FilamentThickness,sizeof(float));
//				f.Read(&m_friction,sizeof(float));
////				For Finite element analysis.	
//				f.Read(&m,sizeof(m));
//				f.Read(&k,sizeof(k));
//				f.Read(&nocycle,sizeof(nocycle));
//				f.Read(&large,sizeof(large));
//
////				Read fiber path.
//				pPath=new PathPoint[m_WindingCount[0]+1];
//				if(pPath==NULL)
//				{
//					AfxMessageBox(_T("Memory allocate failed, close other program and try again !"));
//					return;
//				}
//				for(i=0;i<=m_WindingCount[0];i++)f.Read((pPath+i),sizeof(PathPoint));
//				f.Close();
//				m_bFiberPathComplete=true;
//				m_bSavePath=true;
//				lpOpenPathFile=lpFileName;
//				UpdateAllViews(NULL,0l,NULL);
//		}
//		else
//		{
//			if(lpFileExtName=="mdf")
//			{
//				if(m_bPayeyeComplete)
//				{
//					AfxMessageBox(_T("You are trying to open file incompatible with \
//\npath data file !"));
//					return;
//				}
//				if(lpOpenPathFile==" ")
//				{
//					AfxMessageBox(_T("Please compute payeye track by using menu !"));
//					return;
//				}
//				CFile f;
//				CFileException fe;
//				unsigned long i;
//				if(!f.Open(lpFileName,CFile::modeRead,&fe))
//				{
//					AfxMessageBox("You input the wrong parameters, input again please!");
//					return;
//				}
//				if(fe.m_cause==CFileException::diskFull)
//				{
//					AfxMessageBox(_T("Disk is full,please free some space and try again !"));
//					return;
//				}
//				if(ptrack!=NULL)
//				{
//					delete []ptrack;
//					ptrack=NULL;
//				}
//
//				f.Read(m_WindingCount,sizeof(unsigned long));
//				ptrack=new track[m_WindingCount[0]+1];
//				if(ptrack==NULL)
//				{
//					AfxMessageBox(_T("Memory allocate failed, close other program and try again !"));
//					return;
//				}
//				for(i=0;i<=m_WindingCount[0];i++)f.Read((ptrack+i),sizeof(track));
//				f.Close();
//				m_bPayeyeComplete=true;
//				m_bSavePayeye=true;
//				lpOpenTrackFile=lpFileName;
//			}
//			else AfxMessageBox(_T("Unknown file format !"));
//		}
//}
//
//void CMfcogl1Doc::OnFileSave() 
//{
//	if(m_bFiberPathComplete)
//	{
//		if(m_bPayeyeComplete)
//		{
//			if(m_bSavePath)
//			{
//				if(m_bSavePayeye)AfxMessageBox(_T("There is no new data to be saved !"));
//				else SaveDataToDisk("Payeye track data file(*.mdf)|*.mdf|\
//Text data file for machine(*.txt)|*.txt||");
//			}
//			else 
//			{
//				if(m_bSavePayeye)SaveDataToDisk("Winding path data file(*.wdf)|*.wdf||");
//				else SaveDataToDisk("Winding path data file(*.wdf)|*.wdf|Payeye track data \
//file(*.mdf)|*.mdf||");
//			}
//		}
//		else
//		{
//			if(m_bSavePath)AfxMessageBox(_T("There's no new data to be saved !"));
//			else SaveDataToDisk("Winding path data file(*.wdf)|*.wdf||");
//		}
//	}
//	else AfxMessageBox(_T("There's no new data to be saved !"));
//	// TODO: Add your command handler code here
//	
//}
//
//void CMfcogl1Doc::OnFileOpen() 
//{
//	if((!m_bSavePath&&m_bFiberPathComplete)||(!m_bSavePayeye&&m_bPayeyeComplete))
//	{
//
//		CHintDialog hdlg;
//		if(hdlg.DoModal()==IDOK)return;
//		else ResetWndDesign();
//	}
////	ReadDataFromDisk();
//	// TODO: Add your command handler code here
//	
//}

/*added by LMK*/
//参数设置
void CMfcogl1Doc::OnSwitchFiberPathControlDlg() {
	if (m_isShowing == 1) {//tube
		OnOpenFiberPathControlTubeParametersDlg();
	}
	else if(m_isShowing == 3) {//cylinder
		OnOpenFiberPathControlCylinderParametersDlg();
	}
	else if (m_isShowing == 4) {//cone
		OnOpenFiberPathControlConeParametersDlg();
	}
}

//纤维路径计算
void CMfcogl1Doc::OnSwitchComputeFiberPath() {
	if (m_isShowing == 1) {//tube
		OnComputeFiberPathTube();
	}
	else if (m_isShowing == 3) {//cylinder
		OnComputeFiberPathCylinder();
	}
	else if (m_isShowing == 4) {//cone
		debug_show("cone is doing...");
		//OnComputeFiberPathCone();
	}
}

//机器路径计算
void CMfcogl1Doc::OnSwitchComputePayeye() {
	if (m_isShowing == 1) {//tube
		OnComputePayeyeTube();
	}
	if (m_isShowing == 3) {
		debug_show("cylinder is doing...");
	}
	if (m_isShowing == 4) {
		debug_show("cone is doing...");
	}
}

//保存机器路径
void CMfcogl1Doc::OnSwitchSaveTrack() {
	if (m_isShowing == 1) {//tube
		OnSaveTrackTube();
	}
	if (m_isShowing == 3) {
		debug_show("cylinder is doing...");
	}
	if (m_isShowing == 4) {
		debug_show("cone is doing...");
	}
}

void CMfcogl1Doc::OnOpenFiberPathControlTubeParametersDlg() {
	CDlgFiberPathControlsTube fpTubeDlg;
	if (IDOK == fpTubeDlg.DoModal()) {

		ResetWndDesign();

		cutNum = fpTubeDlg.m_dlg_tube_cut_num;
		payeye.mandrel_speed = fpTubeDlg.m_dlg_tube_mandrel_speed;
		payeye.pm_distance = fpTubeDlg.m_dlg_tube_pm_distance;
		//payeye.pm_left_distance = fpTubeDlg.m_dlg_tube_pm_left_distance;
		payeye.pm_left_distance = 5.0;
		CMfcogl1View* pView;
		POSITION pos = GetFirstViewPosition();
		pView = (CMfcogl1View*)GetNextView(pos);

		tubeModel.a = pView->m_view_tube_a;
		tubeModel.b = pView->m_view_tube_b;
		tubeModel.length = pView->m_view_tube_length;
		tubeModel.r = pView->m_view_tube_r;
		tubeModel.redundance = pView->m_view_tube_redundence;
		tubeModel.angle = fpTubeDlg.m_dlg_tube_winding_angle * PI / 180;
		tubeModel.width = fpTubeDlg.m_dlg_tube_band_width;
		tubeModel.round = 2 * PI * tubeModel.r + 4 * tubeModel.a + 4 * tubeModel.b;//周长

		//计算合适的切点数(缠绕一周出发点的总量)，跳跃数(两次出发点序号的差值)
		float real_width = tubeModel.width / abs(cos(tubeModel.angle));//纤维带按照缠绕角摆放的横切面
		M0 = ceil(tubeModel.round / real_width);//实际剖分段数
		interval = tubeModel.round / M0;//剖分步长
		while ((M0 + 1) % cutNum != 0) {//缩小纤维半径以满足切点数约束
			tubeModel.width -= 0.02;
			real_width = tubeModel.width / abs(cos(tubeModel.angle));
			M0 = ceil(tubeModel.round / real_width);
			interval = tubeModel.round / M0;
		}
		jumpNum = (M0 + 1) / cutNum;

		m_bCanComputing = true;
		CString STemp;
		STemp.Format(_T("纱宽(修正) = %.2f mm\n缠绕角 = %.2f degree\n等分数=%d\n切点数=%d\n跳跃数=%d"),
			real_width, fpTubeDlg.m_dlg_tube_winding_angle, M0, cutNum, jumpNum);
		AfxMessageBox(STemp);
}
	}

void CMfcogl1Doc::CleanTubeMemory() {
	if (TubePointList != NULL) {
		for (auto i = TubePointList->begin(); i < TubePointList->end(); i++) {
			delete *i;
		}
		delete TubePointList;
		TubePointList = NULL;
	}
	if (TubeTrackList != NULL) {
		for (auto i = TubeTrackList->begin(); i < TubeTrackList->end(); i++) {
			delete* i;
		}
		delete TubeTrackList;
		TubeTrackList = NULL;
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
	TubeBothList.clear();
}

//生成起始点列表 在头上绕一圈，每隔一个切片采集一个点
int CMfcogl1Doc::OnGenerateTubeEdgeList(std::deque<struct tubePathPosition>& TubeList,float interval,int sides) {
	//sides: 1.回来 2.过去+回来
	//interval: 端面每一片切分的长度
	float halfInterval = interval / 2;
	int i = 0;
	float tiny = 0.001;
	struct tubePathPosition p;
	for (i = 0; i * interval < tubeModel.a; i++) {
		p.x = i * interval;
		p.y = tubeModel.b + tubeModel.r;
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < tubeModel.a + PI * tubeModel.r / 2; i++) {
		p.x = tubeModel.a + tubeModel.r * sin((i * interval - tubeModel.a) / tubeModel.r);
		p.y = tubeModel.b + tubeModel.r * cos((i * interval - tubeModel.a) / tubeModel.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < tubeModel.a + PI * tubeModel.r / 2 + 2 * tubeModel.b; i++) {
		p.x = tubeModel.a + tubeModel.r;
		p.y = tubeModel.b - (i * interval - tubeModel.a - PI * tubeModel.r / 2);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < tubeModel.a + PI * tubeModel.r + 2 * tubeModel.b; i++) {
		p.x = tubeModel.a + tubeModel.r * cos((i * interval - (tubeModel.a + PI * tubeModel.r / 2 + 2 * tubeModel.b)) / tubeModel.r);
		p.y = -tubeModel.b - tubeModel.r * sin((i * interval - (tubeModel.a + PI * tubeModel.r / 2 + 2 * tubeModel.b)) / tubeModel.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < 3 * tubeModel.a + PI * tubeModel.r + 2 * tubeModel.b; i++) {
		p.x = 2 * tubeModel.a - i * interval + PI * tubeModel.r + 2 * tubeModel.b;
		p.y = -tubeModel.b - tubeModel.r;
		p.z = 0;
		p.index = i;
		p.direction = 1; 
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < 3 * tubeModel.a + 1.5 * PI * tubeModel.r + 2 * tubeModel.b; i++) {
		p.x = -tubeModel.a - tubeModel.r * sin((i * interval - (3 * tubeModel.a + PI * tubeModel.r + 2 * tubeModel.b)) / tubeModel.r);
		p.y = -tubeModel.b - tubeModel.r * cos((i * interval - (3 * tubeModel.a + PI * tubeModel.r + 2 * tubeModel.b)) / tubeModel.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < 3 * tubeModel.a + 1.5 * PI * tubeModel.r + 4 * tubeModel.b; i++) {
		p.x = -tubeModel.a - tubeModel.r;
		p.y = -tubeModel.b + i * interval - 3 * tubeModel.a - 1.5 * PI * tubeModel.r - 2 * tubeModel.b;
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < 3 * tubeModel.a + 2 * PI * tubeModel.r + 4 * tubeModel.b; i++) {
		p.x = -tubeModel.a - tubeModel.r * cos((i * interval - (3 * tubeModel.a + 1.5 * PI * tubeModel.r + 4 * tubeModel.b)) / tubeModel.r);
		p.y = tubeModel.b + tubeModel.r * sin((i * interval - (3 * tubeModel.a + 1.5 * PI * tubeModel.r + 4 * tubeModel.b)) / tubeModel.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < 4 * tubeModel.a + 2 * PI * tubeModel.r + 4 * tubeModel.b - tiny; i++) {
		p.x = i * interval - (4 * tubeModel.a + 2 * PI * tubeModel.r + 4 * tubeModel.b);
		p.y = tubeModel.b + tubeModel.r;
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}

	return TubeList.size();
}

//方管纤维路径的计算
//1.先按照切分，查找两端所有的纤维起始点
//2.从所有起始点开始来回计算缠绕
//3.在每两条纤维中间插入从本条的终点到下条起点的路
void CMfcogl1Doc::OnComputeFiberPathTube() {
	CMfcogl1Doc::CleanTubeMemory();

	//一条纤维路径的数据结构：一段从上一纤维末尾位置到本纤维起点的缠绕点+一段从纤维起点到实际缠绕旋转点的差值+真正的缠绕部分
	if (m_bCanComputing == true){
		TubePointList = new std::deque<std::deque<struct tubePathCoord>*>;//TubePoint的双向链表指针的双向链表，内层表示一条纤维路径

		windingPathStep = 5;//计算的步长
		TubeCurrentStart[0] = TubeCurrentStart[1] = -1;

		//generate GL list
		glNewList(FIBER_PATH_LIST, GL_COMPILE); 

		GLfloat matAmb[4] = { 0.8F, 0.5F, 0.3F, 1.00F };
		GLfloat matDiff[4] = { 0.8F, 0.5F, 0.3F, 0.80F };
		GLfloat matSpec[4] = { 0.30F, 0.30F, 0.30F, 0.30F };
		GLfloat matShine = 60.00F;
		glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
		glMaterialf(GL_FRONT, GL_SHININESS, matShine);

		//生成所有路径的起点
		OnGenerateTubeEdgeList(TubeBothList,interval,2);

		double buffer_angle = PI * 60/ 180;
		//从一头绕到另一头经过的环方管切面长度
		//先不考虑冗余段的长度 已经对周长取余数
		double circle_length = tubeModel.length * tan(tubeModel.angle);
		while (circle_length > tubeModel.round) circle_length -= tubeModel.round;
		//在冗余段进出需要的缓冲距离最少要这么多
		double min_length = tubeModel.round * (buffer_angle / (2 * PI));
		double cut_min = M0 * (buffer_angle / (2 * PI));
		//每一个切片经过的长度
		double cut_interval_length = tubeModel.round / M0;
		//环方管切面长度换算成切点数
		int cut_point_circle_length = ceil(circle_length / cut_interval_length);
		//同一端两次出发间隔距离（跳跃数*切片长）
		double jump_interval_length = jumpNum * cut_interval_length;
		//最终需要求得的值：在一端原有的切点基础上往右偏移几个切点？
		//两侧在冗余段经过的长度相等
		cut_num_offset = 0;
		int current_minus = M0 * 2;//当前两端在冗余段经过的长度差
		for (int tmp_num_offset=0; tmp_num_offset < M0; tmp_num_offset++) {
			//右端冗余段长度
			// |(0+cut_num_offset)  -  (0+cut_point_circle_length) + k1*M0| 出 - 进
			int right = tmp_num_offset - cut_point_circle_length;
			while (right < 0) right += M0;
			//左端冗余段长度
			// |(0+jumpNum)  - (0+cut_num_offset + cut_point_circle_length) +k2*M0|  出 - 进
			int left = jumpNum - tmp_num_offset - cut_point_circle_length;
			while (left < 0) left += M0;
			if (left > cut_min && right > cut_min&& abs(left - right) < current_minus) {
				current_minus = abs(left - right);
				cut_num_offset = tmp_num_offset;
			}
		}

		currentPosition.direction = -1;
		int i = 3;
		//计算循环
		while (TubeBothList.size()!=0) {
			std::deque<struct tubePathCoord>* currentPathList = new std::deque<struct tubePathCoord>;
			TubePointList->push_back(currentPathList);
			OnGeneratePosition(TubeBothList,currentPathList);
			OnRenderSinglePath(currentPathList);//计算本纤维路径的所有顶点位置并输入glList和TubePointList
		}
		glEndList();

		//插入两条纤维路径的连接段
		//按步长聚集端面的点，两个端面共用TubeStepList，所以属性direction不重要
		//OnGenerateTubeEdgeList(TubeStepList, windingPathStep, 1);
		//for (auto list = TubePointList->begin(); list < TubePointList->end()-1;list++ ) {
		//	std::deque<struct tubePathCoord>* currentPathList = new std::deque<struct tubePathCoord>;
		//	tubePathCoord begin = *(*list)->rbegin();//本纤维段的末端
		//	tubePathCoord end = *(*(list + 1))->begin();//下纤维段的起点
		//	std::deque<tubePathPosition>::iterator closest_begin;
		//	std::deque<tubePathPosition>::iterator closest_end;
		//	float dis;
		//	dis= INFINITY;
		//	for (auto j = TubeStepList.begin(); j < TubeStepList.end(); j++) {
		//		float tmp_dis = (begin.x - j->x) * (begin.x - j->x) + (begin.y - j->y) * (begin.y - j->y);
		//		if (tmp_dis < dis) {
		//			dis = tmp_dis;
		//			closest_begin = j;
		//		}
		//	}
		//	dis = INFINITY;
		//	for (auto j = TubeStepList.begin(); j < TubeStepList.end(); j++) {
		//		float tmp_dis = (end.x - j->x) * (end.x - j->x) + (end.y - j->y) * (end.y - j->y);
		//		if (tmp_dis < dis) {
		//			dis = tmp_dis;
		//			closest_end = j;
		//		}
		//	}
		//	if (closest_begin < closest_end) {
		//		for (auto i = closest_begin; i <= closest_end; i++) {
		//			tubePathCoord tmp = begin;
		//			tmp.x = (*i).x;
		//			tmp.y = (*i).y;
		//			currentPathList->push_back(tmp);
		//		}
		//	}
		//	else {
		//		for (auto i = closest_begin; i <TubeStepList.end(); i++) {
		//			tubePathCoord tmp = begin;
		//			tmp.x = (*i).x;
		//			tmp.y = (*i).y;
		//			currentPathList->push_back(tmp);
		//		}
		//		for (auto i = TubeStepList.begin(); i <= closest_end; i++) {
		//			tubePathCoord tmp = begin;
		//			tmp.x = (*i).x;
		//			tmp.y = (*i).y;
		//			currentPathList->push_back(tmp);
		//		}
		//	}
		//	list = TubePointList->insert(list+1, currentPathList);
		//}

		//统计总的坐标数
		m_WindingCount[0] = 0;
		for (auto i = TubePointList->begin(); i < TubePointList->end(); i++) { m_WindingCount[0] += (*i)->size(); }
		AfxMessageBox("纤维路径计算完毕\ntube Track Computation Finished.");
		m_bFiberPathComplete = true;
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
void CMfcogl1Doc::OnGeneratePosition(std::deque<struct tubePathPosition>&TubeStartList, std::deque<struct tubePathCoord>* singlePathList) {

	auto tmpPosition = TubeStartList.begin();
	//direction==-1 ->刚才是从尾部回来->说明现在要从头部开始绕->得到上一次从头部出去的起始点->加一个跳跃数
	int currentStart = (currentPosition.direction == -1) ? TubeCurrentStart[0] : TubeCurrentStart[1];

	//如果已经初始化：接着上一条算跳跃数
	if (currentStart != -1) {
		auto i = TubeStartList.begin();
		for (; i < TubeStartList.end(); i++) {//遍历 给上一个起始点加一个跳跃数
			if (currentPosition.direction != (*i).direction && (*i).index == (currentStart + jumpNum) % M0) {
				tmpPosition = i;
				break;
			}
		}
	}
	else {
		//还未初始化
		if (currentPosition.direction == 1) {//&& currentStart == 1
			tmpPosition = TubeStartList.begin() + 2 * cut_num_offset;
		}
	}

	currentPosition = *tmpPosition;//得到下一跳的起始点

	if (currentPosition.direction ==1) {//出去
		TubeCurrentStart[0] = (*tmpPosition).index;
	}
	else {//回来
		TubeCurrentStart[1] = (*tmpPosition).index;
	}
	TubeStartList.erase(tmpPosition);
}

//算出单根纤维路径
void CMfcogl1Doc::OnRenderSinglePath(std::deque<struct tubePathCoord>* singlePathList) {
	GLfloat matAmb[4] = { 1.0F, 0.0F, 0.0F, 1.00F };
	if (currentPosition.direction == 1) {
		matAmb[1] = 1.0F;
	}
	else {
		matAmb[2] = 1.0F;
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glBegin(GL_LINE_STRIP);
	//initial OpenGL
	//判断当前position处于芯模的哪个面，从而选中合适的入口。
	//类似于状态机，本质上是对两个OnRender不同状态的调用
	//缠绕到面的边缘时，会在OnRender中递归调用下一个面的OnRender，直到缠绕到芯模边缘退出。
	if (-tubeModel.a <= currentPosition.x && currentPosition.x < tubeModel.a && currentPosition.y>0) {
		OnRenderLinePart(1, singlePathList);
	}
	else if (tubeModel.a <= currentPosition.x && currentPosition.x < tubeModel.a + tubeModel.r && currentPosition.y > 0) {
		OnRenderCurvePart(2, singlePathList);
	}
	else if (-tubeModel.b < currentPosition.y && currentPosition.y <= tubeModel.b && currentPosition.x>0) {
		OnRenderLinePart(3, singlePathList);
	}
	else if (-tubeModel.b - tubeModel.r < currentPosition.y && currentPosition.y <= -tubeModel.b && currentPosition.x>0) {
		OnRenderCurvePart(4, singlePathList);
	}
	else if (-tubeModel.a < currentPosition.x && currentPosition.x <= tubeModel.a && currentPosition.y < 0) {
		OnRenderLinePart(5, singlePathList);
	}
	else if (-tubeModel.a - tubeModel.r < currentPosition.x && currentPosition.x <= -tubeModel.a && currentPosition.y < 0) {
		OnRenderCurvePart(6, singlePathList);
	}
	else if (-tubeModel.b <= currentPosition.y && currentPosition.y < tubeModel.b && currentPosition.x < 0) {
		OnRenderLinePart(7, singlePathList);
	}
	else if (-tubeModel.a - tubeModel.r <= currentPosition.x && currentPosition.x < -tubeModel.a && currentPosition.y >0) {
		OnRenderCurvePart(8, singlePathList);
	}
	glEnd();
}

inline void CMfcogl1Doc::updatePosition(float* nextPoint) {
	currentPosition.x = nextPoint[0];	currentPosition.y = nextPoint[1]; currentPosition.z = nextPoint[2];
}

inline int CMfcogl1Doc::outTubeEdge(float z) {
	return (z < 0) ? -1 : (z > (tubeModel.length) ? 1 : 0);
}

inline void CMfcogl1Doc::insertPoint(float* nextPoint) {
	glVertex3fv(nextPoint);
}

void CMfcogl1Doc::pushTubePathCoord(const float*nextPoint,float normal_radian,const int direction, std::deque<struct tubePathCoord>* singlePathList,float angle = -1000){
	tempTubePathCoord.x = nextPoint[0];
	tempTubePathCoord.y = nextPoint[1];
	tempTubePathCoord.z = nextPoint[2];
	if (normal_radian < -PI) normal_radian += 2 * PI;
	if (normal_radian >  PI) normal_radian -= 2 * PI;
	tempTubePathCoord.normal_radian = normal_radian;
	if (angle < -100) {
		tempTubePathCoord.tangent.x = sin(tubeModel.angle) * cos(tempTubePathCoord.normal_radian - PI / 2);
		tempTubePathCoord.tangent.y = sin(tubeModel.angle) * sin(tempTubePathCoord.normal_radian - PI / 2);
		tempTubePathCoord.tangent.z = direction * cos(tubeModel.angle);
	}
	else {
		tempTubePathCoord.tangent.x = sin(angle) * cos(tempTubePathCoord.normal_radian - PI / 2);
		tempTubePathCoord.tangent.y = sin(angle) * sin(tempTubePathCoord.normal_radian - PI / 2);
		tempTubePathCoord.tangent.z = direction * cos(angle);
	}
	tempTubePathCoord.direction = currentPosition.direction;
	tempTubePathCoord.index = currentPosition.index;
	singlePathList->push_back(tempTubePathCoord);
}

int CMfcogl1Doc::OnRenderLinePart(int state, std::deque<struct tubePathCoord>* singlePathList) {
	float tiny = 0.01;
	int size = singlePathList->size();
	float nextPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	float startPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	tubeModel.curveLength = 0;
	int direction = currentPosition.direction;
	int delta_alpha = 2;
	switch (state) {
	case 1:
		//glNormal3f(cos((360-2*segmentw)*3.14/180),sin((360-2*segmentw)*3.14/180),0.0f);		
		if (size == 0) {
			float cur_normal = PI, jus = (PI / 2 - cur_normal) / 10;
			float angle = PI / 2;
			float tan_hp_minus_a = tan(PI / 2 - tubeModel.angle);
			float halfpi_minus_angle = 0, jus2 = tan_hp_minus_a / delta_alpha;
			while (abs(cur_normal - PI / 2)>tiny) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, cur_normal, direction, singlePathList,angle);
				cur_normal += jus;
				if (abs(angle-tubeModel.angle) > tiny) {
					halfpi_minus_angle = atan(tan(halfpi_minus_angle)+jus2);
					angle = PI / 2 - halfpi_minus_angle;
				}
			}
		}
		else {//嵌入起点
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, PI / 2, direction, singlePathList);
		}
		while (nextPoint[0] < tubeModel.a && outTubeEdge(nextPoint[2]) == 0) {
			nextPoint[0] = startPoint[0] + tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[1] = startPoint[1];
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;
			tubeModel.curveLength += windingPathStep;
		}//嵌入共同的终点
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, PI / 2, direction, singlePathList);
		break;
	case 3:
		if (size == 0) {
			float cur_normal = PI / 2, jus = (0 - cur_normal) / 10;
			float angle = PI / 2;
			float tan_hp_minus_a = tan(PI / 2 - tubeModel.angle);
			float halfpi_minus_angle = 0, jus2 = tan_hp_minus_a / delta_alpha;
			while (abs(cur_normal - 0)>tiny) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, cur_normal, direction, singlePathList,angle);
				cur_normal += jus;
				if (abs(angle - tubeModel.angle) > tiny) {
					halfpi_minus_angle = atan(tan(halfpi_minus_angle) + jus2);
					angle = PI / 2 - halfpi_minus_angle;
				}
			}
		}
		else {//嵌入起点
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, 0, direction, singlePathList);
		}
		while (-tubeModel.b < nextPoint[1] && outTubeEdge(nextPoint[2])==0) {
			//insertPoint(nextPoint);
			nextPoint[0] = startPoint[0];
			nextPoint[1] = startPoint[1] - tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;
			tubeModel.curveLength += windingPathStep;
		}
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, 0, direction, singlePathList);
		break;
	case 5:
		if (size == 0) {
			float cur_normal = 0, jus = (-PI / 2 - cur_normal) / 10;
			float angle = PI / 2;
			float tan_hp_minus_a = tan(PI / 2 - tubeModel.angle);
			float halfpi_minus_angle = 0, jus2 = tan_hp_minus_a / delta_alpha;
			while (abs(cur_normal - -PI/2)>tiny) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, cur_normal, direction, singlePathList, angle);
				cur_normal += jus;
				if (abs(angle - tubeModel.angle) > tiny) {
					halfpi_minus_angle = atan(tan(halfpi_minus_angle) + jus2);
					angle = PI / 2 - halfpi_minus_angle;
				}
			}
		}
		else {//嵌入起点
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, -PI / 2, direction, singlePathList);
		}
		while (-tubeModel.a < nextPoint[0] && outTubeEdge(nextPoint[2]) == 0) {
			//insertPoint(nextPoint);
			nextPoint[0] = startPoint[0] - tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[1] = startPoint[1];
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;
			tubeModel.curveLength += windingPathStep;
		}
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, -PI / 2, direction, singlePathList);
		break;
	case 7:
		if (size == 0) {
			float cur_normal = -PI / 2, jus = (-PI - cur_normal) / 10;
			float tan_hp_minus_a = tan(PI / 2 - tubeModel.angle);
			float angle = PI / 2;
			float halfpi_minus_angle = 0, jus2 = tan_hp_minus_a / delta_alpha;
			while (abs(cur_normal - -PI)>tiny) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, cur_normal, direction, singlePathList, angle);
				cur_normal += jus;
				if (abs(angle - tubeModel.angle) > tiny) {
					halfpi_minus_angle = atan(tan(halfpi_minus_angle) + jus2);
					angle = PI / 2 - halfpi_minus_angle;
				}
			}
		}
		else {//嵌入起点
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, -PI , direction, singlePathList);
		}
		while (nextPoint[1] < tubeModel.b && outTubeEdge(nextPoint[2]) == 0) {
		//	insertPoint(nextPoint);
			nextPoint[0] = startPoint[0];
			nextPoint[1] = startPoint[1] + tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;
			tubeModel.curveLength += windingPathStep;
		}
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, -PI, direction, singlePathList);
		break;
	default:
		break;
	}

	updatePosition(nextPoint);
	if (outTubeEdge(currentPosition.z) == 0) {//from one part move into another neighbor part.   some how like a LSM.
		OnRenderCurvePart(state + 1,singlePathList);
	}
	else {
		return outTubeEdge(currentPosition.z);
	}
}

int CMfcogl1Doc::OnRenderCurvePart(int state, std::deque<struct tubePathCoord>* singlePathList) {
	int size = singlePathList->size();
	float startPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	float nextPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	float phase = 0;
	tubeModel.curveLength = 0;
	int direction = currentPosition.direction;
	//此时，为了便于计算，将stepLength定义为在该分面经过的弧度（而非曲线弧长）。为了保持曲线弧长增量不变，需要计算其在本分面上对应Δ弧度。
	float adjust = tubeModel.r / sin(tubeModel.angle);
	switch (state) {
	case 2:
		phase = atan((currentPosition.x - tubeModel.a) / (currentPosition.y - tubeModel.b));
		tubeModel.curveLength = phase;
		startPoint[0] = tubeModel.a;
		startPoint[1] = tubeModel.b;
		if (size == 0) {
			//0:横向（2a）1:高（2b）2:纵向（C）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			insertPoint(nextPoint);
			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;
			pushTubePathCoord(nextPoint, -tubeModel.curveLength +PI, direction, singlePathList,PI/2);
		}
		//圆柱面上测地线 对弧面的三条边约束
		while (tubeModel.curveLength <= PI / 2 && outTubeEdge(nextPoint[2]) == 0) {
			insertPoint(nextPoint);

			//0:横向（2a）1:高（2b）2:纵向（C）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;

			pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction, singlePathList);

			tubeModel.curveLength += windingPathStep/adjust;
		}

		break;
	case 4:
		phase = PI / 2 + atan((-tubeModel.b - currentPosition.y) / (currentPosition.x - tubeModel.a));
		tubeModel.curveLength = phase;
		startPoint[0] = tubeModel.a;
		startPoint[1] = -tubeModel.b;
		if (size == 0) {
			insertPoint(nextPoint);
			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;
			pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI, direction, singlePathList,PI/2);
		}
		while (tubeModel.curveLength <= PI && outTubeEdge(nextPoint[2]) == 0) {
			insertPoint(nextPoint);

			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;

			pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction, singlePathList);

			tubeModel.curveLength += windingPathStep/adjust;
		}

		break;
	case 6:
		phase = PI + atan((-currentPosition.x - tubeModel.a) / (-currentPosition.y - tubeModel.b));
		tubeModel.curveLength = phase;
		startPoint[0] = -tubeModel.a;
		startPoint[1] = -tubeModel.b;
		if (size == 0) {
			insertPoint(nextPoint);
			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;
			pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI, direction, singlePathList, PI / 2);
		}
		while (tubeModel.curveLength <= 1.5 * PI && outTubeEdge(nextPoint[2]) == 0) {
			insertPoint(nextPoint);

			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;
			
			pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction, singlePathList);

			tubeModel.curveLength += windingPathStep/adjust;
		}

		break;
	case 8:
		phase = 1.5 * PI + atan((currentPosition.y - tubeModel.b) / (-currentPosition.x - tubeModel.a));
		tubeModel.curveLength = phase;
		startPoint[0] = -tubeModel.a;
		startPoint[1] = tubeModel.b;
		if (size == 0) {
			insertPoint(nextPoint);
			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;
			pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI, direction, singlePathList, PI / 2);
		}
		while (tubeModel.curveLength <= 2 * PI && outTubeEdge(nextPoint[2]) == 0) {
			insertPoint(nextPoint);

			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;

			pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction, singlePathList);

			tubeModel.curveLength += windingPathStep/adjust;
		}

		break;
	default:
		break;
	}

	updatePosition(nextPoint);
	if (outTubeEdge(currentPosition.z) == 0) {
		OnRenderLinePart(state % 8 + 1,singlePathList);
	}
	else {
		return outTubeEdge(currentPosition.z);
	}
}

struct {
	int initialized;
	float x, y, z;
} final_position;
float circle_minus(float a,float b) {
	float ret = a - b;
	while (ret < 0) {
		ret += 2 * PI;
	}
	return ret;
}
//计算方管机器路径
void CMfcogl1Doc::OnComputePayeyeTube() {
	if (m_bFiberPathComplete) {
		glNewList(FIBER_TRACK_LIST, GL_COMPILE);

		GLfloat matAmb[4] = { 1.0F, 0.0F, 0.0F, 1.00F };
		GLfloat matDiff[4] = { 1.0F, 0.0F, 0.0F, 0.80F };
		GLfloat matSpec[4] = { 1.0F, 0.0F, 0.0F, 0.30F };
		GLfloat matShine = 60.00F;
		glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
		glMaterialf(GL_FRONT, GL_SHININESS, matShine);
		glLineWidth(2);

		TubeTrackList = new std::deque<std::deque<struct track>*>;//存储吐丝嘴位置的双向链表指针的双向链表，内层表示一条纤维路径上对应的吐丝嘴坐标
		for (auto line = TubePointList->begin(); line < TubePointList->end(); line++) {//每条纤维路径
			glBegin(GL_LINE_STRIP);
			std::deque<struct track>* currentTrackList = new std::deque<struct track>;
			TubeTrackList->push_back(currentTrackList);
			final_position.initialized = 0;
			for (auto currentTubePointIterator = (*line)->begin(); currentTubePointIterator < (*line)->end(); currentTubePointIterator++) {//纤维路径上每个点
				track tempTubeTrack;
				computeTubeTrack(currentTubePointIterator, tempTubeTrack);
				currentTrackList->push_back(tempTubeTrack);
				float trackMandrelLength = payeye.pm_distance - tempTubeTrack.x;//吐丝嘴到芯模中心轴的垂直距离
				float trackPoint[3] = { trackMandrelLength * cos(tempTubeTrack.spindleAngle), trackMandrelLength * sin(tempTubeTrack.spindleAngle), tempTubeTrack.z };
				glVertex3fv(trackPoint);
			}
			glEnd();
			
		}
		//冗余段计算
		auto fiber = TubePointList->begin();
		auto payeye_track = TubeTrackList->begin();
		for (; fiber < TubePointList->end()-1,payeye_track<TubeTrackList->end()-1; fiber++,payeye_track++) {
			//冗余段的载体
			std::deque<struct tubePathCoord>* currentPointList = new std::deque<struct tubePathCoord>;
			std::deque<struct track>* currentTrackList = new std::deque<struct track>;
			tubePathCoord fiber_begin = *(*fiber)->rbegin();//本纤维段的末端
			tubePathCoord fiber_end = *(*(fiber + 1))->begin();//下纤维段的起点

			track payeye_begin = *(*payeye_track)->rbegin();//本机器路径的末端
			auto _iter_payeye_begin_pre = (*payeye_track)->rbegin()+1; 
			while (_iter_payeye_begin_pre->z == payeye_begin.z) { 
				_iter_payeye_begin_pre++; 
			}
			track payeye_begin_pre = *_iter_payeye_begin_pre;//本机器路径的末端减一
			track payeye_end = *(*(payeye_track + 1))->begin();//下机器路径的起点
			track payeye_end_next = *((*(payeye_track + 1))->begin()+1);//下机器路径的起点加一
			float d = fiber_begin.direction;

			float Vbegin = d * abs((payeye_begin.z - payeye_begin_pre.z) / circle_minus(payeye_begin_pre.spindleAngle, payeye_begin.spindleAngle));
			float Vend = -d * abs((payeye_end_next.z - payeye_end.z) / circle_minus(payeye_end.spindleAngle, payeye_end_next.spindleAngle));
			float Dangle = circle_minus(payeye_begin.spindleAngle, payeye_end.spindleAngle);
			float K = (Vbegin * Vbegin) / (Vend * Vend);

			float t1 = (Vbegin * Dangle) / (-K * Vend + Vbegin);
			float t2 = Dangle - t1;
			float a1 = -Vbegin / t1;
			float a2 = Vend / t2;

			float cut = 180 * (Dangle / PI);
			float d_angle = Dangle / cut;
			float cur_V = Vbegin;
			track tempTubeTrack = payeye_begin;
			float ratio = t1 / (t1 + t2);
			glBegin(GL_LINE_STRIP);

			for (int i = 0; i < cut-1; i++) {

				tempTubeTrack.spindleAngle -= d_angle;
				if (tempTubeTrack.spindleAngle > 2 * PI)tempTubeTrack.spindleAngle -= 2 * PI;

				tempTubeTrack.z += cur_V * d_angle;
				if (i < cut * ratio) {
					cur_V += a1 * d_angle;
				}
				else {
					cur_V +=  a2 * d_angle;
				}
				float dirx = cos(tempTubeTrack.spindleAngle);
				float diry = sin(tempTubeTrack.spindleAngle);
				tempTubeTrack.x = payeye.pm_distance - suspensionCompute(tubeModel.a, tubeModel.b, tubeModel.r+ payeye.pm_left_distance, payeye.pm_left_distance, 0, 0, dirx, diry);
				tempTubeTrack.swingAngle = payeye_begin.swingAngle + (payeye_end.swingAngle - payeye_begin.swingAngle) * (i / cut);
				tubePathCoord tmpPathPoint;
				float length_of_intersection = suspensionCompute(tubeModel.a, tubeModel.b, tubeModel.r, 0, 0, 0, dirx, diry);
				tmpPathPoint.x = length_of_intersection * dirx;
				tmpPathPoint.y = length_of_intersection * diry;
				tmpPathPoint.z = tempTubeTrack.z;
				tmpPathPoint.normal_radian = tempTubeTrack.spindleAngle;

				currentPointList->push_back(tmpPathPoint);
				currentTrackList->push_back(tempTubeTrack);
				float trackMandrelLength = payeye.pm_distance - tempTubeTrack.x;//吐丝嘴到芯模中心轴的垂直距离
				float trackPoint[3] = { trackMandrelLength * cos(tempTubeTrack.spindleAngle), trackMandrelLength * sin(tempTubeTrack.spindleAngle), tempTubeTrack.z };
				glVertex3fv(trackPoint);
			}
			glEnd();
			fiber = TubePointList->insert(fiber + 1, currentPointList);
			payeye_track = TubeTrackList->insert(payeye_track + 1, currentTrackList);
		}
		glEndList();

		//插入两条纤维路径的连接段
		//按步长聚集端面的点，两个端面共用TubeStepList，所以属性direction不重要
		/*
		OnGenerateTubeEdgeList(TubeStepList, windingPathStep, 1);
		for (auto list = TubePointList->begin(); list < TubePointList->end()-1;list++ ) {
			std::deque<struct tubePathCoord>* currentPathList = new std::deque<struct tubePathCoord>;
			tubePathCoord begin = *(*list)->rbegin();//本纤维段的末端
			tubePathCoord end = *(*(list + 1))->begin();//下纤维段的起点
			std::deque<tubePathPosition>::iterator closest_begin;
			std::deque<tubePathPosition>::iterator closest_end;
			float dis;
			dis= INFINITY;
			for (auto j = TubeStepList.begin(); j < TubeStepList.end(); j++) {
				float tmp_dis = (begin.x - j->x) * (begin.x - j->x) + (begin.y - j->y) * (begin.y - j->y);
				if (tmp_dis < dis) {
					dis = tmp_dis;
					closest_begin = j;
				}
			}
			dis = INFINITY;
			for (auto j = TubeStepList.begin(); j < TubeStepList.end(); j++) {
				float tmp_dis = (end.x - j->x) * (end.x - j->x) + (end.y - j->y) * (end.y - j->y);
				if (tmp_dis < dis) {
					dis = tmp_dis;
					closest_end = j;
				}
			}
			if (closest_begin < closest_end) {
				for (auto i = closest_begin; i <= closest_end; i++) {
					tubePathCoord tmp = begin;
					tmp.x = (*i).x;
					tmp.y = (*i).y;
					currentPathList->push_back(tmp);
				}
			}
			else {
				for (auto i = closest_begin; i <TubeStepList.end(); i++) {
					tubePathCoord tmp = begin;
					tmp.x = (*i).x;
					tmp.y = (*i).y;
					currentPathList->push_back(tmp);
				}
				for (auto i = TubeStepList.begin(); i <= closest_end; i++) {
					tubePathCoord tmp = begin;
					tmp.x = (*i).x;
					tmp.y = (*i).y;
					currentPathList->push_back(tmp);
				}
			}
			list = TubePointList->insert(list+1, currentPathList);
		}
		*/

		//重新统计总的数
		m_WindingCount[0] = 0;
		for (auto i = TubePointList->begin(); i < TubePointList->end(); i++) { m_WindingCount[0] += (*i)->size(); }
		CMfcogl1View* pView;
		POSITION pos = GetFirstViewPosition();
		pView = (CMfcogl1View*)GetNextView(pos);
		pView->Invalidate(false);
		m_bPayeyeComplete = true;
		pView->m_cview_enable_track_display = true;
	}
	else {
		AfxMessageBox(_T("请先计算纤维路径\nPlease compute path first."));
	}
}
float ray_curve(int quad1, int quad2,vec2& src,vec2& dir,float a,float b,float r) {
	//( dir * s  + some).sqr_len() = r * r;
	//(some.x + s * dir.x) ^ 2 + (some.y + s * dir.y) ^ 2 = r * r
	//dir.sqr_len() * s ^ 2 + 2(some.x * dir.x + some.y * dir.y) * s + some.sqr_len() - r * r = 0;
	vec2 some = src - vec2(quad1 * a, quad2 * b);
	float _a = dir.sqr_len(), _b = 2 * (some.x * dir.x + some.y * dir.y), _c = some.sqr_len() - r * r;
	float s = (-_b + sqrt(_b * _b - 4 * _a * _c)) / (2 * _a);
	return s;
};
float CMfcogl1Doc::suspensionCompute(float a,float b,float r,float shell,float x,float y,float dir_x,float dir_y) {
	vec2 src = { x,y };
	vec2 dir = { dir_x,dir_y };
	//model: a,b,r
	float s = 99999;//suspension
	// 矩形内的射线和矩形的交点 负数 负数 正数大 正数小 显然小的正数是解
	float s1 = (a + r - src.x) / dir.x;
	if (s1 > 0 && s1 < s)s = s1;
	float s2 = (-a - r - src.x) / dir.x;
	if (s2 > 0 && s2 < s)s = s2;
	float s3 = (b + r - src.y) / dir.y;
	if (s3 > 0 && s3 < s)s = s3;
	float s4 = (-b - r - src.y) / dir.y;
	if (s4 > 0 && s4 < s)s = s4;
	vec2 tmp = src + dir * s;

	if (tmp.x > a && tmp.y > b) s = ray_curve(1, 1, src, dir, a, b, r);
	if (tmp.x < -a && tmp.y > b) s=ray_curve(-1,1, src, dir, a, b, r);
	if (tmp.x < -a  && tmp.y < -b) s=ray_curve(-1,-1, src, dir, a, b, r);
	if (tmp.x > a && tmp.y < -b ) s=ray_curve(1,-1, src, dir, a, b, r);
	return s;
}

//计算出currentTubePoint的suspension(悬丝长度)和tempTubeTrack的四自由度参数
void CMfcogl1Doc::computeTubeTrack(std::deque<struct tubePathCoord>::iterator currentTubePointIterator, track& tempTubeTrack) {
	struct tubePathCoord& currentTubePoint = *currentTubePointIterator;
	float shell_a = tubeModel.a;
	float shell_b = tubeModel.b;
	float shell_r = tubeModel.r + payeye.pm_left_distance;
	float R = payeye.pm_distance;
	//(currentTubePoint.x + currentTubePoint.tangent.x * suspension) ^ 2 + (currentTubePoint.y + currentTubePoint.tangent.y * suspension) ^ 2 = R * R;    syms:suspension
	float a = currentTubePoint.tangent.x, b = currentTubePoint.x, c = currentTubePoint.tangent.y, d = currentTubePoint.y;
	currentTubePoint.suspension = suspensionCompute(shell_a, shell_b, shell_r, payeye.pm_left_distance, b, d, a, c);
	//float A = a * a + c * c;
	//float B = 2 * a * b + 2 * c * d;
	//float C = b * b + d * d - R * R;
	//float root1 = (-B + sqrt(B * B - 4 * A * C)) / (2 * A);
	//float root2 = (-B - sqrt(B * B - 4 * A * C)) / (2 * A);
	//currentTubePoint.suspension = max(root1, root2);
	//纤维束起始段对应机器路径
	//if (currentTubePointIterator._Myoff == 0) { 
	/*	currentTubePoint.suspension = 0;
	}*/
	
	float track_x = currentTubePoint.x + currentTubePoint.tangent.x * currentTubePoint.suspension;
	float track_y = currentTubePoint.y + currentTubePoint.tangent.y * currentTubePoint.suspension;
	float track_z = currentTubePoint.z + currentTubePoint.tangent.z * currentTubePoint.suspension;

	//纤维束终点段对应机器路径。吐丝嘴位置不能超过两端，到达端点时要把纤维束“压”到芯模上(这部分是冗余段的事情)
	if (track_z <-0.01 || track_z >(tubeModel.length + 0.01)) {
		if (final_position.initialized == 0) {
			struct tubePathCoord& pre = *(currentTubePointIterator -1);
			float a = pre.tangent.x, b = pre.x, c = pre.tangent.y, d = pre.y;
			pre.suspension = suspensionCompute(shell_a, shell_b, shell_r, payeye.pm_left_distance, b, d, a, c);
			float pre_z = pre.z + pre.tangent.z * pre.suspension;
			float pre_x = pre.x + pre.tangent.x * pre.suspension;
			float pre_y = pre.y + pre.tangent.y * pre.suspension;

			vec3 v = { track_x - pre_x,track_y - pre_y,track_z - pre_z };
			float k = track_z < -0.01 ? (-pre_z / v.z) : ((tubeModel.length - pre_z) / v.z);

			track_z = pre_z + k * v.z;
			track_x = pre_x + k * v.x;
			track_y = pre_y + k * v.y;
			final_position.x = pre_x + k * v.x;
			final_position.y = pre_y + k * v.y;
			final_position.z = pre_z + k * v.z;
			final_position.initialized = 1;
		}
		else {
			track_x = final_position.x;
			track_y = final_position.y;
			track_z = final_position.z;
		}
	}
	tempTubeTrack.x = payeye.pm_distance - sqrt(track_x * track_x + track_y * track_y);
	tempTubeTrack.z = track_z;
	tempTubeTrack.spindleAngle = atan2(track_y, track_x) >= 0 ? atan2(track_y, track_x) : atan2(track_y, track_x) + 2 * PI;
	vec3 rotatedTangent;//平放时的切线旋转后，计算其对应的吐丝嘴角度
	CMfcogl1Doc::vectorRotateAroundZ(currentTubePoint.tangent, tempTubeTrack.spindleAngle, rotatedTangent,currentTubePoint.direction);
	tempTubeTrack.swingAngle = atan(rotatedTangent.z / rotatedTangent.y);
}

void CMfcogl1Doc::vectorRotateAroundZ(const vec3& currentVector,const float& angle,vec3& rotatedVector,float direction){
	//x-y平面旋转矩阵
	if (abs(currentVector.z - direction * cos(tubeModel.angle)) < 0.001) {
		rotatedVector.x = cos(angle) * currentVector.x + sin(angle) * currentVector.y;
		rotatedVector.y = -sin(angle) * currentVector.x + cos(angle) * currentVector.y;
		rotatedVector.z = currentVector.z;
		if (rotatedVector.y > 0)rotatedVector.y = -0.0001;
	}
	else {
		rotatedVector.z = direction * 100;
		rotatedVector.y = -1;
	}
}

void CMfcogl1Doc::_fill_the_gap(track* preTrack,track* curTrack, std::vector<data>& in,int no) {
	char BufData[100];
	float diff = abs(preTrack->z - curTrack->z)/2;
	float tmp = curTrack->spindleAngle - preTrack->spindleAngle;
	if (tmp > PI) { tmp -= 2 * PI; }
	float diff_spindle = (tmp) / diff;
	float diff_z = (curTrack->z - preTrack->z) / diff;
	float diff_x = (curTrack->x - preTrack->x) / diff;
	float diff_swing = (curTrack->swingAngle - preTrack->swingAngle) / diff;
	for (float i = 1; i < diff; i++) {
		data tmp = { (preTrack->spindleAngle + i * diff_spindle) * 180.0 / PI, preTrack->z + i * diff_z, preTrack->x + i * diff_x, (preTrack->swingAngle + i * diff_swing) * 180.0 / PI, payeye.mandrel_speed,no};
		if (tmp.a < 0) tmp.a += 360;
		in.push_back(tmp);
	}
}

void CMfcogl1Doc::calData(std::vector<data>& in){
	track* preTrack = nullptr;
	for (auto line = TubeTrackList->begin(); line < TubeTrackList->end(); line++) {
		int no = (int)ceil(float(line - TubeTrackList->begin()) / 2.0f) + 1;
		float just_spindleAngle;
		//draw this line
		for (auto track = (*line)->begin(); track < (*line)->end(); track++) {
			if (preTrack == nullptr) {
				preTrack = &(*track);
			}
			else {
				if (abs(preTrack->z - track->z) < 0.01 && abs(preTrack->spindleAngle - track->spindleAngle) < 0.01 && abs(preTrack->x - track->x) < 0.01) {
					continue;
				}
				preTrack = &(*track);
			}
			float currentNormal[3] = { 0,cos(track->swingAngle),sin(track->swingAngle) };
			float trackMandrelLength =payeye.pm_distance - track->x;//吐丝嘴到芯模中心轴的垂直距离
			float trackPoint[3] = { trackMandrelLength * cos(track->spindleAngle), trackMandrelLength * sin(track->spindleAngle), track->z };
			data tmp = { track->spindleAngle * 180.0 / PI, track->z, track->x, track->swingAngle * 180.0 / PI, payeye.mandrel_speed, no };
			in.push_back(tmp);
		}
	}
}

void CMfcogl1Doc::OnSaveTrackTube() {
	if (m_bPayeyeComplete == true) {
		//firstly, out something.
		char szFilters[] =
			"csv File(*.csv)\0*.csv\0"\
			"Text File(*.txt)\0*.txt\0"\
			"All Typle(*.*)\0*.*\0" \
			"\0";
		//当过滤器或者默认构造参数赋值较少情况下，
		//使用构造函数修改对话框初始状态可能更好，这过滤器较多
		CFileDialog FileDlg(FALSE, "txt", _T("Test"));
		FileDlg.m_ofn.lpstrTitle = "Save File";
		FileDlg.m_ofn.lpstrFilter = szFilters;

		if (IDOK == FileDlg.DoModal())
		{
			std::vector<data> in;
			calData(in);
			CFile File(FileDlg.GetPathName(), CFile::modeCreate | CFile::modeReadWrite);
			char BufData[100];
			sprintf(BufData, "芯模端头冗余,吐丝嘴-旋转轴距离,吐丝嘴-钉圈距离,数据行数\n%.1f, %.1f, %.1f, %d\n", tubeModel.redundance, payeye.pm_distance, payeye.pm_left_distance, in.size());
			File.Write(BufData, strlen(BufData));
			sprintf(BufData, "芯模宽度,芯模高度,芯模长度,缠绕角,纤维带宽\n%.1f, %.1f, %.1f, %.1f, %.1f\n",2.0f*(tubeModel.a+ tubeModel.r),2.0f*(tubeModel.b+ tubeModel.r),tubeModel.length,tubeModel.angle*180.0f/PI,tubeModel.width);
			File.Write(BufData, strlen(BufData));
			sprintf(BufData,"芯模旋转,吐丝嘴位移,吐丝嘴探出,吐丝嘴旋转,电机速度,纤维编号\n");
			File.Write(BufData, strlen(BufData));
			auto i = in.begin();
			sprintf(BufData, "%.1f, %.1f, %.1f, %.1f, %.1f, %d\n", i->a, i->b, i->c, i->d, i->e,i->no);
			File.Write(BufData, strlen(BufData));
			for (auto i = in.begin()+1; i != in.end(); i++){
				auto p = i - 1;
				float tmp_angle = p->a - i->a;
				if (tmp_angle < -180) {
					tmp_angle += 360.0;
				}
				sprintf(BufData, "%.5f, %.5f, %.5f, %.5f, %.5f, %d\n", -tmp_angle, i->b - p->b, i->c - p->c, i->d - p->d, i->e - p->e, i->no);
				//sprintf(BufData, "%.5f, %.5f, %.5f, %.5f, %.5f, %d\n", i->a, i->b , i->c , i->d , i->e ,i->no);
				File.Write(BufData, strlen(BufData));
			}
			File.Flush();
			File.Close();
		}
		AfxMessageBox("Saved.");
	}
	else {
		AfxMessageBox("Please Compute Track Path Firstly!");
	}
}

//接受压力容器的纤维路径参数
void CMfcogl1Doc::OnOpenFiberPathControlCylinderParametersDlg() {
	CDlgFiberPathControlsCylinder fpCylinderDlg;
	if (IDOK == fpCylinderDlg.DoModal()) {
		ResetWndDesign();
		cylinderModel.angle = fpCylinderDlg.m_dlg_cylinder_winding_angle * PI / 180.0;
		cylinderModel.width = fpCylinderDlg.m_dlg_cylinder_band_width;
		cylinderModel.slippage_coefficient = fpCylinderDlg.m_dlg_cylinder_slippage_coefficient;//μ
		cutNum = fpCylinderDlg.m_dlg_cylinder_cut_num;
		cylinderWindingAlgorithm = fpCylinderDlg.m_dlg_cylinder_winding_algorithm;
		m_bCanComputing = true;

		CMfcogl1View* pView;
		POSITION pos = GetFirstViewPosition();
		pView = (CMfcogl1View*)GetNextView(pos);

		cylinderModel.left_length = pView->m_view_cylinder_left_length;
		cylinderModel.left_radius = pView->m_view_cylinder_left_radius;
		cylinderModel.middle_length = pView->m_view_cylinder_middle_length;
		cylinderModel.middle_radius = pView->m_view_cylinder_middle_radius;
		cylinderModel.right_length = pView->m_view_cylinder_right_length;
		cylinderModel.right_radius = pView->m_view_cylinder_right_radius;
		cylinderModel.round = 2.0F * PI * cylinderModel.middle_radius;
		cylinderModel.left_using_coef = cylinderModel.slippage_coefficient;
		cylinderModel.left_slippage_point = cylinderModel.slippage_coefficient;
		cylinderModel.right_using_coef = cylinderModel.slippage_coefficient;
		cylinderModel.right_slippage_point = cylinderModel.slippage_coefficient;

		void (CMfcogl1Doc:: * leftCylinder)(std::deque <struct cylinderPathCoord> *singlePathList, cylinderPathCoord & currentPoint, bool show);
		void (CMfcogl1Doc:: * rightCylinder)(std::deque <struct cylinderPathCoord> *singlePathList, cylinderPathCoord & currentPoint, bool show);
		void (CMfcogl1Doc:: * middleCylinder)(std::deque <struct cylinderPathCoord> *singlePathList, cylinderPathCoord & currentPoint, bool show);
		middleCylinder = &CMfcogl1Doc::OnRenderMiddleCylinder;
		if (cylinderWindingAlgorithm == "GEODESIC") {
			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoid;
			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoid;
		}
		else if (cylinderWindingAlgorithm == "FIXED_ANGLE") {
			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoidFixedAngle;
			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoidFixedAngle;
		}
		else if (cylinderWindingAlgorithm == "NON_GEODESIC") {
			//同一根母线上不同初始位置相同缠绕角的半测地线与端头交汇点
			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic;
			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoidNonGeodesic;
		}
		else if (cylinderWindingAlgorithm == "AUTO") {
			//debug_show("now auto~");
			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoidAuto;
			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoidAuto;
			middleCylinder = &CMfcogl1Doc::OnRenderMiddleCylinderAuto;
		}

		//计算合适的切点数(缠绕一周出发点的总量)，跳跃数(两次出发点序号的差值
		float real_width = 0, total_theta = 0;
		cylinderModel.width = fpCylinderDlg.m_dlg_cylinder_band_width;
		real_width = 0.1*cylinderModel.width / abs(cos(cylinderModel.angle));
		M0 = ceil(cylinderModel.round / real_width);//实际剖分段数
		interval = cylinderModel.round / M0;//剖分步长
		jumpNum = (M0 + 1) / cutNum;
		jumpAngle = (float)(jumpNum) * 2.0 * PI / (float)M0;
		std::deque<data> quoters;
		//实际切面宽
		float _theta;
		float min_diff=999;
		float real_left_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));
		for (float x = cylinderModel.left_length; x > 0; x -= 5) {
			float y = sqrt(1 - pow(cylinderModel.left_length-x,2) / (real_left_length * real_left_length)) * cylinderModel.middle_radius;
			global_cylinder_coord = cylinderPathCoord(x, y, 0.0f);
			min_diff = 999;
			float left_cut_theta;
			float left_using_coef;
			for (cylinderModel.left_using_coef = cylinderModel.slippage_coefficient; cylinderModel.left_using_coef > -cylinderModel.slippage_coefficient; cylinderModel.left_using_coef -= 0.002) {
				auto tmp = global_cylinder_coord;
				(this->*leftCylinder)(nullptr, tmp, false);
				if (abs(global_left_min_r - cylinderModel.left_radius) < min_diff) {
					min_diff = abs(global_left_min_r - cylinderModel.left_radius);
					left_using_coef = cylinderModel.left_using_coef;
					left_cut_theta = global_left_cut_theta;//global_left_cut_theta:一条纤维中最接近切点的theta；left_cut_theta：本组所有纤维中最接近切点的theta
				}
			}
			cylinderModel.left_using_coef = left_using_coef;
			if (min_diff<0.1) {
				// x y left_using_coef left_cut_theta
				struct data d = { x,y,left_using_coef,180.0 * left_cut_theta / PI,0,0 };
				quoters.push_back(d);
				OnComputeFiberPathCylinder();
			}
		}

	/*	for (; cylinderModel.left_slippage_point > 0  && i==0; cylinderModel.left_slippage_point -= 0.01) {
			for (cylinderModel.left_using_coef = cylinderModel.slippage_coefficient; i==0&&cylinderModel.left_using_coef > -cylinderModel.slippage_coefficient; cylinderModel.left_using_coef -= 0.01) {
				cylinderPathCoord currentPoint(cylinderModel.left_length, cylinderModel.middle_radius, 0.0f);
				(this->*leftCylinder)(nullptr, currentPoint, false);
				(this->*middleCylinder)(nullptr, currentPoint, false);
				auto tmpPoint = currentPoint;
				//_theta = currentPoint.currentTheta;
				//while (_theta > 2 * PI) _theta -= 2 * PI;
				if (abs(global_left_min_r - cylinderModel.left_radius) < 0.05) {
					for (cylinderModel.right_slippage_point = cylinderModel.slippage_coefficient; cylinderModel.right_slippage_point > 0; cylinderModel.right_slippage_point -= 0.01) {
						for (cylinderModel.right_using_coef = cylinderModel.slippage_coefficient; cylinderModel.right_using_coef > -cylinderModel.slippage_coefficient; cylinderModel.right_using_coef -= 0.01) {
							currentPoint = tmpPoint;
							(this->*rightCylinder)(nullptr, currentPoint, false);
							(this->*middleCylinder)(nullptr, currentPoint, false);
							if (abs(global_right_min_r - cylinderModel.right_radius) <0.05) {
								OnComputeFiberPathCylinder();
								break;
							}
						}
					}
				}
			}
		}*/

	//do {
	//		cylinderModel.left_using_coef -= 0.01;
	//		cylinderModel.width = fpCylinderDlg.m_dlg_cylinder_band_width;
	//		real_width = cylinderModel.width / abs(cos(cylinderModel.angle));
	//		M0 = ceil(cylinderModel.round / real_width);//实际剖分段数
	//		interval = cylinderModel.round / M0;//剖分步长
	//		float left_min_r;
	//		//while ((M0 + 1) % cutNum != 0) {//缩小纤维半径以满足切点数约束
	//		//	cylinderModel.width -= 0.05;
	//		//	real_width = cylinderModel.width / abs(cos(cylinderModel.angle));
	//		//	M0 = ceil(cylinderModel.round / real_width);
	//		//	interval = cylinderModel.round / M0;
	//		//}

	//		jumpNum = (M0 + 1) / cutNum;
	//		jumpAngle = (float)(jumpNum) * 2.0 * PI / (float)M0;

	//		void (CMfcogl1Doc:: * leftCylinder)(std::deque <struct cylinderPathCoord> * singlePathList, cylinderPathCoord & currentPoint, bool show);
	//		void (CMfcogl1Doc:: * rightCylinder)(std::deque <struct cylinderPathCoord> * singlePathList, cylinderPathCoord & currentPoint, bool show);
	//		void (CMfcogl1Doc:: * middleCylinder)(std::deque <struct cylinderPathCoord> * singlePathList, cylinderPathCoord & currentPoint, bool show);

	//		middleCylinder = &CMfcogl1Doc::OnRenderMiddleCylinder;
	//		if (cylinderWindingAlgorithm == "GEODESIC") {
	//			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoid;
	//			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoid;
	//		}
	//		else if (cylinderWindingAlgorithm == "FIXED_ANGLE") {
	//			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoidFixedAngle;
	//			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoidFixedAngle;
	//		}
	//		else if (cylinderWindingAlgorithm == "NON_GEODESIC") {
	//			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic;
	//			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoidNonGeodesic;
	//		}
	//		else if (cylinderWindingAlgorithm == "AUTO") {
	//			//debug_show("now auto~");
	//			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoidAuto;
	//			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoidAuto;
	//			middleCylinder = &CMfcogl1Doc::OnRenderMiddleCylinderAuto;
	//		}
	//		cylinderPathCoord currentPoint(cylinderModel.left_length, cylinderModel.middle_radius, 0.0f);
	//		(this->*leftCylinder)(nullptr, currentPoint, false);
	//		(this->*middleCylinder)(nullptr, currentPoint, false);

	//		(this->*rightCylinder)(nullptr, currentPoint, false);
	//		(this->*middleCylinder)(nullptr, currentPoint, false);
	//		_theta = currentPoint.currentTheta;
	//		while (_theta > 2 * PI) _theta -= 2 * PI;
	//		//将下一条纤维的起始点(currentStartPoint)置于下一个切点上，再将当前点(currentPoint)置于起始点，使得每条纤维的起始位置标准化
	//} while (/*abs(jumpAngle - _theta) > 0.001 && abs(2 * PI - jumpAngle - _theta)>0.001*/abs(global_left_min_r-cylinderModel.left_radius)>0.1);
	CString STemp;
	STemp.Format(_T("纱宽(修正) = %.3f mm\n等分数=%d\n缠绕角 = %.4f degree\n切点数=%d\n跳跃数=%d\n算法: %s\n左极限 %.2f"), real_width, M0, cylinderModel.angle * 180 / PI, cutNum, jumpNum, cylinderWindingAlgorithm, cylinderModel.right_slippage_point);
	quoters;
	//debug_show(cylinderModel.left_using_coef);
	//debug_show(cylinderModel.left_slippage_point);
	AfxMessageBox(STemp);
	}
}


//计算纤维路径
void CMfcogl1Doc::OnComputeFiberPathCylinder() {
	CMfcogl1Doc::CleanTubeMemory();
	if (m_bCanComputing == true){
		CylinderPointList = new std::deque<struct cylinderPathCoord>;//CylinderTubePoint的双向链，用之前生成一条纤维的方法生成整个连续纤维
		windingPathStep = 1.6;//计算的步长
		//generate GL list
		//gl path list +1 para tmp gl path
		GLfloat matDiff[4] = { 0.2F, 0.2F, 0.2F, 0.80F };
		static int path_id = FIBER_PATH_LIST;
		if (cylinderWindingAlgorithm == "GEODESIC") {
			glNewList(path_id++, GL_COMPILE); //FIBER_PATH_LIST     2
			matDiff[1] = 0.8F;
		}
		else if (cylinderWindingAlgorithm == "FIXED_ANGLE") {
			glNewList(path_id++, GL_COMPILE); //FIBER_PATH_LIST     2
			matDiff[0] = 1.0F;
		}
		else if (cylinderWindingAlgorithm == "NON_GEODESIC") {
			glNewList(path_id++, GL_COMPILE); //FIBER_PATH_LIST     2
			matDiff[2] = 0.8F;
		}
		else if (cylinderWindingAlgorithm == "AUTO") {
			glNewList(path_id++, GL_COMPILE); //FIBER_PATH_LIST     2
			matDiff[2] = 1.0F;
		}
		GLfloat matSpec[4] = { 0.30F, 0.30F, 0.30F, 0.30F };
		GLfloat matShine = 60.00F;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
		glMaterialf(GL_FRONT, GL_SHININESS, matShine);

		void (CMfcogl1Doc:: * leftCylinder)(std::deque <struct cylinderPathCoord> * singlePathList, cylinderPathCoord & currentPoint, bool show);
		void (CMfcogl1Doc:: * rightCylinder)(std::deque <struct cylinderPathCoord> * singlePathList, cylinderPathCoord & currentPoint, bool show);
		void (CMfcogl1Doc:: * middleCylinder)(std::deque <struct cylinderPathCoord> * singlePathList, cylinderPathCoord & currentPoint, bool show);

		if (cylinderWindingAlgorithm == "GEODESIC") {
			leftCylinder =&CMfcogl1Doc::OnRenderLeftEllipsoid;
			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoid;
			middleCylinder = &CMfcogl1Doc::OnRenderMiddleCylinder;
		}
		else if (cylinderWindingAlgorithm == "FIXED_ANGLE") {
			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoidFixedAngle;
			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoidFixedAngle;
			middleCylinder = &CMfcogl1Doc::OnRenderMiddleCylinder;
		}
		else if (cylinderWindingAlgorithm == "NON_GEODESIC") {
			leftCylinder =&CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic;
			rightCylinder =&CMfcogl1Doc::OnRenderRightEllipsoidNonGeodesic;
			middleCylinder = &CMfcogl1Doc::OnRenderMiddleCylinder;
		}
		else if (cylinderWindingAlgorithm == "AUTO") {
			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoidAuto;
			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoidAuto;
			middleCylinder = &CMfcogl1Doc::OnRenderMiddleCylinderAuto;
		}
		cylinderPathCoord currentStartPoint(cylinderModel.left_length, cylinderModel.middle_radius, 0.0f);
		cylinderPathCoord currentPoint(global_cylinder_coord);
		for (int t = 0; t < 1; t++) {//M0:等分数 改了
			(this->*leftCylinder)(CylinderPointList, currentPoint,true);
		/*	(this->*middleCylinder)(CylinderPointList, currentPoint,true);
			(this->*rightCylinder)(CylinderPointList, currentPoint,true);
			(this->*middleCylinder)(CylinderPointList, currentPoint,true);*/

			//将下一条纤维的起始点(currentStartPoint)置于下一个切点上，再将当前点(currentPoint)置于起始点，使得每条纤维的起始位置标准化
			currentStartPoint.currentTheta += jumpAngle;
			currentStartPoint.y= cylinderModel.middle_radius * cos(currentStartPoint.currentTheta);
			currentStartPoint.z = cylinderModel.middle_radius * sin(currentStartPoint.currentTheta);
			currentPoint = currentStartPoint;
		}
		glEndList();
		//统计总的坐标数
		int size = CylinderPointList->size();
		m_WindingCount[0] = size;
		//AfxMessageBox("纤维路径计算完毕\ntube Track Computation Finished.");
		m_bFiberPathComplete = true;
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

void CMfcogl1Doc::OnRenderMiddleCylinder(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {

	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//线段起始的缠绕角α(弧度)
	double theta = 0;//线段起始的theta相位
	double dcurve = d_curve; // dcurve * sin(angle) = r * dtheta
	double adjust = cylinderModel.middle_radius / sin(alpha);
	double dthera = dcurve / adjust;;
	while (currentPoint.x >= cylinderModel.left_length && currentPoint.x <= (cylinderModel.left_length + cylinderModel.middle_length)) {
		currentPoint.x += currentPoint.direction * dcurve * cos(alpha);
		currentPoint.y = cylinderModel.middle_radius * cos(currentPoint.currentTheta);
		currentPoint.z = cylinderModel.middle_radius * sin(currentPoint.currentTheta);
		 if(show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);

		currentPoint.currentTheta += dthera;
	}
	glEnd();
}

void CMfcogl1Doc::OnRenderLeftEllipsoid(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dcurve = d_curve;
	double diff_gx = 0;
	while (currentPoint.x <= (cylinderModel.left_length)) {
		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		double dx = dcurve * cos(alpha) * cos(atan(diff_gx));
		currentPoint.x += currentPoint.direction * dx;
		double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)) / pow(cylinderModel.left_length, 2);
		double gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(currentPoint.x - cylinderModel.left_length, 2), 0));
		diff_gx = -B * (cylinderModel.left_length - currentPoint.x) / gx;
		currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
		currentPoint.y = gx * cos(currentPoint.currentTheta);
		currentPoint.z = gx * sin(currentPoint.currentTheta);
		alpha += -diff_gx * tan(alpha) * dx / (gx);
	}
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();
}

void CMfcogl1Doc::OnRenderLeftEllipsoidFixedAngle(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dcurve = d_curve;
	double diff_gx = 0;

	//为了控制误差，以alpha代替实际缠绕角参与计算，实际缠绕角不改变
	float real_left_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));
	while (currentPoint.x <= (cylinderModel.left_length)) {
		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		double dx = dcurve * cos(alpha) * cos(atan(diff_gx));
		currentPoint.x += currentPoint.direction * dx;
		double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)) / pow(cylinderModel.left_length, 2);
		double gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(currentPoint.x - cylinderModel.left_length, 2), 0));
		diff_gx = -B * (cylinderModel.left_length - currentPoint.x) / gx;
		currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
		currentPoint.y = gx * cos(currentPoint.currentTheta);
		currentPoint.z = gx * sin(currentPoint.currentTheta);

		float sv = sin(alpha);
		float cv = cos(alpha);
		float tmpy2b = (cylinderModel.left_length - currentPoint.x) * (cylinderModel.left_length - currentPoint.x) / (real_left_length * real_left_length);
		float tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_left_length * real_left_length);
		float numerator = sv * (cylinderModel.left_length - currentPoint.x) / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
		float denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
		float lambda = numerator / denominator;
		if (lambda >= cylinderModel.slippage_coefficient) {
			alpha += -diff_gx * tan(alpha) * dx / (gx);
		}
	}

	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();
}

//void CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
//	glBegin(GL_LINE_STRIP);
//	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
//	double dcurve = d_curve;
//	double dr = 0;
//	double dx, dtheta, r;
//	while (currentPoint.x <= (cylinderModel.left_length)) {
//		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
//		//x:轴线长度 
//		auto adjoint = [=, &r, &dr](double in_x, double alpha) {
//			double lambda = cylinderModel.left_using_coef;
//			double cos2a = cos(alpha) * cos(alpha);
//			double sin2a = sin(alpha) * sin(alpha);
//			double tana = tan(alpha);
//			double a = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));
//			double b = cylinderModel.middle_radius;
//			double a2 = a * a;
//			double x = cylinderModel.left_length - in_x;//
//			double x2 = x * x;
//			double _tmp = (1 - x2 / a2) > 0 ? sqrt(1 - x2 / a2) : sqrt(x2 / a2 - 1);
//			r = b * _tmp;//截面半径 小
//			dr = -(b * x) / (a2 * _tmp);//dr/dx 截面半径关于轴线长x的微分 
//			double dd_r = -b / (a2 * _tmp) - b * x2 / (a2 * a2 * _tmp * abs((1 - x2 / a2)));
//			double A = sqrt(1 + dr * dr);
//			////dalpha/dx
//			//return  lambda * ((-dd_r * r * cos2a + sin2a) / (pow(A, 3) * r * cos2a + A * r * sin2a)) - dr * tana / r;
//			//r = b * sqrt((1 - x2 / a2));//截面半径 小
//			//dr = -(b * x) / (a2 * sqrt(1 - x2 / a2));//dr/dx 截面半径关于轴线长x的微分 
//			//double dd_r = -b / (a2 * sqrt(1 - x2 / a2)) - b * x2 / (a2 * a2 * sqrt(pow(1 - x2 / a2, 3)));
//			//double A = sqrt(1 + dr * dr);
//			//dalpha/dx
//			return [&dx] {return dx > 0 ? -1 : 1; }() * lambda * ((-dd_r * r * cos2a + sin2a) / (pow(A, 3) * r * cos2a + A * r * sin2a)) - dr * tana / r;
//		};
//
//		dx = dcurve * cos(alpha) * cos(atan(dr)); //dr(截面半径关于轴线的微分) = ds(子午线)*cos(φ) = ds(弧长)*cos(α)*cos(切平面法向与轴线夹角)
//		
//			//龙格库塔
//			//double RK1 = adjoint(currentPoint.x, alpha);
//			//double RK2 = adjoint(currentPoint.x + currentPoint.direction * dx / 2, alpha + RK1 * dx / 2);
//			//double RK3 = adjoint(currentPoint.x + currentPoint.direction * dx / 2, alpha + RK2 * dx / 2);
//			//double RK4 = adjoint(currentPoint.x + currentPoint.direction * dx, alpha + RK3 * dx);//r d_r会在这里被更新
//		double RK = adjoint(currentPoint.x + currentPoint.direction * dx, alpha);//get dalpha && change r,dr(dr/dx)
//
//		dtheta = tan(alpha) * sqrt(1 + dr * dr) / r;//回转体旋转角
//		currentPoint.x += currentPoint.direction * dx;
//		currentPoint.currentTheta += dtheta * dx;
//		currentPoint.y = r * cos(currentPoint.currentTheta);
//		currentPoint.z = r * sin(currentPoint.currentTheta);
//		alpha += dx * RK;
//		//d_alpha 由四阶龙格库塔决定
//		//alpha += dx * (RK1 + 2 * RK2 + 2 * RK3 + RK4) / 6.0;
//	}
//	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
//	glEnd();
//}

void CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dcurve = d_curve;
	double dr = 0;
	double dx, dtheta, r;
	float min_r = sqrt(currentPoint.y * currentPoint.y + currentPoint.z * currentPoint.z);
	float cut_theta = 0;
	float real_left_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));

	while (currentPoint.x <= (cylinderModel.left_length)) {
		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		//x:轴线长度 
		auto adjoint = [=, &r, &dr](double in_x, double alpha) {
			double lambda = cylinderModel.left_using_coef;
			double cos2a = cos(alpha) * cos(alpha);
			double sin2a = sin(alpha) * sin(alpha);
			double tana = tan(alpha);
			double a = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));
			double b = cylinderModel.middle_radius;
			double a2 = a * a;
			double x = cylinderModel.left_length - in_x;//
			double x2 = x * x;
			double _tmp = (1 - x2 / a2) > 0 ? sqrt(1 - x2 / a2) : sqrt(x2 / a2 - 1);
			r = b * _tmp;//截面半径 小
			dr = -(b * x) / (a2 * _tmp);//dr/dx 截面半径关于轴线长x的微分 
			double dd_r = -b / (a2 * _tmp) - b * x2 / (a2 * a2 * _tmp * abs((1 - x2 / a2)));
			double A = sqrt(1 + dr * dr);
			return [&dx] {return dx > 0 ? -1 : 1; }() * lambda * ((-dd_r * r * cos2a + sin2a) / (pow(A, 3) * r * cos2a + A * r * sin2a)) - dr * tana / r;
		};

		dx = dcurve * cos(alpha) * cos(atan(dr)); //dr(截面半径关于轴线的微分) = ds(子午线)*cos(φ) = ds(弧长)*cos(α)*cos(切平面法向与轴线夹角)

		double RK = adjoint(currentPoint.x + currentPoint.direction * dx, alpha);//get dalpha && change r,dr(dr/dx)
		float sv = sin(alpha);
		float cv = cos(alpha);
		float tmpy2b = (cylinderModel.left_length - currentPoint.x) * (cylinderModel.left_length - currentPoint.x) / (real_left_length * real_left_length);
		float tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_left_length * real_left_length);
		float numerator = sv * (cylinderModel.left_length - currentPoint.x) / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
		float denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
		float current_lambda = numerator / denominator;
		if (current_lambda < cylinderModel.left_slippage_point) {
			//RK = 0;
		}
		dtheta = tan(alpha) * sqrt(1 + dr * dr) / r;//回转体旋转角
		currentPoint.x += currentPoint.direction * dx;
		currentPoint.currentTheta += dtheta * dx;
		currentPoint.y = r * cos(currentPoint.currentTheta);
		currentPoint.z = r * sin(currentPoint.currentTheta);
		if (r < min_r) {
			min_r = r;
			cut_theta = currentPoint.currentTheta;
		}
		alpha += dx * RK;
	}
	global_left_min_r = min_r;
	global_left_cut_theta = cut_theta;
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();

}

void CMfcogl1Doc::OnRenderRightEllipsoid(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dcurve = d_curve;//弧长步长
	double diff_gx = 0;
	while (currentPoint.x >= (cylinderModel.left_length + cylinderModel.middle_length)) {
		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		double dx = dcurve * cos(alpha) * cos(atan(diff_gx));
		currentPoint.x += currentPoint.direction * dx;
		double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)) / pow(cylinderModel.right_length, 2);
		double gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length, 2), 0));
		diff_gx = -B * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / gx;
		currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
		currentPoint.y = gx * cos(currentPoint.currentTheta);
		currentPoint.z = gx * sin(currentPoint.currentTheta);
		alpha += -diff_gx * tan(alpha) * dx / (gx);
	}
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();
}

void CMfcogl1Doc::OnRenderRightEllipsoidFixedAngle(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dcurve = d_curve;
	double diff_gx = 0;
	float real_right_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.right_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)));
	while (currentPoint.x >= (cylinderModel.left_length + cylinderModel.middle_length)) {
		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		double dx = dcurve * cos(alpha) * cos(atan(diff_gx));
		currentPoint.x += currentPoint.direction * dx;
		double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)) / pow(cylinderModel.right_length, 2);
		double gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length, 2), 0));
		diff_gx = -B * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / gx;
		currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
		currentPoint.y = gx * cos(currentPoint.currentTheta);
		currentPoint.z = gx * sin(currentPoint.currentTheta);

		float sv = sin(alpha);
		float cv = cos(alpha);
		float tmpy2b = (currentPoint.x-cylinderModel.left_length-cylinderModel.middle_length) * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / (real_right_length * real_right_length);
		float tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_right_length * real_right_length);
		float numerator = sv * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
		float denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
		float lambda = numerator / denominator;
		if (lambda >= cylinderModel.slippage_coefficient) {
			alpha += -diff_gx * tan(alpha) * dx / (gx);
		}
	}
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();
}

void CMfcogl1Doc::OnRenderRightEllipsoidNonGeodesic(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dcurve = d_curve;
	double d_r = 0;
	double dx, dtheta, r;
	float min_r = sqrt(currentPoint.y * currentPoint.y + currentPoint.z * currentPoint.z);
	float real_right_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.right_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)));
	while (currentPoint.x >= (cylinderModel.left_length + cylinderModel.middle_length)) {
		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);

		auto adjoint = [=, &r, &d_r](double in_x, double alpha) {
			double lambda = cylinderModel.right_using_coef;
			double cos2a = cos(alpha) * cos(alpha);
			double sin2a = sin(alpha) * sin(alpha);
			double tana = tan(alpha);
			double a = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.right_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)));
			double b = cylinderModel.middle_radius;
			double a2 = a * a;
			double x = in_x - cylinderModel.left_length - cylinderModel.middle_length;
			double x2 = x * x;
			r = b * sqrt((1 - x2 / a2));//截面半径
			d_r = -(b * x) / (a2 * sqrt(1 - x2 / a2));
			double dd_r = -b / (a2 * sqrt(1 - x2 / a2)) - b * x2 / (a2 * a2 * sqrt(pow(1 - x2 / a2, 3)));
			double A = sqrt(1 + d_r * d_r);
			return [&dx] {return dx > 0 ? -1 :  1; }() * lambda * ((-dd_r * r * cos2a + sin2a) / (pow(A, 3) * r * cos2a + A * r * sin2a)) - d_r * tana / r;
		};

		dx = dcurve * cos(alpha) * cos(atan(d_r));

		double RK = adjoint(currentPoint.x + currentPoint.direction * dx, alpha);

		float sv = sin(alpha);
		float cv = cos(alpha);
		float tmpy2b = (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / (real_right_length * real_right_length);
		float tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_right_length * real_right_length);
		float numerator = sv * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
		float denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
		float current_lambda = numerator / denominator;
		if (current_lambda < cylinderModel.right_slippage_point) {
			RK = 0;
		}
		dtheta = tan(alpha) * sqrt(1 + d_r * d_r) / r;
		currentPoint.x += currentPoint.direction * dx;
		currentPoint.currentTheta += dtheta * dx;
		currentPoint.y = r * cos(currentPoint.currentTheta);
		currentPoint.z = r * sin(currentPoint.currentTheta);
		if (r < min_r)min_r = r;
		alpha += dx * RK;
	}
	global_right_min_r = min_r;
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();
}

void CMfcogl1Doc::OnRenderLeftEllipsoidAuto(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show){
	
}
void CMfcogl1Doc::OnRenderRightEllipsoidAuto(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show){
	
}
void CMfcogl1Doc::OnRenderMiddleCylinderAuto(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show){
	
}

void CMfcogl1Doc::OnOpenFiberPathControlConeParametersDlg() {
	//get parameter...
}

void CMfcogl1Doc::OnComputeFiberPathCone() {
	//geodesic static-angle non-geodesic...
}