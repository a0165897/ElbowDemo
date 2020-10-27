// mfcogl1Doc.h : interface of the CMfcogl1Doc class

#pragma once

#include "HintDialog.h"
#include<typeinfo>
#include <deque>

const float tolerance=1.0e-06;
const int size=800;

/*全局的芯模相关数据*/

struct float3 {
	float x, y, z;
};

/*芯模参数*/
struct model {
	double angle;//缠绕角的弧度(与旋转轴夹角)
	double width;//纤维带宽度
	float curveLength;//当前本纤维的长度
	float a;//x横轴方向 半横轴长(从x=0到倒角开始处)
	float b;//y纵轴方向 半纵轴长(从y=0到倒角开始处)
	float length;//z旋转轴方向 长度
	float r;//倒角半径
};

/*吐丝嘴和机器参数*/
struct payeye {
	float mandrel_speed;//芯模转速
	float pm_distance;//吐丝嘴距芯模最内缘距离
	float pm_left_distance;//吐丝嘴距芯模左侧距离
	float mandrel_redundancy;//芯模左右侧冗余
};

/*吐丝嘴的位置(和芯模旋转弧度)*/
struct track {
	float x;//吐丝嘴自初始位置(payeye.pm_distance)伸出的长度
	float z;//吐丝嘴在旋转轴z方向自初始位置(payeye.pm_left_distance)位移长度
	float swingAngle;//吐丝嘴绕x轴旋转的弧度(±PI/2)
	float spindleAngle;//芯模绕z轴向外旋转的弧度(0~2*PI，初始时平放为0)
};

/*缠绕点的位置(基本)*/
struct tubePathPosition {
	float x, y, z;//平放时的位置
	float direction; //缠绕点缠绕的方向  1:z轴正向 -1:z轴负向
	float index;//缠绕点所属的纤维束序号
};

/*缠绕点的位置(带坐标架信息)*/
struct tubePathCoord {
	float x, y, z;//平放时的位置
	float direction; //缠绕点缠绕的方向  1:z轴正向 -1:z轴负向
	float index;//缠绕点所属的纤维束序号
	float normal_radian;//平放时，法向量的弧度，以x-y平面x轴正向为0
	float3 tangent;//平放时，切向量的方向矢量
	float suspension;//缠绕到该点时，点到吐丝嘴的距离长度(悬丝长度)
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
	int testStop = 1;
	int M0;
	float interval;
	int jumpNum;//跳跃数 从对话框中算出
	int cutNum;//切点数 意思是 第一条纱经过几个来回后到达相邻位置 应该和跳跃数相对 如 5等分 1 3 5 2 4 1 .... 切点数为3 跳跃数为2 在一个切分中无限循环 每次从出去点+cutNum再出发
	model model;
	payeye payeye;
	tubePathPosition currentPosition;
	float m_doc_tube_band_width;
	float m_doc_tube_winding_angle;
	int m_doc_tube_cut_num;
	float windingPathStep;
	float currentAngle,angleStep;
	tubePathCoord tempTubePoint;
	std::deque<struct tubePathPosition> TubeStartList;//聚集全部两端的起点
	int TubeCurrentStart[2];//为计算下一跳纤维起点的编号，分别记录两端当前纤维起点的编号
	std::deque<std::deque<struct tubePathCoord>*>* TubePointList = NULL;//缠绕接触点序列 
	std::deque<std::deque<struct track>*>* TubeTrackList = NULL;//吐丝嘴位置序列 
	std::deque<struct track>* TubeTrackListTime = NULL;//缠绕机器路径序列 等时
	std::deque<struct tubePathCoord>* TubePointListTime = NULL;
	std::deque<float>* distanceE = NULL;
	int m_isShowing;//1:tube(方直管) 2:elbow(圆弯管) 3:cylinder(开口压力容器)  

public:
	int quarter_elbow,circuit_step,total_cylinder,circuit_num;
	int m,k;
	unsigned long m_WindingCount[6];
	unsigned short m_numlayer;
	float m_FilamentWidth,cylinder_radius[6];
	PathPoint *pPath;
	track *ptrack;
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
	float tiny = 0.001;//消除周期性误差
	void OnSwitchFiberPathControlDlg();
	void OnSwitchComputeFiberPath();
	void OnSwitchComputePayeye();
	afx_msg void OnRenderSinglePath(std::deque<struct tubePathCoord>* singlePathList);
	afx_msg int OnRenderLinePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg int OnRenderCurvePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	void updatePosition(float* nextPoint);
	void insertPoint(float* nextPoint);
	int outTubeEdge();
	afx_msg int OnGenerateTubeStartList( std::deque<struct tubePathPosition>& pque);
	afx_msg void OnGeneratePosition(std::deque<struct tubePathPosition>& pque);
	std::deque<tubePathCoord>::iterator updateTubeTrackListTime(std::deque<tubePathCoord>::iterator currentPoint);
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

