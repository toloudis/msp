/*****************************************************************************
**	cptrToPhotoshopDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/wxGUI/cptrToPhotoshopdialog.hpp"

#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrModeRenderFrame.hpp"
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Capture/wxGUI/ExportConstants.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"


//============================================================================
//============================================================================
namespace Photoshop
{
	modeModeID l_ModeIDFrame;
	
	//========================================================================
	//========================================================================
	namespace
	{
		const char* c_Toolbar_Photoshop_Name = "Photoshop";

		int l_MenuIdPhotoshop = -1;
		int l_MenuIdZip = -1;
		int l_MenuIdAfterEffects = -1;
		
		//----------------------------------------------------------------------------
		// Exports the current frame to Photoshop
		//----------------------------------------------------------------------------
		void Export_Photoshop()
		{
			cptrRenderUtil::SetPSflag(true);
			modeModeMgr::Push(GetFrameID());
		}
		
	}// End of namespace

	//----------------------------------------------------------------------------
	// Get the l_ModeIDFrame
	//----------------------------------------------------------------------------
	int GetFrameID()
	{
		return l_ModeIDFrame;
	}

	//----------------------------------------------------------------------------
	// l_ModeIDFrame is set from cptrPackage.cpp
	//----------------------------------------------------------------------------
	void SetFrameID(int i_ID)
	{
		l_ModeIDFrame = i_ID;
	}

	//----------------------------------------------------------------------------
	// Sets the Menu for the icon to appear on the taskbar
	//----------------------------------------------------------------------------
	void SetupMenu()
	{
		cmaCommand* pCmd = NULL;
		pCmd = new cmaCommandSimple("Export to Photoshop",	
									"Render",
									"Export the current frame to Photoshop",
									&Export_Photoshop);
		l_MenuIdPhotoshop = guiMenuMgr::AddMenuItem("Render", pCmd->GetTag().c_str(), 
		exportConstants::mc_Toolbar_Export_Name,"export-pshop.png");
		guiCommandMgr::Add(pCmd, pCmd->GetTag().c_str(), l_MenuIdPhotoshop);
	}

	//----------------------------------------------------------------------------
	// Enable Menu on the toolbar
	//----------------------------------------------------------------------------
	void EnableMenus()
	{
		guiToolbarMgr::Show(c_Toolbar_Photoshop_Name, true);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdPhotoshop, true);
	}
	
} //end of namespace

