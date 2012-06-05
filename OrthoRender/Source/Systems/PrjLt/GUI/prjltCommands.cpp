/*****************************************************************************
**	prjltCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/GUI/prjltCommands.hpp"

#include "Systems/PrjLt/GUI/prjltCreateKeyLight.h"
#include "Systems/PrjLt/GUI/prjltDialogUtil.hpp"
#include "Systems/PrjLt/Timeline/prjltDriverCreator.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"

#include "Features/Channels/chnlCommandUtil.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"


//============================================================================
//============================================================================
namespace prjltCommands
{
	//
	//	Command Functions
	//
	namespace
	{
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateKeyLight()
		{
#ifdef _MANAGED
			SystemProjectedLights::prjltCreateKeyLight ^dialog = 
				gcnew SystemProjectedLights::prjltCreateKeyLight();
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				// Get name of object to attach to
				nameString object_name;
				dialog->GetObjectName(object_name);

				// Create key light
				prjltScriptData light_data;

				std::string light_name;
				dialog->GetLightName(light_name);
				if (!light_name.empty())
				{
					light_data.m_BaseData.m_Name.SetValue(light_name);
				}

				// attachment of target channel
				tmlnDriverAttachInfo* pDriverTarget = new tmlnDriverAttachInfo(
					prjltDriverCreator::GetTargetAttachChunkName());
				pDriverTarget->m_Name = "Attach";
				pDriverTarget->m_ObjectName = object_name;
				dialog->GetAttachNode( pDriverTarget->m_AttachName );
				dialog->GetTargetOffset( pDriverTarget->m_WorldSpaceOffset );
				pDriverTarget->m_EndTime = tmlnTimeLine::GetMaximum();
				light_data.m_Drivers.push_back(pDriverTarget);

				// attachment on position channel
				tmlnDriverAttachInfo* pDriverPosition = new tmlnDriverAttachInfo(
					prjltDriverCreator::GetPositionAttachChunkName());
				pDriverPosition->m_Name = "Attach";
				pDriverPosition->m_ObjectName = object_name;
				pDriverPosition->m_AttachName = "root";
				dialog->GetPositionOffset( pDriverPosition->m_WorldSpaceOffset );
				pDriverPosition->m_EndTime = tmlnTimeLine::GetMaximum();
				light_data.m_Drivers.push_back(pDriverPosition);

				prjltOperations::AddObject(light_data);

				// Get index of the light we just created
				int index = prjltObjectMgr::GetNumObjects() - 1;
				prjltOperations::SelectObject(index);

				// Now add light to light set
				std::string light_set;
				dialog->GetLightSet(light_set);
				if (!light_set.empty())
				{
					nameString light_set_name(light_set);
					if (ltstLightSetMgr::IsValidLightSetName(light_set))
					{
						ltstLightSetMgr::CreateLightSet(light_set_name);
					}

					// Add light to light set
					ltstLightSetMgr::AddLightToSet(	light_set_name, 
													prjltObjectMgr::GetBaseData(index).m_Name.GetValue());

					// Assign character to light set
					ltstLightSetMgr::AddObjectToLightSet(light_set_name, object_name);
				}
			}

			delete dialog;
#endif
		}
	}

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar and tabs
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
#ifdef _MANAGED
		//	COMMAND: Create key light assigned to a character
		pCmd = new cmaCommandSimple("Create Key Light", 
									"Actions", 
									"Create a Key Light",
									&Execute_CreateKeyLight );
		menu_id = guiMenuMgr::AddMenuItem( "Actions", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Projected Lights", "Create Key Light", pCmd );
#endif

		//	create the system tab page
		//System::Windows::Forms::TabPage^ pTP = tmaCommandTabControlUtil::CreateSystemTabPage( "Projected Lights" );

		//
		//	commands
		//
		guiMenuMgr::AddMenu( "View", "Icons" );

		//	COMMAND: view icons
		cmaCommand* pCmd = new cmaCommandToggle("Projected Light Icons", 
												"Icons", 
												"Toggle Icon Visibility",
												&prjltObjectMgr::ShowIcons, 
												&prjltObjectMgr::IconsVisible);
		int menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Projected Lights", "Toggle Icon Visibility", pCmd );

		//
		//	driver commands (create ALL of them)
		//
		chnlCommandUtil::CreateDriverButton( "Spline", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Attach", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Static Position", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Enabled", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Color", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Target Spline", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Target Attach", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Target Static Position", "Projected Lights" );
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
