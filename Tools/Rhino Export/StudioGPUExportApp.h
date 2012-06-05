/////////////////////////////////////////////////////////////////////////////
// StudioGPUExportApp.h : main header file for the StudioGPUExport DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CStudioGPUExportApp
// See StudioGPUExportApp.cpp for the implementation of this class
//

class CStudioGPUExportApp : public CWinApp
{
public:
	CStudioGPUExportApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
