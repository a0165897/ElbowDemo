// mfcogl1View.h : interface of the CMfcogl1View class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCOGL1VIEW_H__27D94B34_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_)
#define AFX_MFCOGL1VIEW_H__27D94B34_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define PE  pDoc->pPath
#define PT pDoc->ptrack

#define MANDRELDISPLAYLIST  1
#define FIBER_PATH_LIST     2
#define FIBER_TAPE_LIST     3
#define COMPOSITE_LIST      4
#include "mfcogl1Doc.h" 
#include "MainFrm.h"

class CMfcogl1View : public CView
{
protected: // create from serialization only
	CMfcogl1View();
	DECLARE_DYNCREATE(CMfcogl1View)

// Attributes
public:
	CMfcogl1Doc* GetDocument();
	CClientDC  * m_pdc;
	HGLRC      m_hrc;
	CRect       m_oldRect;
	fPathDataStructure *m_cview_pPathPoint;
	unsigned long m_cview_total_path_point_number;
	unsigned long m_cview_pnt_num_displayed_to;
	bool m_cview_dll_vanish;
	bool m_cview_dll_data_valid;
	long int m_cview_dll_pData[8];
	unsigned int m_cview_display_elapse;
	CString astring;
	CMainFrame* pFrame;
	CRect arect;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcogl1View)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	bool m_cview_disable_mandrel_display;
	bool m_cview_enable_tape_display;
	void ReadFiberPathDataFromDisk(char *pFileName);
	int	 ReadDataFromDLL(char *pFileName);
	int  SetDataInDLL(char *pFileName);

	int CreateFiberPathList();
	int DisplayPayeyeProcess();
	int DisplaytheWindingProcess();

	void CreateMandrelDisplayList();
	void InitFrustum();
	GLfloat m_view_span_angle;
	virtual ~CMfcogl1View();

	/*added by LMK*/
	void CMfcogl1View::CreateTubeDisplayList();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMfcogl1View)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRotateAroundXAxis();
	afx_msg void OnRotateAroundYAxis();
	afx_msg void OnRotateAroundZAxis();
	afx_msg void OnTranslateX();
	afx_msg void OnTranslateY();
	afx_msg void OnTranslateZ();
	afx_msg void OnToggleViewingDirection();
	afx_msg void OnCreateNewElbowMandrel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFillPolygonMode();
	afx_msg void OnLinePolygonMode();
	afx_msg void OnUpdateFillPolygonMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLinePolygonMode(CCmdUI* pCmdUI);
	afx_msg void OnAutoRotate();
	afx_msg void OnUpdateAutoRotate(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT   nFlags, short   zDelta, CPoint   pt);
	afx_msg void OnSetRotationSpeed();
	afx_msg void OnDrawFiberPath();
	afx_msg void OnDisplayAll();
	afx_msg void OnViewDisableMandrelDisplay();
	afx_msg void OnPayeyeSimulation();
	afx_msg void OnComputeDisplayData();
	afx_msg void OnDrawMandrel();
	afx_msg void OnDisplayWindingSequence();
	afx_msg void OnResetDisplayProcessToStart();
	afx_msg void OnDisplayNextPointOfWindingProcess();
	afx_msg void OnAutoDisplayNextPoint();
	afx_msg void OnAdjustDisplayElapse();
	afx_msg void OnViewDisplayMandrel();
	afx_msg void OnUpdateViewDisplayMandrel(CCmdUI* pCmdUI);

	/*added by LMK*/
	afx_msg void OnCreateNewTubeMandrel();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	
	unsigned short step_number,scircuit;
	unsigned long m_payeye_to;
	bool m_bCanDisplayPayeye,m_bCanDisplayPath;
	bool m_bCanDisplayMandrel,bDynamicFlag;
	CMfcogl1Doc *pDoc;


	bool m_cview_display_winding_in_sequence,m_bHaveCylinder;	
	float m_view_rotation_speed;
	CPoint m_mouse_down_point;
	BOOL m_elbow_updated;
	BOOL m_polygon_line_mode;
	BOOL m_auto_rotate;
	bool m_cview_auto_draw_two_points;
	float m_view_cylinder_height;
	GLfloat m_fRadius;
	GLfloat m_view_sweep_radius;
	GLfloat m_view_pipe_radius;
	GLfloat m_rotate_step;
	GLfloat m_translate_step;
	GLint m_viewing_sign;
	GLfloat m_Xangle;
	GLfloat m_Yangle;
	GLfloat m_Zangle;
	GLfloat m_auto_rotate_angle;
	GLfloat m_angle_increment;
	
	GLfloat m_X_translate;
	GLfloat m_Y_translate;
	GLfloat m_Z_translate;
	GLint   m_elbow_cnt;
public:
	/*added by LMK*/
	//dialog(IDC_TUBE_WIDTH/2 - RADIUS)->Class CreateNewTubeDlg(m_dlg_tube_a)->view(m_view_tube_a)
	GLfloat m_view_tube_a;
	GLfloat m_view_tube_b;
	GLfloat m_view_tube_length;
	GLfloat m_view_tube_r;

};

#ifndef _DEBUG  // debug version in mfcogl1View.cpp
inline CMfcogl1Doc* CMfcogl1View::GetDocument()
   { return (CMfcogl1Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCOGL1VIEW_H__27D94B34_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_)
