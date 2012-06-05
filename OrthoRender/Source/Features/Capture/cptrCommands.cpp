/*****************************************************************************
**  cptrCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007- All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrCommands.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmScreenCaptureUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "ToolUIManaged/tma/tmaCommandTabControlUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_ScreenCapCounter;

	//
	//	Command Functions
	//

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ViewCapture()
	{
		// Screen capture
		fsLocator locator = gfPaths::GetPath( mnmPaths::e_SaveFootage );

		//	test for the directory first
		if ( !fsFileUtil::DirectoryExists( locator ) )
		{
			fsFileUtil::CreateDirectory( locator );
		}

		//	write the image
		char buffer[64];
		::sprintf(buffer, "grab%04d.bmp", l_ScreenCapCounter++);
		locator.Push(itString(buffer));

		mnmScreenCaptureUtil::WriteBMP(locator);

		// debug only
		std::string fullpath;
		fsFileUtil::LocatorToANSIFilename(locator,fullpath);
		DBG_LOG1("Screen Grab: (%s)", fullpath.c_str());

		//	write something to the status bar
//WXGUI		guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Messages, buffer );
	}
}

//--------------------------------------------------------------------
// SetupMenu
//--------------------------------------------------------------------
void cptrCommands::SetupMenu()
{
//WXGUI	
/*	
	l_ScreenCapCounter = 0;

	//
	//	commands
	//
	int menu_id;
	cmaCommand* pCmd;

	//	COMMAND: Single Frame Render
	pCmd = new cmaCommandSimple("Render Frame", 
								"Actions", 
								"Screen capture of view",
									
								&Execute_ViewCapture );
	menu_id = guiMenuMgr::AddMenuItem( "Actions", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Render Frame", pCmd );
*/
}
