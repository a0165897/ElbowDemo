// mfcogl1Doc.cpp : implementation of the CMfcogl1Doc class
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "publicfun.h"
#include <process.h>
#include "mfcogl1Doc.h"
#include "DlgFiberPathControls.h"
//#include "cdlgsemiwndanglelaw.h"
#include "PressDialog.h"
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
	ON_COMMAND(IDM_COMPUTE_FIBER_PATH, OnSwitchComputeFiberPath)
	ON_COMMAND(IDM_DESIGN_ALGEBRA_PATTERN, OnComputePayeye)
	ON_COMMAND(ID_PRESSURE_TEST, OnPressureTest)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_VIEW_DISPLAY_PARAMETER, OnViewDisplayParameter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1Doc construction/destruction

CMfcogl1Doc::CMfcogl1Doc()
{
	// TODO: add one-time construction code here
	m_friction=0.2;
	circuit_num=6;
	m_FilamentWidth=2.0;//cm
	m_FilamentThickness=0.025;//cm
	m_WindingCount[0]=0;
	m_numlayer=1;
	step=5;
	m_pipe_radius=15.75; //cm
	m_sweep_radius=45.75;  //cm
	m_height=8.5;  //cm
	cylinder_radius[0]=m_pipe_radius;
	m_span_angle=PI/2.0;
	deviation=30.44;
	Offset=5.0;
	eyeWidth=6.0;
	eyeAccelerate=0.5;
	rollerWidth=5.0;
	payeye_offset=4.5;
	track_layer=1;

	phi=alpha=phi_offset=alpha_offset=NULL;
	pPath=NULL;
	ptrack=NULL;
	psite=NULL;
	pPress=NULL;
	flag_add=NULL;
	phi_add=y_add=alpha_add=NULL;
	phi_turn=y_turn=alpha_turn=NULL;
	friction_test=m_bCanComputing=m_bCanDisplayFiber=false;
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
}

BOOL CMfcogl1Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMfcogl1Doc serialization

void CMfcogl1Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1Doc diagnostics

#ifdef _DEBUG
void CMfcogl1Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMfcogl1Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1Doc commands
//修改芯模参数
void CMfcogl1Doc::ModifyMandrelParameters(float a, float b, float span_angle,float height)
{
   if(fabs(height-15.0)<tolerance)
   {
	   m_pipe_radius=15.0;
	   m_sweep_radius=45.0;
	   m_height=height;
	   m_span_angle=span_angle*PI/180;
   }
}

//打开输入纤维轨迹控制参数对话框，完成数据更新
void CMfcogl1Doc::OnOpenFiberPathControlParametersDlg() 
{
	if((!m_bSavePath&&m_bCanDisplayFiber)||(!m_bSavePayeye&&m_bPayeyeComplete))
	{
		CHintDialog hdlg;
		if(hdlg.DoModal()==IDOK)return;
	}
	InitialAngle();
	CDlgFiberPathControls fpDlg;
	fpDlg.m_reference_winding_angle=m_IniWndAngle*180/PI;
	if(IDOK==fpDlg.DoModal())
	{
		ResetWndDesign();
		m_friction=fpDlg.m_max_friction;
		if(	fpDlg.m_reference_winding_angle>m_IniWndAngle*180/PI)
		{
			m_IniWndAngle=fpDlg.m_reference_winding_angle*PI/180.0;//should convert from degree to radian
		}
		m_FilamentWidth=fpDlg.m_fiber_band_width;
		m_bUseLayer=fpDlg.m_use_layer;
		if(m_bUseLayer)/*m_numlayer=fpDlg.m_layer*/;
		else circuit_num=fpDlg.m_path_loops;

		m_FilamentThickness=fpDlg.m_thickness;
		Offset=fpDlg.m_offset;
		eyeWidth=fpDlg.m_eye_width;
		eyeAccelerate=fpDlg.m_eye_accelerate;
		rollerWidth=fpDlg.m_roller_width;
		payeye_offset=fpDlg.m_payeye_offset;
		track_layer=fpDlg.m_payout_layer;
		data_format=fpDlg.m_data_format;
		data_compress=fpDlg.m_data_compress;
		m_bCanComputing=true;

	}
}



