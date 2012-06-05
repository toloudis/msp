/*****************************************************************************
**  keyfPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/Keyframing/keyfPackage.hpp"

#include "Features/Keyframing/keyfKeyframeUtil.hpp"
#include "Features/Keyframing/keyfPython.hpp"
#include "Features/Keyframing/keyfThinkInterest.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace keyfPackage
{
	namespace
	{
		keyfThinkInterest*		l_pKeyframingTI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// register the think interest
		if ( l_pKeyframingTI == 0 )
		{
			l_pKeyframingTI = new keyfThinkInterest();
			mnmThinkMgr::RegisterThinkInterest( l_pKeyframingTI );
		}

		//	COMMAND: Set Key
		cmaCommand* pCmd = new cmaCommandSimple("Set Key", 
			"Actions",
			"Set a key on the appropriate channel",
			keyfKeyframeUtil::DoKeyframe);
		int index = guiMenuMgr::AddMenuItem( "Actions", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), index );

		//	COMMAND: auto key toggle
		pCmd = new cmaCommandToggle("Auto Key", 
			"Actions",
			"Toggle Auto Key",
			keyfKeyframeUtil::SetAutoKey,
			keyfKeyframeUtil::IsAutoKey);
		index = guiMenuMgr::AddCheckableMenuItem( "Actions", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), index );

		// Separator after timeline commands in menu
		guiMenuMgr::AddSeparator("Actions");

		//	COMMAND: Add in python commands
		keyfPython::AddCommands("mach");
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Think Interest
		if ( l_pKeyframingTI != 0 )
		{
			mnmThinkMgr::UnRegisterThinkInterest( l_pKeyframingTI );
			delete l_pKeyframingTI;
			l_pKeyframingTI = 0;
		}
	}

}	// end of namespace