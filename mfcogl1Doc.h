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
	//data(double a,double b,double c, double d, double e=0, int no=0):a(a),b(b),c(c),d(d),e(e),no(no) {}
	double a, b, c, d, e;
	int no;
};
struct vec2 {
	double x, y;
	vec2(){}
	vec2(double x,double y):x(x),y(y){}
	vec2 operator+(const vec2& b) { vec2 r; r.x = this->x + b.x; r.y = this->y + b.y; return r; }
	vec2 operator-(const vec2& b) { vec2 r; r.x = this->x - b.x; r.y = this->y - b.y; return r; }
	vec2 operator *(const double k) { vec2 r; r.x = this->x *k; r.y = this->y *k; return r; }
	double sqr_len() { return this->x * this->x + this->y * this->y; }
};
struct vec3 {
	vec3() {}
	vec3(double x, double y, double z) :x(x), y(y), z(z) {}
	vec3 operator-(const vec3& b) { this->x -= b.x; this->y -= b.y; this->z -= b.z; return *this; }
	vec3 operator *(const double k) { this->x *= k; this->y *= k; this->z *= k; return *this; }
	vec3 operator /(const double k) { this->x /= k; this->y /= k; this->z /= k; return *this; }
	double length() { return sqrt(this->x * this->x + this->y * this->y + this->z * this->z); }
	vec3 normalized_cross(const vec3& b) {
		const double x1 = this->x, x2 = b.x, y1 = this->y, y2 = b.y, z1 = this->z, z2 = b.z;
		vec3 r = vec3(y1 * z2 - z1 * y2, z1 * x2 - x1 * z2, x1 * y2 - y1 * x2);
		r = r / r.length();
		return r;
	}
	double x, y, z;
};

/*芯模参数*/
struct tubeModel {
	double a;//x横轴方向 半横轴长(从x=0到倒角开始处)
	double b;//y纵轴方向 半纵轴长(从y=0到倒角开始处)
	double length;//z旋转轴方向 长度
	double r;//倒角半径
	double redundance;//两端冗余长度

	double angle;//缠绕角的弧度(与旋转轴夹角)
	double width;//纤维带宽度
	double round;//周长

	double curveLength;//当前本纤维的长度
};

/*吐丝嘴和机器参数*/
struct payeye {
	double mandrel_speed;//芯模转速
	double pm_distance;//吐丝嘴-芯模转轴距离
	double pm_left_distance;//吐丝嘴-钉圈平面距离
};

/*吐丝嘴的位置(和芯模旋转弧度)*/
struct track {
	double x;//吐丝嘴自初始位置(payeye.pm_distance)伸出的长度
	double z;//吐丝嘴在旋转轴z方向自初始位置(payeye.pm_left_distance)位移长度
	double swingAngle;//吐丝嘴绕x轴旋转的弧度(±PI/2)
	double spindleAngle;//芯模绕z轴向外旋转的弧度，按顺时针算(0~2*PI，初始时平放为0)
};

/*缠绕点的位置(基本)*/
struct tubePathPosition {
	double x, y, z;//平放时的位置
	double direction; //缠绕点缠绕的方向  1:z轴正向 -1:z轴负向
	int index;//缠绕点所属的纤维束序号
};

/*缠绕点的位置(带坐标架信息)*/
struct tubePathCoord {
	double x, y, z;//平放时的位置
	double direction; //缠绕点缠绕的方向  1:z轴正向 -1:z轴负向
	double index;//缠绕点所属的纤维束序号
	double normal_radian;//平放时，丝线法向量的弧度，以x-y平面x轴正向为0 ; x-θ坐标系中的θ
	vec3 tangent;//平放时，切向量的方向矢量
	double suspension;//缠绕到该点时，点到吐丝嘴的距离长度(悬丝长度)
};

struct cylinderModel {
	double middle_length;
	double middle_radius;
	double left_length;
	double left_radius;
	double right_length;
	double right_radius;
	double cut_angle;
	double slippage_coefficient;//μ 
	double left_semi_geodesic_slip_coef;//左半测地线段落的实用μ
	double right_semi_geodesic_slip_coef;//右半测地线段落的实用μ
	double left_const_winding_angle_max_slip_coef;//等缠绕角段落的极限μ
	double right_const_winding_angle_max_slip_coef; // 等缠绕角段落的极限μ
	double angle;//缠绕角的弧度(与旋转轴夹角)
	double width;//纤维带宽度
	double round;//周长
	double cur_length;//当前本纤维的长度
	double now_y;
};

