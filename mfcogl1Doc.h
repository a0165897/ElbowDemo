// mfcogl1Doc.h : interface of the CMfcogl1Doc class

#pragma once

#include "HintDialog.h"
#include<typeinfo>
#include <deque>

const float tolerance=1.0e-06;
const int size=800;

/*global tube model
	ȫ�ֵ�оģ�������
*/
struct vector {
	float x, y, z;
};
struct Track
{
	float x, z;
	float swingAngle, spindleAngle;
};
struct model {
	double angle;//���ƽǶ�(����ת��н�)
	double width;//��ά�����
	float stepLength;
	GLfloat a, b, length, r;//a,b �����/���᳤��
};

struct payeye {
	float mandrel_speed;//оģת��
	float pm_distance;//��˿���оģ����Ե����
	float pm_left_distance;//��˿���оģ������
	float mandrel_redundancy;//оģ���Ҳ�����
};
/*���ƽӴ����λ��*/
struct TubePoint {
	float x, y, z, direction;
	int index;
	float normal_radian;//������״�﷨���Ȼ��ֱ�ھ������normalֻ��¼����������ת�ǣ�rad������X��������ʼ����ʱ��Ϊ����
	float tPoint;
	vector tangent;
};
struct bestPoint {
	float distance;
	std::deque<struct TubePoint>::iterator pointNum;
	struct Track track;
};
/*���ƶ˵��λ��*/
struct position {
	float x, y, z, direction; //direction back or go
	int index;//index ���
};





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
public:
	/*added by LMK*/
	int M0;
	float interval;
	int testStop = 1;
	int jumpNum;//��Ծ�� �ӶԻ��������
	int cutNum;//�е��� ��˼�� ��һ��ɴ�����������غ󵽴�����λ�� Ӧ�ú���Ծ����� �� 5�ȷ� 1 3 5 2 4 1 .... �е���Ϊ3 ��Ծ��Ϊ2 ��һ���з�������ѭ�� ÿ�δӳ�ȥ��+cutNum�ٳ���
	model model;
	payeye payeye;
	position currentPosition;
	float m_doc_tube_band_width;
	float m_doc_tube_winding_angle;
	int m_doc_tube_cut_num;
	float windingPathStep;
	float currentAngle,angleStep;
	TubePoint tempTubePoint;
	std::deque<struct position> TubeStartList;//�ۼ�ȫ�����˵����
	int TubeCurrentStart[2];//Ϊ������һ����ά���ı�ţ��ֱ��¼���˵�ǰ��ά���ı��
	std::deque<struct TubePoint>* TubePointList = NULL;//���ƽӴ������� 
	std::deque<struct Track>* TubeTrackListTime = NULL;//���ƻ���·������ ��ʱ
	std::deque<struct TubePoint>* TubePointListTime = NULL;
	std::deque<float>* distanceE = NULL;
	int m_isShowing;//1:tube(��ֱ��) 2:elbow(Բ���) 3:cylinder(����ѹ������)  

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

// Implementation
public:
	void ResetWndDesign();
	bool IsEachPrime(int m1,int m2);
	void InitialAngle();

	virtual ~CMfcogl1Doc();

	/*added by LMK*/
	float tiny = 0.001;//�������������
	void OnSwitchFiberPathControlDlg();
	void OnSwitchComputeFiberPath();
	void OnSwitchComputePayeye();
	afx_msg void OnRenderSinglePath();
	afx_msg int OnRenderLinePart(int state);
	afx_msg int OnRenderCurvePart(int state);
	void updatePosition(float* nextPoint);
	void insertPoint(float* nextPoint);
	int outTubeEdge();
	afx_msg int OnGenerateTubeStartList( std::deque<struct position>& pque);
	afx_msg void OnGeneratePosition(std::deque<struct position>& pque);
	std::deque<TubePoint>::iterator updateTubeTrackListTime(std::deque<TubePoint>::iterator currentPoint);
	void OnComputeStartAngle();

	template<typename T>
	static void debug_show(T x) {
		CString strMsg;
		if (typeid(x) == typeid(unsigned int)) {
			strMsg.Format("Value:%d\n", x);
		}
		else if (typeid(x) == typeid(float)) {
			strMsg.Format("Value:%f\n", x);
		}
		else if (typeid(x) == typeid(const char *)) {
			strMsg.Format("Value:%s\n", x);
		}
		else {
			strMsg.Format("missing template typeID:%s\n", typeid(x).name());
		}
		MessageBoxA(0, strMsg, "debug", 0);
	}
	//END LMK


protected:

// Generated message map functions
protected:
	void WriteTestData(unsigned short latitude,unsigned short long_elbow,unsigned short long_cylinder);
	//{{AFX_MSG(CMfcogl1Doc)
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();

	/*added by LMK*/
	afx_msg void OnOpenFiberPathControlTubeParametersDlg();
	afx_msg void OnComputeFiberPathTube();
	afx_msg void OnComputePayeyeTube();
	void CMfcogl1Doc::CleanTubeMemory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ReadDataFromDisk(LPCTSTR lpFileName, CString lpFileExtName);
	void SaveDataToDisk(LPCTSTR lpszFilter);
};

