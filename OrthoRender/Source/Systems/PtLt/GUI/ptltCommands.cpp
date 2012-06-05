/*****************************************************************************
**	ptltCommands.cpp
**
**	Sets up menu buttons for system ptlt
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/GUI/ptltCommands.hpp"

#include "Systems/PtLt/GUI/ptltCreateFillLight.h"
#include "Systems/PtLt/GUI/ptltDialogUtil.hpp"
#include "Systems/PtLt/Timeline/ptltDriverCreator.hpp"
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"
#include "Systems/PtLt/Undo/ptltOperations.hpp"

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
namespace ptltCommands
{
	//
	//	Command Functions
	//
	namespace
	{
		const int c_NumLights = 4;
		const char* c_FillNames[] = {
			"FILL_%s_LEFT", "FILL_%s_RIGHT", "FILL_%s_FRONT", "FILL_%s_BACK"
		};
		maVector3d c_Offsets[] = {
			maVector3d(-1, 0.25, 0), maVector3d(1, 0.25, 0), maVector3d(0, 0.25, -1), maVector3d(0, 0.25, 1)
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateFillRing()
		{
#ifdef _MANAGED
			SystemPointLights::ptltCreateFillLight ^dialog = gcnew SystemPointLights::ptltCreateFillLight();
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				// Get name of object to attach to
				nameString object_name;
				dialog->GetObjectName(object_name);

				for (int i=0; i<c_NumLights; i++)
				{
					// Create fill light
					ptltScriptData light_data;

					std::string light_name;
					dialog->GetLightName(light_name);
					if (!light_name.empty() && light_name.length() < 200)
					{
						char buffer[256];
						::sprintf(buffer, c_FillNames[i], light_name.c_str());
						light_data.m_BaseData.m_Name.SetValue( std::string(buffer) );
					}

					// Set data on light itself
					maFloatRGBA color;
					dialog->GetColor(color);
					light_data.m_BaseData.m_Color.SetValue(color);
					light_data.m_BaseData.m_bDiffuseEnabled.SetValue(dialog->GetDiffuseEnabled());
					light_data.m_BaseData.m_bSpecularEnabled.SetValue(dialog->GetSpecularEnabled());
					maVector3d falloff;
					dialog->GetFalloff(falloff);
					light_data.m_BaseData.m_Falloff.SetValue(falloff);
					light_data.m_BaseData.m_ShadowSource.SetValue(true);

					// attachment on position channel
					tmlnDriverAttachInfo* pDriverPosition = new tmlnDriverAttachInfo(
						ptltDriverCreator::GetPositionAttachChunkName());
					pDriverPosition->m_Name = "Attach";
					pDriverPosition->m_ObjectName = object_name;
					pDriverPosition->m_AttachName = "root";
					maVector3d offset = c_Offsets[i] * dialog->GetRadius();
					pDriverPosition->m_WorldSpaceOffset = offset;
					pDriverPosition->m_EndTime = tmlnTimeLine::GetMaximum();
					light_data.m_Drivers.push_back(pDriverPosition);

					ptltOperations::AddObject(light_data);

					// Get index of the light we just created
					int index = ptltObjectMgr::GetNumObjects() - 1;
					ptltOperations::SelectObject(index);

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
						ltstLightSetMgr::AddLightToSet(light_set_name, 
							ptltObjectMgr::GetBaseData(index).m_Name.GetValue());

						// Assign character to light set
						ltstLightSetMgr::AddObjectToLightSet(light_set_name, object_name);
					}
				}
			}

			delete dialog;
#endif
		}
	}

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
#ifdef _MANAGED
		//	COMMAND: Create a fill ring
		pCmd = new cmaCommandSimple("Create Fill Ring", 
									"Actions", 
									"Create a fill ring",
									
									&Execute_CreateFillRing );
		menu_id = guiMenuMgr::AddMenuItem( "Actions", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Point Lights", "Create Fill Ring", pCmd );
#endif

		const char* lc_SYSTEMNAME = "Point Light";
		//	create the system tab page
		//System::Windows::Forms::TabPage^ pTP = tmaCommandTabControlUtil::CreateSystemTabPage( "Point Light" );

		//
		//	Commands
		//
		int menu_id;
		cmaCommand* pCmd;
		guiMenuMgr::AddMenu( "View", "Icons" );

		//	COMMAND: view icon
		pCmd = new cmaCommandToggle("Point Light Icons", 
									"Icons", 
									"Toggle Icon Visibility",
									&ptltObjectMgr::ShowIcons, 
									
									&ptltObjectMgr::IconsVisible);
		menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Point Lights", "Toggle Icon Visibility", pCmd );

		//
		//	driver commands (create ALL of them)
		//
		chnlCommandUtil::CreateDriverButton( "Spline", "Point Light" );
		chnlCommandUtil::CreateDriverButton( "Attach", "Point Light" );
		chnlCommandUtil::CreateDriverButton( "Static Position", "Point Light" );
		chnlCommandUtil::CreateDriverButton( "Enabled", "Point Light" );
		chnlCommandUtil::CreateDriverButton( "Color", "Point Light" );
		chnlCommandUtil::CreateDriverButton( "ColorFlicker", "Point Light" );
	}


	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