void CMfcogl1Doc::OnComputeFiberPath() 
{
	if(m_bCanDisplayFiber)
	{
		AfxMessageBox(_T("Old path data has been computed !"));
		return;
	}
	if(m_bCanComputing)
	{
		CWaitCursor wait;
		CString astring;
		if(fabs(m_height-15.0)<tolerance)
		{

			if(fabs(m_FilamentWidth-1.0)<tolerance)
			{
				if(m_bUseLayer)ReadDataFromDisk("FiberPath1f.wdf","wdf");
				else ReadDataFromDisk("FiberPath1p.wdf","wdf");
			}
			if(fabs(m_FilamentWidth-2.0)<tolerance)
			{
				if(m_bUseLayer)ReadDataFromDisk("FiberPath2f.wdf","wdf");
				else ReadDataFromDisk("FiberPath2p.wdf","wdf");
			}
		}
		else
		{
			if(fabs(m_FilamentWidth-1.0)<tolerance)
			{
				if(m_bUseLayer)ReadDataFromDisk("FiberPathr1f.wdf","wdf");
				else ReadDataFromDisk("FiberPathr1p.wdf","wdf");
			} 
			if(fabs(m_FilamentWidth-2.0)<tolerance)
			{
				if(m_bUseLayer)ReadDataFromDisk("FiberPathr2f.wdf","wdf");
				else ReadDataFromDisk("FiberPathr2p.wdf","wdf");
			}
		}


		astring.Format("Stable winding path completed,algebric pattern is M=%d,K=%d",m,k);
		AfxMessageBox(astring);

		m_bCanComputing=false;
		m_bCanDisplayFiber=true;

	}	
	else
	{
		AfxMessageBox(_T("Please setup filament winding parameter first"));
	}
}


