// mfcogl1Doc.cpp : implementation of the CMfcogl1Doc class
//

#include "stdafx.h"
#include <process.h>
#include "mfcogl1.h"
#include "mfcogl1Doc.h"
#include "DlgFiberPathControls.h"
#include "mfcogl1View.h"

/*added by LMK*/
#include "DlgFiberPathControlsTube.h"
#include "DlgFiberPathControlsCylinder.h"
#include "DlgFiberPathControlsToroid.h"
#include <deque>
#include <vector>
#include <set>
#include <string>
#include <typeinfo>
#include <memory>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define d_curve 0.02
#define LEFT_OUTLINE 1114
#define RIGHT_OUTLINE 1118
//#define TEST_GEODESIC

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1Doc

IMPLEMENT_DYNCREATE(CMfcogl1Doc, CDocument)

BEGIN_MESSAGE_MAP(CMfcogl1Doc, CDocument)
	//{{AFX_MSG_MAP(CMfcogl1Doc)
	ON_COMMAND(IDM_FIBER_PATH_CONTROL_PARAMETERS, OnSwitchFiberPathControlDlg)//参数设置
	ON_COMMAND(IDM_COMPUTE_FIBER_PATH, OnSwitchComputeFiberPath)//纤维路径计算
	ON_COMMAND(IDM_COMPUTE_PAYEYE_TRACK, OnSwitchComputePayeye)//机器路径计算
	ON_COMMAND(IDM_SAVE_TRACK, OnSwitchSaveTrack)//输出机器路径
	//ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1Doc construction/destruction

CMfcogl1Doc::CMfcogl1Doc()
{
	// TODO: add one-time construction code here

	m_WindingCount[0]=0;
	m_bCanComputing=m_bFiberPathComplete=false;
	m_bPayeyeComplete=false;

}