/*压力容器缠绕点的位置 带坐标架信息*/
struct cylinderPathCoord {
	cylinderPathCoord(){}
	cylinderPathCoord(double x,double y,double z,int direction =-1,double currentTheta = 0):x(x),y(y),z(z),direction(direction),currentTheta(currentTheta){}
	double x, y, z;
	int direction;//缠绕的方向 1:x正向 -1:x反向
	vec3 tangent;//当前点的切线方向
	vec3 normal;//当前点的法线方向
	double suspension;//悬丝长
	double currentTheta;//x-θ平面以y轴为起始点的θ值
};

struct toroidModel {
	double R;
	double r;
	double angle;//初始缠绕角度
	double band_width;//带宽
	double lambda;//打滑系数 slippage coefficient
	double M;
};

struct toroidPathCoord {
	toroidPathCoord() {}
	toroidPathCoord(double theta,double phi):theta(theta),phi(phi) {}
	double theta;
	double phi;
	vec3 position;
	vec3 normal;
	void reset(double theta = 0, double phi = 0) { this->theta = theta; this->phi = phi; }
};

class CMfcogl1Doc : public CDocument
{
protected: // create from serialization only
	CMfcogl1Doc();
	DECLARE_DYNCREATE(CMfcogl1Doc)
	void* myGetView();
// Attributes
private:
	bool m_bCanComputing;
public: // properties
	/*added by LMK*/
	int cut_num_offset;//在一端原有的切点基础上往右偏移几个切点？
	int cutNum;//切点数 意思是 第一条纱经过几个来回后到达相邻位置 应该和跳跃数相对 如 5等分 1 3 5 2 4 1 .... 切点数为3 跳跃数为2 在一个切分中无限循环 每次从出去点+cutNum再出发
	int M;//切分的片数
	double interval;//端面每一片切分的长度
	int jumpNum;//跳跃数 跳过的切片数量
	double windingPathStep;//纤维束步长
	int windingRedundentStep;
	double windingStepAngle;
	tubeModel tubeModel;
	payeye payeye;
	tubePathPosition currentPosition;

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

	CString cylinderWindingAlgorithm;
	cylinderModel cylinderModel;
	double jump_angle;
	double global_left_min_r;
	double global_left_cut_theta;
	double global_right_min_r;
	cylinderPathCoord global_cylinder_coord;
	std::shared_ptr<std::deque <struct cylinderPathCoord>> CylinderPointList = NULL;//压力容器缠绕接触点序列
//std::deque<std::deque<struct cylinderPathCoord>*>* CylinderPointList = NULL;

	int m_isShowing;//1:tube(方直管) 2:elbow(圆弯管) 3:cylinder(开口压力容器)  4:cone(圆锥) 5:toroid(圆环)

	/*added by LMK*/
	/*union button*/
	void OnSwitchFiberPathControlDlg();//路径设计->参数设置
	void OnSwitchComputeFiberPath();//路径设计->计算纤维路径
	void OnSwitchComputePayeye();//路径设计->计算机器路径
	void OnSwitchSaveTrack();//路径设计->保存路径