void CMfcogl1Doc::OnComputePayeye() 
{
	if(m_bPayeyeComplete)
	{
		AfxMessageBox(_T("Old payout eye track has been computed !"));
		return;
	}
	if(m_bCanDisplayFiber)
	{
		if(fabs(m_height-15.0)<tolerance)
		{

			if(fabs(m_FilamentWidth-2.0)<tolerance)
			{
				if(m_bUseLayer)ReadDataFromDisk("FiberPath2f.mdf","mdf");
				else ReadDataFromDisk("FiberPath2p.mdf","mdf");
			}
		}
		else
		{
			if(fabs(m_FilamentWidth-2.0)<tolerance)
			{
				if(m_bUseLayer)ReadDataFromDisk("FiberPathr2f.mdf","mdf");
				else ReadDataFromDisk("FiberPathr2p.mdf","mdf");
			}
		}
	}
	else AfxMessageBox(_T("Please compute fiber path first!"));

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
bool CMfcogl1Doc::Turn(int m,int step,float b,float alpha2, float frictbound,int &g,
					   float &y1,float &frict)
{
	return TRUE;
}

void CMfcogl1Doc::Transition(int m,int adjust,int phi,float b,float yadjust,float alpha,
							 float phitorus[],float alphatorus[],int flagad[][size],
							 float phiad[][size],float yad[][size],float alphaad[][size])
{
}

void CMfcogl1Doc::InitialAngle()
{ 
	  float c1;
	  c1=sqrt((m_sweep_radius-m_pipe_radius)/m_sweep_radius)*(m_sweep_radius-m_pipe_radius)/(m_sweep_radius+m_pipe_radius);
	  m_IniWndAngle=acos(c1)+0.12;
}

int CMfcogl1Doc::CalculateSplittingNum(float b,float &thetacycle,int countlayer,float thick)
{
	return countlayer;

}

void CMfcogl1Doc::Lattice(int m,int step,float thetacycle,float spanangle)
{
}

int CMfcogl1Doc::FillInPath(int m,int step,int total,int circuitnum,int large,int ismall,int mid,int g,
	  int nocycle,float phi[],float alpha[],int flagad[][size],float phiad[][size],float yad[][size],
	  float alphaad[][size],float phiturn[],float yturn[],float alphaturn[])
{
	return large;

}

float CMfcogl1Doc::Simpson(float start, float end, int step)
{
	return start;
}

float CMfcogl1Doc::ComplexSimpson(float start, float end, float tolerant_error)
{
	return start;

}

float CMfcogl1Doc::IntegralFun(float b, float x)
{
	return x;
}

void CMfcogl1Doc::PhiPartition(int m, int step, float phi[], float alpha[], float thetacycle, float tolerant_error)
{

}

void CMfcogl1Doc::SolvePhiOffset(int m, int step, float phi[], float phi_offset[], float alpha_offset[], 
		float span_angle, float thetacycle, float tolerant_error, int &ismall)
{
}

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

void CMfcogl1Doc::OnPressureTest() 
{
	if(m_bPressTest)
	{
		AfxMessageBox(_T("Finite element analysis has been completed !"));
		return;
	}
	if(m_bCanDisplayFiber)
	{
		float dphi,dz,dtheta,phi,theta,AveThick,*thick_elbow,ra,rb,h;
		unsigned short index,total,num,slong,slati,countf,countb,i,number;
		unsigned short slatitude,slong_cylinder=10,slong_elbow;
		CWaitCursor wait;
/*		CPressDialog pdlg;
		if(pdlg.DoModal()==IDOK)
		{
			slatitude=pdlg.m_latitude;
			slong_cylinder=pdlg.m_long_cylinder;
			slong_elbow=pdlg.m_long_elbow;
		}
*/		slatitude=(WORD)2.0*PI/pPath[step].phi+1;
		slong_elbow=2*(m*nocycle+large);
		dphi=2.0*PI/slatitude;
		dz=m_height/slong_cylinder;
		dtheta=m_span_angle/slong_elbow;
		total=slong_elbow+2*slong_cylinder;
		number=slatitude*total;
		ra=m_sweep_radius;
		rb=m_pipe_radius;
		h=m_FilamentThickness;
		pPress=new Pressure[number*m_numlayer];
		if(pPress==NULL)
		{
			AfxMessageBox(_T("Memory allocate failed,please close other program \
and try again !"));
			return;
		}
		thick_elbow=new float[slatitude];
		if(thick_elbow==NULL)
		{
			AfxMessageBox(_T("Memory allocate failed,please close other program \
and try again !"));
			return;
		}
		phi=0.0;
		for(i=0;i<slatitude;i++)
		{
			thick_elbow[i]=2.0*h*rb*(ra+rb)*dphi/(ra*rb*dphi+rb*rb*(sin(phi+dphi)-sin(phi)));
			phi+=dphi;
		}
		AveThick=h*m*m_FilamentWidth/cos(m_IniWndAngle)/(PI*rb);
		for(num=0;num<m_numlayer;num++)
		{
			index=num*number;
			theta=m_height;
			for(slong=0;slong<total;slong++)
			{
				phi=0.0;
				for(slati=0;slati<slatitude;slati++)
				{
					pPress[index+slati].alpha_f=pPress[index+slati].alpha_b=0.0;
					countf=countb=0;
					i=(num>0)?m_WindingCount[num-1]:0;
					for(;i<=m_WindingCount[num];i++)
					{
						if(slong<slong_cylinder||slong>=(slong_cylinder+slong_elbow))
						{
							if(pPath[i].flag!=0)
								if(pPath[i].phi>=phi&&pPath[i].phi<(phi+dphi)
									&&(pPath[i].flag*pPath[i].theta)<=theta
									&&(pPath[i].flag*pPath[i].theta)>(theta-dz))
								{
									if(pPath[i].alpha<=PI/2.0)
									{
										pPress[index+slati].alpha_f+=pPath[i].alpha;
										countf++;
									}
									else 
									{
										pPress[index+slati].alpha_b+=pPath[i].alpha;
										countb++;
									}
								}
						}
						else
						{
							if(pPath[i].flag==0)
								if(pPath[i].phi>=phi&&pPath[i].phi<(phi+dphi)
									&&pPath[i].theta<=theta&&pPath[i].theta>(theta-dtheta))
								{
									if(pPath[i].alpha<=PI/2.0)
									{
										pPress[index+slati].alpha_f+=pPath[i].alpha;
										countf++;
									}
									else 
									{
										pPress[index+slati].alpha_b+=pPath[i].alpha;
										countb++;
									}
								}
						}
					}
					if(slong<slong_cylinder||slong>=(slong_cylinder+slong_elbow))
						pPress[index+slati].thickness_ave=AveThick;
					else pPress[index+slati].thickness_ave=thick_elbow[slati];
					if(countf)pPress[index+slati].alpha_f/=countf;
					if(countb)pPress[index+slati].alpha_b/=countb;
					phi+=dphi;
				}
				index+=slatitude;
				if(slong<(slong_cylinder-1)||slong>=(slong_elbow+slong_cylinder))theta-=dz;
				if(slong==(slong_cylinder-1))theta=m_span_angle/2.0;
				if(slong<(slong_cylinder+slong_elbow-1)&&slong>=slong_cylinder)theta-=dtheta;
				if(slong==(slong_cylinder+slong_elbow-1))theta=0.0;
			}
		}
		if(thick_elbow!=NULL)
		{
			delete []thick_elbow;
			thick_elbow=NULL;
		}
		WriteTestData(slatitude,slong_elbow,slong_cylinder);
		m_bPressTest=true;
	}
	else AfxMessageBox(_T("Please design fiber path first !"));
	// TODO: Add your command handler code here
	
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

float CMfcogl1Doc::DetermineStep(int m, float thetacycle, float tolerant_error)
{
	return thetacycle;
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

void CMfcogl1Doc::OnViewDisplayParameter() 
{
	CString InfString;
	InfString.Format("The torus radius  a=%fcm,\npipe radius b=%fcm,\nlength of cylinder h=%fcm;\
\nspan angle is %f(Degree),\nInitial winding angle is %f(Degree),\nfiber width is %fcm;\n\
fiber thickness is %fcm,\nmaximun friction is %f.",m_sweep_radius,m_pipe_radius,m_height,m_span_angle*180/PI,\
m_IniWndAngle*180/PI,m_FilamentWidth,m_FilamentThickness,m_friction);
	AfxMessageBox(InfString);
	// TODO: Add your command handler code here
	
}
// Source code for payout eye trajectory
void CMfcogl1Doc::posite (struct Mandrel mand,struct PathPoint path)
{
}

float CMfcogl1Doc::falpha(float alpha,float xcarriage,float zcarriage,
			 float highPayeye,struct Site *psite)
{
	return alpha;
}

float CMfcogl1Doc::findspindleangle(float alpha,float xcarriage,float zcarriage,
				float highPayeye,float step,struct Site *psite)
{
	return alpha;

}

float CMfcogl1Doc::calxcarriage(float& alpha0,float alpha,float dswing,float ycylinderi,
					float ycylinder,float slip,int& index,int frontback,int flag,int flag2,
					int n,struct Mandrel mand,struct Miscellence misc)
{
	return alpha;

}

void CMfcogl1Doc::CalculateTrack(struct Mandrel mand,struct Miscellence misc,
				float epsilon,struct PathPoint *path,int countPathPoint)
{
	return;     //ptrack;
}

/*added by LMK*/
//Since now we have two mandrels(elbow,tube),choose right dialog to get parameters.
void CMfcogl1Doc::OnSwitchFiberPathControlDlg() {
	if (m_isShowing == 1) {//tube
		OnOpenFiberPathControlTubeParametersDlg();
	}
	else if(m_isShowing == 2) {//elbow
		OnOpenFiberPathControlParametersDlg();
	}
}

void CMfcogl1Doc::OnOpenFiberPathControlTubeParametersDlg() {

	CDlgFiberPathControlsTube fpTubeDlg;
	if (IDOK == fpTubeDlg.DoModal()) {
		ResetWndDesign();
		m_doc_tube_winding_angle = fpTubeDlg.m_dlg_tube_winding_angle * PI / 180;
		m_doc_tube_band_width = fpTubeDlg.m_dlg_tube_band_width;
		m_doc_tube_cut_num = fpTubeDlg.m_dlg_tube_cut_num;
	}
	m_bCanComputing = true;

	CMfcogl1View* pView;
	POSITION pos = GetFirstViewPosition();
	pView = (CMfcogl1View*)GetNextView(pos);

	model.a = pView->m_view_tube_a + 0.1;
	model.b = pView->m_view_tube_b + 0.1;
	model.length = pView->m_view_tube_length;
	model.r = pView->m_view_tube_r;
	model.angle = m_doc_tube_winding_angle;
	model.width = m_doc_tube_band_width;

	float round = 2 * PI * model.r + 4 * model.a + 4 * model.b;
	float real_width = model.width / abs(cos(model.angle));
	int M0 = ceil(round / real_width);//返回大于等于它的最小整数
	float interval = round / M0;
	cutNum = m_doc_tube_cut_num;
	int rem = (M0 + 1) % cutNum;//最小互素数 27  1 5 9 13 17 21 25 2   最小切点数7
	if (rem == 0) {
		jumpNum = (M0 + 1) / cutNum;
	}
	else {
		jumpNum = -1;
	}
	CString STemp;
	STemp.Format(_T("纱宽(修正) = %.3f mm\n等分数=%d\n缠绕角 = %.3f degree\n切点数=%d\n跳跃数=%d"), real_width, M0, fpTubeDlg.m_dlg_tube_winding_angle,cutNum,jumpNum);
	AfxMessageBox(STemp);
	if (jumpNum == -1) {
		OnOpenFiberPathControlTubeParametersDlg();
	}

}
//纤维束
void CMfcogl1Doc::OnSwitchComputeFiberPath() {
	if (m_isShowing == 1) {//tube
		OnComputeFiberPathTube();
	}
	else if (m_isShowing == 2) {//elbow
		OnComputeFiberPath();
	}
}
//方管纤维束
void CMfcogl1Doc::OnComputeFiberPathTube() {
	if (m_bCanComputing == true)
	{
		/* algorithm:
			step1:according to origin angle and width, and model, draw a path from origin place to edge
			step2:according to algebraic pattern(M,	), decide which edge point to wind back
			step3:keep the winding angle, wind to another edge,back to step2,until winded M time.
		*/

		CMfcogl1View* pView;
		POSITION pos = GetFirstViewPosition();
		pView = (CMfcogl1View*)GetNextView(pos);

		model.stepLength = 0;
		windingPathStep = 0.1;
		tmpAngle = 0;
		int M0 = ceil((2 * PI * model.r + 4 * model.a + 4 * model.b) / (model.width / abs(cos(model.angle))));
		std::deque<struct position>* pque = new std::deque<struct position>;
		TubePointList = new std::deque<struct TubePoint>;
		TubeTrackListTime = new std::deque<struct Track>;
		TubePointListTime = new std::deque<struct TubePoint>;
		kList = new std::deque<int>;
		distanceE = new std::deque<float>;

		int totalNum = OnGenerateTubeWindingOrder( pque);
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
		glLineWidth(2);

		position.d = pque->front().d;
		position.x = pque->front().x;
		position.y = pque->front().y;
		position.z = pque->front().z;
		kList->push_front(pque->front().i);
		pque->erase(pque->begin());
		OnRenderSinglePath();
		//OnComputeTubePayeye();
		while (pque->size() != 0 && testStop) {
			//TubePointList = new std::deque<struct TubePoint>;
			OnGeneratePosition(pque);//
			OnRenderSinglePath();
			//OnComputeStartAngle();
			//OnComputeTubePayeye();			//计算一条纤维带
		}
		m_WindingCount[0] = TubePointList->size();
		AfxMessageBox("纤维路径计算完毕\ntube Track Computation Finished.");

 		m_bCanDisplayFiber = true;
		glEndList();
		pView->m_cview_enable_tape_display = false;
		pView->Invalidate(false);
	}
	else{
		AfxMessageBox(_T("请先设置纤维参数\nPlease setup parameter first."));
	}
}

int CMfcogl1Doc::OnGenerateTubeWindingOrder(std::deque<struct position>* pque) {
	//设置一组位于两端的position,符合代数模式 并生成所有路径起点
	float round = 2 * PI * model.r + 4 * model.a + 4 * model.b;
	int M0 = ceil( round / ( model.width / abs(cos(model.angle)) )  );//返回大于等于它的最小整数
	float interval =round / M0;
	float halfInterval = interval / 2;
	int i = 0;
	struct position p;
	for (i = 0; i * interval < model.a; i++) {
		p.x = i * interval;
		p.y = model.b + model.r;
		p.z = 0;
		p.i = i;
		p.d = 1;
		pque->push_back(p);
		p.z = model.length;
		p.d = -1;
		pque->push_back(p);
	}
	for (; i * interval < model.a + PI * model.r / 2; i++) {
		p.x = model.a + model.r * sin((i * interval - model.a) / model.r);
		p.y = model.b + model.r * cos((i * interval - model.a) / model.r);
		p.z = 0;
		p.i = i;
		p.d = 1;
		pque->push_back(p);
		p.z = model.length;
		p.d = -1;
		pque->push_back(p);
	}
	for (; i * interval < model.a + PI * model.r / 2 + 2 * model.b; i++) {
		p.x = model.a + model.r;
		p.y = model.b - (i * interval - model.a - PI * model.r / 2);
		p.z = 0;
		p.i = i;
		p.d = 1;
		pque->push_back(p);
		p.z = model.length;
		p.d = -1;
		pque->push_back(p);
	}
	for (; i * interval < model.a + PI * model.r + 2 * model.b; i++) {
		p.x = model.a + model.r * cos((i * interval - (model.a + PI * model.r / 2 + 2 * model.b)) / model.r);
		p.y = -model.b - model.r * sin((i * interval - (model.a + PI * model.r / 2 + 2 * model.b)) / model.r);
		p.z = 0;
		p.i = i;
		p.d = 1;
		pque->push_back(p);
		p.z = model.length;
		p.d = -1;
		pque->push_back(p);
	}
	for (; i * interval < 3 * model.a + PI * model.r + 2 * model.b; i++) {
		p.x = 2 * model.a - i * interval + PI * model.r + 2 * model.b;
		p.y = -model.b - model.r;
		p.z = 0;
		p.i = i;
		p.d = 1;
		pque->push_back(p);
		p.z = model.length;
		p.d = -1;
		pque->push_back(p);
	}
	for (; i * interval < 3 * model.a + 1.5 * PI * model.r + 2 * model.b; i++) {
		p.x = -model.a - model.r * sin((i * interval - (3 * model.a + PI * model.r + 2 * model.b)) / model.r);
		p.y = -model.b - model.r * cos((i * interval - (3 * model.a + PI * model.r + 2 * model.b)) / model.r);
		p.z = 0;
		p.i = i;
		p.d = 1;
		pque->push_back(p);
		p.z = model.length;
		p.d = -1;
		pque->push_back(p);
	}
	for (; i * interval < 3 * model.a + 1.5 * PI * model.r + 4 * model.b; i++) {
		p.x = -model.a - model.r;
		p.y = -model.b + i * interval - 3 * model.a - 1.5 * PI * model.r - 2 * model.b;
		p.z = 0;
		p.i = i;
		p.d = 1;
		pque->push_back(p);
		p.z = model.length;
		p.d = -1;
		pque->push_back(p);
	}
	for (; i * interval < 3 * model.a + 2 * PI * model.r + 4 * model.b; i++) {
		p.x = -model.a - model.r * cos((i * interval - (3 * model.a + 1.5 * PI * model.r + 4 * model.b)) / model.r);
		p.y = model.b + model.r * sin((i * interval - (3 * model.a + 1.5 * PI * model.r + 4 * model.b)) / model.r);
		p.z = 0;
		p.i = i;
		p.d = 1;
		pque->push_back(p);
		p.z = model.length;
		p.d = -1;
		pque->push_back(p);
	}
	for (; i * interval < 4 * model.a + 2 * PI * model.r + 4 * model.b-tiny; i++) {
		p.x = i * interval - (4 * model.a + 2 * PI * model.r + 4 * model.b);
		p.y = model.b + model.r;
		p.z = 0;
		p.i = i;
		p.d = 1;
		pque->push_back(p);
		p.z = model.length;
		p.d = -1;
		pque->push_back(p);
	}

	return pque->size();
}

void CMfcogl1Doc::OnGeneratePosition(std::deque<struct position>* pque) {

	//vector trackStart = { cos(-tmpAngle), sin(-tmpAngle),0 };
	float angle = -1;
	float cross, localAngle;
	int M0 = ceil((2 * PI * model.r + 4 * model.a + 4 * model.b) / (model.width / abs(cos(model.angle))));
	auto tmpPosition = pque->begin();
	int lastOut;
	if (position.d == -1) {
		lastOut = kList->front();
	}
	else {
		lastOut = kList->back();
	}
	for (auto i = pque->begin(); i < pque->end(); i++) {//遍历 找到下一个合适的节点 
		if (position.d != (*i).d && (*i).i == (lastOut+jumpNum)%M0) {
			/*伴随角度的变换计算*   完全不用这样考虑 在钉外绕圈 可以从任意一个角度进去 即使是反向的角度，最多绕一圈而已/
			/*cross = trackStart.x * (*i).y - trackStart.y * (*i).x;
			if (cross > 0) {
				localAngle = ((*i).x * trackStart.x + (*i).y * trackStart.y) / sqrt(pow((*i).x, 2) + pow((*i).y, 2));
				if (localAngle > angle) { angle = localAngle; tmpPosition = i; }
			}*/
			/*最小距离计算*/
			/*cross =position.x * (*i).y - position.y * (*i).x;
			if (cross < 0) {
				localAngle = ((*i).x * position.x + (*i).y * position.y) / sqrt(pow((*i).x, 2) + pow((*i).y, 2));
				if (localAngle > angle) {
					angle = localAngle;
					tmpPosition = i;
				}
			}*/
			/*MK 固定跳跃数(切点数)* /
			/*
			5等分 3切点 2跳跃数  1 3 5 2 4   1 +3X = 22 X =21/3=7
			20等分 3 切点 7跳跃数 1 X X 2 X X 3 X X   1 8 15 2 9 16 3    
			1+CUT*JUMP = DF+2  JUMP=(DF+1) / CUT 
			CUT*JUMP = DF+1         3J=DF+1    */
			tmpPosition = i;
			break;
		}
	}

	/*Track lastTrack;
	lastTrack.x = model.a + model.r ;
	lastTrack.z = (*tmpPosition).d > 0 ? 0 : model.length;
	lastTrack.spindleAngle = tmpAngle + acos(angle);
	lastTrack.swingAngle = (*tmpPosition).d > 0 ? -PI / 2 : PI / 2;
	tmpAngle += acos(angle);
	TubeTrackListTime->push_back(lastTrack);*/

	position.x = (*tmpPosition).x;
	position.y = (*tmpPosition).y;
	position.z = (*tmpPosition).z;
	position.d = (*tmpPosition).d;
	if (position.d ==1) {
		kList->push_front((*tmpPosition).i);
	}
	else {
		kList->push_back((*tmpPosition).i);
	}
	pque->erase(tmpPosition);
}

void CMfcogl1Doc::OnRenderSinglePath() {
	//initial OpenGL
	//判断当前position处于芯模的哪个面，从而选中合适的入口。
	//类似于状态机，本质上是对两个OnRender不同状态的调用
	//缠绕到面的边缘时，会在OnRender中递归调用下一个面的OnRender，直到缠绕到芯模边缘退出。
	if (-model.a <= position.x && position.x < model.a && position.y>0) {
		OnRenderLinePart(1);
	}
	else if (model.a <= position.x && position.x < model.a + model.r && position.y > 0) {
		OnRenderCurvePart(2);
	}
	else if (-model.b < position.y && position.y <= model.b && position.x>0) {
		OnRenderLinePart(3);
	}
	else if (-model.b - model.r < position.y && position.y <= -model.b && position.x>0) {
		OnRenderCurvePart(4);
	}
	else if (-model.a < position.x && position.x <= model.a && position.y < 0) {
		OnRenderLinePart(5);
	}
	else if (-model.a - model.r < position.x && position.x <= -model.a && position.y < 0) {
		OnRenderCurvePart(6);
	}
	else if (-model.b <= position.y && position.y < model.b && position.x < 0) {
		OnRenderLinePart(7);
	}
	else if (-model.a - model.r <= position.x && position.x < -model.a && position.y >0) {
		OnRenderCurvePart(8);
	}

	//iff out of tube come here.
	//choose next line in the list p.
	//position is global.
}

//to render an arbitrary fiber path 
void CMfcogl1Doc::updatePosition(float* nextPoint) {
	position.x = nextPoint[0];	position.y = nextPoint[1]; position.z = nextPoint[2];
}

int CMfcogl1Doc::outTubeEdge() {
	return (position.z < 0) ? -1 : (position.z >= model.length ? 1 : 0);
}

int CMfcogl1Doc::OnRenderCurvePart(int state) {
	float startPoint[3] = { position.x,position.y,position.z };
	float nextPoint[3] = { position.x,position.y,position.z };
	float phase = 0;
	model.stepLength = 0;
	int direction = position.d;

	switch (state) {
	case 2:
		phase = atan((position.x - model.a) / (position.y - model.b));
		model.stepLength = phase;
		startPoint[0] = model.a;
		startPoint[1] = model.b;
		glBegin(GL_LINE_STRIP);
		//圆柱面上测地线
		while (model.stepLength <= PI / 2 && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			//0:横向（2a）1:高（2b）2:纵向（C）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			nextPoint[0] = startPoint[0] + model.r * sin(model.stepLength);
			nextPoint[1] = startPoint[1] + model.r * cos(model.stepLength);
			nextPoint[2] = startPoint[2] + model.r * (model.stepLength - phase) / tan(model.angle) * direction;
			insertPoint(nextPoint);

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal = -model.stepLength + PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.z = cos(model.angle) * direction;
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}
		glEnd();
		break;
	case 4:
		phase = PI / 2 + atan((-model.b - position.y) / (position.x - model.a));
		model.stepLength = phase;
		startPoint[0] = model.a;
		startPoint[1] = -model.b;
		glBegin(GL_LINE_STRIP);
		//圆柱面上测地线
		while (model.stepLength <= PI && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			//0:横向（2a）1:高（2b）2:纵向（C）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			nextPoint[0] = startPoint[0] + model.r * sin(model.stepLength);
			nextPoint[1] = startPoint[1] + model.r * cos(model.stepLength);
			nextPoint[2] = startPoint[2] + model.r * (model.stepLength - phase) / tan(model.angle) * direction;
			insertPoint(nextPoint);

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal = -model.stepLength + PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}
		glEnd();
		break;
	case 6:
		phase = PI + atan((-position.x - model.a) / (-position.y - model.b));
		model.stepLength = phase;
		startPoint[0] = -model.a;
		startPoint[1] = -model.b;
		glBegin(GL_LINE_STRIP);
		//圆柱面上测地线
		while (model.stepLength <= 1.5 * PI && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			//0:横向（2a）1:高（2b）2:纵向（C）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			nextPoint[0] = startPoint[0] + model.r * sin(model.stepLength);
			nextPoint[1] = startPoint[1] + model.r * cos(model.stepLength);
			nextPoint[2] = startPoint[2] + model.r * (model.stepLength - phase) / tan(model.angle) * direction;
			insertPoint(nextPoint);

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal = -model.stepLength + PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}
		glEnd();
		break;
	case 8:
		phase = 1.5 * PI + atan((position.y - model.b) / (-position.x - model.a));
		model.stepLength = phase;
		startPoint[0] = -model.a;
		startPoint[1] = model.b;
		glBegin(GL_LINE_STRIP);
		//圆柱面上测地线
		while (model.stepLength <= 2 * PI && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			//0:横向（2a）1:高（2b）2:纵向（c）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			nextPoint[0] = startPoint[0] + model.r * sin(model.stepLength);
			nextPoint[1] = startPoint[1] + model.r * cos(model.stepLength);
			nextPoint[2] = startPoint[2] + model.r * (model.stepLength - phase) / tan(model.angle) * direction;
			insertPoint(nextPoint);

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal = -model.stepLength + PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}
		glEnd();
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

int CMfcogl1Doc::OnRenderLinePart(int state) {
	float nextPoint[3] = { position.x,position.y,position.z };
	float startPoint[3] = { position.x,position.y,position.z };
	model.stepLength = 0;
	int direction = position.d;
	switch (state) {
	case 1:
		glBegin(GL_LINE_STRIP);
		//glNormal3f(cos((360-2*segmentw)*3.14/180),sin((360-2*segmentw)*3.14/180),0.0f);		

		while (nextPoint[0] < model.a && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			insertPoint(nextPoint);

			nextPoint[0] = startPoint[0] + model.stepLength * sin(model.angle);
			nextPoint[1] = startPoint[1];
			nextPoint[2] = startPoint[2] + model.stepLength * cos(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal = PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}
		glEnd();
		break;
	case 3:
		glBegin(GL_LINE_STRIP);
		//glNormal3f(cos((360-2*segmentw)*3.14/180),sin((360-2*segmentw)*3.14/180),0.0f);		

		while (-model.b < nextPoint[1] && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			insertPoint(nextPoint);
			nextPoint[0] = startPoint[0];
			nextPoint[1] = startPoint[1] - model.stepLength * sin(model.angle);
			nextPoint[2] = startPoint[2] + model.stepLength * cos(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal = 0;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}
		glEnd();
		break;
	case 5:
		glBegin(GL_LINE_STRIP);
		//glNormal3f(cos((360-2*segmentw)*3.14/180),sin((360-2*segmentw)*3.14/180),0.0f);		

		while (-model.a < nextPoint[0] && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			insertPoint(nextPoint);

			nextPoint[0] = startPoint[0] - model.stepLength * sin(model.angle);
			nextPoint[1] = startPoint[1];
			nextPoint[2] = startPoint[2] + model.stepLength * cos(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal = -PI / 2;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}
		glEnd();
		break;
	case 7:
		glBegin(GL_LINE_STRIP);
		//glNormal3f(cos((360-2*segmentw)*3.14/180),sin((360-2*segmentw)*3.14/180),0.0f);		

		while (nextPoint[1] < model.b && 0 <= nextPoint[2] && nextPoint[2] <= model.length) {
			insertPoint(nextPoint);

			nextPoint[0] = startPoint[0];
			nextPoint[1] = startPoint[1] + model.stepLength * sin(model.angle);
			nextPoint[2] = startPoint[2] + model.stepLength * cos(model.angle) * direction;

			tempTubePoint.x = nextPoint[0];
			tempTubePoint.y = nextPoint[1];
			tempTubePoint.z = nextPoint[2];
			tempTubePoint.normal = -PI;
			tempTubePoint.tangent.x = sin(model.angle) * cos(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.y = sin(model.angle) * sin(tempTubePoint.normal - PI / 2);
			tempTubePoint.tangent.z = direction * cos(model.angle);
			TubePointList->push_back(tempTubePoint);

			model.stepLength += windingPathStep;
		}
		glEnd();
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

void CMfcogl1Doc::insertPoint(float *nextPoint ) {
	nextPoint[2] -= model.length/2;
	glVertex3fv(nextPoint);
	nextPoint[2] += model.length / 2;

}