CMfcogl1Doc::~CMfcogl1Doc()
{

	CMfcogl1Doc::DestructTubeMemory();
	CMfcogl1Doc::DestructCylinderMemory();
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

//状态机归零
void CMfcogl1Doc::ResetWndDesign()
{
	m_bCanComputing=false;
	m_bFiberPathComplete=false;
	m_bPayeyeComplete=false;
}

void* CMfcogl1Doc::myGetView() {
	CMfcogl1View* pView;
	POSITION pos = GetFirstViewPosition();
	pView = (CMfcogl1View*)GetNextView(pos);
	return pView;
}

/*added by LMK*/
//参数设置
void CMfcogl1Doc::OnSwitchFiberPathControlDlg() {
	if (m_isShowing == 1) {//tube
		OnOpenFiberPathControlTubeParametersDlg();
	}
	else if(m_isShowing == 3) {//cylinder
		OnOpenFiberPathControlCylinderParametersDlg();
	}
	else if (m_isShowing == 4) {//cone
		debug_show("cone is doing... ");
	}
	else if (m_isShowing == 5) {//Toroid
		OnOpenFiberPathControlToroidParametersDlg();
	}
}

//纤维路径计算
void CMfcogl1Doc::OnSwitchComputeFiberPath() {
	if (m_isShowing == 1) {//tube
		OnComputeFiberPathTube();
	}
	else if (m_isShowing == 3) {//cylinder
		OnComputeFiberPathCylinder();
	}
	else if (m_isShowing == 4) {//cone
		debug_show("cone is doing...");
		//OnComputeFiberPathCone();
	}
	else if (m_isShowing == 5) {//Toroid
		OnComputeFiberPathToroid();
	}
}

//机器路径计算
void CMfcogl1Doc::OnSwitchComputePayeye() {
	if (m_isShowing == 1) {//tube
		OnComputePayeyeTube();
	}
	if (m_isShowing == 3) {
		debug_show("cylinder is doing...");
	}
	if (m_isShowing == 4) {
		debug_show("cone is doing...");
	}
}

//保存机器路径
void CMfcogl1Doc::OnSwitchSaveTrack() {
	if (m_isShowing == 1) {//tube
		OnSaveTubeTrackData();
	}
	if (m_isShowing == 3) {
		debug_show("cylinder is doing...");
	}
	if (m_isShowing == 4) {
		debug_show("cone is doing...");
	}
}

void CMfcogl1Doc::OnOpenFiberPathControlTubeParametersDlg() {
	DestructTubeMemory();

	CDlgFiberPathControlsTube fpTubeDlg;
	if (IDOK == fpTubeDlg.DoModal()) {

		ResetWndDesign();

		cutNum = fpTubeDlg.m_dlg_tube_cut_num;
		payeye.mandrel_speed = fpTubeDlg.m_dlg_tube_mandrel_speed;
		payeye.pm_distance = fpTubeDlg.m_dlg_tube_pm_distance;
		//payeye.pm_left_distance = fpTubeDlg.m_dlg_tube_pm_left_distance;
		payeye.pm_left_distance = 5.0;
		payeye.level = fpTubeDlg.m_dlg_tube_level;
		auto pView = (CMfcogl1View*)myGetView();
		tubeModel.a = pView->m_view_tube_a;
		tubeModel.b = pView->m_view_tube_b;
		tubeModel.length = pView->m_view_tube_length;
		tubeModel.r = pView->m_view_tube_r;
		tubeModel.redundance = pView->m_view_tube_redundence;
		tubeModel.angle = fpTubeDlg.m_dlg_tube_winding_angle * PI / 180;
		tubeModel.width = fpTubeDlg.m_dlg_tube_band_width;
		tubeModel.round = 2 * PI * tubeModel.r + 4 * tubeModel.a + 4 * tubeModel.b;//周长

		//计算合适的切点数(缠绕一周出发点的总量)，跳跃数(两次出发点序号的差值)
		double real_width = tubeModel.width / abs(cos(tubeModel.angle));//纤维带按照缠绕角摆放的横切面
		M = ceil(tubeModel.round / real_width);//实际剖分段数
		interval = tubeModel.round / M;//剖分步长
		while ((M + 1) % cutNum != 0) {//缩小纤维半径以满足切点数约束
			tubeModel.width -= 0.02;
			real_width = tubeModel.width / abs(cos(tubeModel.angle));
			M = ceil(tubeModel.round / real_width);
			interval = tubeModel.round / M;
		}
		jumpNum = (M + 1) / cutNum;

		m_bCanComputing = true;
		debug_show(fpTubeDlg.m_dlg_tube_round_winding);
		CString STemp;
		STemp.Format(_T("纱宽(修正) = %.2f mm\n缠绕角 = %.2f degree\n等分数=%d\n切点数=%d\n跳跃数=%d"),
			real_width, fpTubeDlg.m_dlg_tube_winding_angle, M, cutNum, jumpNum);
		AfxMessageBox(STemp);
	}
}

void CMfcogl1Doc::DestructTubeMemory() {
	m_WindingCount[0] = 0;
	if (TubePointList != NULL) {
		std::set<std::deque<struct tubePathCoord>*> tmp_ptr;
		for (auto i : *TubePointList) {
			tmp_ptr.insert(i);
		}
		for (auto i : tmp_ptr) {
			delete i;
			i = NULL;
		}
		delete TubePointList;
		TubePointList = NULL;
	}

	if (TubeTrackList != NULL) {
		for (auto i = TubeTrackList->begin(); i < TubeTrackList->end(); i++) {
			delete (*i);
			*i = NULL;
		}
		delete TubeTrackList;
		TubeTrackList = NULL;
	}
}

//生成起始点列表 在头上绕一圈，每隔一个切片采集一个点
int CMfcogl1Doc::OnGenerateTubeEdgeList(std::deque<struct tubePathPosition>& TubeList,double interval,int sides) {
	//sides: 1.回来 2.过去+回来
	//interval: 端面每一片切分的长度
	double halfInterval = interval / 2;
	int i = 0;
	double tiny = 0.001;
	struct tubePathPosition p;
	for (i = 0; i * interval < tubeModel.a; i++) {
		p.x = i * interval;
		p.y = tubeModel.b + tubeModel.r;
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < tubeModel.a + PI * tubeModel.r / 2; i++) {
		p.x = tubeModel.a + tubeModel.r * sin((i * interval - tubeModel.a) / tubeModel.r);
		p.y = tubeModel.b + tubeModel.r * cos((i * interval - tubeModel.a) / tubeModel.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < tubeModel.a + PI * tubeModel.r / 2 + 2 * tubeModel.b; i++) {
		p.x = tubeModel.a + tubeModel.r;
		p.y = tubeModel.b - (i * interval - tubeModel.a - PI * tubeModel.r / 2);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < tubeModel.a + PI * tubeModel.r + 2 * tubeModel.b; i++) {
		p.x = tubeModel.a + tubeModel.r * cos((i * interval - (tubeModel.a + PI * tubeModel.r / 2 + 2 * tubeModel.b)) / tubeModel.r);
		p.y = -tubeModel.b - tubeModel.r * sin((i * interval - (tubeModel.a + PI * tubeModel.r / 2 + 2 * tubeModel.b)) / tubeModel.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < 3 * tubeModel.a + PI * tubeModel.r + 2 * tubeModel.b; i++) {
		p.x = 2 * tubeModel.a - i * interval + PI * tubeModel.r + 2 * tubeModel.b;
		p.y = -tubeModel.b - tubeModel.r;
		p.z = 0;
		p.index = i;
		p.direction = 1; 
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < 3 * tubeModel.a + 1.5 * PI * tubeModel.r + 2 * tubeModel.b; i++) {
		p.x = -tubeModel.a - tubeModel.r * sin((i * interval - (3 * tubeModel.a + PI * tubeModel.r + 2 * tubeModel.b)) / tubeModel.r);
		p.y = -tubeModel.b - tubeModel.r * cos((i * interval - (3 * tubeModel.a + PI * tubeModel.r + 2 * tubeModel.b)) / tubeModel.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < 3 * tubeModel.a + 1.5 * PI * tubeModel.r + 4 * tubeModel.b; i++) {
		p.x = -tubeModel.a - tubeModel.r;
		p.y = -tubeModel.b + i * interval - 3 * tubeModel.a - 1.5 * PI * tubeModel.r - 2 * tubeModel.b;
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < 3 * tubeModel.a + 2 * PI * tubeModel.r + 4 * tubeModel.b; i++) {
		p.x = -tubeModel.a - tubeModel.r * cos((i * interval - (3 * tubeModel.a + 1.5 * PI * tubeModel.r + 4 * tubeModel.b)) / tubeModel.r);
		p.y = tubeModel.b + tubeModel.r * sin((i * interval - (3 * tubeModel.a + 1.5 * PI * tubeModel.r + 4 * tubeModel.b)) / tubeModel.r);
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}
	for (; i * interval < 4 * tubeModel.a + 2 * PI * tubeModel.r + 4 * tubeModel.b - tiny; i++) {
		p.x = i * interval - (4 * tubeModel.a + 2 * PI * tubeModel.r + 4 * tubeModel.b);
		p.y = tubeModel.b + tubeModel.r;
		p.z = 0;
		p.index = i;
		p.direction = 1;
		TubeList.push_back(p);
		if (sides == 2) {
			p.z = tubeModel.length;
			p.direction = -1;
			TubeList.push_back(p);
		}
	}

	return TubeList.size();
}

//方管纤维路径的计算
//1.先按照切分，查找两端所有的纤维起始点
//2.从所有起始点开始来回计算缠绕
//3.在每两条纤维中间插入从本条的终点到下条起点的路
void CMfcogl1Doc::OnComputeFiberPathTube() {

	//一条纤维路径的数据结构：一段从上一纤维末尾位置到本纤维起点的缠绕点+一段从纤维起点到实际缠绕旋转点的差值+真正的缠绕部分
	if (m_bCanComputing == true){
		TubePointList = new std::deque<std::deque<struct tubePathCoord>*>;//TubePoint的双向链表指针的双向链表，内层表示一条纤维路径
		auto TubeBothList = std::make_unique<std::deque<struct tubePathPosition>>();
		//参数！
		//中间段 非初始拟合部的结果
		windingPathStep = 0.05;//计算的步长
		//决定冗余段的精度，如果初始拟合部因为角度小直接来到冗余，则会出现差异缺陷。
		windingRedundentStep = 400;
		windingStepAngle = PI * 5 / 180;
		TubeCurrentStart[0] = TubeCurrentStart[1] = -1;

		//generate GL list
		glNewList(FIBER_PATH_LIST, GL_COMPILE); 

		GLfloat matAmb[4] = { 0.8F, 0.5F, 0.3F, 1.00F };
		GLfloat matDiff[4] = { 0.8F, 0.5F, 0.3F, 0.80F };
		GLfloat matSpec[4] = { 0.30F, 0.30F, 0.30F, 0.30F };
		GLfloat matShine = 60.00F;
		glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
		glMaterialf(GL_FRONT, GL_SHININESS, matShine);

		//生成所有路径的起点
		OnGenerateTubeEdgeList(*TubeBothList, interval, 2);

		double buffer_angle = PI * 90/ 180;
		//从一头绕到另一头经过的环方管切面长度
		//先不考虑冗余段的长度 已经对周长取余数
		double circle_length = tubeModel.length * tan(tubeModel.angle);
		while (circle_length > tubeModel.round) circle_length -= tubeModel.round;
		//在冗余段进出需要的缓冲距离最少要这么多
		double min_length = tubeModel.round * (buffer_angle / (2 * PI));
		double cut_min = M * (buffer_angle / (2 * PI));
		//每一个切片经过的长度
		double cut_interval_length = tubeModel.round / M;
		//环方管切面长度换算成切点数
		int cut_point_circle_length = ceil(circle_length / cut_interval_length);
		//同一端两次出发间隔距离（跳跃数*切片长）
		double jump_interval_length = jumpNum * cut_interval_length;
		//最终需要求得的值：在一端原有的切点基础上往右偏移几个切点？
		//两侧在冗余段经过的长度相等
		cut_num_offset = 0;
		int current_minus = M * 2;//当前两端在冗余段经过的长度差
		for (int tmp_num_offset=0; tmp_num_offset < M; tmp_num_offset++) {
			//右端冗余段长度
			// |(0+cut_num_offset)  -  (0+cut_point_circle_length) + k1*M| 出 - 进
			int right = tmp_num_offset - cut_point_circle_length;
			while (right < 0) right += M;
			//左端冗余段长度
			// |(0+jumpNum)  - (0+cut_num_offset + cut_point_circle_length) +k2*M|  出 - 进
			int left = jumpNum - tmp_num_offset - cut_point_circle_length;
			while (left < 0) left += M;
			if (left > cut_min && right > cut_min&& abs(left - right) < current_minus) {
				current_minus = abs(left - right);
				cut_num_offset = tmp_num_offset;
			}
		}

		currentPosition.direction = -1;
		int i = 3;
		//计算循环
		while (TubeBothList->size()!=0) {
			std::deque<struct tubePathCoord>* currentPathList = new std::deque<struct tubePathCoord>;
			TubePointList->push_back(currentPathList);
			OnGeneratePosition(*TubeBothList, currentPathList);
			OnRenderSinglePath(currentPathList);//计算本纤维路径的所有顶点位置并输入glList和TubePointList
		}
		glEndList();
		int size_per_level = TubePointList->size();
		for (int i = 1; i < payeye.level; i++) {
			for (int j = 0; j < size_per_level; j++) {
				TubePointList->push_back((*TubePointList)[j]);
			}
		}
		//统计总的坐标数
		m_WindingCount[0] = 0;
		for (auto i = TubePointList->begin(); i < TubePointList->end(); i++) { m_WindingCount[0] += (*i)->size(); }
		AfxMessageBox("纤维路径计算完毕\ntube Track Computation Finished.");
		m_bFiberPathComplete = true;
		auto pView = (CMfcogl1View*)myGetView();
		pView->m_cview_enable_tape_display = true;
		pView->Invalidate(false);

	}
	else{
		AfxMessageBox(_T("请先设置纤维参数\nPlease setup parameter first."));
	}
}

//根据当前位置和跳跃数算出本条路径的起点
void CMfcogl1Doc::OnGeneratePosition(std::deque<struct tubePathPosition>&TubeStartList, std::deque<struct tubePathCoord>* singlePathList) {

	auto tmpPosition = TubeStartList.begin();
	//direction==-1 ->刚才是从尾部回来->说明现在要从头部开始绕->得到上一次从头部出去的起始点->加一个跳跃数
	int currentStart = (currentPosition.direction == -1) ? TubeCurrentStart[0] : TubeCurrentStart[1];

	//如果已经初始化：接着上一条算跳跃数
	if (currentStart != -1) {
		auto i = TubeStartList.begin();
		for (; i < TubeStartList.end(); i++) {//遍历 给上一个起始点加一个跳跃数
			if (currentPosition.direction != (*i).direction && (*i).index == (currentStart + jumpNum) % M) {
				tmpPosition = i;
				break;
			}
		}
	}
	else {
		//还未初始化
		if (currentPosition.direction == 1) {//&& currentStart == 1
			tmpPosition = TubeStartList.begin() + 2 * cut_num_offset;
		}
	}

	currentPosition = *tmpPosition;//得到下一跳的起始点

	if (currentPosition.direction ==1) {//出去
		TubeCurrentStart[0] = (*tmpPosition).index;
	}
	else {//回来
		TubeCurrentStart[1] = (*tmpPosition).index;
	}
	TubeStartList.erase(tmpPosition);
}

//算出单根纤维路径
void CMfcogl1Doc::OnRenderSinglePath(std::deque<struct tubePathCoord>* singlePathList) {
	GLfloat matAmb[4] = { 1.0F, 0.0F, 0.0F, 1.00F };
	if (currentPosition.direction == 1) {
		matAmb[1] = 1.0F;
	}
	else {
		matAmb[2] = 1.0F;
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glBegin(GL_LINE_STRIP);
	//initial OpenGL
	//判断当前position处于芯模的哪个面，从而选中合适的入口。
	//类似于状态机，本质上是对两个OnRender不同状态的调用
	//缠绕到面的边缘时，会在OnRender中递归调用下一个面的OnRender，直到缠绕到芯模边缘退出。
	if (-tubeModel.a <= currentPosition.x && currentPosition.x < tubeModel.a && currentPosition.y>0) {
		OnRenderLinePart(1, singlePathList);
	}
	else if (tubeModel.a <= currentPosition.x && currentPosition.x < tubeModel.a + tubeModel.r && currentPosition.y > 0) {
		OnRenderCurvePart(2, singlePathList);
	}
	else if (-tubeModel.b < currentPosition.y && currentPosition.y <= tubeModel.b && currentPosition.x>0) {
		OnRenderLinePart(3, singlePathList);
	}
	else if (-tubeModel.b - tubeModel.r < currentPosition.y && currentPosition.y <= -tubeModel.b && currentPosition.x>0) {
		OnRenderCurvePart(4, singlePathList);
	}
	else if (-tubeModel.a < currentPosition.x && currentPosition.x <= tubeModel.a && currentPosition.y < 0) {
		OnRenderLinePart(5, singlePathList);
	}
	else if (-tubeModel.a - tubeModel.r < currentPosition.x && currentPosition.x <= -tubeModel.a && currentPosition.y < 0) {
		OnRenderCurvePart(6, singlePathList);
	}
	else if (-tubeModel.b <= currentPosition.y && currentPosition.y < tubeModel.b && currentPosition.x < 0) {
		OnRenderLinePart(7, singlePathList);
	}
	else if (-tubeModel.a - tubeModel.r <= currentPosition.x && currentPosition.x < -tubeModel.a && currentPosition.y >0) {
		OnRenderCurvePart(8, singlePathList);
	}
	glEnd();
}

inline void CMfcogl1Doc::updatePosition(double* nextPoint) {
	currentPosition.x = nextPoint[0];	currentPosition.y = nextPoint[1]; currentPosition.z = nextPoint[2];
}

inline int CMfcogl1Doc::outTubeEdge(double z) {
	return (z < 0) ? -1 : (z > (tubeModel.length) ? 1 : 0);
}

inline void CMfcogl1Doc::insertPoint(double* nextPoint) {
	glVertex3dv(nextPoint);
}

tubePathCoord _tempTubePathCoord;

void CMfcogl1Doc::pushTubePathCoord(const double* nextPoint,double normal_radian,const int direction, std::deque<struct tubePathCoord>* singlePathList,double angle){
	_tempTubePathCoord.x = nextPoint[0];
	_tempTubePathCoord.y = nextPoint[1];
	_tempTubePathCoord.z = nextPoint[2];
	if (normal_radian < -PI) normal_radian += 2 * PI;
	if (normal_radian >  PI) normal_radian -= 2 * PI;
	_tempTubePathCoord.normal_radian = normal_radian;

	_tempTubePathCoord.tangent.x = sin(angle) * cos(_tempTubePathCoord.normal_radian - PI / 2);
	_tempTubePathCoord.tangent.y = sin(angle) * sin(_tempTubePathCoord.normal_radian - PI / 2);
	_tempTubePathCoord.tangent.z = direction * cos(angle);

	_tempTubePathCoord.direction = currentPosition.direction;
	_tempTubePathCoord.index = currentPosition.index;
	singlePathList->push_back(_tempTubePathCoord);
}

void CMfcogl1Doc::pushTubePathCoord(const double* nextPoint, double normal_radian, const int direction, std::deque<struct tubePathCoord>* singlePathList) {
	_tempTubePathCoord.x = nextPoint[0];
	_tempTubePathCoord.y = nextPoint[1];
	_tempTubePathCoord.z = nextPoint[2];
	if (normal_radian < -PI) normal_radian += 2 * PI;
	if (normal_radian > PI) normal_radian -= 2 * PI;
	_tempTubePathCoord.normal_radian = normal_radian;

	_tempTubePathCoord.tangent.x = sin(tubeModel.angle) * cos(_tempTubePathCoord.normal_radian - PI / 2);
	_tempTubePathCoord.tangent.y = sin(tubeModel.angle) * sin(_tempTubePathCoord.normal_radian - PI / 2);
	_tempTubePathCoord.tangent.z = direction * cos(tubeModel.angle);

	_tempTubePathCoord.direction = currentPosition.direction;
	_tempTubePathCoord.index = currentPosition.index;
	singlePathList->push_back(_tempTubePathCoord);
}

void CMfcogl1Doc::justCalculateTubePathCoord(const double* nextPoint, double normal_radian, const int direction) {
	_tempTubePathCoord.x = nextPoint[0];
	_tempTubePathCoord.y = nextPoint[1];
	_tempTubePathCoord.z = nextPoint[2];
	if (normal_radian < -PI) normal_radian += 2 * PI;
	if (normal_radian > PI) normal_radian -= 2 * PI;
	_tempTubePathCoord.normal_radian = normal_radian;

	_tempTubePathCoord.tangent.x = sin(tubeModel.angle) * cos(_tempTubePathCoord.normal_radian - PI / 2);
	_tempTubePathCoord.tangent.y = sin(tubeModel.angle) * sin(_tempTubePathCoord.normal_radian - PI / 2);
	_tempTubePathCoord.tangent.z = direction * cos(tubeModel.angle);

	_tempTubePathCoord.direction = currentPosition.direction;
	_tempTubePathCoord.index = currentPosition.index;
}
int CMfcogl1Doc::OnRenderLinePart(int state, std::deque<struct tubePathCoord>* singlePathList) {
	double tiny = 0.01;
	int size = singlePathList->size();
	double nextPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	double startPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	tubeModel.curveLength = 0;
	int direction = currentPosition.direction;
	//参数！
	int delta_alpha = 4;//delta_alpha:决定前端的长度的值，和ratio有一定的比例关系1:2左右比较好
	int ratio=8;//ratio 小:头部平滑 ratio大:尾部平滑，结论:ratio应该随着缠绕角度变大 4~128
	switch (state) {
	case 1:
		//glNormal3f(cos((360-2*segmentw)*3.14/180),sin((360-2*segmentw)*3.14/180),0.0f);		
		if (size == 0) {
			double cur_normal = PI, jus = (PI / 2 - cur_normal) / ratio;
			double angle = PI / 2;
			double tan_hp_minus_a = tan(PI / 2 - tubeModel.angle);
			double halfpi_minus_angle = 0, jus2 = tan_hp_minus_a / delta_alpha;
			while (abs(cur_normal - PI / 2)>tiny) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, cur_normal, direction, singlePathList,angle);
				cur_normal += jus;
				if (abs(angle-tubeModel.angle) > tiny) {
					halfpi_minus_angle = atan(tan(halfpi_minus_angle)+jus2);
					angle = PI / 2 - halfpi_minus_angle;
				}
			}
		}
		else {//嵌入起点
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, PI / 2, direction, singlePathList);
		}
		while (nextPoint[0] < tubeModel.a && outTubeEdge(nextPoint[2]) == 0) {
			nextPoint[0] = startPoint[0] + tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[1] = startPoint[1];
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;
			tubeModel.curveLength += windingPathStep;
		}//嵌入共同的终点
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, PI / 2, direction, singlePathList);
		break;
	case 3:
		if (size == 0) {
			double cur_normal = PI / 2, jus = (0 - cur_normal) / ratio;
			double angle = PI / 2;
			double tan_hp_minus_a = tan(PI / 2 - tubeModel.angle);
			double halfpi_minus_angle = 0, jus2 = tan_hp_minus_a / delta_alpha;
			while (abs(cur_normal - 0)>tiny) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, cur_normal, direction, singlePathList,angle);
				cur_normal += jus;
				if (abs(angle - tubeModel.angle) > tiny) {
					halfpi_minus_angle = atan(tan(halfpi_minus_angle) + jus2);
					angle = PI / 2 - halfpi_minus_angle;
				}
			}
		}
		else {//嵌入起点
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, 0, direction, singlePathList);
		}
		while (-tubeModel.b < nextPoint[1] && outTubeEdge(nextPoint[2])==0) {
			//insertPoint(nextPoint);
			nextPoint[0] = startPoint[0];
			nextPoint[1] = startPoint[1] - tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;
			tubeModel.curveLength += windingPathStep;
		}
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, 0, direction, singlePathList);
		break;
	case 5:
		if (size == 0) {
			double cur_normal = 0, jus = (-PI / 2 - cur_normal) / ratio;
			double angle = PI / 2;
			double tan_hp_minus_a = tan(PI / 2 - tubeModel.angle);
			double halfpi_minus_angle = 0, jus2 = tan_hp_minus_a / delta_alpha;
			while (abs(cur_normal - -PI/2)>tiny) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, cur_normal, direction, singlePathList, angle);
				cur_normal += jus;
				if (abs(angle - tubeModel.angle) > tiny) {
					halfpi_minus_angle = atan(tan(halfpi_minus_angle) + jus2);
					angle = PI / 2 - halfpi_minus_angle;
				}
			}
		}
		else {//嵌入起点
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, -PI / 2, direction, singlePathList);
		}
		while (-tubeModel.a < nextPoint[0] && outTubeEdge(nextPoint[2]) == 0) {
			//insertPoint(nextPoint);
			nextPoint[0] = startPoint[0] - tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[1] = startPoint[1];
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;
			tubeModel.curveLength += windingPathStep;
		}
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, -PI / 2, direction, singlePathList);
		break;
	case 7:
		if (size == 0) {
			double cur_normal = -PI / 2, jus = (-PI - cur_normal) / ratio;
			double tan_hp_minus_a = tan(PI / 2 - tubeModel.angle);
			double angle = PI / 2;
			double halfpi_minus_angle = 0, jus2 = tan_hp_minus_a / delta_alpha;
			while (abs(cur_normal - -PI)>tiny) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, cur_normal, direction, singlePathList, angle);
				cur_normal += jus;
				if (abs(angle - tubeModel.angle) > tiny) {
					halfpi_minus_angle = atan(tan(halfpi_minus_angle) + jus2);
					angle = PI / 2 - halfpi_minus_angle;
				}
			}
		}
		else {//嵌入起点
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, -PI , direction, singlePathList);
		}
		while (nextPoint[1] < tubeModel.b && outTubeEdge(nextPoint[2]) == 0) {
		//	insertPoint(nextPoint);
			nextPoint[0] = startPoint[0];
			nextPoint[1] = startPoint[1] + tubeModel.curveLength * sin(tubeModel.angle);
			nextPoint[2] = startPoint[2] + tubeModel.curveLength * cos(tubeModel.angle) * direction;
			tubeModel.curveLength += windingPathStep;
		}
		insertPoint(nextPoint);
		pushTubePathCoord(nextPoint, -PI, direction, singlePathList);
		break;
	default:
		break;
	}

	updatePosition(nextPoint);
	if (outTubeEdge(currentPosition.z) == 0) {//from one part move into another neighbor part.   some how like a LSM.
		OnRenderCurvePart(state + 1,singlePathList);
	}
	else {
		return outTubeEdge(currentPosition.z);
	}
}

