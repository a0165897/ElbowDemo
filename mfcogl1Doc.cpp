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

/*added by LMK*/
#include "DlgFiberPathControlsTube.h"

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
	ON_COMMAND(IDM_COMPUTE_FIBER_PATH, OnComputeFiberPath)
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
		m_doc_tube_winding_angle = fpTubeDlg.m_dlg_tube_winding_angle;
		m_doc_tube_band_width = fpTubeDlg.m_dlg_tube_band_width;
		debug_show(m_doc_tube_band_width);
	}
}
