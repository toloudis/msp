/****************************************************************************\
**	pythPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythPackage.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/pyth/pythCommands.hpp"
#include "Support/pyth/pythDebug.hpp"
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
#include "Support/pyth/pythRenderLayers.hpp"
#include "Support/pyth/pythControlProperty.hpp"
#include "Support/pyth/pythEventCallbackMgr.hpp"
#include "Support/pyth/pythThinkInterest.hpp"
#include "Features/Prefs/PrefsPythLayouts.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	void ChooseAndExecuteScript()
	{
		std::string filter = "Python scripts (*.py)|*.py";
		fsLocator initial_dir = gfPaths::GetPath( mnmPaths::e_Python );
		fsLocator file_loc;
		if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
		{
			pythUtil::ScriptFile(file_loc);
		}
	}

	pythThinkInterest* l_pPythonEventTI = NULL;

}	// end of namespace


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pythPackage::Initialize()
{
#if defined(PYTHON_ENABLED)
	Py_Initialize();

	pythCommands::Initialize();

	pythDialogUtil::Init();
	
	pythEventCallbackMgr::Initialize();

	cmaCommand* pCmd;


	// Command to execute a script file of the user's choice
	pCmd = new cmaCommandSimple("Execute Python Script...", 
		"Scripts",
		"Choose a python script file (*.py) and execute it.",
		ChooseAndExecuteScript);
	int index = guiMenuMgr::AddMenuItem( "Scripts", pCmd->GetTag().c_str() );
#if( SGPU_APP == MS_CORE )
	guiMenuMgr::EnableMenuItem("Scripts", "Execute Python Script...", false);
#endif
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), index );

	// Create the Python menu and add in a button and command to 
	// display the script dialog
	pCmd = new cmaCommandSimple("Python Dialog", 
		"Scripts",
		"Display the window for entering python commands.",	
		pythDialogUtil::ShowPythonDialog);
	index = guiMenuMgr::AddMenuItem( "Scripts", pCmd->GetTag().c_str() );
#if( SGPU_APP == MS_CORE )
	guiMenuMgr::EnableMenuItem("Scripts", "Python Dialog", false);
#endif
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), index );

	// Separator before specific scripts in menu
	guiMenuMgr::AddSeparator("Scripts");

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
	pythEnvironments::AddCommands("mach");
	pythGroups::AddCommands("mach");
	pythDialogs::AddCommands("mach");
	pythRenderLayers::AddCommands("mach");
	pythDebug::AddCommands("mach");
	pythEventCallbackMgr::AddCommands("mach");

	// register the think interest
	if ( l_pPythonEventTI == NULL )
	{
		l_pPythonEventTI = new pythThinkInterest();
		mnmThinkMgr::RegisterThinkInterest( l_pPythonEventTI );
	}

#endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pythPackage::DeInitialize()
{
#if defined(PYTHON_ENABLED)
	pythDialogUtil::CleanUp();

	pythCommands::DeInitialize();

	pythEventCallbackMgr::DeInitialize();

	//	Unregister the Think Interest
	if ( l_pPythonEventTI != NULL )
	{
		mnmThinkMgr::UnRegisterThinkInterest( l_pPythonEventTI );
		delete l_pPythonEventTI;
		l_pPythonEventTI = NULL;
	}

	Py_Finalize();
#endif
}

