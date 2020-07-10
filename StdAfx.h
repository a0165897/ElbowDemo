// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__27D94B2C_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_)
#define AFX_STDAFX_H__27D94B2C_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//#define PI 3.1415926535897932384626
const double PI=3.1415926535897932384626;
#include  <gl/gl.h>
#include  <gl/glu.h>
//#include  <gl/glaux.h>
#include <math.h>
#include <afxtempl.h>
#include "datatype.h"
#include "pay.h"

//#include <valarray> //Determine to use which type of Trigonometry Functions, defined in <math.h> or <valarray>
//is <valarray> compatible to <math.h> and has the use just like Matlab.

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__27D94B2C_DAC8_11D2_A30C_0000E83E71C7__INCLUDED_)