int CMfcogl1Doc::OnRenderCurvePart(int state, std::deque<struct tubePathCoord>* singlePathList) {
	int size = singlePathList->size();
	double startPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	double nextPoint[3] = { currentPosition.x,currentPosition.y,currentPosition.z };
	double phase = 0;
	tubeModel.curveLength = 0;
	int direction = currentPosition.direction;
	//TODO 计算当前位置射到外轮廓上的角度增加定值(比如5度)，所经过的弧度，而代替此处的弧长增量，以达到匀速转动计算的结果。
	//此时，为了便于计算，将stepLength定义为在该分面经过的弧度（而非曲线弧长）。为了保持曲线弧长增量不变，需要计算其在本分面上对应Δ弧度。
	double adjust = tubeModel.r / sin(tubeModel.angle);
	switch (state) {
	case 2:
		phase = atan((currentPosition.x - tubeModel.a) / (currentPosition.y - tubeModel.b));
		tubeModel.curveLength = phase;
		startPoint[0] = tubeModel.a;
		startPoint[1] = tubeModel.b;
		if (size == 0) {
			//0:横向（2a）1:高（2b）2:纵向（C）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, -tubeModel.curveLength +PI, direction, singlePathList,PI/2);
		}
		//圆柱面上测地线 对弧面的三条边约束
		//double diff_global_angle = 0,last_angle,last_sus;
		while (tubeModel.curveLength <= PI / 2 && outTubeEdge(nextPoint[2]) == 0) {

			//0:横向（2a）1:高（2b）2:纵向（C）angle:缠绕角 nextPoint:递推 m_view_r:倒角半径
			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;

			double shell_a = tubeModel.a;
			double shell_b = tubeModel.b;
			double shell_r = tubeModel.r + payeye.pm_left_distance;
			auto last = *singlePathList->rbegin();
			double pre_angle = _rayHitAngle(shell_a, shell_b, shell_r, payeye.pm_left_distance, last.x, last.y, last.tangent.x, last.tangent.y);
			justCalculateTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction);
			double cur_angle = _rayHitAngle(shell_a, shell_b, shell_r, payeye.pm_left_distance, _tempTubePathCoord.x, _tempTubePathCoord.y, _tempTubePathCoord.tangent.x, _tempTubePathCoord.tangent.y);
			double diff = _circleMinus(pre_angle, cur_angle);

			if (diff > windingStepAngle) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction, singlePathList);
			}

			tubeModel.curveLength += windingPathStep/adjust;
		}

		break;
	case 4:
		phase = PI / 2 + atan((-tubeModel.b - currentPosition.y) / (currentPosition.x - tubeModel.a));
		tubeModel.curveLength = phase;
		startPoint[0] = tubeModel.a;
		startPoint[1] = -tubeModel.b;
		if (size == 0) {
			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI, direction, singlePathList,PI/2);
		}
		while (tubeModel.curveLength <= PI && outTubeEdge(nextPoint[2]) == 0) {

			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;

			double shell_a = tubeModel.a;
			double shell_b = tubeModel.b;
			double shell_r = tubeModel.r + payeye.pm_left_distance;
			auto last = *singlePathList->rbegin();
			double pre_angle = _rayHitAngle(shell_a, shell_b, shell_r, payeye.pm_left_distance, last.x, last.y, last.tangent.x, last.tangent.y);
			justCalculateTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction);
			double cur_angle = _rayHitAngle(shell_a, shell_b, shell_r, payeye.pm_left_distance, _tempTubePathCoord.x, _tempTubePathCoord.y, _tempTubePathCoord.tangent.x, _tempTubePathCoord.tangent.y);
			double diff = _circleMinus(pre_angle, cur_angle);

			if (diff > windingStepAngle) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction, singlePathList);
			}

			tubeModel.curveLength += windingPathStep/adjust;
		}

		break;
	case 6:
		phase = PI + atan((-currentPosition.x - tubeModel.a) / (-currentPosition.y - tubeModel.b));
		tubeModel.curveLength = phase;
		startPoint[0] = -tubeModel.a;
		startPoint[1] = -tubeModel.b;
		if (size == 0) {
			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI, direction, singlePathList, PI / 2);
		}
		while (tubeModel.curveLength <= 1.5 * PI && outTubeEdge(nextPoint[2]) == 0) {

			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;
			 
			double shell_a = tubeModel.a;
			double shell_b = tubeModel.b;
			double shell_r = tubeModel.r + payeye.pm_left_distance;
			auto last = *singlePathList->rbegin();
			double pre_angle = _rayHitAngle(shell_a, shell_b, shell_r, payeye.pm_left_distance, last.x, last.y, last.tangent.x, last.tangent.y);
			justCalculateTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction);
			double cur_angle = _rayHitAngle(shell_a, shell_b, shell_r, payeye.pm_left_distance, _tempTubePathCoord.x, _tempTubePathCoord.y, _tempTubePathCoord.tangent.x, _tempTubePathCoord.tangent.y);
			double diff = _circleMinus(pre_angle, cur_angle);

			if (diff > windingStepAngle) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction, singlePathList);
			}

			tubeModel.curveLength += windingPathStep/adjust;
		}

		break;
	case 8:
		phase = 1.5 * PI + atan((currentPosition.y - tubeModel.b) / (-currentPosition.x - tubeModel.a));
		tubeModel.curveLength = phase;
		startPoint[0] = -tubeModel.a;
		startPoint[1] = tubeModel.b;
		if (size == 0) {
			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;
			insertPoint(nextPoint);
			pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI, direction, singlePathList, PI / 2);
		}
		while (tubeModel.curveLength <= 2 * PI && outTubeEdge(nextPoint[2]) == 0) {

			nextPoint[0] = startPoint[0] + tubeModel.r * sin(tubeModel.curveLength);
			nextPoint[1] = startPoint[1] + tubeModel.r * cos(tubeModel.curveLength);
			nextPoint[2] = startPoint[2] + tubeModel.r * (tubeModel.curveLength - phase) / tan(tubeModel.angle) * direction;

			double shell_a = tubeModel.a;
			double shell_b = tubeModel.b;
			double shell_r = tubeModel.r + payeye.pm_left_distance;
			auto last = *singlePathList->rbegin();
			double pre_angle = _rayHitAngle(shell_a, shell_b, shell_r, payeye.pm_left_distance, last.x, last.y, last.tangent.x, last.tangent.y);
			justCalculateTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction);
			double cur_angle = _rayHitAngle(shell_a, shell_b, shell_r, payeye.pm_left_distance, _tempTubePathCoord.x, _tempTubePathCoord.y, _tempTubePathCoord.tangent.x, _tempTubePathCoord.tangent.y);
			double diff = _circleMinus(pre_angle, cur_angle);

			if (diff > windingStepAngle) {
				insertPoint(nextPoint);
				pushTubePathCoord(nextPoint, -tubeModel.curveLength + PI / 2, direction, singlePathList);
			}

			tubeModel.curveLength += windingPathStep/adjust;
		}

		break;
	default:
		break;
	}

	updatePosition(nextPoint);
	if (outTubeEdge(currentPosition.z) == 0) {
		OnRenderLinePart(state % 8 + 1,singlePathList);
	}
	else {
		return outTubeEdge(currentPosition.z);
	}
}

//将机器路径中超出芯模的所有点压在最后一个点上，最后用dataCleaner清洗。
struct {
	int initialized;
	double x, y, z;
} _finalPosition;

//计算方管机器路径
void CMfcogl1Doc::OnComputePayeyeTube() {
	if (m_bFiberPathComplete) {
		glNewList(FIBER_TRACK_LIST, GL_COMPILE);

		GLfloat matAmb[4] = { 1.0F, 0.0F, 0.0F, 1.00F };
		GLfloat matDiff[4] = { 1.0F, 0.0F, 0.0F, 0.80F };
		GLfloat matSpec[4] = { 1.0F, 0.0F, 0.0F, 0.30F };
		GLfloat matShine = 60.00F;
		glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
		glMaterialf(GL_FRONT, GL_SHININESS, matShine);
		glLineWidth(1);
		glPointSize(1);

		TubeTrackList = new std::deque<std::deque<struct track>*>;//存储吐丝嘴位置的双向链表指针的双向链表，内层表示一条纤维路径上对应的吐丝嘴坐标
		for (auto line = TubePointList->begin(); line < TubePointList->end(); line++) {//每条纤维路径
			glBegin(GL_LINE_STRIP);
			std::deque<struct track>* currentTrackList = new std::deque<struct track>;
			TubeTrackList->push_back(currentTrackList);
			_finalPosition.initialized = 0;
			for (auto currentTubePointIterator = (*line)->begin(); currentTubePointIterator < (*line)->end(); currentTubePointIterator++) {//纤维路径上每个点
				track tempTubeTrack;
				computeTubeTrack(currentTubePointIterator, tempTubeTrack);
				currentTrackList->push_back(tempTubeTrack);
				double trackMandrelLength = payeye.pm_distance - tempTubeTrack.x;//吐丝嘴到芯模中心轴的垂直距离
				double trackPoint[3] = { trackMandrelLength * cos(tempTubeTrack.spindleAngle), trackMandrelLength * sin(tempTubeTrack.spindleAngle), tempTubeTrack.z };
				glVertex3dv(trackPoint);
			}
			glEnd();
			
		}
		//冗余段计算
		auto fiber = TubePointList->begin();
		auto payeye_track = TubeTrackList->begin();
		for (; fiber < TubePointList->end()-1,payeye_track<TubeTrackList->end()-1; fiber++,payeye_track++) {
			//冗余段的载体
			std::deque<struct tubePathCoord>* currentPointList = new std::deque<struct tubePathCoord>;
			std::deque<struct track>* currentTrackList = new std::deque<struct track>;
			tubePathCoord fiber_begin = *(*fiber)->rbegin();//本纤维段的末端
			tubePathCoord fiber_end = *(*(fiber + 1))->begin();//下纤维段的起点

			track payeye_begin = *(*payeye_track)->rbegin();//本机器路径的末端
			auto _iter_payeye_begin_pre = (*payeye_track)->rbegin()+1; 
			while (_iter_payeye_begin_pre->z == payeye_begin.z) { 
				_iter_payeye_begin_pre++; 
			}
			track payeye_begin_pre = *_iter_payeye_begin_pre;//本机器路径的末端减一
			track payeye_end = *(*(payeye_track + 1))->begin();//下机器路径的起点
			track payeye_end_next = *((*(payeye_track + 1))->begin()+1);//下机器路径的起点加一
			double d = fiber_begin.direction;

			double Vbegin = d * abs((payeye_begin.z - payeye_begin_pre.z) / _circleMinus(payeye_begin_pre.spindleAngle, payeye_begin.spindleAngle));
			double Vend = -d * abs((payeye_end_next.z - payeye_end.z) / _circleMinus(payeye_end.spindleAngle, payeye_end_next.spindleAngle));
			double Dangle = _circleMinus(payeye_begin.spindleAngle, payeye_end.spindleAngle);
			double L = tubeModel.redundance;

			//匀减速会拖出去很长
			//double K = (Vbegin * Vbegin) / (Vend * Vend);
			//double t1 = (Vbegin * Dangle) / (-K * Vend + Vbegin);
			//double t2 = Dangle - t1;
			//double a1 = -Vbegin / t1;
			//double a2 = Vend / t2;
			//double ratio1 = t1 / (t1 + t2);
			//double ratio2 = ratio1;

			//匀减速到头就停
			//double tstop = -1, a1, t1, a2, t2;
			//while (tstop < 0) {
			//	a1 = (0 - Vbegin * Vbegin) / (2 * L * d);
			//	t1 = (0 - Vbegin) / a1;
			//	a2 = (Vend * Vend - 0) / (2 * -L * d);
			//	t2 = (Vend - 0) / a2;
			//	tstop = Dangle - t1 - t2;
			//	L -= 1;
			//}
			//double ratio1 = t1 / Dangle;
			//double ratio2 = (Dangle - t2) / Dangle;

			//余弦减速那种
			double t1 = abs(L / (Vbegin * (1 - 2 / PI)));
			double t2 = abs(L / (Vend * (1 - 2 / PI)));
			double ratio1 = t1 / Dangle;
			double ratio2 = (Dangle - t2) / Dangle;

			while (ratio1 > ratio2) {
				L = L*0.9;
				t1 = abs(L / (Vbegin * (1 - 2 / PI)));
				t2 = abs(L / (Vend * (1 - 2 / PI)));
				ratio1 = t1 / Dangle;
				ratio2 = (Dangle - t2) / Dangle;
			}

			double Ddegree = 180 * (Dangle / PI);
			double degree2angle = PI / 180.0;
			double cur_V = Vbegin;
			track tempTubeTrack = payeye_begin;

			glBegin(GL_LINE_STRIP);
			double d_degree = 0.01;
			int out;
			double i;
			for (i = 0,out =0; i < Ddegree; i+=d_degree,out++) {

				tempTubeTrack.spindleAngle -= d_degree*degree2angle;
				if (tempTubeTrack.spindleAngle > 2 * PI)tempTubeTrack.spindleAngle -= 2 * PI;
				                                    
				tempTubeTrack.z += cur_V * d_degree * degree2angle;
				cur_V = 0;
				if (i < Ddegree * ratio1) {
					//cur_V += a1 * degree2angle;
					cur_V += Vbegin * (1 - sin(i * degree2angle * (PI / (2.0 * t1))));
				}
				if (i > Ddegree * ratio2) {
					//cur_V += a2 * degree2angle;
					cur_V += Vend * (1 - sin((Ddegree - i) * degree2angle * (PI / (2.0 * t2))));
				}
				if (i >= Ddegree * ratio1 && i <= Ddegree * ratio2) {
					cur_V = 0.0;
				}
				double dirx = cos(tempTubeTrack.spindleAngle);
				double diry = sin(tempTubeTrack.spindleAngle);
				tempTubeTrack.x = payeye.pm_distance - _rayHitSuspension(tubeModel.a, tubeModel.b, tubeModel.r+ payeye.pm_left_distance, payeye.pm_left_distance, 0, 0, dirx, diry);
				tempTubeTrack.swingAngle = payeye_begin.swingAngle + (payeye_end.swingAngle - payeye_begin.swingAngle) * (i / Ddegree);
				tubePathCoord tmpPathPoint;
				double length_of_intersection = _rayHitSuspension(tubeModel.a, tubeModel.b, tubeModel.r, 0, 0, 0, dirx, diry);
				tmpPathPoint.x = length_of_intersection * dirx;
				tmpPathPoint.y = length_of_intersection * diry;
				tmpPathPoint.z = tempTubeTrack.z;
				tmpPathPoint.normal_radian = tempTubeTrack.spindleAngle;
				if (out % windingRedundentStep == 0 || (i+d_degree)>Ddegree ) {
					currentPointList->push_back(tmpPathPoint);//data
					currentTrackList->push_back(tempTubeTrack);//data
					double trackMandrelLength = payeye.pm_distance - tempTubeTrack.x;//吐丝嘴到芯模中心轴的垂直距离
					double trackPoint[3] = { trackMandrelLength * cos(tempTubeTrack.spindleAngle), trackMandrelLength * sin(tempTubeTrack.spindleAngle), tempTubeTrack.z };//opengl
					glVertex3dv(trackPoint);
				}
			}
			glEnd();
			fiber = TubePointList->insert(fiber + 1, currentPointList);
			payeye_track = TubeTrackList->insert(payeye_track + 1, currentTrackList);
		}
		glEndList();

		//重新统计总的数
		m_bPayeyeComplete = true;
		m_WindingCount[0] = 0;
		for (auto i = TubePointList->begin(); i < TubePointList->end(); i++) { m_WindingCount[0] += (*i)->size(); }
		auto pView = (CMfcogl1View*)myGetView();
		pView->Invalidate(false);
		pView->m_cview_enable_track_display = true;
	}
	else {
		AfxMessageBox(_T("请先计算纤维路径\nPlease compute path first."));
	}
}

