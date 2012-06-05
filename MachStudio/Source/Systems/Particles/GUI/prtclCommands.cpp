/*****************************************************************************
**  prtclCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/GUI/prtclCommands.hpp"

#include "Systems/Particles/Object/prtclObjectMgr.hpp"

#include "Features/Channels/chnlCommandUtil.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"


//============================================================================
//============================================================================
namespace prtclCommands
{
	//--------------------------------------------------------------------
	//
	//	Command Functions
	//
	//--------------------------------------------------------------------
	namespace
	{
		//----------------------------------------------------------------
		//	Particles Active
		//----------------------------------------------------------------
		void Set_ParticlesActive(bool i_bChecked)
		{
			prtclObjectMgr::Pause(i_bChecked);
			//guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Particles, i_bChecked?"Prtcls":"" );
			//guiStatusBarMgr::SetToolTip(mnmConstants::e_SBPanel_Particles, 
			//	i_bChecked ? "Particles Active" : "Particles Inactive" );
		}
		bool Get_ParticlesActive()
		{
			return prtclObjectMgr::Paused();
		}
	}

	//--------------------------------------------------------------------
	// SetupMenu
	//--------------------------------------------------------------------
	void SetupMenu()
	{
		//	create the system tab page
		//System::Windows::Forms::TabPage^ pTP = tmaCommandTabControlUtil::CreateSystemTabPage( "Particles" );

		//
		//	commands
		//
		//cmaCommand* pCmd; 
		//guiMenuMgr::AddMenu( "View", "Icons" );

		//	COMMAND: view icons
		//pCmd = new cmaCommandToggle("Particle Icons", 
		//							"Icons", 
		//							"Toggle Icon Visibility",
		//							&prtclObjectMgr::ShowIcons, 
		//							&prtclObjectMgr::IconsVisible);
		//int menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
		//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		//cmmSystemDialogUtil::AddSystemCommand( "Particles", "Toggle Icon Visibility", pCmd );

		//	COMMAND: particles on/off
		//pCmd = new cmaCommandToggle("Particles Active", 
		//							"Actions", 
		//							"Turn on or off particles",
		//							&Set_ParticlesActive, 
		//							&Get_ParticlesActive);
		//menu_id = guiMenuMgr::AddCheckableMenuItem( "Actions", pCmd->GetTag().c_str() );
		//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		//cmmSystemDialogUtil::AddSystemCommand( "Particles", "Toggle Particles Active", pCmd );

		//
		//	driver commands (create ALL of them)
		//
		//chnlCommandUtil::CreateDriverButton( "Spline", "Particles" );
		//chnlCommandUtil::CreateDriverButton( "Attach", "Particles" );
		//chnlCommandUtil::CreateDriverButton( "Static Position", "Particles" );
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
