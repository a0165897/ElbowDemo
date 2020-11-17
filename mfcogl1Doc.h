// mfcogl1Doc.h : interface of the CMfcogl1Doc class

#pragma once

#include "HintDialog.h"
#include<typeinfo>
#include <deque>

/*ȫ�ֵ�оģ�������*/

struct double3 {
	float x, y, z;
};

/*оģ����*/
struct tubeModel {
	float a;//x���᷽�� ����᳤(��x=0�����ǿ�ʼ��)
	float b;//y���᷽�� �����᳤(��y=0�����ǿ�ʼ��)
	float length;//z��ת�᷽�� ����
	float r;//���ǰ뾶

	float angle;//���ƽǵĻ���(����ת��н�)
	float width;//��ά�����
	float curveLength;//��ǰ����ά�ĳ���
	float round;//�ܳ�
};

struct cylinderModel {
	float middle_length;
	float middle_radius;
	float left_length;
	float left_radius;
	float right_length;
	float right_radius;

	double angle;//���ƽǵĻ���(����ת��н�)
	double width;//��ά�����
	double round;//�ܳ�
	double curveLength;//��ǰ����ά�ĳ���
};

/*��˿��ͻ�������*/
struct payeye {
	float mandrel_speed;//оģת��
	float pm_distance;//��˿���оģ����Ե����
	float pm_left_distance;//��˿���оģ������
	float mandrel_redundancy;//оģ���Ҳ�����
};

/*��˿���λ��(��оģ��ת����)*/
struct track {
	float x;//��˿���Գ�ʼλ��(payeye.pm_distance)����ĳ���
	float z;//��˿������ת��z�����Գ�ʼλ��(payeye.pm_left_distance)λ�Ƴ���
	float swingAngle;//��˿����x����ת�Ļ���(��PI/2)
	float spindleAngle;//оģ��z��������ת�Ļ���(0~2*PI����ʼʱƽ��Ϊ0)
};

/*���Ƶ��λ��(����)*/
struct tubePathPosition {
	float x, y, z;//ƽ��ʱ��λ��
	float direction; //���Ƶ���Ƶķ���  1:z������ -1:z�Ḻ��
	float index;//���Ƶ���������ά�����
};

/*���Ƶ��λ��(���������Ϣ)*/
struct tubePathCoord {
	float x, y, z;//ƽ��ʱ��λ��
	float direction; //���Ƶ���Ƶķ���  1:z������ -1:z�Ḻ��
	float index;//���Ƶ���������ά�����
	float normal_radian;//ƽ��ʱ���������Ļ��ȣ���x-yƽ��x������Ϊ0 ; x-������ϵ�еĦ�
	double3 tangent;//ƽ��ʱ���������ķ���ʸ��
	float suspension;//���Ƶ��õ�ʱ���㵽��˿��ľ��볤��(��˿����)
};

/*ѹ���������Ƶ��λ�� ���������Ϣ*/
struct cylinderPathCoord {
	float x, y, z;
	int direction;//���Ƶķ��� 1:x���� -1:x����
	double3 tangent;
	double3 normal;
	float suspension;
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
public:
	/*added by LMK*/
	float jumpAngle;
	int M0;
	float interval;
	int jumpNum;//��Ծ�� �ӶԻ��������
	int cutNum;//�е��� ��˼�� ��һ��ɴ�����������غ󵽴�����λ�� Ӧ�ú���Ծ����� �� 5�ȷ� 1 3 5 2 4 1 .... �е���Ϊ3 ��Ծ��Ϊ2 ��һ���з�������ѭ�� ÿ�δӳ�ȥ��+cutNum�ٳ���
	tubeModel tubeModel;
	payeye payeye;
	tubePathPosition currentPosition;
	cylinderModel cylinderModel;
	float windingPathStep;
	float currentAngle,angleStep;
	std::deque<struct tubePathPosition> TubeStartList;//�ۼ�ȫ�����˵����
	int TubeCurrentStart[2];//Ϊ������һ����ά���ı�ţ��ֱ��¼���˵�ǰ��ά���ı��
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
	std::deque <struct cylinderPathCoord>* CylinderPointList = NULL;//ѹ���������ƽӴ�������
	//std::deque<std::deque<struct cylinderPathCoord>*>* CylinderPointList = NULL;
	std::deque<float>* distanceE = NULL;
	tubePathCoord tempTubePathCoord;
	int m_isShowing;//1:tube(��ֱ��) 2:elbow(Բ���) 3:cylinder(����ѹ������)  

public:
	unsigned long m_WindingCount[6];
	unsigned short m_numlayer;
	float m_FilamentWidth,cylinder_radius[6];

	bool m_bFiberPathComplete,m_bPayeyeComplete,m_bUseLayer;
// Implementation
public:
	void ResetWndDesign();
	bool IsEachPrime(int m1,int m2);

	virtual ~CMfcogl1Doc();

	/*added by LMK*/
	float tiny = 0.001;//�������������
	//��Ӧ�����˵���ť
	void OnSwitchFiberPathControlDlg();
	void OnSwitchComputeFiberPath();
	void OnSwitchComputePayeye();
	//cylidner��ά·������
	void CMfcogl1Doc::OnRenderRightEllipsoid(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint);
	void CMfcogl1Doc::OnRenderLeftEllipsoid(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint);
	void CMfcogl1Doc::OnRenderMiddleCylinder(std::deque<struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint);
	//tube��ά·������
	afx_msg void OnRenderSinglePath(std::deque<struct tubePathCoord>* singlePathList);
	afx_msg int OnRenderLinePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg int OnRenderCurvePart(int state, std::deque<struct tubePathCoord>* singlePathList);
	inline void updatePosition(float* nextPoint);
	inline void insertPoint(float* nextPoint);
	inline int outTubeEdge();
	//tube����·������
	void CMfcogl1Doc::pushTubePathCoord(const float* nextPoint, const float normal_radian, const int direction, std::deque<struct tubePathCoord>* singlePathList);
	afx_msg int OnGenerateTubeStartList( std::deque<struct tubePathPosition>& pque);
	afx_msg void OnGeneratePosition(std::deque<struct tubePathPosition>& pque);
	afx_msg void CMfcogl1Doc::computeTubeTrack(std::deque<struct tubePathCoord>::iterator currentTubePoint, track& tempTubeTrack);
	void CMfcogl1Doc::vectorRotateAroundZ(const double3& currentVector, const float& angle, double3& rotatedVector);
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
		else {
			strMsg.Format("missing template typeID:%s\n", typeid(x).name());
		}
		MessageBoxA(0, strMsg, "debug", 0);
	}
	//END LMK


protected:

// Generated message map functions
protected:
	//void WriteTestData(unsigned short latitude,unsigned short long_elbow,unsigned short long_cylinder);
	////{{AFX_MSG(CMfcogl1Doc)
	//afx_msg void OnFileSave();
	//afx_msg void OnFileOpen();

	/*added by LMK*/
	afx_msg void OnOpenFiberPathControlTubeParametersDlg();
	afx_msg void OnOpenFiberPathControlCylinderParametersDlg();
	afx_msg void OnComputeFiberPathTube();
	afx_msg void OnComputeFiberPathCylinder();
	afx_msg void OnComputePayeyeTube();
	void CMfcogl1Doc::CleanTubeMemory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ReadDataFromDisk(LPCTSTR lpFileName, CString lpFileExtName);
	void SaveDataToDisk(LPCTSTR lpszFilter);
};

