/****************************************************************************\
**	pythPackage.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythPackage.hpp"

#include "Support/pyth/pythCommands.hpp"
#include "Support/pyth/GUI/pythDialogUtil.hpp"
#include "Support/pyth/pythDialogs.hpp"
#include "Support/pyth/pythEnvironments.hpp"
#include "Support/pyth/pythGroups.hpp"
#include "Support/pyth/pythLayers.hpp"
#include "Support/pyth/pythLightSets.hpp"
#include "Support/pyth/pythProperty.hpp"
#include "Support/pyth/pythPython.hpp"
#include "Support/pyth/pythSelection.hpp"
#include "Support/pyth/pythTimeline.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Support/pyth/pythMtrlProperty.hpp"
#include "Support/pyth/pythFgmtProperty.hpp"
#include "Support/pyth/pythControlProperty.hpp"
#include "Features/Prefs/PrefsPythLayouts.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

namespace
{
	void ChooseAndExecuteScript()
	{
		std::string filter = "Python scripts (*.py)|*.py";
		fsLocator initial_dir = gfPaths::GetPath(gfPaths::e_ExePath);
		initial_dir.Push("python");
		fsLocator file_loc;
		if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
		{
			pythUtil::ScriptFile(file_loc);
		}
	}

}	// end of namespace

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pythPackage::Initialize()
{
#if defined(PYTHON_ENABLED)
	Py_Initialize();

	pythCommands::Initialize();

	pythDialogUtil::Init();

//WXGUI
/*
	// Create the Python menu and add in a button and command to 
	// display the script dialog
	cmaCommand* pCmd = new cmaCommandSimple("Python Dialog", 
		"Windows",
		"Display the window for entering python commands.",
									
		pythDialogUtil::ShowPythonDialog);
	int index = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), index );
	// Command to execute a script file of the user's choice
	pCmd = new cmaCommandSimple("Execute Python Script...", 
		"Tools",
		"Choose a python script file (*.py) and execute it.",
									
		ChooseAndExecuteScript);
	index = guiMenuMgr::AddMenuItem( "Tools", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), index );

	// Add in built in python commands:
	pythSelection::AddCommands("mach");
	pythProperty::AddCommands("mach");
	pythCommands::AddCommands("mach");
	pythTimeline::AddCommands("mach");
	pythLayers::AddCommands("mach");
	prefsPython::AddCommands("mach");
	pythLightSets::AddCommands("mach");
	pythMtrlProperty::AddCommands("mach");
	pythFgmtProperty::AddCommands("mach");
	pythControlProperty::AddCommands("mach");
	//pythEnvironments::AddCommands("mach");
	pythGroups::AddCommands("mach");
	pythDialogs::AddCommands("mach");
*/
#endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pythPackage::DeInitialize()
{
#if defined(PYTHON_ENABLED)
	pythDialogUtil::CleanUp();

	pythCommands::DeInitialize();

	Py_Finalize();
#endif
}