double CMfcogl1Doc::_circleMinus(double a, double b) {
	double ret = a - b;
	while (ret < 0) {
		ret += 2 * PI;
	}
	return ret;
}

//计算方管芯模射线到机器路径包络面圆弧段的长度的辅助函数
double CMfcogl1Doc::_rayHitCurvePartAdj(int quad1, int quad2,vec2& src,vec2& dir,double a,double b,double r) {
	//( dir * s  + some).sqr_len() = r * r;
	//(some.x + s * dir.x) ^ 2 + (some.y + s * dir.y) ^ 2 = r * r
	//dir.sqr_len() * s ^ 2 + 2(some.x * dir.x + some.y * dir.y) * s + some.sqr_len() - r * r = 0;
	vec2 some = src - vec2(quad1 * a, quad2 * b);
	double _a = dir.sqr_len(), _b = 2 * (some.x * dir.x + some.y * dir.y), _c = some.sqr_len() - r * r;
	double s = (-_b + sqrt(_b * _b - 4 * _a * _c)) / (2 * _a);
	return s;
};

//方管芯模射线到机器路径包络面的长度
double CMfcogl1Doc::_rayHitSuspension(const double& a, const double& b, const double& r, const double& shell, const double& x, const double& y, const double& dir_x, const double& dir_y) {
	vec2 src = { x,y };
	vec2 dir = { dir_x,dir_y };
	//model: a,b,r
	double s = 99999;//suspension
	// 矩形内的射线和矩形的交点 负数 负数 正数大 正数小 显然小的正数是解
	double s1 = (a + r - src.x) / dir.x;
	if (s1 > 0 && s1 < s)s = s1;
	double s2 = (-a - r - src.x) / dir.x;
	if (s2 > 0 && s2 < s)s = s2;
	double s3 = (b + r - src.y) / dir.y;
	if (s3 > 0 && s3 < s)s = s3;
	double s4 = (-b - r - src.y) / dir.y;
	if (s4 > 0 && s4 < s)s = s4;
	vec2 tmp = src + dir * s;

	if (tmp.x > a && tmp.y > b) s = _rayHitCurvePartAdj(1, 1, src, dir, a, b, r);
	if (tmp.x < -a && tmp.y > b) s=_rayHitCurvePartAdj(-1,1, src, dir, a, b, r);
	if (tmp.x < -a  && tmp.y < -b) s=_rayHitCurvePartAdj(-1,-1, src, dir, a, b, r);
	if (tmp.x > a && tmp.y < -b ) s=_rayHitCurvePartAdj(1,-1, src, dir, a, b, r);
	return s;
}
//方管芯模射线和机器路径包络面交点关于原点的弧度 [-PI,+PI]
double CMfcogl1Doc::_rayHitAngle(const double& a, const double& b, const double& r, const double& shell, const double& x, const double& y, const double& dir_x, const double& dir_y) {
	vec2 src = { x,y };
	vec2 dir = { dir_x,dir_y };
	double s = _rayHitSuspension(a, b, r, shell, x, y, dir_x, dir_y);
	vec2 des = src + dir * s;
	return atan2(des.y, des.x);
}

//计算出currentTubePoint的suspension(悬丝长度)和tempTubeTrack的四自由度参数
void CMfcogl1Doc::computeTubeTrack(std::deque<struct tubePathCoord>::iterator currentTubePointIterator, track& tempTubeTrack) {
	struct tubePathCoord& currentTubePoint = *currentTubePointIterator;
	double shell_a = tubeModel.a;
	double shell_b = tubeModel.b;
	double shell_r = tubeModel.r + payeye.pm_left_distance;
	double R = payeye.pm_distance;
	//(currentTubePoint.x + currentTubePoint.tangent.x * suspension) ^ 2 + (currentTubePoint.y + currentTubePoint.tangent.y * suspension) ^ 2 = R * R;    syms:suspension
	double a = currentTubePoint.tangent.x, b = currentTubePoint.x, c = currentTubePoint.tangent.y, d = currentTubePoint.y;
	currentTubePoint.suspension = _rayHitSuspension(shell_a, shell_b, shell_r, payeye.pm_left_distance, b, d, a, c);
	//double A = a * a + c * c;
	//double B = 2 * a * b + 2 * c * d;
	//double C = b * b + d * d - R * R;
	//double root1 = (-B + sqrt(B * B - 4 * A * C)) / (2 * A);
	//double root2 = (-B - sqrt(B * B - 4 * A * C)) / (2 * A);
	//currentTubePoint.suspension = max(root1, root2);
	//纤维束起始段对应机器路径
	//if (currentTubePointIterator._Myoff == 0) { 
	/*	currentTubePoint.suspension = 0;
	}*/
	
	double track_x = currentTubePoint.x + currentTubePoint.tangent.x * currentTubePoint.suspension;
	double track_y = currentTubePoint.y + currentTubePoint.tangent.y * currentTubePoint.suspension;
	double track_z = currentTubePoint.z + currentTubePoint.tangent.z * currentTubePoint.suspension;

	//纤维束终点段对应机器路径。吐丝嘴位置不能超过两端，到达端点时要把纤维束“压”到芯模上(这部分是冗余段的事情)
	if (track_z <-0.01 || track_z >(tubeModel.length + 0.01)) {
		if (_finalPosition.initialized == 0) {
			struct tubePathCoord& pre = *(currentTubePointIterator -1);
			double a = pre.tangent.x, b = pre.x, c = pre.tangent.y, d = pre.y;
			pre.suspension = _rayHitSuspension(shell_a, shell_b, shell_r, payeye.pm_left_distance, b, d, a, c);
			double pre_z = pre.z + pre.tangent.z * pre.suspension;
			double pre_x = pre.x + pre.tangent.x * pre.suspension;
			double pre_y = pre.y + pre.tangent.y * pre.suspension;

			vec3 v = { track_x - pre_x,track_y - pre_y,track_z - pre_z };
			double k = track_z < -0.01 ? (-pre_z / v.z) : ((tubeModel.length - pre_z) / v.z);

			track_z = pre_z + k * v.z;
			track_x = pre_x + k * v.x;
			track_y = pre_y + k * v.y;
			_finalPosition.x = pre_x + k * v.x;
			_finalPosition.y = pre_y + k * v.y;
			_finalPosition.z = pre_z + k * v.z;
			_finalPosition.initialized = 1;
		}
		else {
			track_x = _finalPosition.x;
			track_y = _finalPosition.y;
			track_z = _finalPosition.z;
		}
	}
	tempTubeTrack.x = payeye.pm_distance - sqrt(track_x * track_x + track_y * track_y);
	tempTubeTrack.z = track_z;
	tempTubeTrack.spindleAngle = atan2(track_y, track_x) >= 0 ? atan2(track_y, track_x) : atan2(track_y, track_x) + 2 * PI;
	vec3 rotatedTangent;//平放时的切线旋转后，计算其对应的吐丝嘴角度
	CMfcogl1Doc::vectorRotator(currentTubePoint.tangent, tempTubeTrack.spindleAngle, rotatedTangent,currentTubePoint.direction);
	tempTubeTrack.swingAngle = atan(rotatedTangent.z / rotatedTangent.y);
}

void CMfcogl1Doc::vectorRotator(const vec3& currentVector,const double& angle,vec3& rotatedVector,double direction){
	//x-y平面旋转矩阵
	if (abs(currentVector.z - direction * cos(tubeModel.angle)) < 0.001) {
		rotatedVector.x = cos(angle) * currentVector.x + sin(angle) * currentVector.y;
		rotatedVector.y = -sin(angle) * currentVector.x + cos(angle) * currentVector.y;
		rotatedVector.z = currentVector.z;
		if (rotatedVector.y > 0)rotatedVector.y = -0.0001;
	}
	else {
		rotatedVector.z = direction * 100;
		rotatedVector.y = -1;
	}
}

void CMfcogl1Doc::dataCleaner(std::vector<data>& in){
	track* preTrack = nullptr;
	for (auto line = TubeTrackList->begin(); line < TubeTrackList->end(); line++) {
		int no = (int)floor(double(line - TubeTrackList->begin()) / 4.0f) + 1;
		double just_spindleAngle;
		//draw this line
		for (auto track = (*line)->begin(); track < (*line)->end(); track++) {
			if (preTrack == nullptr) {
				preTrack = &(*track);
			}
			else {
				if (abs(preTrack->z - track->z) < 0.01 && abs(_circleMinus(preTrack->spindleAngle , track->spindleAngle)) < 0.01 && abs(preTrack->x - track->x) < 0.01) {
					continue;
				}
				preTrack = &(*track);
			}
			if (track->spindleAngle < 0)track->spindleAngle += 2 * PI;
			double currentNormal[3] = { 0,cos(track->swingAngle),sin(track->swingAngle) };
			double trackMandrelLength =payeye.pm_distance - track->x;//吐丝嘴到芯模中心轴的垂直距离
			double trackPoint[3] = { trackMandrelLength * cos(track->spindleAngle), trackMandrelLength * sin(track->spindleAngle), track->z };
			data tmp = { track->spindleAngle * 180.0 / PI, track->z, track->x, track->swingAngle * 180.0 / PI, payeye.mandrel_speed, no };
			in.push_back(tmp);
		}
	}
}

