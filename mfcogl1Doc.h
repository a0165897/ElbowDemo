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

/*оģ����*/
struct tubeModel {
	float a;//x���᷽�� ����᳤(��x=0�����ǿ�ʼ��)
	float b;//y���᷽�� �����᳤(��y=0�����ǿ�ʼ��)
	float length;//z��ת�᷽�� ����
	float r;//���ǰ뾶
	float redundance;//�������೤��

	float angle;//���ƽǵĻ���(����ת��н�)
	float width;//��ά�����
	float round;//�ܳ�

	float curveLength;//��ǰ����ά�ĳ���
};

/*��˿��ͻ�������*/
struct payeye {
	float mandrel_speed;//оģת��
	float pm_distance;//��˿��-оģת�����
	float pm_left_distance;//��˿��-��Ȧƽ�����
};

/*��˿���λ��(��оģ��ת����)*/
struct track {
	float x;//��˿���Գ�ʼλ��(payeye.pm_distance)����ĳ���
	float z;//��˿������ת��z�����Գ�ʼλ��(payeye.pm_left_distance)λ�Ƴ���
	float swingAngle;//��˿����x����ת�Ļ���(��PI/2)
	float spindleAngle;//оģ��z��������ת�Ļ��ȣ���˳ʱ����(0~2*PI����ʼʱƽ��Ϊ0)
};

/*���Ƶ��λ��(����)*/
struct tubePathPosition {
	float x, y, z;//ƽ��ʱ��λ��
	float direction; //���Ƶ���Ƶķ���  1:z������ -1:z�Ḻ��
	int index;//���Ƶ���������ά�����
};

/*���Ƶ��λ��(���������Ϣ)*/
struct tubePathCoord {
	float x, y, z;//ƽ��ʱ��λ��
	float direction; //���Ƶ���Ƶķ���  1:z������ -1:z�Ḻ��
	float index;//���Ƶ���������ά�����
	float normal_radian;//ƽ��ʱ��˿�߷������Ļ��ȣ���x-yƽ��x������Ϊ0 ; x-������ϵ�еĦ�
	vec3 tangent;//ƽ��ʱ���������ķ���ʸ��
	float suspension;//���Ƶ��õ�ʱ���㵽��˿��ľ��볤��(��˿����)
};

struct cylinderModel {
	float middle_length;
	float middle_radius;
	float left_length;
	float left_radius;
	float right_length;
	float right_radius;
	float cut_angle;
	float slippage_coefficient;//�� 
	float left_using_coef;//�����߶����ʵ�æ�
	float right_using_coef;
	float left_slippage_point;//�Ȳ��ƽǶ���ļ��ަ�
	float right_slippage_point;
	double angle;//���ƽǵĻ���(����ת��н�)
	double width;//��ά�����
	double round;//�ܳ�
	double curveLength;//��ǰ����ά�ĳ���
};

/*ѹ���������Ƶ��λ�� ���������Ϣ*/
struct cylinderPathCoord {
	cylinderPathCoord(){}
	cylinderPathCoord(float x,float y,float z,int direction =-1,float currentTheta = 0):x(x),y(y),z(z),direction(direction),currentTheta(currentTheta){}
	float x, y, z;
	int direction;//���Ƶķ��� 1:x���� -1:x����
	vec3 tangent;//��ǰ������߷���
	vec3 normal;//��ǰ��ķ��߷���
	float suspension;//��˿��
	float currentTheta;//x-��ƽ����y��Ϊ��ʼ��Ħ�ֵ
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
	int cut_num_offset;//��һ��ԭ�е��е����������ƫ�Ƽ����е㣿
	int cutNum;//�е��� ��˼�� ��һ��ɴ�����������غ󵽴�����λ�� Ӧ�ú���Ծ����� �� 5�ȷ� 1 3 5 2 4 1 .... �е���Ϊ3 ��Ծ��Ϊ2 ��һ���з�������ѭ�� ÿ�δӳ�ȥ��+cutNum�ٳ���
	int M0;//�зֵ�Ƭ��
	float interval;//����ÿһƬ�зֵĳ���
	int jumpNum;//��Ծ�� ��������Ƭ����
	float windingPathStep;//��ά������
	float currentAngle;//��ǰ�Ĳ��ƽ� δ��ʼ��������BUG
	tubeModel tubeModel;
	payeye payeye;
	tubePathPosition currentPosition;

	std::deque<struct tubePathPosition> TubeBothList;//�ۼ�ͷ�������
	std::deque<struct tubePathPosition> TubeStepList;//�ۼ�β�������
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
	std::deque<struct track>* TubeTrackListTime = NULL;//���ƻ���·������ ��ʱ
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
	std::shared_ptr<std::deque <struct cylinderPathCoord>> CylinderPointList = NULL;//ѹ���������ƽӴ�������
//std::deque<std::deque<struct cylinderPathCoord>*>* CylinderPointList = NULL;

	int m_isShowing;//1:tube(��ֱ��) 2:elbow(Բ���) 3:cylinder(����ѹ������)  4:cone(Բ׶)

	/*added by LMK*/
	//��Ӧ�����˵���ť
	void OnSwitchFiberPathControlDlg();//·�����->��������
	void OnSwitchComputeFiberPath();//·�����->������ά·��
	void OnSwitchComputePayeye();//·�����->�������·��
	void OnSwitchSaveTrack();//·�����->����·��

	//tube��������
	afx_msg void OnOpenFiberPathControlTubeParametersDlg();//switch��ת->���ܲ������Բ������е���������
	//tube��ά·������
	afx_msg void OnComputeFiberPathTube();//switch��ת->�����㺯��
	afx_msg void OnRenderSinglePath(std::deque<struct tubePathCoord>* singlePathList);
	afx_msg int OnRenderLinePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg int OnRenderCurvePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	inline void updatePosition(float* nextPoint);
	inline void insertPoint(float* nextPoint);
	inline int outTubeEdge(float z);
	//tube����·������
	afx_msg void OnComputePayeyeTube();//switch��ת->�����㺯��
	void pushTubePathCoord(const float* nextPoint, const float normal_radian, const int direction, std::deque<struct tubePathCoord>* singlePathList,float angle);
	afx_msg int OnGenerateTubeEdgeList(std::deque<struct tubePathPosition>& TubeList, float interval, int side);
	afx_msg void OnGeneratePosition(std::deque<struct tubePathPosition>& TubeStartList, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg void computeTubeTrack(std::deque<struct tubePathCoord>::iterator currentTubePoint, track& tempTubeTrack);
	void vectorRotateAroundZ(const vec3& currentVector, const float& angle, vec3& rotatedVector, float direction);
	float suspensionCompute(float a, float b, float r, float shell, float x, float y, float dir_x, float dir_y);
	//tube����·������
	afx_msg void OnSaveTrackTube();//switch��ת->�����㺯��
	void _fill_the_gap(track* preTrack, track* curTrack, std::vector<data>& in,int no);
	void calData(std::vector<data>& in);

	//cylinder��������
	afx_msg void OnOpenFiberPathControlCylinderParametersDlg();
	//cylidner��ά·������
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

