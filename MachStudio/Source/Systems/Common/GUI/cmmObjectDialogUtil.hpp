/*****************************************************************************
**	cmmObjectDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_OBJECTDIALOGUTIL_HPP
#error cmmObjectDialogUtil.hpp multiply included
#endif
#define CMM_OBJECTDIALOGUTIL_HPP

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
//using namespace System::Windows::Forms;
class nameObject;


//============================================================================
//============================================================================
namespace cmmObjectDialogUtil
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CleanUp();
	
	//--------------------------------------------------------------------
	// Display key buttons in front of property names
	//--------------------------------------------------------------------
	void SetViewPropertyKeyButtons(bool i_bVal);
	bool GetViewPropertyKeyButtons();

	//--------------------------------------------------------------------
	// Show
	//--------------------------------------------------------------------
	void  Show();

	//--------------------------------------------------------------------
	//	ShowDrivers - like show but puts Drivers tab to the front
	//--------------------------------------------------------------------
	void  ShowDrivers();

	//--------------------------------------------------------------------
	//  Hide
	//--------------------------------------------------------------------
	void  Hide();

	//--------------------------------------------------------------------
	// Update the dialog
	//--------------------------------------------------------------------
	void UpdateDialog();

	//--------------------------------------------------------------------
	// clear the dialog
	//--------------------------------------------------------------------
	void ClearDialog();

	//--------------------------------------------------------------------
	//	Create the driver
	//--------------------------------------------------------------------
	void CreateDriver(std::string& i_DriverName);

#ifdef USE_WXWIDGETS
	//--------------------------------------------------------------------
	// Add/Remove Tab Pages
	//--------------------------------------------------------------------
	void AddTabPage(wxPanel* i_pTabPage, const std::string i_Title, bool i_bSelectTab = false);
	void RemoveTabPage(wxPanel* i_pTabPage);

	//--------------------------------------------------------------------
	// Returns true if the given tab page is attached.
	//--------------------------------------------------------------------
	bool HasTabPage(wxPanel* i_pTabPage);
#endif

}	// end of namespace

