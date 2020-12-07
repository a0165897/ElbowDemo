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
#include "DlgFiberPathControlsCylinder.h"
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
	ON_COMMAND(IDM_FIBER_PATH_CONTROL_PARAMETERS, OnSwitchFiberPathControlDlg)//参数设置
	ON_COMMAND(IDM_COMPUTE_FIBER_PATH, OnSwitchComputeFiberPath)//纤维路径计算
	ON_COMMAND(IDM_COMPUTE_PAYEYE_TRACK, OnSwitchComputePayeye)//机器路径计算
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

//应该是重置下参数之类的
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
}

//纤维路径计算
void CMfcogl1Doc::OnSwitchComputeFiberPath() {
	if (m_isShowing == 1) {//tube
		OnComputeFiberPathTube();
	}
	else if (m_isShowing == 3) {//cylinder
		//cylidner is doing
		OnComputeFiberPathCylinder();
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
}

void CMfcogl1Doc::OnOpenFiberPathControlTubeParametersDlg() {
	CDlgFiberPathControlsTube fpTubeDlg;
	if (IDOK == fpTubeDlg.DoModal()) {
		ResetWndDesign();
		tubeModel.angle = fpTubeDlg.m_dlg_tube_winding_angle * PI / 180;
		tubeModel.width = fpTubeDlg.m_dlg_tube_band_width;
		cutNum = fpTubeDlg.m_dlg_tube_cut_num;

		payeye.mandrel_redundancy = fpTubeDlg.m_dlg_tube_mandrel_redundancy;
		payeye.mandrel_speed = fpTubeDlg.m_dlg_tube_mandrel_speed;
		payeye.pm_distance = fpTubeDlg.m_dlg_tube_pm_distance;
		payeye.pm_left_distance = fpTubeDlg.m_dlg_tube_pm_left_distance;

		m_bCanComputing = true;
		angleStep = (payeye.mandrel_speed * 2 * PI) / 10;// 1/10秒 转过多少角度

		CMfcogl1View* pView;
		POSITION pos = GetFirstViewPosition();
		pView = (CMfcogl1View*)GetNextView(pos);

		tubeModel.a = pView->m_view_tube_a;
		tubeModel.b = pView->m_view_tube_b;
		tubeModel.length = pView->m_view_tube_length;
		tubeModel.r = pView->m_view_tube_r;
		tubeModel.round = 2 * PI * tubeModel.r + 4 * tubeModel.a + 4 * tubeModel.b;//周长

		//计算合适的切点数(缠绕一周出发点的总量)，跳跃数(两次出发点序号的差值)
		float real_width = tubeModel.width / abs(cos(tubeModel.angle));//实际切面宽

		M0 = ceil(tubeModel.round / real_width);//实际剖分段数
		interval = tubeModel.round / M0;//剖分步长
		while ((M0 + 1) % cutNum != 0) {//缩小纤维半径以满足切点数约束
			tubeModel.width -= 0.05;
			real_width = tubeModel.width / abs(cos(tubeModel.angle));
			M0 = ceil(tubeModel.round / real_width);
			interval = tubeModel.round / M0;
		}
		jumpNum = (M0 + 1) / cutNum;

		CString STemp;
		STemp.Format(_T("纱宽(修正) = %.3f mm\n等分数=%d\n缠绕角 = %.3f degree\n切点数=%d\n跳跃数=%d"), real_width, M0, fpTubeDlg.m_dlg_tube_winding_angle, cutNum, jumpNum);
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
	TubeStartList.clear();
}

//生成起始点列表 在两个头上各绕一圈
int CMfcogl1Doc::OnGenerateTubeEdgeList(std::deque<struct tubePathPosition>& TubeList,float interval,int side) {
	float halfInterval = interval / 2;
	int i = 0;
	struct tubePathPosition p;
	for (i = 0; i * interval < tubeModel.a; i++) {
		p.x = i * interval;
		p.y = tubeModel.b + tubeModel.r;
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (side == 2) {
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
		if (side == 2) {
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
		if (side == 2) {
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
		if (side == 2) {
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
		if (side == 2) {
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
		if (side == 2) {
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
		if (side == 2) {
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
		if (side == 2) {
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
		if (side == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}

	return TubeList.size();
}

//方管纤维路径的计算
void CMfcogl1Doc::OnComputeFiberPathTube() {
	CMfcogl1Doc::CleanTubeMemory();
	if (m_bCanComputing == true){
		TubePointList = new std::deque<std::deque<struct tubePathCoord>*>;//TubePoint的双向链表指针的双向链表，内层表示一条纤维路径

		windingPathStep = 0.2;//计算的步长
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
		int totalNum = OnGenerateTubeEdgeList(TubeStartList,interval,2);
		OnGenerateTubeEdgeList(TubeEndList, windingPathStep,1);
		//计算循环
		while (TubeStartList.size() != 0  ) {
			std::deque<struct tubePathCoord>* currentPathList = new std::deque<struct tubePathCoord>;
			TubePointList->push_back(currentPathList);
			OnGeneratePosition(TubeStartList,currentPathList);
			OnRenderSinglePath(currentPathList);//计算本纤维路径的所有顶点位置并输入glList和TubePointList
		}
		glEndList();

		//插入两条纤维路径的连接段
		for (auto list = TubePointList->begin(); list < TubePointList->end()-1;list++ ) {
			std::deque<struct tubePathCoord>* currentPathList = new std::deque<struct tubePathCoord>;
			tubePathCoord begin = *(*list)->rbegin();
			tubePathCoord end = *(*(list+1))->begin();
			std::deque<tubePathPosition>::iterator closest_begin;
			std::deque<tubePathPosition>::iterator closest_end;
			float dis;
			dis= INFINITY;
			for (auto j = TubeEndList.begin(); j < TubeEndList.end(); j++) {
				float tmp_dis = (begin.x - j->x) * (begin.x - j->x) + (begin.y - j->y) * (begin.y - j->y);
				if (tmp_dis < dis) {
					dis = tmp_dis;
					closest_begin = j;
				}
			}
			dis = INFINITY;
			for (auto j = TubeEndList.begin(); j < TubeEndList.end(); j++) {
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
				for (auto i = closest_begin; i <TubeEndList.end(); i++) {
					tubePathCoord tmp = begin;
					tmp.x = (*i).x;
					tmp.y = (*i).y;
					currentPathList->push_back(tmp);
				}
				for (auto i = TubeEndList.begin(); i <= closest_end; i++) {
					tubePathCoord tmp = begin;
					tmp.x = (*i).x;
					tmp.y = (*i).y;
					currentPathList->push_back(tmp);
				}
			}
			list = TubePointList->insert(list+1, currentPathList);
		}

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

	double3 trackStart = { cos(-currentAngle), sin(-currentAngle),0 };

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
void CMfcogl1Doc::OnRenderSinglePath(std::deque<struct tubePathCoord>* singlePathList) {
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

inline int CMfcogl1Doc::outTubeEdge() {
	return (currentPosition.z < 0) ? -1 : (currentPosition.z >= tubeModel.length ? 1 : 0);
}

inline void CMfcogl1Doc::insertPoint(float* nextPoint) {
	glVertex3fv(nextPoint);
}
void CMfcogl1Doc::pushTubePathCoord(const float*nextPoint,const float normal_radian,const int direction, std::deque<struct tubePathCoord>* singlePathList){
	tempTubePathCoord.x = nextPoint[0];
	tempTubePathCoord.y = nextPoint[1];
	tempTubePathCoord.z = nextPoint[2];
	tempTubePathCoord.normal_radian = normal_radian;
	tempTubePathCoord.tangent.x = sin(tubeModel.angle) * cos(tempTubePathCoord.normal_radian - PI / 2);
	tempTubePathCoord.tangent.y = sin(tubeModel.angle) * sin(tempTubePathCoord.normal_radian - PI / 2);
	tempTubePathCoord.tangent.z = direction * cos(tubeModel.angle);
	tempTubePathCoord.direction = currentPosition.direction;
	tempTubePathCoord.index = currentPosition.index;
	singlePathList->push_back(tempTubePathCoord);
}

int CMfcogl1Doc::OnRenderLinePart(int state, std::deque<struct tubePathCoord>* singlePathList) {
	float nextPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	float startPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	tubeModel.curveLength = 0;
	int direction = currentPosition.direction;
	switch (state) {
	case 1:
		//glNormal3f(cos((360-2*segmentw)*3.14/180),sin((360-2*segmentw)*3.14/180),0.0f);		
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, PI / 2, direction, singlePathList);
		while (nextPoint[0] < tubeModel.a && 0 <= nextPoint[2] && nextPoint[2] <= tubeModel.length) {

			nextPoint[0] = startPoint[0] + tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[1] = startPoint[1];
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;

			//pushTubePathCoord(nextPoint, PI / 2, direction, singlePathList);

			tubeModel.curveLength += windingPathStep;
		}
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, PI / 2, direction, singlePathList);
		break;
	case 3:
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, 0, direction, singlePathList);
		while (-tubeModel.b < nextPoint[1] && 0 <= nextPoint[2] && nextPoint[2] <= tubeModel.length) {
			//insertPoint(nextPoint);
			nextPoint[0] = startPoint[0];
			nextPoint[1] = startPoint[1] - tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;

			//pushTubePathCoord(nextPoint, 0, direction, singlePathList);

			tubeModel.curveLength += windingPathStep;
		}
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, 0, direction, singlePathList);
		break;
	case 5:
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, -PI / 2, direction, singlePathList);
		while (-tubeModel.a < nextPoint[0] && 0 <= nextPoint[2] && nextPoint[2] <= tubeModel.length) {
			//insertPoint(nextPoint);

			nextPoint[0] = startPoint[0] - tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[1] = startPoint[1];
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;

			tempTubePathCoord.x = nextPoint[0];
			tempTubePathCoord.y = nextPoint[1];
			tempTubePathCoord.z = nextPoint[2];

			//pushTubePathCoord(nextPoint, -PI / 2, direction, singlePathList);

			tubeModel.curveLength += windingPathStep;
		}
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, -PI / 2, direction, singlePathList);
		break;
	case 7:
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, -PI, direction, singlePathList);
		while (nextPoint[1] < tubeModel.b && 0 <= nextPoint[2] && nextPoint[2] <= tubeModel.length) {
		//	insertPoint(nextPoint);

			nextPoint[0] = startPoint[0];
			nextPoint[1] = startPoint[1] + tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;

			//pushTubePathCoord(nextPoint, -PI, direction, singlePathList);

			tubeModel.curveLength += windingPathStep;
		}
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, -PI, direction, singlePathList);
		break;
	default:
		break;
	}

	updatePosition(nextPoint);
	if (outTubeEdge() == 0) {//from one part move into another neighbor part.   some how like a LSM.
		OnRenderCurvePart(state + 1,singlePathList);
	}
	else {
		return outTubeEdge();
	}
}

int CMfcogl1Doc::OnRenderCurvePart(int state, std::deque<struct tubePathCoord>* singlePathList) {
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

		//圆柱面上测地线 对弧面的三条边约束
		while (tubeModel.curveLength <= PI / 2 && 0 <= nextPoint[2] && nextPoint[2] <= tubeModel.length) {
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

		while (tubeModel.curveLength <= PI && 0 <= nextPoint[2] && nextPoint[2] <= tubeModel.length) {
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

		while (tubeModel.curveLength <= 1.5 * PI && 0 <= nextPoint[2] && nextPoint[2] <= tubeModel.length) {
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

		while (tubeModel.curveLength <= 2 * PI && 0 <= nextPoint[2] && nextPoint[2] <= tubeModel.length) {
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
	if (outTubeEdge() == 0) {
		OnRenderLinePart(state % 8 + 1,singlePathList);
	}
	else {
		return outTubeEdge();
	}
}

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
			for (auto currentTubePointIterator = (*line)->begin(); currentTubePointIterator < (*line)->end(); currentTubePointIterator++) {//纤维路径上每个点

				track tempTubeTrack;
				computeTubeTrack(currentTubePointIterator, tempTubeTrack);
				currentTrackList->push_back(tempTubeTrack);

				float trackMandrelLength = tubeModel.a + tubeModel.r + payeye.pm_distance - tempTubeTrack.x;//吐丝嘴到芯模中心轴的垂直距离
				float trackPoint[3] = { trackMandrelLength * cos(tempTubeTrack.spindleAngle), trackMandrelLength * sin(tempTubeTrack.spindleAngle), tempTubeTrack.z };
				glVertex3fv(trackPoint);
			}
			glEnd();
			//TODO:两条纤维路径连接过程中的机器路径
		}
		glEndList();

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

//计算出currentTubePoint的suspension(悬丝长度)和tempTubeTrack的四自由度参数
void CMfcogl1Doc::computeTubeTrack(std::deque<struct tubePathCoord>::iterator currentTubePointIterator, track& tempTubeTrack) {
	struct tubePathCoord& currentTubePoint = *currentTubePointIterator;
	float R = tubeModel.a + tubeModel.r + payeye.pm_distance;
	//(currentTubePoint.x + currentTubePoint.tangent.x * suspension) ^ 2 + (currentTubePoint.y + currentTubePoint.tangent.y * suspension) ^ 2 = R * R;    syms:suspension
	float a = currentTubePoint.tangent.x, b = currentTubePoint.x, c = currentTubePoint.tangent.y, d = currentTubePoint.y;
	float A = a * a + c * c;
	float B = 2 * a * b + 2 * c * d;
	float C = b * b + d * d - R * R;
	float root1 = (-B + sqrt(B * B - 4 * A * C)) / (2 * A);
	float root2 = (-B - sqrt(B * B - 4 * A * C)) / (2 * A);
	currentTubePoint.suspension = max(root1, root2);
	if (currentTubePoint.suspension < 0) {
		debug_show("悬丝长度计算错误");
		return;
	}
	//纤维束起始段对应机器路径
	if (currentTubePointIterator._Myoff == 0) {
		currentTubePoint.suspension = 0;
	}
	float track_x = currentTubePoint.x + currentTubePoint.tangent.x * currentTubePoint.suspension;
	float track_y = currentTubePoint.y + currentTubePoint.tangent.y * currentTubePoint.suspension;
	float track_z = currentTubePoint.z + currentTubePoint.tangent.z * currentTubePoint.suspension;

	//纤维束终点段对应机器路径。吐丝嘴位置不能超过两端，到达端点时要把纤维束“压”到芯模上
	if (track_z < 0) {
		track_z = 0;
		currentTubePoint.suspension = (track_z - currentTubePoint.z) / currentTubePoint.tangent.z;
		track_x = currentTubePoint.x + currentTubePoint.tangent.x * currentTubePoint.suspension;
		track_y = currentTubePoint.y + currentTubePoint.tangent.y * currentTubePoint.suspension;
	}
	else if (track_z > tubeModel.length) {
		track_z = tubeModel.length;
		currentTubePoint.suspension = (track_z - currentTubePoint.z) / currentTubePoint.tangent.z;
		track_x = currentTubePoint.x + currentTubePoint.tangent.x * currentTubePoint.suspension;
		track_y = currentTubePoint.y + currentTubePoint.tangent.y * currentTubePoint.suspension;
	}

	tempTubeTrack.x = tubeModel.a + tubeModel.r + payeye.pm_distance - sqrt(track_x * track_x + track_y * track_y);
	tempTubeTrack.z = track_z;
	tempTubeTrack.spindleAngle = atan2(track_y, track_x) >= 0 ? atan2(track_y, track_x) : atan2(track_y, track_x) + 2 * PI;
	double3 rotatedTangent;//平放时的切线旋转后，计算其对应的吐丝嘴角度
	CMfcogl1Doc::vectorRotateAroundZ(currentTubePoint.tangent, tempTubeTrack.spindleAngle, rotatedTangent);
	tempTubeTrack.swingAngle = atan(rotatedTangent.y / rotatedTangent.z);
}

void CMfcogl1Doc::vectorRotateAroundZ(const double3& currentVector,const float& angle,double3& rotatedVector){
	//x-y平面旋转矩阵
	rotatedVector.x = cos(angle) * currentVector.x + sin(angle) * currentVector.y;
	rotatedVector.y = -sin(angle) * currentVector.x + cos(angle) * currentVector.y;
	rotatedVector.z = currentVector.z;
}

//接受压力容器的纤维路径参数
void CMfcogl1Doc::OnOpenFiberPathControlCylinderParametersDlg() {
	CDlgFiberPathControlsCylinder fpCylinderDlg;
	if (IDOK == fpCylinderDlg.DoModal()) {
		ResetWndDesign();
		cylinderModel.angle = fpCylinderDlg.m_dlg_cylinder_winding_angle * PI / 180.0;
		cylinderModel.width = fpCylinderDlg.m_dlg_cylinder_band_width;
		cylinderModel.slippage_coefficient = fpCylinderDlg.m_dlg_cylinder_slippage_coefficient;
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

		//计算左右摩擦极限位置
		float tmp_x;
		float real_left_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));
		for (tmp_x = 0; tmp_x < real_left_length; tmp_x += 0.01) {
			//calculate the...
			float sv = sin(cylinderModel.angle);
			float cv = cos(cylinderModel.angle);
			float tmpy2b = tmp_x * tmp_x / (real_left_length * real_left_length);
			float tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_left_length * real_left_length);
			float numerator = sv * tmp_x / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
			float denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
			float lambda = numerator / denominator;
			if (abs(lambda - cylinderModel.slippage_coefficient) < 0.01) {
				cylinderModel.left_slippage_point = cylinderModel.left_length - tmp_x;
				break;
			}
		}
		float real_right_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.right_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)));
		for (tmp_x = 0; tmp_x < real_right_length; tmp_x += 0.01) {
			//calculate the...
			float sv = sin(cylinderModel.angle);
			float cv = cos(cylinderModel.angle);
			float tmpy2b = tmp_x * tmp_x / (real_right_length * real_right_length);
			float tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_right_length * real_right_length);
			float numerator = sv * tmp_x / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
			float denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
			float lambda = numerator / denominator;
			if (abs(lambda - cylinderModel.slippage_coefficient) < 0.01) {
				cylinderModel.right_slippage_point = cylinderModel.right_length + cylinderModel.middle_length + tmp_x;
				break;
			}
		}

		//计算合适的切点数(缠绕一周出发点的总量)，跳跃数(两次出发点序号的差值
		float real_width = 0, total_theta = 0;
		//实际切面宽
		//do {
		//	cylinderModel.angle -= 0.0001;
		//	cylinderModel.width = fpCylinderDlg.m_dlg_cylinder_band_width;
		//	real_width = cylinderModel.width / abs(cos(cylinderModel.angle));
		//	M0 = ceil(cylinderModel.round / real_width);//实际剖分段数
		//	interval = cylinderModel.round / M0;//剖分步长
		//	while ((M0 + 1) % cutNum != 0) {//缩小纤维半径以满足切点数约束
		//		cylinderModel.width -= 0.05;
		//		real_width = cylinderModel.width / abs(cos(cylinderModel.angle));
		//		M0 = ceil(cylinderModel.round / real_width);
		//		interval = cylinderModel.round / M0;
		//	}
		//	jumpNum = (M0 + 1) / cutNum;
		//	jumpAngle = (float)(jumpNum) * 2.0 * PI / (float)M0;
		//	total_theta = 0;
		//	float alpha = cylinderModel.angle, diff_gx = 0, theta = 0;
		//	float x = cylinderModel.left_length;
		//	while (x <= (cylinderModel.left_length)) {
		//		float dx = 0.1 * cos(alpha) * cos(atan(diff_gx));
		//		x += -1 * dx;
		//		float B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)) / pow(cylinderModel.left_length, 2);
		//		float gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(x - cylinderModel.left_length, 2), 0));
		//		diff_gx = -B * (cylinderModel.left_length - x) / gx;
		//		theta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
		//		alpha += -diff_gx * tan(alpha) * dx / (gx);
		//	}
		//	total_theta += abs(theta);
		//	total_theta += cylinderModel.middle_length * tan(cylinderModel.angle) / cylinderModel.middle_radius;
		//	alpha = cylinderModel.angle; diff_gx = 0, theta = 0;
		//	x = cylinderModel.left_length + cylinderModel.middle_length;
		//	while (x >= (cylinderModel.left_length + cylinderModel.middle_length)) {
		//		float dx = 0.1 * cos(alpha) * cos(atan(diff_gx));
		//		x += dx;
		//		float B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)) / pow(cylinderModel.right_length, 2);
		//		float gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(x - cylinderModel.left_length - cylinderModel.middle_length, 2), 0));
		//		diff_gx = -B * (x - cylinderModel.left_length - cylinderModel.middle_length) / gx;
		//		theta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
		//		alpha += -diff_gx * tan(alpha) * dx / (gx);
		//	}
		//	total_theta += abs(theta);
		//	total_theta += cylinderModel.middle_length * tan(cylinderModel.angle) / cylinderModel.middle_radius;
		//	while (total_theta >= 2 * PI) {
		//		total_theta -= 2 * PI;
		//	}
		//	if (total_theta > PI) {
		//		jumpAngle = 2 * PI - jumpAngle;
		//	}
		//} while (abs(jumpAngle - total_theta) > 0.001);
		CString STemp;
		STemp.Format(_T("纱宽(修正) = %.3f mm\n等分数=%d\n缠绕角 = %.4f degree\n切点数=%d\n跳跃数=%d\n算法: %s"), real_width, M0, cylinderModel.angle * 180 / PI, cutNum, jumpNum, cylinderWindingAlgorithm);
		AfxMessageBox(STemp);
	}
}

//计算纤维路径
void CMfcogl1Doc::OnComputeFiberPathCylinder() {
	CMfcogl1Doc::CleanTubeMemory();
	if (m_bCanComputing == true){
		CylinderPointList = new std::deque<struct cylinderPathCoord>;//CylinderTubePoint的双向链，用之前生成一条纤维的方法生成整个连续纤维
		windingPathStep = 0.2;//计算的步长
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
		cylinderPathCoord currentPoint;
		currentPoint.direction = -1;
		currentPoint.x = cylinderModel.left_length;
		currentPoint.y = cylinderModel.middle_radius;
		currentPoint.z = 0.0;
		currentPoint.currentTheta = 0;
		cylinderPathCoord currentStartPoint;
		currentStartPoint.direction = -1;
		currentStartPoint.x = cylinderModel.left_length;
		currentStartPoint.y = cylinderModel.middle_radius;
		currentStartPoint.z = 0.0;
		currentStartPoint.currentTheta = 0;
		for (int t = 0; t < 1; t++) {//M0:等分数
			if (cylinderWindingAlgorithm == "GEODESIC") {
				OnRenderLeftEllipsoid(CylinderPointList, currentPoint);
				OnRenderMiddleCylinder(CylinderPointList, currentPoint);
				OnRenderRightEllipsoid(CylinderPointList, currentPoint);
				OnRenderMiddleCylinder(CylinderPointList, currentPoint);
			}
			else if (cylinderWindingAlgorithm == "FIXED_ANGLE") {
				OnRenderLeftEllipsoidFixedAngle(CylinderPointList, currentPoint);
				OnRenderMiddleCylinder(CylinderPointList, currentPoint);
				OnRenderRightEllipsoidFixedAngle(CylinderPointList, currentPoint);
				OnRenderMiddleCylinder(CylinderPointList, currentPoint);
			}
			else if (cylinderWindingAlgorithm == "NON_GEODESIC") {
				//TODO: algorithm
				OnRenderLeftEllipsoidNonGeodesic(CylinderPointList, currentPoint);
				OnRenderMiddleCylinder(CylinderPointList, currentPoint);
				OnRenderRightEllipsoidNonGeodesic(CylinderPointList, currentPoint);
				OnRenderMiddleCylinder(CylinderPointList, currentPoint);
			}
			//矫正
			currentStartPoint.currentTheta += jumpAngle;
			currentStartPoint.y= cylinderModel.middle_radius * cos(currentStartPoint.currentTheta);
			currentStartPoint.z = cylinderModel.middle_radius * sin(currentStartPoint.currentTheta);
			currentPoint.direction = -1;
			currentPoint.x = currentStartPoint.x;
			currentPoint.y = currentStartPoint.y;
			currentPoint.z = currentStartPoint.z;
			currentPoint.currentTheta = currentStartPoint.currentTheta;
		}
		glEndList();
		//统计总的坐标数
		int size = CylinderPointList->size();
		m_WindingCount[0] = size;
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
void CMfcogl1Doc::OnRenderRightEllipsoid(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint) {
	glLineWidth(2);
	glPointSize(2);
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dx_curve = 0.05;
	double diff_gx = 0;
	while (currentPoint.x >= (cylinderModel.left_length + cylinderModel.middle_length)) {
		glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		double dx = dx_curve * cos(alpha)*cos(atan(diff_gx));
		currentPoint.x += currentPoint.direction*dx;
		double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2))  / pow(cylinderModel.right_length, 2);
		double gx = sqrt( max(pow(cylinderModel.middle_radius, 2) - B* pow(currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length, 2),0) );
		diff_gx = -B * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / gx;
		currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha)*dx / (gx);
		currentPoint.y = gx * cos(currentPoint.currentTheta);
		currentPoint.z = gx * sin(currentPoint.currentTheta);
		alpha += -diff_gx * tan(alpha)*dx / (gx);
	}
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();
}
void CMfcogl1Doc::OnRenderLeftEllipsoid(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint) {
	glLineWidth(2);
	glPointSize(2);
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dx_curve = 0.05;
	double diff_gx = 0;
	while (currentPoint.x <= (cylinderModel.left_length)) {
		glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		double dx = dx_curve * cos(alpha) * cos(atan(diff_gx));
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
void CMfcogl1Doc::OnRenderRightEllipsoidFixedAngle(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint) {
	glLineWidth(2);
	glPointSize(2);
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dx_curve = 0.05;
	double diff_gx = 0;
	while (currentPoint.x >= (cylinderModel.left_length + cylinderModel.middle_length)) {
		glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		double dx = dx_curve * cos(alpha) * cos(atan(diff_gx));
		currentPoint.x += currentPoint.direction * dx;
		double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)) / pow(cylinderModel.right_length, 2);
		double gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length, 2), 0));
		diff_gx = -B * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / gx;
		currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
		currentPoint.y = gx * cos(currentPoint.currentTheta);
		currentPoint.z = gx * sin(currentPoint.currentTheta);
		if (currentPoint.x < cylinderModel.right_slippage_point) {
		
		}
		else {
			alpha += -diff_gx * tan(alpha) * dx / (gx);
		}
	}
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();
}
void CMfcogl1Doc::OnRenderLeftEllipsoidFixedAngle(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint) {
	glLineWidth(2);
	glPointSize(2);
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double max_slippage_cofficient = cylinderModel.slippage_coefficient;
	double dx_curve = 0.05;
	double diff_gx = 0;
	float current_slippage = 0;
	//while (current_slippage < max_slippage_cofficient) {
	//	//x,y,z定缠绕角地增加
	//	glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
	//	double dx = dx_curve * cos(alpha) * cos(atan(diff_gx));
	//	currentPoint.x += currentPoint.direction * dx;

	//	float sv = sin(alpha);
	//	float cv = cos(alpha);
	//	float tmpx2b = pow(currentPoint.x, 2) / pow(cylinderModel.middle_radius, 2);
	//	float a = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.right_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)));
	//	float tmpa2b = pow(a, 2) / pow(cylinderModel.middle_radius, 2);

	//	double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)) / pow(cylinderModel.right_length, 2);
	//	double gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length, 2), 0));
	//	diff_gx = -B * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / gx;
	//	currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
	//	currentPoint.y = gx * cos(currentPoint.currentTheta);
	//	currentPoint.z = gx * sin(currentPoint.currentTheta);
	//	/*alpha += -diff_gx * tan(alpha) * dx / (gx);*/

	//	float numerator = sv * currentPoint.x / (a * sqrt(1 - tmpx2b));
	//	float denominator = (cv * cv) / (1 - (1 - tmpa2b) * tmpx2b) + (sv * sv) / tmpa2b;
	//	current_slippage = numerator / denominator;
	//}
	while (currentPoint.x <= (cylinderModel.left_length)) {
		glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		double dx = dx_curve * cos(alpha) * cos(atan(diff_gx));
		currentPoint.x += currentPoint.direction * dx;
		double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)) / pow(cylinderModel.left_length, 2);
		double gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(currentPoint.x - cylinderModel.left_length, 2), 0));
		diff_gx = -B * (cylinderModel.left_length - currentPoint.x) / gx;
		currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
		currentPoint.y = gx * cos(currentPoint.currentTheta);
		currentPoint.z = gx * sin(currentPoint.currentTheta);
		if (currentPoint.x> cylinderModel.left_slippage_point) {
			//just keep the alpha angle
		}
		else {
			alpha += -diff_gx * tan(alpha) * dx / (gx);
		}
	}
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();
}
void CMfcogl1Doc::OnRenderRightEllipsoidNonGeodesic(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint) {
	glLineWidth(2);
	glPointSize(2);
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dx_curve = 0.05;
	double diff_gx = 0;
	while (currentPoint.x >= (cylinderModel.left_length + cylinderModel.middle_length)) {
		glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		double dx = dx_curve * cos(alpha) * cos(atan(diff_gx));
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
void CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint) {
	glLineWidth(2);
	glPointSize(2);
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dx_curve = 0.05;
	double diff_gx = 0;
	while (currentPoint.x <= (cylinderModel.left_length)) {
		glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		double dx = dx_curve * cos(alpha) * cos(atan(diff_gx));
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
void CMfcogl1Doc::OnRenderMiddleCylinder(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint) {
	glLineWidth(2);
	glPointSize(2);
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double theta = 0;//初始的x-θ中的θ(弧度)
	double dx_curve = 0.05;
	double adjust = cylinderModel.middle_radius / sin(cylinderModel.angle);
	double dx_theta = dx_curve / adjust;
	double3 begin = { currentPoint.x, currentPoint.y, currentPoint.z };
	while (currentPoint.x >=cylinderModel.left_length && currentPoint.x<=(cylinderModel.left_length+cylinderModel.middle_length)) {
		currentPoint.x += currentPoint.direction * dx_curve * cos(cylinderModel.angle);
		currentPoint.y = cylinderModel.middle_radius * cos(currentPoint.currentTheta);
		currentPoint.z = cylinderModel.middle_radius * sin(currentPoint.currentTheta);
		glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);

		currentPoint.currentTheta += dx_theta;
	}
	glEnd();
}