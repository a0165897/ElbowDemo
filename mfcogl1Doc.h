// mfcogl1Doc.h : interface of the CMfcogl1Doc class

#pragma once

#include "HintDialog.h"
#include<typeinfo>
#include <deque>

const float tolerance=1.0e-06;
const int size=800;

/*global tube model
	全局的芯模相关数据
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
	double angle;//缠绕角度(与旋转轴夹角)
	double width;//纤维带宽度
	float stepLength;
	GLfloat a, b, length, r;//a,b 半横轴/纵轴长度
};

struct payeye {
	float mandrel_speed;//芯模转速
	float pm_distance;//吐丝嘴距芯模最内缘距离
	float pm_left_distance;//吐丝嘴距芯模左侧距离
	float mandrel_redundancy;//芯模左右侧冗余
};
/*缠绕接触点的位置*/
struct TubePoint {
	float x, y, z, direction;
	int index;
	float normal_radian;//对于柱状物法向必然垂直于径向，因此normal只记录法向量的旋转角（rad），以X轴正向起始，逆时针为正。
	float tPoint;
	vector tangent;
};
struct bestPoint {
	float distance;
	std::deque<struct TubePoint>::iterator pointNum;
	struct Track track;
};
/*缠绕端点的位置*/
struct position {
	float x, y, z, direction; //direction back or go
	int index;//index 编号
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
	int jumpNum;//跳跃数 从对话框中算出
	int cutNum;//切点数 意思是 第一条纱经过几个来回后到达相邻位置 应该和跳跃数相对 如 5等分 1 3 5 2 4 1 .... 切点数为3 跳跃数为2 在一个切分中无限循环 每次从出去点+cutNum再出发
	model model;
	payeye payeye;
	position currentPosition;
	float m_doc_tube_band_width;
	float m_doc_tube_winding_angle;
	int m_doc_tube_cut_num;
	float windingPathStep;
	float currentAngle,angleStep;
	TubePoint tempTubePoint;
	std::deque<struct position> TubeStartList;//聚集全部两端的起点
	int TubeCurrentStart[2];//为计算下一跳纤维起点的编号，分别记录两端当前纤维起点的编号
	std::deque<struct TubePoint>* TubePointList = NULL;//缠绕接触点序列 
	std::deque<struct Track>* TubeTrackListTime = NULL;//缠绕机器路径序列 等时
	std::deque<struct TubePoint>* TubePointListTime = NULL;
	std::deque<float>* distanceE = NULL;
	int m_isShowing;//1:tube(方直管) 2:elbow(圆弯管) 3:cylinder(开口压力容器)  

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
	float tiny = 0.001;//消除周期性误差
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

