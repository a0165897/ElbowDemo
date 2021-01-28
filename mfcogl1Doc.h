// mfcogl1Doc.h : interface of the CMfcogl1Doc class

#pragma once

#include "HintDialog.h"
#include<typeinfo>
#include <deque>
#include <vector>
#include <string>
#include <memory>

//模板 类型无关方法/类 payeye track
//OOP 多态 fiber path
//smart ptr - new/delete

/*全局的芯模相关数据*/
struct data {
	//data(float a,float b,float c, float d, float e=0, int no=0):a(a),b(b),c(c),d(d),e(e),no(no) {}
	float a, b, c, d, e;
	int no;
};
struct vec2 {
	float x, y;
	vec2(){}
	vec2(float x,float y):x(x),y(y){}
	vec2 operator+(const vec2& b) { vec2 r; r.x = this->x + b.x; r.y = this->y + b.y; return r; }
	vec2 operator-(const vec2& b) { vec2 r; r.x = this->x - b.x; r.y = this->y - b.y; return r; }
	vec2 operator *(const float k) { vec2 r; r.x = this->x *k; r.y = this->y *k; return r; }
	float sqr_len() { return this->x * this->x + this->y * this->y; }
};
struct vec3 {
	float x, y, z;
};

/*芯模参数*/
struct tubeModel {
	float a;//x横轴方向 半横轴长(从x=0到倒角开始处)
	float b;//y纵轴方向 半纵轴长(从y=0到倒角开始处)
	float length;//z旋转轴方向 长度
	float r;//倒角半径
	float redundance;//两端冗余长度

	float angle;//缠绕角的弧度(与旋转轴夹角)
	float width;//纤维带宽度
	float round;//周长

	float curveLength;//当前本纤维的长度
};

/*吐丝嘴和机器参数*/
struct payeye {
	float mandrel_speed;//芯模转速
	float pm_distance;//吐丝嘴-芯模转轴距离
	float pm_left_distance;//吐丝嘴-钉圈平面距离
};

/*吐丝嘴的位置(和芯模旋转弧度)*/
struct track {
	float x;//吐丝嘴自初始位置(payeye.pm_distance)伸出的长度
	float z;//吐丝嘴在旋转轴z方向自初始位置(payeye.pm_left_distance)位移长度
	float swingAngle;//吐丝嘴绕x轴旋转的弧度(±PI/2)
	float spindleAngle;//芯模绕z轴向外旋转的弧度，按顺时针算(0~2*PI，初始时平放为0)
};

/*缠绕点的位置(基本)*/
struct tubePathPosition {
	float x, y, z;//平放时的位置
	float direction; //缠绕点缠绕的方向  1:z轴正向 -1:z轴负向
	int index;//缠绕点所属的纤维束序号
};

/*缠绕点的位置(带坐标架信息)*/
struct tubePathCoord {
	float x, y, z;//平放时的位置
	float direction; //缠绕点缠绕的方向  1:z轴正向 -1:z轴负向
	float index;//缠绕点所属的纤维束序号
	float normal_radian;//平放时，丝线法向量的弧度，以x-y平面x轴正向为0 ; x-θ坐标系中的θ
	vec3 tangent;//平放时，切向量的方向矢量
	float suspension;//缠绕到该点时，点到吐丝嘴的距离长度(悬丝长度)
};

struct cylinderModel {
	float middle_length;
	float middle_radius;
	float left_length;
	float left_radius;
	float right_length;
	float right_radius;
	float cut_angle;
	float slippage_coefficient;//μ 
	float left_using_coef;//半测地线段落的实用μ
	float right_using_coef;
	float left_slippage_point;//等缠绕角段落的极限μ
	float right_slippage_point;
	double angle;//缠绕角的弧度(与旋转轴夹角)
	double width;//纤维带宽度
	double round;//周长
	double curveLength;//当前本纤维的长度
};

