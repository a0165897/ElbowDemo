// mfcogl1Doc.h : interface of the CMfcogl1Doc class

#pragma once

#include "HintDialog.h"
#include<typeinfo>
#include <deque>
#include <vector>
#include <string>
#include <memory>

//ģ�� �����޹ط���/�� payeye track
//OOP ��̬ fiber path
//smart ptr - new/delete

/*ȫ�ֵ�оģ�������*/
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

/*оģ����*/
struct tubeModel {
	double a;//x���᷽�� ����᳤(��x=0�����ǿ�ʼ��)
	double b;//y���᷽�� �����᳤(��y=0�����ǿ�ʼ��)
	double length;//z��ת�᷽�� ����
	double r;//���ǰ뾶
	double redundance;//�������೤��

	double angle;//���ƽǵĻ���(����ת��н�)
	double width;//��ά�����
	double round;//�ܳ�

	double curveLength;//��ǰ����ά�ĳ���
};

/*��˿��ͻ�������*/
struct payeye {
	double mandrel_speed;//оģת��
	double pm_distance;//��˿��-оģת�����
	double pm_left_distance;//��˿��-��Ȧƽ�����
};

/*��˿���λ��(��оģ��ת����)*/
struct track {
	double x;//��˿���Գ�ʼλ��(payeye.pm_distance)����ĳ���
	double z;//��˿������ת��z�����Գ�ʼλ��(payeye.pm_left_distance)λ�Ƴ���
	double swingAngle;//��˿����x����ת�Ļ���(��PI/2)
	double spindleAngle;//оģ��z��������ת�Ļ��ȣ���˳ʱ����(0~2*PI����ʼʱƽ��Ϊ0)
};

/*���Ƶ��λ��(����)*/
struct tubePathPosition {
	double x, y, z;//ƽ��ʱ��λ��
	double direction; //���Ƶ���Ƶķ���  1:z������ -1:z�Ḻ��
	int index;//���Ƶ���������ά�����
};

/*���Ƶ��λ��(���������Ϣ)*/
struct tubePathCoord {
	double x, y, z;//ƽ��ʱ��λ��
	double direction; //���Ƶ���Ƶķ���  1:z������ -1:z�Ḻ��
	double index;//���Ƶ���������ά�����
	double normal_radian;//ƽ��ʱ��˿�߷������Ļ��ȣ���x-yƽ��x������Ϊ0 ; x-������ϵ�еĦ�
	vec3 tangent;//ƽ��ʱ���������ķ���ʸ��
	double suspension;//���Ƶ��õ�ʱ���㵽��˿��ľ��볤��(��˿����)
};

struct cylinderModel {
	double middle_length;
	double middle_radius;
	double left_length;
	double left_radius;
	double right_length;
	double right_radius;
	double cut_angle;
	double slippage_coefficient;//�� 
	double left_semi_geodesic_slip_coef;//������߶����ʵ�æ�
	double right_semi_geodesic_slip_coef;//�Ұ����߶����ʵ�æ�
	double left_const_winding_angle_max_slip_coef;//�Ȳ��ƽǶ���ļ��ަ�
	double right_const_winding_angle_max_slip_coef; // �Ȳ��ƽǶ���ļ��ަ�
	double angle;//���ƽǵĻ���(����ת��н�)
	double width;//��ά�����
	double round;//�ܳ�
	double cur_length;//��ǰ����ά�ĳ���
	double now_y;
};

/*ѹ���������Ƶ��λ�� ���������Ϣ*/
struct cylinderPathCoord {
	cylinderPathCoord(){}
	cylinderPathCoord(double x,double y,double z,int direction =-1,double currentTheta = 0):x(x),y(y),z(z),direction(direction),currentTheta(currentTheta){}
	double x, y, z;
	int direction;//���Ƶķ��� 1:x���� -1:x����
	vec3 tangent;//��ǰ������߷���
	vec3 normal;//��ǰ��ķ��߷���
	double suspension;//��˿��
	double currentTheta;//x-��ƽ����y��Ϊ��ʼ��Ħ�ֵ
};

struct toroidModel {
	double R;
	double r;
	double angle;//��ʼ���ƽǶ�
	double band_width;//����
	double lambda;//��ϵ�� slippage coefficient
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
	int cut_num_offset;//��һ��ԭ�е��е����������ƫ�Ƽ����е㣿
	int cutNum;//�е��� ��˼�� ��һ��ɴ�����������غ󵽴�����λ�� Ӧ�ú���Ծ����� �� 5�ȷ� 1 3 5 2 4 1 .... �е���Ϊ3 ��Ծ��Ϊ2 ��һ���з�������ѭ�� ÿ�δӳ�ȥ��+cutNum�ٳ���
	int M;//�зֵ�Ƭ��
	double interval;//����ÿһƬ�зֵĳ���
	int jumpNum;//��Ծ�� ��������Ƭ����
	double windingPathStep;//��ά������
	int windingRedundentStep;
	double windingStepAngle;
	tubeModel tubeModel;
	payeye payeye;
	tubePathPosition currentPosition;

