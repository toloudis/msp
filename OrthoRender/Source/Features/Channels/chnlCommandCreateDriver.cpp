/*****************************************************************************
**  chnlCommandCreateDriver.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlCommandCreateDriver.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"

#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"


///////////////////////////////////////////////////
// Event Handler(s)
//
namespace chnlCommandCreateDriverNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		chnlCommandCreateDriver* pCom = dynamic_cast<chnlCommandCreateDriver*>(pCmd);
		DBG_ASSERT0( pCom != NULL, "Invalid command hooked up to Create Driver command" );

		//
		//	create the driver
		//
		tmlnScriptObject* script_obj = tmlnSelectionUtil::GetSelectedScriptObject();
		if (!script_obj)
		{
			guiMessageBox::Show("Cannot create channels for an object of this type.","Error", guiMessageBox::e_OKOnly);
		}
		else
		{
			// get the drivers and find if the driver exists
			//
			tmlnDriverNameList driver_names;
			tmlnCreator::GatherPossibleDrivers(script_obj, driver_names);

			if (driver_names.Empty())
			{
				guiMessageBox::Show("No drivers available for an object of this type.","Error", guiMessageBox::e_OKOnly);
			}
			else
			{
				//	find the index
				//
				int i;
				for (i=0; i< driver_names.m_DriverNames.size(); i++)
				{
					if (_stricmp(driver_names.m_DriverNames[i].m_Name.c_str(),pCom->GetDriverName().c_str()) == 0)
					{
						break;
					}
				}

				if ( i >= driver_names.m_DriverNames.size() )
				{
					return;
				}

				tmlnDriverNameList::DriverName driver_info = driver_names.m_DriverNames[i];

				// Create Driver
				//
				tmlnDriver* pDriver = driver_info.m_Creator->CreateDriverByName( 
																pCom->GetDriverName().c_str(), script_obj );

				// Give driver to script object to own
				if (pDriver)
				{
					script_obj->AddDriver( pDriver );
					script_obj->NotifyDriverChanged();

					chnlDialogUtil::ObjectSelected(script_obj);

					//
					if (pDriver->IsAutoPopUpEditProperties())
						pDriver->DoEditProperties();
				}
			}
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string chnlCommandCreateDriver::GetConstTagName()
{
	return std::string("tmlnCreateDriver");
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
chnlCommandCreateDriver::chnlCommandCreateDriver()
: cmaCommand( std::string(GetConstTagName()),
			 chnlCommandCreateDriverNS::CommandExecuteHandler,
			 NULL )
{
	//this->SetTag( GetConstTagName() );

	this->SetDescription(std::string("Create a driver"));
	this->SetCategory(std::string("Drivers"));
	this->SetEnableHotKey(true);
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
chnlCommandCreateDriver::~chnlCommandCreateDriver()
{
}

//--------------------------------------------------------------------
//	driver name for creating the driver
//--------------------------------------------------------------------
void chnlCommandCreateDriver::SetDriverName( std::string i_DriverName )
{
	m_DriverName = i_DriverName;
}


//--------------------------------------------------------------------
//	driver name for creating the driver
//--------------------------------------------------------------------
std::string& chnlCommandCreateDriver::GetDriverName()
{
	return m_DriverName;
}