/*压力容器缠绕点的位置 带坐标架信息*/
struct cylinderPathCoord {
	cylinderPathCoord(){}
	cylinderPathCoord(float x,float y,float z,int direction =-1,float currentTheta = 0):x(x),y(y),z(z),direction(direction),currentTheta(currentTheta){}
	float x, y, z;
	int direction;//缠绕的方向 1:x正向 -1:x反向
	vec3 tangent;//当前点的切线方向
	vec3 normal;//当前点的法线方向
	float suspension;//悬丝长
	float currentTheta;//x-θ平面以y轴为起始点的θ值
};

class CMfcogl1Doc : public CDocument
{
protected: // create from serialization only
	CMfcogl1Doc();
	DECLARE_DYNCREATE(CMfcogl1Doc)

// Attributes
private:
	bool m_bCanComputing;
	Site *psite;
public: // properties
	/*added by LMK*/
	int cut_num_offset;//在一端原有的切点基础上往右偏移几个切点？
	int cutNum;//切点数 意思是 第一条纱经过几个来回后到达相邻位置 应该和跳跃数相对 如 5等分 1 3 5 2 4 1 .... 切点数为3 跳跃数为2 在一个切分中无限循环 每次从出去点+cutNum再出发
	int M0;//切分的片数
	float interval;//端面每一片切分的长度
	int jumpNum;//跳跃数 跳过的切片数量
	float windingPathStep;//纤维束步长
	float currentAngle;//当前的缠绕角 未初始化的属性BUG
	tubeModel tubeModel;
	payeye payeye;
	tubePathPosition currentPosition;

	std::deque<struct tubePathPosition> TubeBothList;//聚集头部的起点
	std::deque<struct tubePathPosition> TubeStepList;//聚集尾部的起点
	int TubeCurrentStart[2];//0：头部的纤维序号 1：尾部的纤维序号(当前)
	/*
	<deque>纤维路径序列:
		<deque>纤维路径0:
			<struct tubePathCoord>路径0上点0
			<struct tubePathCoord>点1
			...
		</deque>
		<deque>纤维路径1:
			<struct tubePathCoord>路径1上点0
			<struct tubePathCoord>点1
			...
		</deque>
		...
	</deque>
	*/
	std::deque<std::deque<struct tubePathCoord>*>* TubePointList = NULL;//缠绕接触点序列 
	std::deque<std::deque<struct track>*>* TubeTrackList = NULL;//吐丝嘴位置序列 
	std::deque<struct track>* TubeTrackListTime = NULL;//缠绕机器路径序列 等时
	std::deque<struct tubePathCoord>* TubePointListTime = NULL;
	std::deque<float>* distanceE = NULL;
	tubePathCoord tempTubePathCoord;

	CString cylinderWindingAlgorithm;
	cylinderModel cylinderModel;
	float jumpAngle;
	float global_left_min_r;
	float global_left_cut_theta;
	float global_right_min_r;
	cylinderPathCoord global_cylinder_coord;
	std::shared_ptr<std::deque <struct cylinderPathCoord>> CylinderPointList = NULL;//压力容器缠绕接触点序列
//std::deque<std::deque<struct cylinderPathCoord>*>* CylinderPointList = NULL;

	int m_isShowing;//1:tube(方直管) 2:elbow(圆弯管) 3:cylinder(开口压力容器)  4:cone(圆锥)

	/*added by LMK*/
	//相应三个菜单按钮
	void OnSwitchFiberPathControlDlg();//路径设计->参数设置
	void OnSwitchComputeFiberPath();//路径设计->计算纤维路径
	void OnSwitchComputePayeye();//路径设计->计算机器路径
	void OnSwitchSaveTrack();//路径设计->保存路径