void CMfcogl1Doc::OnSaveTubeTrackData() {
	if (m_bPayeyeComplete == true) {
		//firstly, out something.
		char szFilters[] =
			"csv File(*.csv)\0*.csv\0"\
			"Text File(*.txt)\0*.txt\0"\
			"All Typle(*.*)\0*.*\0" \
			"\0";
		//当过滤器或者默认构造参数赋值较少情况下，
		//使用构造函数修改对话框初始状态可能更好，这过滤器较多
		CFileDialog FileDlg(FALSE, "txt", _T("Test"));
		FileDlg.m_ofn.lpstrTitle = "Save File";
		FileDlg.m_ofn.lpstrFilter = szFilters;

		if (IDOK == FileDlg.DoModal())
		{
			std::vector<data> in;
			dataCleaner(in);
			CFile File(FileDlg.GetPathName(), CFile::modeCreate | CFile::modeReadWrite);
			char BufData[100];
			sprintf(BufData, "芯模端头冗余,吐丝嘴-旋转轴距离,吐丝嘴-钉圈距离,数据行数,纱条数\n%.1f, %.1f, %.1f, %d,%d\n", tubeModel.redundance, payeye.pm_distance, payeye.pm_left_distance, in.size(),M);
			File.Write(BufData, strlen(BufData));
			sprintf(BufData, "芯模宽度,芯模高度,芯模长度,缠绕角,纤维带宽\n%.1f, %.1f, %.1f, %.1f, %.1f\n",2.0f*(tubeModel.a+ tubeModel.r),2.0f*(tubeModel.b+ tubeModel.r),tubeModel.length,tubeModel.angle*180.0f/PI,tubeModel.width);
			File.Write(BufData, strlen(BufData));
			sprintf(BufData,"芯模旋转,吐丝嘴位移,吐丝嘴探出,吐丝嘴旋转,电机速度,纤维编号\n");
			File.Write(BufData, strlen(BufData));
			auto i = in.begin();
			sprintf(BufData, "%.1f, %.1f, %.1f, %.1f, %.1f, %d\n", i->a, i->b, i->c, i->d, i->e,i->no);
			File.Write(BufData, strlen(BufData));
			for (auto i = in.begin()+1; i != in.end(); i++){
				auto p = i - 1;
				double tmp_angle = p->a - i->a;
				if (tmp_angle < -180) {
					tmp_angle += 360.0;
				}
				//diff
				//sprintf(BufData, "%.5f, %.5f, %.5f, %.5f, %.5f, %d\n", -tmp_angle, i->b - p->b, i->c - p->c, i->d - p->d, i->e - p->e, i->no);
				//all
				sprintf(BufData, "%.5f, %.5f, %.5f, %.5f, %.5f, %d\n", i->a, i->b , i->c , i->d , i->e ,i->no);
				File.Write(BufData, strlen(BufData));
			}
			File.Flush();
			File.Close();
		}
		AfxMessageBox("Saved.");
	}
	else {
		AfxMessageBox("Please Compute Track Path Firstly!");
	}
}

void CMfcogl1Doc::DestructCylinderMemory() {
	//清理 glList
}
void CMfcogl1Doc::single_angle_multi_line(std::vector<double>& point, struct cylinderModel& _min_model) {
	std::vector<double> miu;
	std::vector<double> phi;
	std::vector<double> y;
	double _min_theta_diff = 10000;//所有结尾点和M0一圈的回归点最近的点
	for (cylinderModel.left_const_winding_angle_max_slip_coef =0.0; cylinderModel.left_const_winding_angle_max_slip_coef < cylinderModel.slippage_coefficient; cylinderModel.left_const_winding_angle_max_slip_coef += 0.08) {
		//get left_semi_geodesic_slip_coef
		double tmp_left_semi_geodesic_slip_coef = 0;
		double tmp_min_r = 1000;
		double tmp_phi;
		for (cylinderModel.left_semi_geodesic_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.left_semi_geodesic_slip_coef > -cylinderModel.slippage_coefficient; cylinderModel.left_semi_geodesic_slip_coef -= 0.001) {
			cylinderPathCoord currentPoint(cylinderModel.left_length, cylinderModel.middle_radius, 0.0f);
			(this->*leftCylinder)(currentPoint);
			(this->*middleCylinder)(currentPoint);
			auto tmpPoint = currentPoint;
			if (abs(global_left_min_r - cylinderModel.left_radius) < tmp_min_r) {//该 等缠绕角摩擦极限 & 半测地线摩擦系数 组合存在左侧相切绕回点
				tmp_min_r = abs(global_left_min_r - cylinderModel.left_radius);
				tmp_left_semi_geodesic_slip_coef = cylinderModel.left_semi_geodesic_slip_coef;
				tmp_phi = cylinderModel.cut_angle;
			}
		}
		if(tmp_min_r<0.01){ 
			cylinderModel.left_semi_geodesic_slip_coef = tmp_left_semi_geodesic_slip_coef;
			OnComputeFiberPathCylinder(); 
			miu.push_back(cylinderModel.left_const_winding_angle_max_slip_coef);
			phi.push_back(tmp_phi);
			y.push_back(cylinderModel.now_y);
		}
	}
	miu;
	phi;
	y;
}
double CMfcogl1Doc::loop_each_angle(std::vector<double>& point, struct cylinderModel &_min_model) {
	double _min_theta_diff = 10000;//所有结尾点和M0一圈的回归点最近的点
	for (cylinderModel.left_const_winding_angle_max_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.left_const_winding_angle_max_slip_coef > 0; cylinderModel.left_const_winding_angle_max_slip_coef -= 0.05) {
		for (cylinderModel.left_semi_geodesic_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.left_semi_geodesic_slip_coef > -cylinderModel.slippage_coefficient; cylinderModel.left_semi_geodesic_slip_coef -= 0.05) {
			cylinderPathCoord currentPoint(cylinderModel.left_length, cylinderModel.middle_radius, 0.0f);
			(this->*leftCylinder)(currentPoint);
			(this->*middleCylinder)(currentPoint);
			auto tmpPoint = currentPoint;
			if (abs(global_left_min_r - cylinderModel.left_radius) < 0.02) {//该 等缠绕角摩擦极限 & 半测地线摩擦系数 组合存在左侧相切绕回点
				for (cylinderModel.right_const_winding_angle_max_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.right_const_winding_angle_max_slip_coef > 0; cylinderModel.right_const_winding_angle_max_slip_coef -= 0.05) {
					for (cylinderModel.right_semi_geodesic_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.right_semi_geodesic_slip_coef > -cylinderModel.slippage_coefficient; cylinderModel.right_semi_geodesic_slip_coef -= 0.05) {
						currentPoint = tmpPoint;
						(this->*rightCylinder)(currentPoint);
						(this->*middleCylinder)(currentPoint);
						if (abs(global_right_min_r - cylinderModel.right_radius) < 0.02) {//该 等缠绕角摩擦极限 & 半测地线摩擦系数 组合存在右侧相切绕回点				
							double _theta = [&] {double i = 180.0 * currentPoint.currentTheta / PI; while (i > 360.0) { i -= 360.0; }return i; }();
							double _theta_diff = 10000;
							double _closest_jump_angle = 0;
							//得到最靠近本绕回点的互素点(_closest_jump_angle)
							for (auto i : point) {
								double _tmp_diff = abs(i - _theta);
								if (_tmp_diff < _theta_diff) { _theta_diff = _tmp_diff;  _closest_jump_angle = i; }
							}
							if (_theta_diff < _min_theta_diff) {
								_min_theta_diff = _theta_diff;
								_min_model = cylinderModel;
								jump_angle = _closest_jump_angle * PI / 180.0;
							}
							break;
						}
					}
				}
			}
		}
	}
	return _min_theta_diff;
}

//接受压力容器的纤维路径参数
void CMfcogl1Doc::OnOpenFiberPathControlCylinderParametersDlg() {
	CMfcogl1Doc::DestructTubeMemory();
	CMfcogl1Doc::DestructCylinderMemory();
	CDlgFiberPathControlsCylinder fpCylinderDlg;
	if (IDOK == fpCylinderDlg.DoModal()) {
		ResetWndDesign();
		cylinderModel.angle = fpCylinderDlg.m_dlg_cylinder_winding_angle * PI / 180.0;
		cylinderModel.width = fpCylinderDlg.m_dlg_cylinder_band_width;
		cylinderModel.slippage_coefficient = fpCylinderDlg.m_dlg_cylinder_slippage_coefficient;//μ
		cutNum = fpCylinderDlg.m_dlg_cylinder_cut_num;
		cylinderWindingAlgorithm = fpCylinderDlg.m_dlg_cylinder_winding_algorithm;
		m_bCanComputing = true;

		auto pView = (CMfcogl1View*)myGetView();

		cylinderModel.left_length = pView->m_view_cylinder_left_length;
		cylinderModel.left_radius = pView->m_view_cylinder_left_radius;
		cylinderModel.middle_length = pView->m_view_cylinder_middle_length;
		cylinderModel.middle_radius = pView->m_view_cylinder_middle_radius;
		cylinderModel.right_length = pView->m_view_cylinder_right_length;
		cylinderModel.right_radius = pView->m_view_cylinder_right_radius;
		cylinderModel.round = 2.0F * PI * cylinderModel.middle_radius;
		cylinderModel.left_semi_geodesic_slip_coef = cylinderModel.slippage_coefficient;
		cylinderModel.left_const_winding_angle_max_slip_coef = cylinderModel.slippage_coefficient;
		cylinderModel.right_semi_geodesic_slip_coef = cylinderModel.slippage_coefficient;
		cylinderModel.right_const_winding_angle_max_slip_coef = cylinderModel.slippage_coefficient;

		middleCylinder = &CMfcogl1Doc::OnRenderMiddleCylinder;
		if (cylinderWindingAlgorithm == "NON_GEODESIC") {
			leftCylinder = &CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic;
			rightCylinder = &CMfcogl1Doc::OnRenderRightEllipsoidNonGeodesic;
		}

		//计算合适的切点数(缠绕一周出发点的总量)，跳跃数(两次出发点序号的差值
		double real_width = 0, total_theta = 0;
		cylinderModel.width = fpCylinderDlg.m_dlg_cylinder_band_width;
		real_width = 0.1 * cylinderModel.width / abs(cos(cylinderModel.angle));//mm->cm
		M = ceil(cylinderModel.round / real_width);//实际剖分段数
		interval = cylinderModel.round / (double)M;//剖分步长
		double theta_interval = 2 * PI / M;
		jumpNum = 1;
		jump_angle = (double)(jumpNum) * 2.0 * PI / (double)M;
		std::deque<double> quoters;
		std::vector<double> point;//和M0互素的所有数，对应的角度
		auto gcd = [](int a,int b) { 
			int t;
			if (a < b){
				t = a; a = b; b = t;
			}
			while (a % b){
				t = b;
				b = a % b;
				a = t;
			}
			return b; 
		};

		for (int i = 1; i < M; i++) {
			if (gcd(i, M) == 1) {
				point.push_back(360.0 * ((double)i / (double)M));
			}
		}

		//实际切面宽
		struct cylinderModel _origin_model = cylinderModel;
		struct cylinderModel _min_model = cylinderModel;

		/*测试同一缠绕角的等缠绕角-半测地线的落点*/
		//auto loop_each_angle = [&]() ->double{
		//	double _min_theta_diff = 10000;//所有结尾点和M0一圈的回归点最近的点
		//	for (cylinderModel.left_const_winding_angle_max_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.left_const_winding_angle_max_slip_coef > 0; cylinderModel.left_const_winding_angle_max_slip_coef -= 0.05) {
		//		for (cylinderModel.left_semi_geodesic_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.left_semi_geodesic_slip_coef > -cylinderModel.slippage_coefficient; cylinderModel.left_semi_geodesic_slip_coef -= 0.05) {
		//			cylinderPathCoord currentPoint(cylinderModel.left_length, cylinderModel.middle_radius, 0.0f);
		//			(this->*leftCylinder)(currentPoint);
		//			(this->*middleCylinder)(currentPoint);
		//			auto tmpPoint = currentPoint;
		//			if (abs(global_left_min_r - cylinderModel.left_radius) < 0.05) {//该 等缠绕角摩擦极限 & 半测地线摩擦系数 组合存在左侧相切绕回点
		//				for (cylinderModel.right_const_winding_angle_max_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.right_const_winding_angle_max_slip_coef > 0; cylinderModel.right_const_winding_angle_max_slip_coef -= 0.05) {
		//					for (cylinderModel.right_semi_geodesic_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.right_semi_geodesic_slip_coef > -cylinderModel.slippage_coefficient; cylinderModel.right_semi_geodesic_slip_coef -= 0.05) {
		//						currentPoint = tmpPoint;
		//						(this->*rightCylinder)(currentPoint);
		//						(this->*middleCylinder)(currentPoint);
		//						if (abs(global_right_min_r - cylinderModel.right_radius) < 0.05) {//该 等缠绕角摩擦极限 & 半测地线摩擦系数 组合存在右侧相切绕回点				
		//							double _theta = [&] {double i = 180.0 * currentPoint.currentTheta / PI; while (i > 360.0) { i -= 360.0; }return i; }();
		//							double _theta_diff = 10000;
		//							double _closest_jump_angle = 0;
		//							//得到最靠近本绕回点的互素点(_closest_jump_angle)
		//							for (auto i : point) {
		//								double _tmp_diff = abs(i - _theta);
		//								if (_tmp_diff < _theta_diff) { _theta_diff = _tmp_diff;  _closest_jump_angle = i; }
		//							}
		//							if (_theta_diff < _min_theta_diff) {
		//								_min_theta_diff = _theta_diff;
		//								_min_model = cylinderModel;
		//								jump_angle = _closest_jump_angle * PI / 180.0;
		//							}
		//							break;
		//						}
		//					}
		//				}
		//			}
		//		}
		//	}
		//	return _min_theta_diff;
		//};

		//single_angle_multi_line(point, _min_model);

		double pingpong = 0.5,direction = 1.0;
		while (loop_each_angle(point,_min_model) > 1.0 && cylinderModel.angle > 0) {
			cylinderModel.angle = _origin_model.angle + PI * pingpong * direction / 180.0;
			direction = -direction;
			if (direction > 0) pingpong = pingpong + 0.5;
			//renew m
			real_width = 0.1 * cylinderModel.width / abs(cos(cylinderModel.angle));//mm->cm
			M = ceil(cylinderModel.round / real_width);//实际剖分段数
			interval = cylinderModel.round / (double)M;//剖分步长
			theta_interval = 2 * PI / M;
			point.clear();
			for (int i = 1; i < M; i++) {
				if (gcd(i, M) == 1) {
					point.push_back(360.0 * ((double)i / (double)M));
				}
			}

		}

		//该缠绕角下所有绕回组合均不能到达互素点
		if (abs(_origin_model.angle-_min_model.angle)>(PI*15.0/180)) {
			debug_show(180 * _min_model.angle / PI);
		}
		else {
			double ja = jump_angle  / theta_interval;
			cylinderModel = _min_model;
			OnComputeFiberPathCylinder();
			CString STemp;
			STemp.Format(_T("纱宽(修正) = %.3f mm\n等分数=%d\n缠绕角 = %.4f degree\n切点数=%d\n跳跃数=%f\n算法: %s\n左极限 %.2f"), real_width, M, cylinderModel.angle * 180 / PI, cutNum, ja, cylinderWindingAlgorithm, cylinderModel.right_const_winding_angle_max_slip_coef);
			AfxMessageBox(STemp);
		}

		/*测试同一母线上不同起点的半测地线旋转角差异*/
		//double min_diff=999;
		//double real_left_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));
		//for (double x = cylinderModel.left_length; x > 0; x -= 2) {
		//	double y = sqrt(1 - pow(cylinderModel.left_length-x,2) / (real_left_length * real_left_length)) * cylinderModel.middle_radius;
		//	global_cylinder_coord = cylinderPathCoord(x, y, 0.0f);
		//	min_diff = 999;
		//	double left_cut_theta;
		//	double left_semi_geodesic_slip_coef;
		//	for (cylinderModel.left_semi_geodesic_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.left_semi_geodesic_slip_coef > -cylinderModel.slippage_coefficient; cylinderModel.left_semi_geodesic_slip_coef -= 0.002) {
		//		auto tmp = global_cylinder_coord;
		//		(this->*leftCylinder)(nullptr, tmp, false);
		//		if (abs(global_left_min_r - cylinderModel.left_radius) < min_diff) {
		//			min_diff = abs(global_left_min_r - cylinderModel.left_radius);
		//			left_semi_geodesic_slip_coef = cylinderModel.left_semi_geodesic_slip_coef;
		//			left_cut_theta = global_left_cut_theta;//global_left_cut_theta:一条纤维中最接近切点的theta；left_cut_theta：本组所有纤维中最接近切点的theta
		//		}
		//	}
		//	cylinderModel.left_semi_geodesic_slip_coef = left_semi_geodesic_slip_coef;
		//	
		//	// x y left_semi_geodesic_slip_coef left_cut_theta
		//	struct data d = { x,y,left_semi_geodesic_slip_coef,180.0 * left_cut_theta / PI,0,0 };
		//	quoters.push_back(d);
		//	OnComputeFiberPathCylinder();
		//}

		/*测试不同缠绕角 半测地线的形态*/
		//double tmp_min, left_coef,cut_angle;
		//double origin_winding_angle = cylinderModel.angle;
		//for (cylinderModel.angle = PI/2-0.05; cylinderModel.angle > 0.05; cylinderModel.angle -= 0.1) {
		//	tmp_min = 100;
		//	for (cylinderModel.left_semi_geodesic_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.left_semi_geodesic_slip_coef > -cylinderModel.slippage_coefficient; cylinderModel.left_semi_geodesic_slip_coef -= 0.001) {
		//		cylinderPathCoord currentPoint(cylinderModel.left_length, cylinderModel.middle_radius, 0.0f);
		//		(this->*leftCylinder)(nullptr, currentPoint, false);
		//		//(this->*middleCylinder)(nullptr, currentPoint, false);
		//		auto tmpPoint = currentPoint;
		//		if (abs(global_left_min_r - cylinderModel.left_radius) < tmp_min) {
		//			//for (cylinderModel.right_const_winding_angle_max_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.right_const_winding_angle_max_slip_coef > 0; cylinderModel.right_const_winding_angle_max_slip_coef -= 0.01) {
		//			//	for (cylinderModel.right_semi_geodesic_slip_coef = cylinderModel.slippage_coefficient; cylinderModel.right_semi_geodesic_slip_coef > -cylinderModel.slippage_coefficient; cylinderModel.right_semi_geodesic_slip_coef -= 0.01) {
		//			//		currentPoint = tmpPoint;
		//			//		(this->*rightCylinder)(nullptr, currentPoint, false);
		//			//		(this->*middleCylinder)(nullptr, currentPoint, false);
		//			//		if (abs(global_right_min_r - cylinderModel.right_radius) <0.1) {
		//			//			OnComputeFiberPathCylinder();
		//			//			break;
		//			//		}
		//			//	}
		//			//}
		//			tmp_min = abs(global_left_min_r - cylinderModel.left_radius);
		//			left_coef = cylinderModel.left_semi_geodesic_slip_coef;
		//			cut_angle = cylinderModel.cut_angle;
		//		}
		//	}
		//	if (tmp_min < 0.02) {
		//		cylinderModel.left_semi_geodesic_slip_coef = left_coef;
		//		quoters.push_back(cut_angle);
		//		OnComputeFiberPathCylinder();
		//	}
		//}

	}
}


