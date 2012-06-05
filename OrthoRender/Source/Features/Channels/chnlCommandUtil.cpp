/*****************************************************************************
**	chnlCommandUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlCommandUtil.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Features/Channels/chnlCommandCreateDriver.hpp"
#include "ToolUIManaged/tma/tmaCommandTabControlUtil.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"

#include "Tool/sel3d/sel3dMgr.hpp"


//============================================================================
//============================================================================
namespace chnlCommandUtil
{
	//--------------------------------------------------------------------
	// CreateDriverButton
	//--------------------------------------------------------------------
	void  CreateDriverButton( const char* i_pDriverName, 
							  const char* i_pTabPageName)
	{
		chnlCommandCreateDriver* pCmdCD = new chnlCommandCreateDriver();
		pCmdCD->SetDriverName(i_pDriverName);

#ifdef _MANAGED
		int objectID = tmaCommandTabControlUtil::AddDriverButton( gcnew String(i_pTabPageName), 
							pCmdCD, 
							gcnew String(pCmdCD->GetDriverName().c_str()) );
#else
		int objectID = -1;
#endif

		const bool lc_RegisterInMenu = false;
		std::string tag(i_pTabPageName);
		tag += " ";
		tag += i_pDriverName;
		pCmdCD->SetTag(tag);
		guiCommandMgr::Add( pCmdCD, tag, objectID, lc_RegisterInMenu );
	}

	//--------------------------------------------------------------------
	// CreateDriverButtons
	//--------------------------------------------------------------------
	void  CreateDriverButtons(tmlnDriverCreator* i_pDriverCreator,
							  const char* i_pTabPageName)
	{
		tmlnScriptObject* script_obj = 0;
		pick3dPickObject* pPO = sel3dMgr::GetSelected();
		script_obj = dynamic_cast<tmlnScriptObject*>(pPO);

		if (script_obj)
		{
			// get the drivers and find if the driver exists
			//
			tmlnDriverNameList driver_names;
			i_pDriverCreator->GatherPossibleDrivers(script_obj, driver_names);

			if (!driver_names.Empty())
			{
				for (int i=0; i< driver_names.m_DriverNames.size(); i++)
				{
					CreateDriverButton( driver_names.m_DriverNames[i].m_Name.c_str(), i_pTabPageName );
				}
			}
		}
	}
}