	/*tube 参数设置*/
	afx_msg void OnOpenFiberPathControlTubeParametersDlg();//OnSwitchFiberPathControlDlg跳转
	/*tube path*/
	afx_msg void OnComputeFiberPathTube();//OnSwitchComputeFiberPath跳转
	afx_msg int OnRenderLinePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg int OnRenderCurvePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg void OnRenderSinglePath(std::deque<struct tubePathCoord>* singlePathList);
	inline void updatePosition(double* nextPoint);
	inline void insertPoint(double* nextPoint);
	inline int outTubeEdge(double z);
	/*tube 纤维路径*/
	afx_msg void OnComputePayeyeTube();// OnSwitchComputePayeye跳转
	void pushTubePathCoord(const double* nextPoint, const double normal_radian, const int direction, std::deque<struct tubePathCoord>* singlePathList);
	void pushTubePathCoord(const double* nextPoint, const double normal_radian, const int direction, std::deque<struct tubePathCoord>* singlePathList, double angle);
	void justCalculateTubePathCoord(const double* nextPoint, const double normal_radian, const int direction);
	afx_msg int OnGenerateTubeEdgeList(std::deque<struct tubePathPosition>& TubeList, double interval, int side);
	afx_msg void OnGeneratePosition(std::deque<struct tubePathPosition>& TubeStartList, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg void computeTubeTrack(std::deque<struct tubePathCoord>::iterator currentTubePoint, track& tempTubeTrack);
	void vectorRotator(const vec3& currentVector, const double& angle, vec3& rotatedVector, double direction);
	//保存机器路径到文件中
	afx_msg void OnSaveTubeTrackData();//OnSwitchSaveTrack跳转
	void dataCleaner(std::vector<data>& in);
	/*helper*/
	double _rayHitCurvePartAdj(int quad1, int quad2, vec2& src, vec2& dir, double a, double b, double r);
	double _rayHitSuspension(const double& a, const double& b, const double& r, const double& shell, const double& x, const double& y, const double& dir_x, const double& dir_y);
	double _rayHitAngle(const double& a, const double& b, const double& r, const double& shell, const double& x, const double& y, const double& dir_x, const double& dir_y);
	inline double _circleMinus(double a, double b);//计算a b的弧度差(a>b)

	//cylinder参数设置
	afx_msg void OnOpenFiberPathControlCylinderParametersDlg();//OnSwitchFiberPathControlDlg跳转
	//cylidner纤维路径计算
	afx_msg void OnComputeFiberPathCylinder();//OnSwitchComputeFiberPath跳转
	void OnRenderRightEllipsoidNonGeodesic(cylinderPathCoord& currentPoint);
	void OnRenderLeftEllipsoidNonGeodesic(cylinderPathCoord& currentPoint);
	void OnRenderMiddleCylinder(cylinderPathCoord& currentPoint);
	void OnRenderRightEllipsoidNonGeodesic(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint);
	void OnRenderLeftEllipsoidNonGeodesic(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint);
	void OnRenderMiddleCylinder(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint);
	typedef void (CMfcogl1Doc::* CylinderLineAlgorithm)(cylinderPathCoord& currentPoint);
	CylinderLineAlgorithm leftCylinder, rightCylinder, middleCylinder;
	//void OnRenderRightEllipsoid(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	//void OnRenderRightEllipsoidFixedAngle(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	//void OnRenderLeftEllipsoid(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	//void OnRenderLeftEllipsoidFixedAngle(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show);
	double CMfcogl1Doc::loop_each_angle(std::vector<double>& point, struct cylinderModel &_min_model);
	void CMfcogl1Doc::single_angle_multi_line(std::vector<double>& point, struct cylinderModel& _min_model);

	/*toroid参数设置*/
	afx_msg void OnOpenFiberPathControlToroidParametersDlg();//OnSwitchFiberPathControlDlg跳转
	/*toroid纤维路径计算*/
	afx_msg void OnComputeFiberPathToroid();//OnSwitchComputeFiberPath跳转
	toroidModel toroidModel;
	/*helper*/
	vec3 thetaphi2xyz(double theta, double phi);
	vec3 thetaphi2nxyz(double theta, double phi);
	vec3 thetaphi2dxyz(double theta, double phi);
	void thetaphi2gl(double oldtheta, double oldphi, double theta, double phi, int GL_MODEL);

	/*debug*/
	GLfloat matRed[4] = { 0.99F, 0.01F, 0.21F, 1.00F };
	GLfloat matYellow[4] = { 0.99F, 0.99F, 0.0F, 1.00F };
	GLfloat matGreen[4] = { 0.01F, 0.99F, 0.0F, 1.00F };
	GLfloat matBlue[4] = { 0.01F, 0.01F, 0.99F, 1.00F };
	template<typename T>
	static void debug_show(T x) {
		CString strMsg;
		if (typeid(x) == typeid(int)) {
			strMsg.Format("Value:%d\n", x);
		}
		else if (typeid(x) == typeid(double)) {
			strMsg.Format("Value:%f\n", x);
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
	double m_FilamentWidth, cylinder_radius[6];
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
	afx_msg void OnComputeFiberPathCone();
	void DestructTubeMemory();
	void DestructCylinderMemory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ReadDataFromDisk(LPCTSTR lpFileName, CString lpFileExtName);
	void SaveDataToDisk(LPCTSTR lpszFilter);
};