//计算纤维路径
void CMfcogl1Doc::OnComputeFiberPathCylinder()  {
	if (m_bCanComputing == true){
		auto CylinderPointList = std::make_shared<std::deque<struct cylinderPathCoord>>();//CylinderTubePoint的双向链，用之前生成一条纤维的方法生成整个连续纤维
		windingPathStep = 1;//计算的步长
		GLfloat matDiff[4] = { 0.2F, 0.2F, 0.2F, 0.80F };
		int path_id = FIBER_PATH_LIST;
		while (glIsList(path_id)) { path_id++; }
		if (cylinderWindingAlgorithm == "NON_GEODESIC") {
			glNewList(path_id, GL_COMPILE); //FIBER_PATH_LIST     2
			matDiff[2] = 0.8F;
		}
		GLfloat matSpec[4] = { 0.30F, 0.30F, 0.30F, 0.30F };
		GLfloat matShine = 60.00F;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
		glMaterialf(GL_FRONT, GL_SHININESS, matShine);
		typedef void (CMfcogl1Doc::* CylinderWindingAlgorithm)(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint);
		CylinderWindingAlgorithm leftCylinder, middleCylinder, rightCylinder;
		 if (cylinderWindingAlgorithm == "NON_GEODESIC") {
			leftCylinder =&CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic;
			rightCylinder =&CMfcogl1Doc::OnRenderRightEllipsoidNonGeodesic;
			middleCylinder = &CMfcogl1Doc::OnRenderMiddleCylinder;
		}
		cylinderPathCoord currentStartPoint(cylinderModel.left_length, cylinderModel.middle_radius, 0.0f);
		cylinderPathCoord currentPoint(currentStartPoint); 
		glLineWidth(3);
		for (int t = 0; t <M; t++) {//M:等分数 改了
			(this->*leftCylinder)(CylinderPointList.get(), currentPoint);
			(this->*middleCylinder)(CylinderPointList.get(), currentPoint);
			(this->*rightCylinder)(CylinderPointList.get(), currentPoint);
			(this->*middleCylinder)(CylinderPointList.get(), currentPoint);

			//将下一条纤维的起始点(currentStartPoint)置于下一个切点上，再将当前点(currentPoint)置于起始点，使得每条纤维的起始位置标准化
			currentStartPoint.currentTheta += jump_angle;
			currentStartPoint.y= cylinderModel.middle_radius * cos(currentStartPoint.currentTheta);
			currentStartPoint.z = cylinderModel.middle_radius * sin(currentStartPoint.currentTheta);
			currentPoint = currentStartPoint;
		}
		glEndList();
		//统计总的坐标数
		int size = CylinderPointList->size();
		m_WindingCount[0] = size;
		//AfxMessageBox("纤维路径计算完毕\ntube Track Computation Finished.");
		m_bFiberPathComplete = true;
		auto pView = (CMfcogl1View*)myGetView();
		pView->m_cview_enable_tape_display = true;
		pView->Invalidate(false);
	}
	else{
		AfxMessageBox(_T("请先设置纤维参数\nPlease setup parameter first."));
	}
}

void CMfcogl1Doc::OnRenderMiddleCylinder(cylinderPathCoord& currentPoint) {

	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//线段起始的缠绕角α(弧度)
	double dcurve = d_curve; // dcurve * sin(angle) = r * delta
	double adjust = cylinderModel.middle_radius / sin(alpha);
	double dthera = dcurve / adjust;

	while (currentPoint.x >= cylinderModel.left_length && currentPoint.x <= (cylinderModel.left_length + cylinderModel.middle_length)) {
		currentPoint.x += currentPoint.direction * dcurve * cos(alpha);
		currentPoint.y = cylinderModel.middle_radius * cos(currentPoint.currentTheta);
		currentPoint.z = cylinderModel.middle_radius * sin(currentPoint.currentTheta);
		currentPoint.currentTheta += dthera;
	}
	glEnd();
}

void CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic(cylinderPathCoord& currentPoint) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dcurve = d_curve;
	double dr = 0;
	double dx, dtheta, r;
	double min_r = sqrt(currentPoint.y * currentPoint.y + currentPoint.z * currentPoint.z);
	double cut_theta = 0;
	double real_left_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));
	auto adjoint = [&](double in_x, double alpha) {
		double lambda = cylinderModel.left_semi_geodesic_slip_coef;
		double cos2a = cos(alpha) * cos(alpha);
		double sin2a = sin(alpha) * sin(alpha);
		double tana = tan(alpha);
		double a = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));
		double b = cylinderModel.middle_radius;
		double a2 = a * a;
		double x = cylinderModel.left_length - in_x;//
		double x2 = x * x;
		double _tmp = (1 - x2 / a2) > 0 ? sqrt(1 - x2 / a2) : sqrt(x2 / a2 - 1);
		r = b * _tmp;//截面半径 小
		dr = -(b * x) / (a2 * _tmp);//dr/dx 截面半径关于轴线长x的微分 
		double dd_r = -b / (a2 * _tmp) - b * x2 / (a2 * a2 * _tmp * abs((1 - x2 / a2)));
		double A = sqrt(1 + dr * dr);
		return [&dx] {return dx > 0 ? -1 : 1; }() * lambda * ((-dd_r * r * cos2a + sin2a) / (pow(A, 3) * r * cos2a + A * r * sin2a)) - dr * tana / r;
	};
	int flag = 1;
	while (currentPoint.x <= (cylinderModel.left_length)) {
		//x:轴线长度 
		dx = dcurve * cos(alpha) * cos(atan(dr)); //dr(截面半径关于轴线的微分) = ds(子午线)*cos(φ) = ds(弧长)*cos(α)*cos(切平面法向与轴线夹角)

		double RK = adjoint(currentPoint.x + currentPoint.direction * dx, alpha);//get dalpha && change r,dr(dr/dx)
		double sv = sin(alpha);
		double cv = cos(alpha);
		double tmpy2b = (cylinderModel.left_length - currentPoint.x) * (cylinderModel.left_length - currentPoint.x) / (real_left_length * real_left_length);
		double tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_left_length * real_left_length);
		double numerator = sv * (cylinderModel.left_length - currentPoint.x) / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
		double denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
		double current_lambda = numerator / denominator; 
		if (current_lambda < cylinderModel.left_const_winding_angle_max_slip_coef) {
 			RK = 0;
		}
		else {
			if (flag) {
				cylinderModel.now_y = cylinderModel.left_length - currentPoint.x;
				flag = 0;
			}
		}
		dtheta = tan(alpha) * sqrt(1 + dr * dr) / r;//回转体旋转角
		currentPoint.x += currentPoint.direction * dx;
		currentPoint.currentTheta += dtheta * dx;
		currentPoint.y = r * cos(currentPoint.currentTheta);
		currentPoint.z = r * sin(currentPoint.currentTheta);
		if (r < min_r) {
			min_r = r;
			cut_theta = currentPoint.currentTheta;
			cylinderModel.cut_angle = 180.0 * currentPoint.currentTheta / PI;
		}
		alpha += dx * RK;
	}
	cylinderModel.cut_angle = 180.0 * currentPoint.currentTheta / PI;
	global_left_min_r = min_r;
	global_left_cut_theta = cut_theta;
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();

}

void CMfcogl1Doc::OnRenderRightEllipsoidNonGeodesic(cylinderPathCoord& currentPoint) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dcurve = d_curve;
	double d_r = 0;
	double dx, dtheta, r;
	double min_r = sqrt(currentPoint.y * currentPoint.y + currentPoint.z * currentPoint.z);
	double real_right_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.right_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)));
	auto adjoint = [&](double in_x, double alpha) {
		double lambda = cylinderModel.right_semi_geodesic_slip_coef;
		double cos2a = cos(alpha) * cos(alpha);
		double sin2a = sin(alpha) * sin(alpha);
		double tana = tan(alpha);
		double a = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.right_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)));
		double b = cylinderModel.middle_radius;
		double a2 = a * a;
		double x = in_x - cylinderModel.left_length - cylinderModel.middle_length;
		double x2 = x * x;
		r = b * sqrt((1 - x2 / a2));//截面半径
		d_r = -(b * x) / (a2 * sqrt(1 - x2 / a2));
		double dd_r = -b / (a2 * sqrt(1 - x2 / a2)) - b * x2 / (a2 * a2 * sqrt(pow(1 - x2 / a2, 3)));
		double A = sqrt(1 + d_r * d_r);
		return [&dx] {return dx > 0 ? -1 : 1; }() * lambda * ((-dd_r * r * cos2a + sin2a) / (pow(A, 3) * r * cos2a + A * r * sin2a)) - d_r * tana / r;
	};

	while (currentPoint.x >= (cylinderModel.left_length + cylinderModel.middle_length)) {

		dx = dcurve * cos(alpha) * cos(atan(d_r));

		double RK = adjoint(currentPoint.x + currentPoint.direction * dx, alpha);

		double sv = sin(alpha);
		double cv = cos(alpha);
		double tmpy2b = (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / (real_right_length * real_right_length);
		double tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_right_length * real_right_length);
		double numerator = sv * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
		double denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
		double current_lambda = numerator / denominator;
		if (current_lambda < cylinderModel.right_const_winding_angle_max_slip_coef) {
			RK = 0; 
		}
		dtheta = tan(alpha) * sqrt(1 + d_r * d_r) / r;
		currentPoint.x += currentPoint.direction * dx;
		currentPoint.currentTheta += dtheta * dx;
		currentPoint.y = r * cos(currentPoint.currentTheta);
		currentPoint.z = r * sin(currentPoint.currentTheta);
		if (r < min_r)min_r = r;
		alpha += dx * RK;
	}
	global_right_min_r = min_r;
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();
}

