// mfcogl1.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "mfcogl1.h"
#include "WelcomeDlg.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "mfcogl1Doc.h"
#include "mfcogl1View.h"

//Code of multivew for indentical document, added on Sept. 22, 1999
#include "WaglView1.h"
//Code of multivew for indentical document, added on Sept. 22, 1999

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1App

BEGIN_MESSAGE_MAP(CMfcogl1App, CWinApp)
	//{{AFX_MSG_MAP(CMfcogl1App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1App construction

CMfcogl1App::CMfcogl1App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMfcogl1App object

CMfcogl1App theApp;

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1App initialization

BOOL CMfcogl1App::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

//The original code before add multiviews
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_MFCOGLTYPE,
		RUNTIME_CLASS(CMfcogl1Doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMfcogl1View));
	AddDocTemplate(pDocTemplate);
//The original code before add multiviews

//Code of multivew for indentical document, added on Sept. 22, 1999
		m_pTemplate_Mandrel = new CMultiDocTemplate(
		IDR_MFCOGLTYPE,
		RUNTIME_CLASS(CMfcogl1Doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMfcogl1View));

	 m_pTemplate_WaglView=new CMultiDocTemplate(
		 IDR_MFCOGLTYPE,
		 RUNTIME_CLASS(CMfcogl1Doc),
		 RUNTIME_CLASS(CChildFrame),
		 RUNTIME_CLASS(CWaglView));
//Code of multivew for indentical document, added on Sept. 22, 1999

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	m_nCmdShow = SW_SHOWMAXIMIZED;
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	//ShowWelcome();
	return TRUE;
}

void CMfcogl1App::ShowWelcome()
{

	CTime startTime = CTime::GetCurrentTime();
	CWelcomeDlg Welcomedlg;
	Welcomedlg.DoModal();
	CTime endTime = CTime::GetCurrentTime();
	CTimeSpan elapsedTime = endTime - startTime;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CMfcogl1App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMfcogl1App message handlers


int CMfcogl1App::ExitInstance() 
{
//Code of multivew for indentical document, added on Sept. 22, 1999
	delete m_pTemplate_WaglView;
	delete m_pTemplate_Mandrel;
//Code of multivew for indentical document, added on Sept. 22, 1999
		
	return CWinApp::ExitInstance();
}
