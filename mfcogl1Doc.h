// mfcogl1Doc.h : interface of the CMfcogl1Doc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCOGL1DOC_H__27D94B32_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_)
#define AFX_MFCOGL1DOC_H__27D94B32_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HintDialog.h"
#include<typeinfo>

const float tolerance=1.0e-06;
const int size=800;
struct PathPoint
{
	float phi;
	float theta;
	float alpha;
	int flag;
	int circle;
};
class CMfcogl1Doc : public CDocument
{
protected: // create from serialization only
	CMfcogl1Doc();
	DECLARE_DYNCREATE(CMfcogl1Doc)

// Attributes
private:
	
	int step;
	int adjust;
	int g;
	int large;
	int mid;
	int ismall;
	int nocycle;
	int (*flag_add)[size];
	float m_IniWndAngle,m_FilamentThickness;
	float m_friction,theta_cycle,yadjust,y1,c,frict;
	float *phi,*alpha,*phi_offset,*alpha_offset,(*phi_add)[size],(*y_add)[size],(*alpha_add)[size];
	float *phi_turn,*y_turn,*alpha_turn;
	bool friction_test,m_bCanComputing,data_format,data_compress;
	Site *psite;
	CString lpOpenPathFile;
	CString lpOpenTrackFile;
	float Offset,eyeWidth,eyeAccelerate,rollerWidth,payeye_offset;
	short int track_layer;

	/*added by LMK*/
	float m_doc_tube_band_width;
	float m_doc_tube_winding_angle;

public:
	int quarter_elbow,circuit_step,total_cylinder,circuit_num;
	int m,k;
	unsigned long m_WindingCount[6];
	unsigned short m_numlayer;
	float m_FilamentWidth,cylinder_radius[6];
	PathPoint *pPath;
	Track *ptrack;
	Pressure *pPress;
	bool m_bCanDisplayFiber,m_bPayeyeComplete,m_bUseLayer;
	bool m_bSavePath,m_bSavePayeye,m_bPressTest;
	double m_sweep_radius,m_height;
	double m_pipe_radius,m_span_angle;
	float deviation;
	/*added by LMK*/
	int m_isShowing;//elbow or tube
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcogl1Doc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	void ResetWndDesign();
	void SolvePhiOffset(int m,int step,float phi[],float phi_offset[],float alpha_offset[],float span_angle,float thetacycle,float tolerant_error,int &ismall);
	void PhiPartition(int m,int step,float phi[],float alpha[],float thetacycle,float tolerant_error);
	float IntegralFun(float b,float x);
	float ComplexSimpson(float start,float end,float tolerant_error);
	float Simpson(float start,float end,int step);
	bool IsEachPrime(int m1,int m2);
	int CalculateSplittingNum(float b,float &thetacycle,int  countlayer=0,float thick=0.0);
    void Lattice(int m,int step,float thetacycle,float spanangle);
	void InitialAngle();
	bool FrictionTest(int m,int adjust,float b,float yadjust,float y0,float frictbound,
		float alpha,float phitorus[],float alphatorus[]);

	void Transition(int m,int adjust,int phi,float b,float yadjust,
		float alpha,float phitorus[],float alphatorus[],int flagad[][size],
		float phiad[][size],float yad[][size],float alphaad[][size]);
	bool Turn(int m,int step,float b,float alpha2, float frictbound,int &g,float &y1,float &frict);
    int FillInPath(int m,int step,int total,int circuitnum,int large,int ismall,int mid,int g,
		int nocycle,float phi[],float alpha[],int flagad[][size],float phiad[][size],
		float yad[][size],float alphaad[][size],float phiturn[],float yturn[],float alphaturn[]);
 
	void ModifyMandrelParameters(float a, float b, float span_angle,float height);
	
	void posite(struct Mandrel mand,struct PathPoint path);
	float falpha(float alpha,float xcarriage,float zcarriage,
			 float highPayeye,struct Site *psite);
	float chord(float a,float b,float xcarriage,float zcarriage,
			float highPayeye,struct Site *psite);
	float findspindleangle(float alpha,float xcarriage,float zcarriage,
				float highPayeye,float step,struct Site *psite);
	float calxcarriage(float& alpha0,float alpha,float dswing,float ycylinderi,
			float ycylinder,float slip,int& index,int frontback,int flag,int flag2,
			int n,struct Mandrel mand,struct Miscellence misc);
	void CalculateTrack(struct Mandrel mand,struct Miscellence misc,
				float epsilon,struct PathPoint *path,
				int countPathPoint);
	virtual ~CMfcogl1Doc();

	/*added by LMK*/
	void OnSwitchFiberPathControlDlg();

	template<typename T>
	void debug_show(T x) {
		CString strMsg;
		if (typeid(x) == typeid(int)) {
			strMsg.Format("Value:%d\n", x);
		}
		if (typeid(x) == typeid(float)) {
			strMsg.Format("Value:%f\n", x);
		}
		if (typeid(x) == typeid(char*)) {
			strMsg.Format("Value:%s\n", x);
		}
		MessageBoxA(0, strMsg, "debug", 0);
	}

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	//unused function are as follows
	//double DetermineAngleByReferenceAngle(double agl_at1qua,double areference_angle);
	//double ResolveClairautByReferenceAngle(double refwagl,double phi);

#endif

protected:

// Generated message map functions
protected:
	float DetermineStep(int m,float thetacycle,float tolerant_error);
	void WriteTestData(unsigned short latitude,unsigned short long_elbow,unsigned short long_cylinder);
	//{{AFX_MSG(CMfcogl1Doc)
	afx_msg void OnOpenFiberPathControlParametersDlg();
	afx_msg void OnComputeFiberPath();
	afx_msg void OnComputePayeye();
	afx_msg void OnPressureTest();
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnViewDisplayParameter();

	/*added by LMK*/
	afx_msg void OnOpenFiberPathControlTubeParametersDlg();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ReadDataFromDisk(LPCTSTR lpFileName, CString lpFileExtName);
	void SaveDataToDisk(LPCTSTR lpszFilter);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCOGL1DOC_H__27D94B32_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_)