void CMfcogl1Doc::OnRenderMiddleCylinder(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint) {

	glBegin(GL_LINE_STRIP);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matRed);
	double  alpha = cylinderModel.angle;//线段起始的缠绕角α(弧度)
	double dcurve = d_curve; // dcurve * sin(angle) = r * delta
	double adjust = cylinderModel.middle_radius / sin(alpha);
	double dthera = dcurve / adjust;

	while (currentPoint.x >= cylinderModel.left_length && currentPoint.x <= (cylinderModel.left_length + cylinderModel.middle_length)) {
		currentPoint.x += currentPoint.direction * dcurve * cos(alpha);
		currentPoint.y = cylinderModel.middle_radius * cos(currentPoint.currentTheta);
		currentPoint.z = cylinderModel.middle_radius * sin(currentPoint.currentTheta);
		glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);

		currentPoint.currentTheta += dthera;
	}
	glEnd();
}

void CMfcogl1Doc::OnRenderLeftEllipsoidNonGeodesic(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dcurve = d_curve;
	double dr = 0;
	double dx, dtheta, r;
	double min_r = sqrt(currentPoint.y * currentPoint.y + currentPoint.z * currentPoint.z);
	double cut_theta = 0;
	double real_left_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));
	auto adjoint = [&](double in_x, double alpha) {
		double lambda = cylinderModel.left_semi_geodesic_slip_coef;
		double cos2a = cos(alpha) * cos(alpha);
		double sin2a = sin(alpha) * sin(alpha);
		double tana = tan(alpha);
		double a = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));
		double b = cylinderModel.middle_radius;
		double a2 = a * a;
		double x = cylinderModel.left_length - in_x;//
		double x2 = x * x;
		double _tmp = (1 - x2 / a2) > 0 ? sqrt(1 - x2 / a2) : sqrt(x2 / a2 - 1);
		r = b * _tmp;//截面半径 小
		dr = -(b * x) / (a2 * _tmp);//dr/dx 截面半径关于轴线长x的微分 
		double dd_r = -b / (a2 * _tmp) - b * x2 / (a2 * a2 * _tmp * abs((1 - x2 / a2)));
		double A = sqrt(1 + dr * dr);
		return [&dx] {return dx > 0 ? -1 : 1; }() * lambda * ((-dd_r * r * cos2a + sin2a) / (pow(A, 3) * r * cos2a + A * r * sin2a)) - dr * tana / r;
	};

	while (currentPoint.x <= (cylinderModel.left_length)) {
		//x:轴线长度 
		dx = dcurve * cos(alpha) * cos(atan(dr)); //dr(截面半径关于轴线的微分) = ds(子午线)*cos(φ) = ds(弧长)*cos(α)*cos(切平面法向与轴线夹角)

		double RK = adjoint(currentPoint.x + currentPoint.direction * dx, alpha);//get dalpha && change r,dr(dr/dx)
		double sv = sin(alpha);
		double cv = cos(alpha);
		double tmpy2b = (cylinderModel.left_length - currentPoint.x) * (cylinderModel.left_length - currentPoint.x) / (real_left_length * real_left_length);
		double tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_left_length * real_left_length);
		double numerator = sv * (cylinderModel.left_length - currentPoint.x) / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
		double denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
		double current_lambda = numerator / denominator;
		if (current_lambda < cylinderModel.left_const_winding_angle_max_slip_coef) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, matYellow);
			RK = 0;
		}
		else {
			glMaterialfv(GL_FRONT, GL_AMBIENT, matBlue);
		}

		glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);

		dtheta = tan(alpha) * sqrt(1 + dr * dr) / r;//回转体旋转角
		currentPoint.x += currentPoint.direction * dx;
		currentPoint.currentTheta += dtheta * dx;
		currentPoint.y = r * cos(currentPoint.currentTheta);
		currentPoint.z = r * sin(currentPoint.currentTheta);
		if (r < min_r) {
			min_r = r;
			cut_theta = currentPoint.currentTheta;
			cylinderModel.cut_angle = 180.0 * currentPoint.currentTheta / PI;
		}
		alpha += dx * RK;
	}
	global_left_min_r = min_r;
	cylinderModel.cut_angle = 180.0 * currentPoint.currentTheta / PI;
	global_left_cut_theta = cut_theta;
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();

}

void CMfcogl1Doc::OnRenderRightEllipsoidNonGeodesic(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint) {
	glBegin(GL_LINE_STRIP);
	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
	double dcurve = d_curve;
	double d_r = 0;
	double dx, dtheta, r;
	double min_r = sqrt(currentPoint.y * currentPoint.y + currentPoint.z * currentPoint.z);
	double real_right_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.right_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)));
	auto adjoint = [&](double in_x, double alpha) {
		double lambda = cylinderModel.right_semi_geodesic_slip_coef;
		double cos2a = cos(alpha) * cos(alpha);
		double sin2a = sin(alpha) * sin(alpha);
		double tana = tan(alpha);
		double a = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.right_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)));
		double b = cylinderModel.middle_radius;
		double a2 = a * a;
		double x = in_x - cylinderModel.left_length - cylinderModel.middle_length;
		double x2 = x * x;
		r = b * sqrt((1 - x2 / a2));//截面半径
		d_r = -(b * x) / (a2 * sqrt(1 - x2 / a2));
		double dd_r = -b / (a2 * sqrt(1 - x2 / a2)) - b * x2 / (a2 * a2 * sqrt(pow(1 - x2 / a2, 3)));
		double A = sqrt(1 + d_r * d_r);
		return [&dx] {return dx > 0 ? -1 : 1; }() * lambda * ((-dd_r * r * cos2a + sin2a) / (pow(A, 3) * r * cos2a + A * r * sin2a)) - d_r * tana / r;
	};

	while (currentPoint.x >= (cylinderModel.left_length + cylinderModel.middle_length)) {
		dx = dcurve * cos(alpha) * cos(atan(d_r));
		double RK = adjoint(currentPoint.x + currentPoint.direction * dx, alpha);
		double sv = sin(alpha);
		double cv = cos(alpha);
		double tmpy2b = (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / (real_right_length * real_right_length);
		double tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_right_length * real_right_length);
		double numerator = sv * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
		double denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
		double current_lambda = numerator / denominator;
		if (current_lambda < cylinderModel.right_const_winding_angle_max_slip_coef) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, matYellow);
			RK = 0;
		}
		else {
			glMaterialfv(GL_FRONT, GL_AMBIENT, matBlue);
		}
		glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
		dtheta = tan(alpha) * sqrt(1 + d_r * d_r) / r;
		currentPoint.x += currentPoint.direction * dx;
		currentPoint.currentTheta += dtheta * dx;
		currentPoint.y = r * cos(currentPoint.currentTheta);
		currentPoint.z = r * sin(currentPoint.currentTheta);
		if (r < min_r)min_r = r;
		alpha += dx * RK;
	}
	global_right_min_r = min_r;
	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
	glEnd();
}

/*几个单一功能的缠绕计算函数fixed_angle geodesic*/
//void CMfcogl1Doc::OnRenderLeftEllipsoid(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
//	glBegin(GL_LINE_STRIP);
//	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
//	double dcurve = d_curve;
//	double diff_gx = 0;
//	while (currentPoint.x <= (cylinderModel.left_length)) {
//		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
//		double dx = dcurve * cos(alpha) * cos(atan(diff_gx));
//		currentPoint.x += currentPoint.direction * dx;
//		double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)) / pow(cylinderModel.left_length, 2);
//		double gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(currentPoint.x - cylinderModel.left_length, 2), 0));
//		diff_gx = -B * (cylinderModel.left_length - currentPoint.x) / gx;
//		currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
//		currentPoint.y = gx * cos(currentPoint.currentTheta);
//		currentPoint.z = gx * sin(currentPoint.currentTheta);
//		alpha += -diff_gx * tan(alpha) * dx / (gx);
//	}
//	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
//	glEnd();
//}
//
//void CMfcogl1Doc::OnRenderLeftEllipsoidFixedAngle(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
//	glBegin(GL_LINE_STRIP);
//	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
//	double dcurve = d_curve;
//	double diff_gx = 0;
//
//	//为了控制误差，以alpha代替实际缠绕角参与计算，实际缠绕角不改变
//	double real_left_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.left_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)));
//	while (currentPoint.x <= (cylinderModel.left_length)) {
//		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
//		double dx = dcurve * cos(alpha) * cos(atan(diff_gx));
//		currentPoint.x += currentPoint.direction * dx;
//		double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.left_radius, 2)) / pow(cylinderModel.left_length, 2);
//		double gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(currentPoint.x - cylinderModel.left_length, 2), 0));
//		diff_gx = -B * (cylinderModel.left_length - currentPoint.x) / gx;
//		currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
//		currentPoint.y = gx * cos(currentPoint.currentTheta);
//		currentPoint.z = gx * sin(currentPoint.currentTheta);
//
//		double sv = sin(alpha);
//		double cv = cos(alpha);
//		double tmpy2b = (cylinderModel.left_length - currentPoint.x) * (cylinderModel.left_length - currentPoint.x) / (real_left_length * real_left_length);
//		double tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_left_length * real_left_length);
//		double numerator = sv * (cylinderModel.left_length - currentPoint.x) / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
//		double denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
//		double lambda = numerator / denominator;
//		if (lambda >= cylinderModel.slippage_coefficient) {
//			alpha += -diff_gx * tan(alpha) * dx / (gx);
//		}
//	}
//
//	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
//	glEnd();
//}
//
//void CMfcogl1Doc::OnRenderRightEllipsoid(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
//	glBegin(GL_LINE_STRIP);
//	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
//	double dcurve = d_curve;//弧长步长
//	double diff_gx = 0;
//	while (currentPoint.x >= (cylinderModel.left_length + cylinderModel.middle_length)) {
//		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
//		double dx = dcurve * cos(alpha) * cos(atan(diff_gx));
//		currentPoint.x += currentPoint.direction * dx;
//		double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)) / pow(cylinderModel.right_length, 2);
//		double gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length, 2), 0));
//		diff_gx = -B * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / gx;
//		currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
//		currentPoint.y = gx * cos(currentPoint.currentTheta);
//		currentPoint.z = gx * sin(currentPoint.currentTheta);
//		alpha += -diff_gx * tan(alpha) * dx / (gx);
//	}
//	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
//	glEnd();
//}
//
//void CMfcogl1Doc::OnRenderRightEllipsoidFixedAngle(std::deque <struct cylinderPathCoord>* singlePathList, cylinderPathCoord& currentPoint, bool show) {
//	glBegin(GL_LINE_STRIP);
//	double  alpha = cylinderModel.angle;//初始的缠绕角α(弧度)
//	double dcurve = d_curve;
//	double diff_gx = 0;
//	double real_right_length = sqrt(pow(cylinderModel.middle_radius, 2) * pow(cylinderModel.right_length, 2) / (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)));
//	while (currentPoint.x >= (cylinderModel.left_length + cylinderModel.middle_length)) {
//		if (show) glVertex3f(currentPoint.x, currentPoint.y, currentPoint.z);
//		double dx = dcurve * cos(alpha) * cos(atan(diff_gx));
//		currentPoint.x += currentPoint.direction * dx;
//		double B = (pow(cylinderModel.middle_radius, 2) - pow(cylinderModel.right_radius, 2)) / pow(cylinderModel.right_length, 2);
//		double gx = sqrt(max(pow(cylinderModel.middle_radius, 2) - B * pow(currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length, 2), 0));
//		diff_gx = -B * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / gx;
//		currentPoint.currentTheta += sqrt(1 + pow((diff_gx), 2)) * tan(alpha) * dx / (gx);
//		currentPoint.y = gx * cos(currentPoint.currentTheta);
//		currentPoint.z = gx * sin(currentPoint.currentTheta);
//
//		double sv = sin(alpha);
//		double cv = cos(alpha);
//		double tmpy2b = (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / (real_right_length * real_right_length);
//		double tmpa2b = (cylinderModel.middle_radius * cylinderModel.middle_radius) / (real_right_length * real_right_length);
//		double numerator = sv * (currentPoint.x - cylinderModel.left_length - cylinderModel.middle_length) / (cylinderModel.middle_radius * sqrt(1 - tmpy2b));
//		double denominator = cv * cv / (1 - (1 - tmpa2b) * tmpy2b) + sv * sv / tmpa2b;
//		double lambda = numerator / denominator;
//		if (lambda >= cylinderModel.slippage_coefficient) {
//			alpha += -diff_gx * tan(alpha) * dx / (gx);
//		}
//	}
//	currentPoint.direction = -currentPoint.direction;//用direction指示缠绕方向 也可用tangent或alpha
//	glEnd();
//}

