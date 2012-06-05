/*****************************************************************************
**  MaxExportOptionsGUI.hpp
**
**	class which holds the GUI and data-binding members of
**  the max export options
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_MAXEXPORTOPTIONSGUI_HPP
#error MAXEXP_MAXEXPORTOPTIONSGUI_HPP multuply defined!!
#endif
#define MAXEXP_MAXEXPORTOPTIONSGUI_HPP

#include "MaxCommon.hpp"

#ifndef MAXEXP_MAXEXPORTOPTIONS_HPP
#include "MaxExportOptions.hpp"
#endif
#include "max.h"
#include "resource.h"

class fxXMLWriter;
namespace MaxExp
{
	class ExportLogger;
}
using namespace MaxExp;

TCHAR* GetString(int id);


class SgpuExportOptionsGUI
{
public:
	
		SgpuExportOptionsGUI();
		~SgpuExportOptionsGUI();
public:
	bool ShowDialog();
	void Init( Interface *pMaxInterface,  DWORD genericOpts );
	void Cleanup(){}
	
private:
	INT_PTR ExportOptionsDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK ExportOptionsDlgProcS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	//helper function to enable/disable a dialog button
	void EnableDlgButton(HWND hDlg, int nIDDlgItem, BOOL bEnable);


public:
	SgpuExportOptions m_Options;
};