	int TubeCurrentStart[2];//0��ͷ������ά��� 1��β������ά���(��ǰ)

	/*
	<deque>��ά·������:
		<deque>��ά·��0:
			<struct tubePathCoord>·��0�ϵ�0
			<struct tubePathCoord>��1
			...
		</deque>
		<deque>��ά·��1:
			<struct tubePathCoord>·��1�ϵ�0
			<struct tubePathCoord>��1
			...
		</deque>
		...
	</deque>
	*/
	std::deque<std::deque<struct tubePathCoord>*>* TubePointList = NULL;//���ƽӴ������� 
	std::deque<std::deque<struct track>*>* TubeTrackList = NULL;//��˿��λ������ 

	CString cylinderWindingAlgorithm;
	cylinderModel cylinderModel;
	double jump_angle;
	double global_left_min_r;
	double global_left_cut_theta;
	double global_right_min_r;
	cylinderPathCoord global_cylinder_coord;
	std::shared_ptr<std::deque <struct cylinderPathCoord>> CylinderPointList = NULL;//ѹ���������ƽӴ�������
//std::deque<std::deque<struct cylinderPathCoord>*>* CylinderPointList = NULL;

	int m_isShowing;//1:tube(��ֱ��) 2:elbow(Բ���) 3:cylinder(����ѹ������)  4:cone(Բ׶) 5:toroid(Բ��)

	/*added by LMK*/
	/*union button*/
	void OnSwitchFiberPathControlDlg();//·�����->��������
	void OnSwitchComputeFiberPath();//·�����->������ά·��
	void OnSwitchComputePayeye();//·�����->�������·��
	void OnSwitchSaveTrack();//·�����->����·��

	/*tube ��������*/
	afx_msg void OnOpenFiberPathControlTubeParametersDlg();//OnSwitchFiberPathControlDlg��ת
	/*tube path*/
	afx_msg void OnComputeFiberPathTube();//OnSwitchComputeFiberPath��ת
	afx_msg int OnRenderLinePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg int OnRenderCurvePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg void OnRenderSinglePath(std::deque<struct tubePathCoord>* singlePathList);
	inline void updatePosition(double* nextPoint);
	inline void insertPoint(double* nextPoint);
	inline int outTubeEdge(double z);
	/*tube ��ά·��*/
	afx_msg void OnComputePayeyeTube();// OnSwitchComputePayeye��ת
	void pushTubePathCoord(const double* nextPoint, const double normal_radian, const int direction, std::deque<struct tubePathCoord>* singlePathList);
	void pushTubePathCoord(const double* nextPoint, const double normal_radian, const int direction, std::deque<struct tubePathCoord>* singlePathList, double angle);
	void justCalculateTubePathCoord(const double* nextPoint, const double normal_radian, const int direction);
	afx_msg int OnGenerateTubeEdgeList(std::deque<struct tubePathPosition>& TubeList, double interval, int side);
	afx_msg void OnGeneratePosition(std::deque<struct tubePathPosition>& TubeStartList, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg void computeTubeTrack(std::deque<struct tubePathCoord>::iterator currentTubePoint, track& tempTubeTrack);
	void vectorRotator(const vec3& currentVector, const double& angle, vec3& rotatedVector, double direction);
	//�������·�����ļ���
	afx_msg void OnSaveTubeTrackData();//OnSwitchSaveTrack��ת
	void dataCleaner(std::vector<data>& in);
	/*helper*/
	double _rayHitCurvePartAdj(int quad1, int quad2, vec2& src, vec2& dir, double a, double b, double r);
	double _rayHitSuspension(const double& a, const double& b, const double& r, const double& shell, const double& x, const double& y, const double& dir_x, const double& dir_y);
	double _rayHitAngle(const double& a, const double& b, const double& r, const double& shell, const double& x, const double& y, const double& dir_x, const double& dir_y);
	inline double _circleMinus(double a, double b);//����a b�Ļ��Ȳ�(a>b)

	//cylinder��������
	afx_msg void OnOpenFiberPathControlCylinderParametersDlg();//OnSwitchFiberPathControlDlg��ת
	//cylidner��ά·������
	afx_msg void OnComputeFiberPathCylinder();//OnSwitchComputeFiberPath��ת
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

	/*toroid��������*/
	afx_msg void OnOpenFiberPathControlToroidParametersDlg();//OnSwitchFiberPathControlDlg��ת
	/*toroid��ά·������*/
	afx_msg void OnComputeFiberPathToroid();//OnSwitchComputeFiberPath��ת
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

