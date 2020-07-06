#if !defined(AFX_WAGLVIEW1_H__61D57C14_70E5_11D3_ACB4_0000E83E71C7__INCLUDED_)
#define AFX_WAGLVIEW1_H__61D57C14_70E5_11D3_ACB4_0000E83E71C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaglView1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaglView view

class CWaglView : public CView
{
protected:
	CWaglView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWaglView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaglView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWaglView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CWaglView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAGLVIEW1_H__61D57C14_70E5_11D3_ACB4_0000E83E71C7__INCLUDED_)