	//tube参数设置
	afx_msg void OnOpenFiberPathControlTubeParametersDlg();//switch跳转->接受缠绕属性并计算切点数等属性
	//tube纤维路径计算
	afx_msg void OnComputeFiberPathTube();//switch跳转->主计算函数
	afx_msg void OnRenderSinglePath(std::deque<struct tubePathCoord>* singlePathList);
	afx_msg int OnRenderLinePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg int OnRenderCurvePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	inline void updatePosition(float* nextPoint);
	inline void insertPoint(float* nextPoint);
	inline int outTubeEdge(float z);
	//tube机器路径计算
	afx_msg void OnComputePayeyeTube();//switch跳转->主计算函数
	void pushTubePathCoord(const float* nextPoint, const float normal_radian, const int direction, std::deque<struct tubePathCoord>* singlePathList,float angle);
	afx_msg int OnGenerateTubeEdgeList(std::deque<struct tubePathPosition>& TubeList, float interval, int side);
	afx_msg void OnGeneratePosition(std::deque<struct tubePathPosition>& TubeStartList, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg void computeTubeTrack(std::deque<struct tubePathCoord>::iterator currentTubePoint, track& tempTubeTrack);
	void vectorRotateAroundZ(const vec3& currentVector, const float& angle, vec3& rotatedVector, float direction);
	float suspensionCompute(float a, float b, float r, float shell, float x, float y, float dir_x, float dir_y);
	//tube机器路径保存
	afx_msg void OnSaveTrackTube();//switch跳转->主计算函数
	void _fill_the_gap(track* preTrack, track* curTrack, std::vector<data>& in,int no);
	void calData(std::vector<data>& in);

	//cylinder参数设置
	afx_msg void OnOpenFiberPathControlCylinderParametersDlg();
	//cylidner纤维路径计算
	void OnRenderRightEllipsoid(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	void OnRenderRightEllipsoidFixedAngle(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	void OnRenderRightEllipsoidNonGeodesic(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	void OnRenderLeftEllipsoid(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	void OnRenderLeftEllipsoidFixedAngle(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	void OnRenderLeftEllipsoidNonGeodesic(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	void OnRenderMiddleCylinder(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	void OnRenderLeftEllipsoidAuto(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	void OnRenderRightEllipsoidAuto(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	void OnRenderMiddleCylinderAuto(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	//debug
	template<typename T>
	static void debug_show(T x) {
		CString strMsg;
		if (typeid(x) == typeid(int)) {
			strMsg.Format("Value:%d\n", x);
		}
		else if (typeid(x) == typeid(float)) {
			strMsg.Format("Value:%f\n", x);
		}
		else if (typeid(x) == typeid(const char *)) {
			strMsg.Format("Value:%s\n", x);
		}
		else if (typeid(x) == typeid(const CString)) {
			strMsg.Format("Value:%s\n", x);
		}
		else {
			strMsg.Format("missing template typeID:%s\n", typeid(x).name());
		}
		MessageBoxA(0, strMsg, "debug", 0);
	}

	/*END LMK*/

	unsigned long m_WindingCount[6];
	unsigned short m_numlayer;
	float m_FilamentWidth, cylinder_radius[6];
	bool m_bFiberPathComplete, m_bPayeyeComplete, m_bUseLayer;
	// Implementation
	void ResetWndDesign();
	bool IsEachPrime(int m1, int m2);
	virtual ~CMfcogl1Doc();
protected:

// Generated message map functions
protected:
	//void WriteTestData(unsigned short latitude,unsigned short long_elbow,unsigned short long_cylinder);
	////{{AFX_MSG(CMfcogl1Doc)
	//afx_msg void OnFileSave();
	//afx_msg void OnFileOpen();

	/*added by LMK*/
	void OnOpenFiberPathControlConeParametersDlg();
	afx_msg void OnComputeFiberPathCylinder();
	afx_msg void OnComputeFiberPathCone();
	void CleanTubeMemory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ReadDataFromDisk(LPCTSTR lpFileName, CString lpFileExtName);
	void SaveDataToDisk(LPCTSTR lpszFilter);
};