void CMfcogl1Doc::OnOpenFiberPathControlToroidParametersDlg() {
	//maybe some memory destroy here

	CDlgFiberPathControlsToroid fpToroidDlg;
	if (IDOK == fpToroidDlg.DoModal()) {

		ResetWndDesign();

		auto pView = (CMfcogl1View*)myGetView();
		toroidModel.R = pView->m_view_toroid_R;
		toroidModel.r = pView->m_view_toroid_r;
		toroidModel.angle = PI * fpToroidDlg.m_dlg_toroid_winding_angle / 180.0;
		toroidModel.band_width = fpToroidDlg.m_dlg_toroid_band_width;
		toroidModel.lambda = fpToroidDlg.m_dlg_toroid_slippage_coefficient;

		//modify alpha
		auto gcd = [](int a, int b) {
			int t;
			if (a < b) {
				t = a; a = b; b = t;
			}
			if (b == 0)return 0;
			while (a % b) {
				t = b;
				b = a % b;
				a = t;
			}
			return b;
		};
		double d_alpha = PI * 0.0001 / 180.0, margin = 0.0001;
		double R = toroidModel.R, r = toroidModel.r,origin_alpha = toroidModel.angle;
		double alpha, period, intersect, M, K;
		
		alpha = origin_alpha;
		period = 2 * PI * r / (tan(alpha) * sqrt(R * R - r * r));//纤维和外圈相邻交点的θ差
		intersect = toroidModel.band_width / sin(alpha);
		M = ceil(period * (R + r) / intersect);
		K = (1 - ((2 * PI / period) - (int)(2 * PI / period))) * M;

		double pingpong = d_alpha, direction = 1.0;
		//以下是等缠绕角的代数模式计算
		//while (!(abs(K - round(K)) < margin && gcd(round(K), M) == 1)&&M>1) {
		//	alpha = origin_alpha + direction * pingpong;
		//	period = 2 * PI * r / (tan(alpha) * sqrt(R * R - r * r));
		//	intersect = toroidModel.band_width / sin(alpha);
		//	M = ceil(period * (R + r) / intersect);
		//	K = (1 - ((2 * PI / period) - (int)(2 * PI / period))) * M;
		//	direction = -direction;
		//	if (direction > 0) pingpong += d_alpha;
		//}
		toroidModel.M = M;
		toroidModel.angle = alpha;

		double t = r / R;
		double m = toroidModel.lambda;
		double p21 = (t * t) / (m * m);
		double p22 = (4 + 4 / (m * m)) * pow(t, 2) + pow(t, 4) / pow(m, 4);
		double p1 = asin(sqrt(t));//不架桥
		double p2 = acos(sqrt((2 + p21 - sqrt(p22)) / 2));//不打滑
		double p = max(p1, p2);

		if (abs(alpha) < p) {
			CString STemp;
			STemp.Format(_T("架桥p1 = %.3f 打滑p2 = %.3f 角度angle = %.3f"), 180 * p1 / PI, 180 * p2 / PI, 180 * alpha / PI);
			AfxMessageBox(STemp);
			m_bCanComputing = true;
			OnOpenFiberPathControlToroidParametersDlg();
		}
		else {
			m_bCanComputing = true;
			debug_show(180 * toroidModel.angle / PI);
		}
	}
}

#define TEST

void CMfcogl1Doc::OnComputeFiberPathToroid() {
	if (m_bCanComputing == true) {

		auto ToroidPointList = std::make_shared<std::deque<struct toroidPathCoord>>();
		int path_id = FIBER_PATH_LIST;
		glNewList(path_id, GL_COMPILE); 
		GLfloat matDiff[4] = { 0.8F, 0.0F, 0.0F, 0.8F };
		GLfloat matDiff2[4] = { 0.2F, 0.0F, 0.0F, 0.9F };
		GLfloat matDiff3[4] = { 0.0F, 0.0F, 0.8F, 0.9F };
		GLfloat matSpec[4] = { 0.10F, 0.10F, 0.10F, 0.30F };
		GLfloat matShine = 60.00F;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
		glMaterialf(GL_FRONT, GL_SHININESS, matShine);

		toroidPathCoord currentStartPoint(0, 0);
		toroidPathCoord currentPoint(currentStartPoint);
		//toroidModel.angle = -toroidModel.angle;
		double R = toroidModel.R, r = toroidModel.r, alpha = toroidModel.angle;

		double delta = PI * 0.1 / 180.0;
		double period = 2 * PI * r / (tan(alpha) * sqrt(R * R - r * r));
		double t1 = 0, p1 = 0, t2 = 0, p2 = 0;
		glLineWidth(2.0f);

#ifdef TEST
		toroidModel.M = 1;
#endif // TEST
		currentPoint.alpha = toroidModel.angle;
		for (int i = 1; i <= toroidModel.M; i++) {
#ifdef TEST
			glBegin(GL_LINE_STRIP);
			while (-i * 2.0 * PI < currentPoint.theta && currentPoint.theta < i * 2.0 * PI) {
				thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, GL_LINE_STRIP);
				//int n = ceil((currentPoint.phi - PI) / (2 * PI));
				//currentPoint.theta = (2 * r * atan((tan(currentPoint.phi / 2) * (R - r)) / sqrt(R * R - r * r))) / (tan(alpha) * sqrt(R * R - r * r)) + n * period;
				//currentPoint.phi += delta;
				double alpha = currentPoint.alpha;
				double phi = currentPoint.phi;
				double lambda = sin(phi) > 0 ? -toroidModel.lambda : toroidModel.lambda;
				double denominator = R + r * cos(phi);
				double ap1 = r * sin(phi);
				double ap2 = denominator * tan(alpha);
				//ref: zulei 基于非测地线纤维缠绕压力容器线型设计与优化
				double dalphadphi = -lambda * ((r * cos(phi) * cos(alpha)) / (denominator * tan(alpha)) + sin(alpha)) - ap1 / ap2;
				double dthetadphi = r / ap2;
				currentPoint.phi += delta;
				currentPoint.theta += dthetadphi * delta;
				currentPoint.alpha += dalphadphi * delta;
			}
			thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, GL_LINE_STRIP);
			glEnd();
#endif // TEST

#ifndef TEST
			//part1
			currentPoint.reset(t1, p1);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff);
			glBegin(GL_TRIANGLE_STRIP);
			while (-i * 2.0 * PI < currentPoint.theta && currentPoint.theta < i * 2.0 * PI) {
				thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, GL_TRIANGLE_STRIP);
				currentPoint.phi += delta;
				int n = ceil((currentPoint.phi - PI) / (2 * PI));
				currentPoint.theta = (2 * r * atan((tan(currentPoint.phi / 2) * (R - r)) / sqrt(R * R - r * r))) / (tan(alpha) * sqrt(R * R - r * r)) + n * period;
			}
			thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, GL_TRIANGLE_STRIP);
			glEnd();
			currentPoint.reset(t1, p1);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff2);
			glBegin(GL_LINE_STRIP);
			while (-i * 2.0 * PI < currentPoint.theta && currentPoint.theta < i * 2.0 * PI) {
				thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, LEFT_OUTLINE);
				currentPoint.phi += delta;
				int n = ceil((currentPoint.phi - PI) / (2 * PI));
				currentPoint.theta = (2 * r * atan((tan(currentPoint.phi / 2) * (R - r)) / sqrt(R * R - r * r))) / (tan(alpha) * sqrt(R * R - r * r)) + n * period;
			}
			thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, LEFT_OUTLINE);
			glEnd();
			currentPoint.reset(t1, p1);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff2);
			glBegin(GL_LINE_STRIP);
			while (-i * 2.0 * PI < currentPoint.theta && currentPoint.theta < i * 2.0 * PI) {
				thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, RIGHT_OUTLINE);
				currentPoint.phi += delta;
				int n = ceil((currentPoint.phi - PI) / (2 * PI));
				currentPoint.theta = (2 * r * atan((tan(currentPoint.phi / 2) * (R - r)) / sqrt(R * R - r * r))) / (tan(alpha) * sqrt(R * R - r * r)) + n * period;
			}
			thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, RIGHT_OUTLINE);
			glEnd();
			t1 = currentPoint.theta, p1 = currentPoint.phi;
			//part 2
			toroidModel.angle = -toroidModel.angle;
			alpha = toroidModel.angle;
			period = 2 * PI * r / (tan(alpha) * sqrt(R * R - r * r));
			currentPoint.reset(t2, p2);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff);
			glBegin(GL_TRIANGLE_STRIP);
			while (-i * 2.0 * PI < currentPoint.theta && currentPoint.theta < i * 2.0 * PI) {
				thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, GL_TRIANGLE_STRIP);
				currentPoint.phi -= delta;
				int n = ceil((currentPoint.phi - PI) / (2 * PI));
				currentPoint.theta = (2 * r * atan((tan(currentPoint.phi / 2) * (R - r)) / sqrt(R * R - r * r))) / (tan(alpha) * sqrt(R * R - r * r)) + n * period;
			}
			thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, GL_TRIANGLE_STRIP);
			glEnd();
			currentPoint.reset(t2, p2);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff2);
			glBegin(GL_LINE_STRIP);
			while (-i * 2.0 * PI < currentPoint.theta && currentPoint.theta < i * 2.0 * PI) {
				thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, LEFT_OUTLINE);
				currentPoint.phi -= delta;
				int n = ceil((currentPoint.phi - PI) / (2 * PI));
				currentPoint.theta = (2 * r * atan((tan(currentPoint.phi / 2) * (R - r)) / sqrt(R * R - r * r))) / (tan(alpha) * sqrt(R * R - r * r)) + n * period;
			}
			thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, LEFT_OUTLINE);
			glEnd();
			currentPoint.reset(t2, p2);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matDiff2);
			glBegin(GL_LINE_STRIP);
			while (-i * 2.0 * PI < currentPoint.theta && currentPoint.theta < i * 2.0 * PI) {
				thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, RIGHT_OUTLINE);
				currentPoint.phi -= delta;
				int n = ceil((currentPoint.phi - PI) / (2 * PI));
				currentPoint.theta = (2 * r * atan((tan(currentPoint.phi / 2) * (R - r)) / sqrt(R * R - r * r))) / (tan(alpha) * sqrt(R * R - r * r)) + n * period;
			}
			thetaphi2gl(0, 0, currentPoint.theta, currentPoint.phi, RIGHT_OUTLINE);
			glEnd();
			t2 = currentPoint.theta, p2 = currentPoint.phi;
			toroidModel.angle = -toroidModel.angle;
			alpha = toroidModel.angle;
			period = 2 * PI * r / (tan(alpha) * sqrt(R * R - r * r));
#endif // !TEST
		}

		glEndList();
		glLineWidth(1.0f);
		//统计总的坐标数
		m_WindingCount[0] = ToroidPointList->size();
		m_bFiberPathComplete = true;
		auto pView = (CMfcogl1View*)myGetView();
		pView->m_cview_enable_tape_display = true;
		pView->Invalidate(false);
	}
	else {
		AfxMessageBox(_T("请先设置纤维参数\nPlease setup parameter first."));
	}
}

vec3 CMfcogl1Doc::thetaphi2xyz(double theta, double phi) {
	double x = (toroidModel.R + toroidModel.r * cos(phi)) * cos(theta);
	double y = toroidModel.r * sin(phi);
	double z = (toroidModel.R + toroidModel.r * cos(phi)) * sin(theta);
	return vec3(x, y, z);
}
vec3 CMfcogl1Doc::thetaphi2nxyz(double theta, double phi) {
	double nx = cos(phi) * cos(theta);
	double ny = sin(phi);
	double nz = cos(phi) * sin(theta);
	return vec3(nx, ny, nz);
}
vec3 CMfcogl1Doc::thetaphi2dxyz(double theta, double phi) {
	double dphi = PI * 1 / 180.0;
	double dtheta = dphi * toroidModel.r / (tan(toroidModel.angle) * (toroidModel.R + toroidModel.r * cos(phi)));
	vec3 p = thetaphi2xyz(theta + dtheta, phi + dphi) - thetaphi2xyz(theta, phi);
	return p/p.length();//normalized direction
}
void CMfcogl1Doc::thetaphi2gl(double oldtheta, double oldphi, double theta, double phi,int GL_MODEL) {
	vec3 xyz   = thetaphi2xyz(theta, phi);
	vec3 nxyz = thetaphi2nxyz(theta, phi);
	vec3 dxyz = thetaphi2dxyz(theta, phi);
	vec3 txyz = nxyz.normalized_cross(dxyz);
	if (GL_MODEL == LEFT_OUTLINE) {
		glNormal3f(nxyz.x, nxyz.y, nxyz.z);
		glVertex3f(xyz.x + txyz.x * toroidModel.band_width / 2, xyz.y + txyz.y * toroidModel.band_width / 2, xyz.z + txyz.z * toroidModel.band_width / 2);
	}
	if (GL_MODEL == RIGHT_OUTLINE) {
		glNormal3f(nxyz.x, nxyz.y, nxyz.z);
		glVertex3f(xyz.x - txyz.x * toroidModel.band_width / 2, xyz.y - txyz.y * toroidModel.band_width / 2, xyz.z - txyz.z * toroidModel.band_width / 2);
	}
	if (GL_MODEL == GL_TRIANGLE_STRIP) {
		glNormal3f(nxyz.x, nxyz.y, nxyz.z);
		glVertex3f(xyz.x + txyz.x * toroidModel.band_width / 2, xyz.y + txyz.y * toroidModel.band_width / 2, xyz.z + txyz.z * toroidModel.band_width / 2);
		glVertex3f(xyz.x - txyz.x * toroidModel.band_width / 2, xyz.y - txyz.y * toroidModel.band_width / 2, xyz.z - txyz.z * toroidModel.band_width / 2);
	}
	if (GL_MODEL == GL_LINE_STRIP) {
		glNormal3f(nxyz.x, nxyz.y, nxyz.z);
		glVertex3f(xyz.x, xyz.y, xyz.z);
	}
}


//garbage

//while (-toroidModel.M * 2 * PI < currentPoint.phi && currentPoint.phi < 2 * PI) {
//	thetaphi2gl(currentPoint.theta, currentPoint.phi);
//	currentPoint.phi += delta;
//	double normalized_phi = currentPoint.phi;
//	while (normalized_phi > 2 * PI)normalized_phi -= 2 * PI;
//	double I, C;
//	C = r * cota / sqrt(R * R - r * r);
//	I = C * atan((sqrt(R * R - r * r) * sin(normalized_phi)) / (r + R * cos(normalized_phi)));
//	if (0 <= normalized_phi && normalized_phi <= (PI - acrR)) {
//		currentPoint.theta = I;
//	}
//	if ((PI - acrR) < normalized_phi && normalized_phi <= (PI + acrR)) {
//		currentPoint.theta = PI * C + I;
//	}
//	if ((PI + acrR) < normalized_phi && normalized_phi <= (2 * PI)) {
//		currentPoint.theta = 2 * PI * C + I;
//	}
//}
