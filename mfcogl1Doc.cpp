// mfcogl1Doc.cpp : implementation of the CMfcogl1Doc class
//
/*
TODO��
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
	ON_COMMAND(IDM_FIBER_PATH_CONTROL_PARAMETERS, OnSwitchFiberPathControlDlg)//��������
	ON_COMMAND(IDM_COMPUTE_FIBER_PATH, OnSwitchComputeFiberPath)//��ά·������
	ON_COMMAND(IDM_COMPUTE_PAYEYE_TRACK, OnSwitchComputePayeye)//����·������
	ON_COMMAND(IDM_SAVE_TRACK, OnSwitchSaveTrack)//�������·��
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

//״̬������
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
//��������
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

//��ά·������
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

//����·������
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

//�������·��
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
		tubeModel.round = 2 * PI * tubeModel.r + 4 * tubeModel.a + 4 * tubeModel.b;//�ܳ�

		//������ʵ��е���(����һ�ܳ����������)����Ծ��(���γ�������ŵĲ�ֵ)
		float real_width = tubeModel.width / abs(cos(tubeModel.angle));//��ά�����ղ��ƽǰڷŵĺ�����
		M0 = ceil(tubeModel.round / real_width);//ʵ���ʷֶ���
		interval = tubeModel.round / M0;//�ʷֲ���
		while ((M0 + 1) % cutNum != 0) {//��С��ά�뾶�������е���Լ��
			tubeModel.width -= 0.02;
			real_width = tubeModel.width / abs(cos(tubeModel.angle));
			M0 = ceil(tubeModel.round / real_width);
			interval = tubeModel.round / M0;
		}
		jumpNum = (M0 + 1) / cutNum;

		m_bCanComputing = true;
		CString STemp;
		STemp.Format(_T("ɴ��(����) = %.2f mm\n���ƽ� = %.2f degree\n�ȷ���=%d\n�е���=%d\n��Ծ��=%d"),
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

//������ʼ���б� ��ͷ����һȦ��ÿ��һ����Ƭ�ɼ�һ����
int CMfcogl1Doc::OnGenerateTubeEdgeList(std::deque<struct tubePathPosition>& TubeList,float interval,int sides) {
	//sides: 1.���� 2.��ȥ+����
	//interval: ����ÿһƬ�зֵĳ���
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

//������ά·���ļ���
//1.�Ȱ����з֣������������е���ά��ʼ��
//2.��������ʼ�㿪ʼ���ؼ������
//3.��ÿ������ά�м����ӱ������յ㵽��������·
void CMfcogl1Doc::OnComputeFiberPathTube() {
	CMfcogl1Doc::CleanTubeMemory();

	//һ����ά·�������ݽṹ��һ�δ���һ��άĩβλ�õ�����ά���Ĳ��Ƶ�+һ�δ���ά��㵽ʵ�ʲ�����ת��Ĳ�ֵ+�����Ĳ��Ʋ���
	if (m_bCanComputing == true){
		TubePointList = new std::deque<std::deque<struct tubePathCoord>*>;//TubePoint��˫������ָ���˫�������ڲ��ʾһ����ά·��

		windingPathStep = 5;//����Ĳ���
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

		//��������·�������
		OnGenerateTubeEdgeList(TubeBothList,interval,2);

		double buffer_angle = PI * 60/ 180;
		//��һͷ�Ƶ���һͷ�����Ļ��������泤��
		//�Ȳ���������εĳ��� �Ѿ����ܳ�ȡ����
		double circle_length = tubeModel.length * tan(tubeModel.angle);
		while (circle_length > tubeModel.round) circle_length -= tubeModel.round;
		//������ν�����Ҫ�Ļ����������Ҫ��ô��
		double min_length = tubeModel.round * (buffer_angle / (2 * PI));
		double cut_min = M0 * (buffer_angle / (2 * PI));
		//ÿһ����Ƭ�����ĳ���
		double cut_interval_length = tubeModel.round / M0;
		//���������泤�Ȼ�����е���
		int cut_point_circle_length = ceil(circle_length / cut_interval_length);
		//ͬһ�����γ���������루��Ծ��*��Ƭ����
		double jump_interval_length = jumpNum * cut_interval_length;
		//������Ҫ��õ�ֵ����һ��ԭ�е��е����������ƫ�Ƽ����е㣿
		//����������ξ����ĳ������
		cut_num_offset = 0;
		int current_minus = M0 * 2;//��ǰ����������ξ����ĳ��Ȳ�
		for (int tmp_num_offset=0; tmp_num_offset < M0; tmp_num_offset++) {
			//�Ҷ�����γ���
			// |(0+cut_num_offset)  -  (0+cut_point_circle_length) + k1*M0| �� - ��
			int right = tmp_num_offset - cut_point_circle_length;
			while (right < 0) right += M0;
			//�������γ���
			// |(0+jumpNum)  - (0+cut_num_offset + cut_point_circle_length) +k2*M0|  �� - ��
			int left = jumpNum - tmp_num_offset - cut_point_circle_length;
			while (left < 0) left += M0;
			if (left > cut_min && right > cut_min&& abs(left - right) < current_minus) {
				current_minus = abs(left - right);
				cut_num_offset = tmp_num_offset;
			}
		}

		currentPosition.direction = -1;
		int i = 3;
		//����ѭ��
		while (TubeBothList.size()!=0) {
			std::deque<struct tubePathCoord>* currentPathList = new std::deque<struct tubePathCoord>;
			TubePointList->push_back(currentPathList);
			OnGeneratePosition(TubeBothList,currentPathList);
			OnRenderSinglePath(currentPathList);//���㱾��ά·�������ж���λ�ò�����glList��TubePointList
		}
		glEndList();

		//����������ά·�������Ӷ�
		//�������ۼ�����ĵ㣬�������湲��TubeStepList����������direction����Ҫ
		//OnGenerateTubeEdgeList(TubeStepList, windingPathStep, 1);
		//for (auto list = TubePointList->begin(); list < TubePointList->end()-1;list++ ) {
		//	std::deque<struct tubePathCoord>* currentPathList = new std::deque<struct tubePathCoord>;
		//	tubePathCoord begin = *(*list)->rbegin();//����ά�ε�ĩ��
		//	tubePathCoord end = *(*(list + 1))->begin();//����ά�ε����
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

		//ͳ���ܵ�������
		m_WindingCount[0] = 0;
		for (auto i = TubePointList->begin(); i < TubePointList->end(); i++) { m_WindingCount[0] += (*i)->size(); }
		AfxMessageBox("��ά·���������\ntube Track Computation Finished.");
		m_bFiberPathComplete = true;
		CMfcogl1View* pView;
		POSITION pos = GetFirstViewPosition();
		pView = (CMfcogl1View*)GetNextView(pos);
		pView->m_cview_enable_tape_display = true;
		pView->Invalidate(false);

	}
	else{
		AfxMessageBox(_T("����������ά����\nPlease setup parameter first."));
	}
}

//���ݵ�ǰλ�ú���Ծ���������·�������
void CMfcogl1Doc::OnGeneratePosition(std::deque<struct tubePathPosition>&TubeStartList, std::deque<struct tubePathCoord>* singlePathList) {

	auto tmpPosition = TubeStartList.begin();
	//direction==-1 ->�ղ��Ǵ�β������->˵������Ҫ��ͷ����ʼ��->�õ���һ�δ�ͷ����ȥ����ʼ��->��һ����Ծ��
	int currentStart = (currentPosition.direction == -1) ? TubeCurrentStart[0] : TubeCurrentStart[1];

	//����Ѿ���ʼ����������һ������Ծ��
	if (currentStart != -1) {
		auto i = TubeStartList.begin();
		for (; i < TubeStartList.end(); i++) {//���� ����һ����ʼ���һ����Ծ��
			if (currentPosition.direction != (*i).direction && (*i).index == (currentStart + jumpNum) % M0) {
				tmpPosition = i;
				break;
			}
		}
	}
	else {
		//��δ��ʼ��
		if (currentPosition.direction == 1) {//&& currentStart == 1
			tmpPosition = TubeStartList.begin() + 2 * cut_num_offset;
		}
	}

	currentPosition = *tmpPosition;//�õ���һ������ʼ��

	if (currentPosition.direction ==1) {//��ȥ
		TubeCurrentStart[0] = (*tmpPosition).index;
	}
	else {//����
		TubeCurrentStart[1] = (*tmpPosition).index;
	}
	TubeStartList.erase(tmpPosition);
}

//���������ά·��
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
	//�жϵ�ǰposition����оģ���ĸ��棬�Ӷ�ѡ�к��ʵ���ڡ�
	//������״̬�����������Ƕ�����OnRender��ͬ״̬�ĵ���
	//���Ƶ���ı�Եʱ������OnRender�еݹ������һ�����OnRender��ֱ�����Ƶ�оģ��Ե�˳���
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
		else {//Ƕ�����
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, PI / 2, direction, singlePathList);
		}
		while (nextPoint[0] < tubeModel.a && outTubeEdge(nextPoint[2]) == 0) {
			nextPoint[0] = startPoint[0] + tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[1] = startPoint[1];
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;
			tubeModel.curveLength += windingPathStep;
		}//Ƕ�빲ͬ���յ�
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
		else {//Ƕ�����
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
		else {//Ƕ�����
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
		else {//Ƕ�����
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
	//��ʱ��Ϊ�˱��ڼ��㣬��stepLength����Ϊ�ڸ÷��澭���Ļ��ȣ��������߻�������Ϊ�˱������߻����������䣬��Ҫ�������ڱ������϶�Ӧ�����ȡ�
	float adjust = tubeModel.r / sin(tubeModel.angle);
	switch (state) {
	case 2:
		phase = atan((currentPosition.x - tubeModel.a) / (currentPosition.y - tubeModel.b));
		tubeModel.curveLength = phase;
		startPoint[0] = tubeModel.a;
		startPoint[1] = tubeModel.b;
		if (size == 0) {
			//0:����2a��1:�ߣ�2b��2:����C��angle:���ƽ� nextPoint:���� m_view_r:���ǰ뾶
			insertPoint(nextPoint);
			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;
			pushTubePathCoord(nextPoint, -tubeModel.curveLength +PI, direction, singlePathList,PI/2);
		}
		//Բ�����ϲ���� �Ի����������Լ��
		while (tubeModel.curveLength <= PI / 2 && outTubeEdge(nextPoint[2]) == 0) {
			insertPoint(nextPoint);

			//0:����2a��1:�ߣ�2b��2:����C��angle:���ƽ� nextPoint:���� m_view_r:���ǰ뾶
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
//���㷽�ܻ���·��
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

		TubeTrackList = new std::deque<std::deque<struct track>*>;//�洢��˿��λ�õ�˫������ָ���˫�������ڲ��ʾһ����ά·���϶�Ӧ����˿������
		for (auto line = TubePointList->begin(); line < TubePointList->end(); line++) {//ÿ����ά·��
			glBegin(GL_LINE_STRIP);
			std::deque<struct track>* currentTrackList = new std::deque<struct track>;
			TubeTrackList->push_back(currentTrackList);
			final_position.initialized = 0;
			for (auto currentTubePointIterator = (*line)->begin(); currentTubePointIterator < (*line)->end(); currentTubePointIterator++) {//��ά·����ÿ����
				track tempTubeTrack;
				computeTubeTrack(currentTubePointIterator, tempTubeTrack);
				currentTrackList->push_back(tempTubeTrack);
				float trackMandrelLength = payeye.pm_distance - tempTubeTrack.x;//��˿�쵽оģ������Ĵ�ֱ����
				float trackPoint[3] = { trackMandrelLength * cos(tempTubeTrack.spindleAngle), trackMandrelLength * sin(tempTubeTrack.spindleAngle), tempTubeTrack.z };
				glVertex3fv(trackPoint);
			}
			glEnd();
			
		}
		//����μ���
		auto fiber = TubePointList->begin();
		auto payeye_track = TubeTrackList->begin();
		for (; fiber < TubePointList->end()-1,payeye_track<TubeTrackList->end()-1; fiber++,payeye_track++) {
			//����ε�����
			std::deque<struct tubePathCoord>* currentPointList = new std::deque<struct tubePathCoord>;
			std::deque<struct track>* currentTrackList = new std::deque<struct track>;
			tubePathCoord fiber_begin = *(*fiber)->rbegin();//����ά�ε�ĩ��
			tubePathCoord fiber_end = *(*(fiber + 1))->begin();//����ά�ε����

			track payeye_begin = *(*payeye_track)->rbegin();//������·����ĩ��
			auto _iter_payeye_begin_pre = (*payeye_track)->rbegin()+1; 
			while (_iter_payeye_begin_pre->z == payeye_begin.z) { 
				_iter_payeye_begin_pre++; 
			}
			track payeye_begin_pre = *_iter_payeye_begin_pre;//������·����ĩ�˼�һ
			track payeye_end = *(*(payeye_track + 1))->begin();//�»���·�������
			track payeye_end_next = *((*(payeye_track + 1))->begin()+1);//�»���·��������һ
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
				float trackMandrelLength = payeye.pm_distance - tempTubeTrack.x;//��˿�쵽оģ������Ĵ�ֱ����
				float trackPoint[3] = { trackMandrelLength * cos(tempTubeTrack.spindleAngle), trackMandrelLength * sin(tempTubeTrack.spindleAngle), tempTubeTrack.z };
				glVertex3fv(trackPoint);
			}
			glEnd();
			fiber = TubePointList->insert(fiber + 1, currentPointList);
			payeye_track = TubeTrackList->insert(payeye_track + 1, currentTrackList);
		}
		glEndList();

		//����������ά·�������Ӷ�
		//�������ۼ�����ĵ㣬�������湲��TubeStepList����������direction����Ҫ
		/*
		OnGenerateTubeEdgeList(TubeStepList, windingPathStep, 1);
		for (auto list = TubePointList->begin(); list < TubePointList->end()-1;list++ ) {
			std::deque<struct tubePathCoord>* currentPathList = new std::deque<struct tubePathCoord>;
			tubePathCoord begin = *(*list)->rbegin();//����ά�ε�ĩ��
			tubePathCoord end = *(*(list + 1))->begin();//����ά�ε����
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

		//����ͳ���ܵ���
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
		AfxMessageBox(_T("���ȼ�����ά·��\nPlease compute path first."));
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
	// �����ڵ����ߺ;��εĽ��� ���� ���� ������ ����С ��ȻС�������ǽ�
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

//�����currentTubePoint��suspension(��˿����)��tempTubeTrack�������ɶȲ���
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
	//��ά����ʼ�ζ�Ӧ����·��
	//if (currentTubePointIterator._Myoff == 0) { 
	/*	currentTubePoint.suspension = 0;
	}*/
	
	float track_x = currentTubePoint.x + currentTubePoint.tangent.x * currentTubePoint.suspension;
	float track_y = currentTubePoint.y + currentTubePoint.tangent.y * currentTubePoint.suspension;
	float track_z = currentTubePoint.z + currentTubePoint.tangent.z * currentTubePoint.suspension;

	//��ά���յ�ζ�Ӧ����·������˿��λ�ò��ܳ������ˣ�����˵�ʱҪ����ά����ѹ����оģ��(�ⲿ��������ε�����)
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
	vec3 rotatedTangent;//ƽ��ʱ��������ת�󣬼������Ӧ����˿��Ƕ�
	CMfcogl1Doc::vectorRotateAroundZ(currentTubePoint.tangent, tempTubeTrack.spindleAngle, rotatedTangent,currentTubePoint.direction);
	tempTubeTrack.swingAngle = atan(rotatedTangent.z / rotatedTangent.y);
}

void CMfcogl1Doc::vectorRotateAroundZ(const vec3& currentVector,const float& angle,vec3& rotatedVector,float direction){
	//x-yƽ����ת����
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
			float trackMandrelLength =payeye.pm_distance - track->x;//��˿�쵽оģ������Ĵ�ֱ����
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
		//������������Ĭ�Ϲ��������ֵ��������£�
		//ʹ�ù��캯���޸ĶԻ����ʼ״̬���ܸ��ã���������϶�
		CFileDialog FileDlg(FALSE, "txt", _T("Test"));
		FileDlg.m_ofn.lpstrTitle = "Save File";
		FileDlg.m_ofn.lpstrFilter = szFilters;

		if (IDOK == FileDlg.DoModal())
		{
			std::vector<data> in;
			calData(in);
			CFile File(FileDlg.GetPathName(), CFile::modeCreate | CFile::modeReadWrite);
			char BufData[100];
			sprintf(BufData, "оģ��ͷ����,��˿��-��ת�����,��˿��-��Ȧ����,��������\n%.1f, %.1f, %.1f, %d\n", tubeModel.redundance, payeye.pm_distance, payeye.pm_left_distance, in.size());
			File.Write(BufData, strlen(BufData));
			sprintf(BufData, "оģ���,оģ�߶�,оģ����,���ƽ�,��ά����\n%.1f, %.1f, %.1f, %.1f, %.1f\n",2.0f*(tubeModel.a+ tubeModel.r),2.0f*(tubeModel.b+ tubeModel.r),tubeModel.length,tubeModel.angle*180.0f/PI,tubeModel.width);
			File.Write(BufData, strlen(BufData));
			sprintf(BufData,"оģ��ת,��˿��λ��,��˿��̽��,��˿����ת,����ٶ�,��ά���\n");
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

//����ѹ����������ά·������
void CMfcogl1Doc::OnOpenFiberPathControlCylinderParametersDlg() {
	CDlgFiberPathControlsCylinder fpCylinderDlg;
	if (IDOK == fpCylinderDlg.DoModal()) {
		ResetWndDesign();
		cylinderModel.angle = fpCylinderDlg.m_dlg_cylinder_winding_angle * PI / 180.0;
		cylinderModel.width = fpCylinderDlg.m_dlg_cylinder_band_width;
		cylinderModel.slippage_coefficient = fpCylinderDlg.m_dlg_cylinder_slippage_coefficient;//��
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
			//ͬһ��ĸ���ϲ�ͬ��ʼλ����ͬ���ƽǵİ��������ͷ�����
			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic;
			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoidNonGeodesic;
		}
		else if (cylinderWindingAlgorithm == "AUTO") {
			//debug_show("now auto~");
			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoidAuto;
			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoidAuto;
			middleCylinder = &CMfcogl1Doc::OnRenderMiddleCylinderAuto;
		}

		//������ʵ��е���(����һ�ܳ����������)����Ծ��(���γ�������ŵĲ�ֵ
		float real_width = 0, total_theta = 0;
		cylinderModel.width = fpCylinderDlg.m_dlg_cylinder_band_width;
		real_width = 0.1*cylinderModel.width / abs(cos(cylinderModel.angle));
		M0 = ceil(cylinderModel.round / real_width);//ʵ���ʷֶ���
		interval = cylinderModel.round / M0;//�ʷֲ���
		jumpNum = (M0 + 1) / cutNum;
		jumpAngle = (float)(jumpNum) * 2.0 * PI / (float)M0;
		std::deque<data> quoters;
		//ʵ�������
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
					left_cut_theta = global_left_cut_theta;//global_left_cut_theta:һ����ά����ӽ��е��theta��left_cut_theta������������ά����ӽ��е��theta
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
	//		M0 = ceil(cylinderModel.round / real_width);//ʵ���ʷֶ���
	//		interval = cylinderModel.round / M0;//�ʷֲ���
	//		float left_min_r;
	//		//while ((M0 + 1) % cutNum != 0) {//��С��ά�뾶�������е���Լ��
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
	//		//����һ����ά����ʼ��(currentStartPoint)������һ���е��ϣ��ٽ���ǰ��(currentPoint)������ʼ�㣬ʹ��ÿ����ά����ʼλ�ñ�׼��
	//} while (/*abs(jumpAngle - _theta) > 0.001 && abs(2 * PI - jumpAngle - _theta)>0.001*/abs(global_left_min_r-cylinderModel.left_radius)>0.1);
	CString STemp;
	STemp.Format(_T("ɴ��(����) = %.3f mm\n�ȷ���=%d\n���ƽ� = %.4f degree\n�е���=%d\n��Ծ��=%d\n�㷨: %s\n���� %.2f"), real_width, M0, cylinderModel.angle * 180 / PI, cutNum, jumpNum, cylinderWindingAlgorithm, cylinderModel.right_slippage_point);
	quoters;
	//debug_show(cylinderModel.left_using_coef);
	//debug_show(cylinderModel.left_slippage_point);
	AfxMessageBox(STemp);
	}
}


//������ά·��
void CMfcogl1Doc::OnComputeFiberPathCylinder() {
	CMfcogl1Doc::CleanTubeMemory();
	if (m_bCanComputing == true){
		CylinderPointList = new std::deque<struct cylinderPathCoord>;//CylinderTubePoint��˫��������֮ǰ����һ����ά�ķ�����������������ά
		windingPathStep = 1.6;//����Ĳ���
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
		for (int t = 0; t < 1; t++) {//M0:�ȷ��� ����
			(this->*leftCylinder)(CylinderPointList, currentPoint,true);
		/*	(this->*middleCylinder)(CylinderPointList, currentPoint,true);
			(this->*rightCylinder)(CylinderPointList, currentPoint,true);
			(this->*middleCylinder)(CylinderPointList, currentPoint,true);*/

			//����һ����ά����ʼ��(currentStartPoint)������һ���е��ϣ��ٽ���ǰ��(currentPoint)������ʼ�㣬ʹ��ÿ����ά����ʼλ�ñ�׼��
			currentStartPoint.currentTheta += jumpAngle;
			currentStartPoint.y= cylinderModel.middle_radius * cos(currentStartPoint.currentTheta);
			currentStartPoint.z = cylinderModel.middle_radius * sin(currentStartPoint.currentTheta);
			currentPoint = currentStartPoint;
		}
		glEndList();
		//ͳ���ܵ�������
		int size = CylinderPointList->size();
		m_WindingCount[0] = size;
		//AfxMessageBox("��ά·���������\ntube Track Computation Finished.");
		m_bFiberPathComplete = true;
		CMfcogl1View* pView;
		POSITION pos = GetFirstViewPosition();
		pView = (CMfcogl1View*)GetNextView(pos);
		pView->m_cview_enable_tape_display = true;
		pView->Invalidate(false);
	}
	else{
		AfxMessageBox(_T("����������ά����\nPlease setup parameter first."));
	}
}

void CMfcogl1Doc::OnRenderMiddleCylinder(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {

	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//�߶���ʼ�Ĳ��ƽǦ�(����)
	double theta = 0;//�߶���ʼ��theta��λ
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
	double  alpha = cylinderModel.angle;//��ʼ�Ĳ��ƽǦ�(����)
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
	currentPoint.direction = -currentPoint.direction;//��directionָʾ���Ʒ��� Ҳ����tangent��alpha
	glEnd();
}

void CMfcogl1Doc::OnRenderLeftEllipsoidFixedAngle(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//��ʼ�Ĳ��ƽǦ�(����)
	double dcurve = d_curve;
	double diff_gx = 0;

	//Ϊ�˿�������alpha����ʵ�ʲ��ƽǲ�����㣬ʵ�ʲ��ƽǲ��ı�
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

	currentPoint.direction = -currentPoint.direction;//��directionָʾ���Ʒ��� Ҳ����tangent��alpha
	glEnd();
}

//void CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
//	glBegin(GL_LINE_STRIP);
//	double  alpha = cylinderModel.angle;//��ʼ�Ĳ��ƽǦ�(����)
//	double dcurve = d_curve;
//	double dr = 0;
//	double dx, dtheta, r;
//	while (currentPoint.x <= (cylinderModel.left_length)) {
//		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
//		//x:���߳��� 
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
//			r = b * _tmp;//����뾶 С
//			dr = -(b * x) / (a2 * _tmp);//dr/dx ����뾶�������߳�x��΢�� 
//			double dd_r = -b / (a2 * _tmp) - b * x2 / (a2 * a2 * _tmp * abs((1 - x2 / a2)));
//			double A = sqrt(1 + dr * dr);
//			////dalpha/dx
//			//return  lambda * ((-dd_r * r * cos2a + sin2a) / (pow(A, 3) * r * cos2a + A * r * sin2a)) - dr * tana / r;
//			//r = b * sqrt((1 - x2 / a2));//����뾶 С
//			//dr = -(b * x) / (a2 * sqrt(1 - x2 / a2));//dr/dx ����뾶�������߳�x��΢�� 
//			//double dd_r = -b / (a2 * sqrt(1 - x2 / a2)) - b * x2 / (a2 * a2 * sqrt(pow(1 - x2 / a2, 3)));
//			//double A = sqrt(1 + dr * dr);
//			//dalpha/dx
//			return [&dx] {return dx > 0 ? -1 : 1; }() * lambda * ((-dd_r * r * cos2a + sin2a) / (pow(A, 3) * r * cos2a + A * r * sin2a)) - dr * tana / r;
//		};
//
//		dx = dcurve * cos(alpha) * cos(atan(dr)); //dr(����뾶�������ߵ�΢��) = ds(������)*cos(��) = ds(����)*cos(��)*cos(��ƽ�淨�������߼н�)
//		
//			//�������
//			//double RK1 = adjoint(currentPoint.x, alpha);
//			//double RK2 = adjoint(currentPoint.x + currentPoint.direction * dx / 2, alpha + RK1 * dx / 2);
//			//double RK3 = adjoint(currentPoint.x + currentPoint.direction * dx / 2, alpha + RK2 * dx / 2);
//			//double RK4 = adjoint(currentPoint.x + currentPoint.direction * dx, alpha + RK3 * dx);//r d_r�������ﱻ����
//		double RK = adjoint(currentPoint.x + currentPoint.direction * dx, alpha);//get dalpha && change r,dr(dr/dx)
//
//		dtheta = tan(alpha) * sqrt(1 + dr * dr) / r;//��ת����ת��
//		currentPoint.x += currentPoint.direction * dx;
//		currentPoint.currentTheta += dtheta * dx;
//		currentPoint.y = r * cos(currentPoint.currentTheta);
//		currentPoint.z = r * sin(currentPoint.currentTheta);
//		alpha += dx * RK;
//		//d_alpha ���Ľ������������
//		//alpha += dx * (RK1 + 2 * RK2 + 2 * RK3 + RK4) / 6.0;
//	}
//	currentPoint.direction = -currentPoint.direction;//��directionָʾ���Ʒ��� Ҳ����tangent��alpha
//	glEnd();
//}

void CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//��ʼ�Ĳ��ƽǦ�(����)
	double dcurve = d_curve;
	double dr = 0;
	double dx, dtheta, r;
	float min_r = sqrt(currentPoint.y * currentPoint.y + currentPoint.z * currentPoint.z);
	float cut_theta = 0;
	float real_left_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));

	while (currentPoint.x <= (cylinderModel.left_length)) {
		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		//x:���߳��� 
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
			r = b * _tmp;//����뾶 С
			dr = -(b * x) / (a2 * _tmp);//dr/dx ����뾶�������߳�x��΢�� 
			double dd_r = -b / (a2 * _tmp) - b * x2 / (a2 * a2 * _tmp * abs((1 - x2 / a2)));
			double A = sqrt(1 + dr * dr);
			return [&dx] {return dx > 0 ? -1 : 1; }() * lambda * ((-dd_r * r * cos2a + sin2a) / (pow(A, 3) * r * cos2a + A * r * sin2a)) - dr * tana / r;
		};

		dx = dcurve * cos(alpha) * cos(atan(dr)); //dr(����뾶�������ߵ�΢��) = ds(������)*cos(��) = ds(����)*cos(��)*cos(��ƽ�淨�������߼н�)

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
		dtheta = tan(alpha) * sqrt(1 + dr * dr) / r;//��ת����ת��
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
	currentPoint.direction = -currentPoint.direction;//��directionָʾ���Ʒ��� Ҳ����tangent��alpha
	glEnd();

}

void CMfcogl1Doc::OnRenderRightEllipsoid(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//��ʼ�Ĳ��ƽǦ�(����)
	double dcurve = d_curve;//��������
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
	currentPoint.direction = -currentPoint.direction;//��directionָʾ���Ʒ��� Ҳ����tangent��alpha
	glEnd();
}

void CMfcogl1Doc::OnRenderRightEllipsoidFixedAngle(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//��ʼ�Ĳ��ƽǦ�(����)
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
	currentPoint.direction = -currentPoint.direction;//��directionָʾ���Ʒ��� Ҳ����tangent��alpha
	glEnd();
}

void CMfcogl1Doc::OnRenderRightEllipsoidNonGeodesic(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//��ʼ�Ĳ��ƽǦ�(����)
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
			r = b * sqrt((1 - x2 / a2));//����뾶
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
	currentPoint.direction = -currentPoint.direction;//��directionָʾ���Ʒ��� Ҳ����tangent��alpha
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