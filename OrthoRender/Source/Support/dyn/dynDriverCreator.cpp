/*****************************************************************************
**	dynDriverCreator.cpp
**
**	Handles parsing of animation drivers on control animations
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/dyn/dynDriverCreator.hpp"

//#include "Support/dyn/dynChannelRotateControl.hpp"
#include "Support/dyn/dynChannelControl.hpp"
#include "Support/dyn/dynScriptObject.hpp"

#include "Support/dyn/Drivers/dynDriverTransformKey.hpp"
#include "Support/dyn/Drivers/dynDriverTransformKeyInfo.hpp"
#include "Support/dyn/Drivers/dynDriverTransformKeyParser.hpp"
//#include "Record/Float/rcdDriverFloat.hpp"
//#include "Record/Float/rcdDriverFloatInfo.hpp"
//#include "Record/Float/rcdDriverFloatParser.hpp"
#include "Drivers/Float/tmlnDriverFloat.hpp"
#include "Drivers/Float/tmlnDriverFloatInfo.hpp"
#include "Drivers/Float/tmlnDriverFloatParser.hpp"

#include "Core/dbg/dbgLog.hpp"

namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_CXFK = chDefs::MakeName('C', 'X', 'F', 'K');  // Control XForm Key
//	const chDefs::Name c_CTRF = chDefs::MakeName('C', 'T', 'R', 'F');  // ConTRol Float driver
//	const chDefs::Name c_CTFR = chDefs::MakeName('C', 'T', 'F', 'R');  // ConTrol Float Recorddriver

	//dynChannelRotateControl* get_rotate_channel_by_name(dynScriptObject* i_pObject, const std::string& i_Name)
	//{
	//	int num_ctrls = i_pObject->GetNumControls();
	//	for (int i=0; i<num_ctrls; i++)
	//	{
	//		if (i_pObject->GetControlData(i).m_Name == i_Name)
	//			return dynamic_cast<dynChannelRotateControl*>(i_pObject->GetControl(i));
	//	}
	//	return NULL;
	//}
	dynChannelControl* get_transform_channel_by_name(dynScriptObject* i_pObject, const std::string& i_Name)
	{
		int num_ctrls = i_pObject->GetNumControls();
//		DBG_LOG1("get_transform_channel_by_name %s", i_Name.c_str()); 
		for (int i=0; i<num_ctrls; i++)
		{
//			DBG_LOG1("name %s", i_pObject->GetControlData(i).m_Name.c_str()); 
			if (i_pObject->GetControlData(i).m_Name == i_Name)
				return dynamic_cast<dynChannelControl*>(i_pObject->GetControl(i));
		}
		return NULL;
	}
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void dynDriverCreator::CreateParsers()
{
	tmlnParser::AddDriverParser(c_CXFK, new dynDriverTransformKeyParser(c_CXFK));
//	tmlnParser::AddDriverParser(c_CTRF, new tmlnDriverFloatParser(c_CTRF));
//	tmlnParser::AddDriverParser(c_CTFR, new rcdDriverFloatParser(c_CTFR));
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void dynDriverCreator::GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers)
{
	const dynScriptObject* pObject = dynamic_cast<const dynScriptObject*>(i_pObject);
	if (pObject)
	{
		int num_ctrls = pObject->GetNumControls();
		for (int i=0; i<num_ctrls; i++)
		{
			//if (pObject->GetControlData(i).m_ControlType == dynControlData::e_RotateControl)
			//{
			//	std::string ctrl("Rotate Control ");
			//	ctrl += pObject->GetControlData(i).m_Name;
			//	io_Drivers.AddDriver( ctrl.c_str(), "Control Animation", this);

			//	ctrl = std::string("Record ") + ctrl;
			//	io_Drivers.AddDriver( ctrl.c_str(), "Record Control Animation", this);
			//}
			//else
			//{
				std::string ctrl("Control ");
				ctrl += pObject->GetControlData(i).m_Name.GetValue();
				io_Drivers.AddDriver( ctrl.c_str(), "Control Animation", this);
			//}
		}
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* dynDriverCreator::CreateDriverByName(	const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	dynScriptObject* dyn_obj = dynamic_cast<dynScriptObject*>(i_pObject);
	if (!dyn_obj) return NULL;

	if (!::_strnicmp(i_DriverName, "Control ", ::strlen("Control ")))
	{
		std::string channel_name = i_DriverName + ::strlen("Control ");
		dynChannelControl *channel = get_transform_channel_by_name(dyn_obj, channel_name);
		if (channel == NULL)
		{
			DBG_WARNING2("Couldn't find channel %s for %s driver", channel_name.c_str(), i_DriverName);
			return NULL;
//			DBG_ASSERT2(channel, "Couldn't find channel %s for %s driver", channel_name.c_str(), i_DriverName);
		}

		dynDriverTransformKey * pDriver = new dynDriverTransformKey( *channel, c_CXFK );
		pDriver->SetName(channel_name.c_str());
		pDriver->SetInitialTime( 0.0f ); // short key

		// Attach driver to the appropriate channels
		channel->AddDriver( pDriver );
		return pDriver;
	}
//	else if (!::_strnicmp(i_DriverName, "Rotate Control ", ::strlen("Rotate Control ")))
//	{
//		std::string channel_name = i_DriverName + ::strlen("Rotate Control ");
//		dynChannelRotateControl *channel = get_rotate_channel_by_name(dyn_obj, channel_name);
//		if (channel == NULL)
//		{
//			DBG_WARNING2("Couldn't find channel %s for %s driver", channel_name.c_str(), i_DriverName);
//			return NULL;
////			DBG_ASSERT2(channel, "Couldn't find channel %s for %s driver", channel_name.c_str(), i_DriverName);
//		}
//
//		tmlnDriverFloat * pDriver = new tmlnDriverFloat( *channel, c_CTRF );
//		pDriver->SetName(channel_name.c_str());
//		pDriver->SetInitialTime( 0.0f ); // short key
//
//		// Attach driver to the appropriate channels
//		channel->AddDriver( pDriver );
//		return pDriver;
//	}
//	else if (!::_strnicmp(i_DriverName, "Record Control ", ::strlen("Record Control ")))
//	{
//		std::string channel_name = i_DriverName + ::strlen("Record Control ");
//		dynChannelRotateControl *channel = get_rotate_channel_by_name(dyn_obj, channel_name);
//		if (channel == NULL)
//		{
//			DBG_WARNING2("Couldn't find channel %s for %s driver", channel_name.c_str(), i_DriverName);
//			return NULL;
////			DBG_ASSERT2(channel, "Couldn't find channel %s for %s driver", channel_name.c_str(), i_DriverName);
//		}
//
//		rcdDriverFloat * pDriver = new rcdDriverFloat( *channel, c_CTFR );
//		//std::string record("Record ");
//		//record += channel_name;
//		//pDriver->SetName(record.c_str());
//		pDriver->SetName(channel_name.c_str());
//		pDriver->SetInitialTime();
//
//		// Attach driver to the appropriate channels
//		channel->AddDriver( pDriver );
//		return pDriver;
//	}
	return 0;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* dynDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												   tmlnChannel* i_pChannel)
{
	dynScriptObject* dyn_obj = dynamic_cast<dynScriptObject*>(io_pObject);
	if (!dyn_obj) return NULL;

	dynChannelControl* pTransformChannel = dynamic_cast<dynChannelControl*>(i_pChannel);
	if (pTransformChannel)
	{
		dynDriverTransformKey * pDriver = new dynDriverTransformKey( *pTransformChannel, c_CXFK );
		pDriver->SetName(i_pChannel->GetName().c_str());
		pDriver->SetInitialTime( 0.0f ); // short key
		i_pChannel->AddDriver( pDriver );
		return pDriver;
	}
	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* dynDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	dynScriptObject* dyn_obj = dynamic_cast<dynScriptObject*>(io_pObject);
	if (!dyn_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	if (name == c_CXFK)
	{
		const dynDriverTransformKeyInfo& driver_info = dynamic_cast<const dynDriverTransformKeyInfo&>(i_Info);
		dynChannelControl *channel = get_transform_channel_by_name(dyn_obj, i_Info.m_Name);
		if (channel == NULL)
		{
			DBG_WARNING1("Couldn't find channel %s for dynDriverTransformKey driver", i_Info.m_Name.c_str());
			return NULL;
//			DBG_ASSERT1(channel, "Couldn't find channel %s for dynDriverTransformKey driver", i_Info.m_Name.c_str());
		}

		dynDriverTransformKey * pDriver = new dynDriverTransformKey( *channel, c_CXFK );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		channel->AddDriver( pDriver );
		return pDriver;
	}
//	else if (name == c_CTRF)
//	{
//		const tmlnDriverFloatInfo& driver_info = dynamic_cast<const tmlnDriverFloatInfo&>(i_Info);
//		dynChannelRotateControl *channel = get_rotate_channel_by_name(dyn_obj, i_Info.m_Name);
//		if (channel == NULL)
//		{
//			DBG_WARNING1("Couldn't find channel %s for tmlnDriverFloat driver", i_Info.m_Name.c_str());
//			return NULL;
////			DBG_ASSERT1(channel, "Couldn't find channel %s for tmlnDriverFloat driver", i_Info.m_Name.c_str());
//		}
//
//		tmlnDriverFloat * pDriver = new tmlnDriverFloat( *channel, c_CTRF );
//		pDriver->SetDriverInfo(driver_info);
//
//		// Attach driver to the appropriate channels
//		channel->AddDriver( pDriver );
//		return pDriver;
//	}
//	else if (name == c_CTFR)
//	{
//		const rcdDriverFloatInfo& driver_info = dynamic_cast<const rcdDriverFloatInfo&>(i_Info);
//		dynChannelRotateControl *channel = get_rotate_channel_by_name(dyn_obj, i_Info.m_Name);
//		if (channel == NULL)
//		{
//			DBG_WARNING1("Couldn't find channel %s for rcdDriverFloat driver", i_Info.m_Name.c_str());
//			return NULL;
////			DBG_ASSERT1(channel, "Couldn't find channel %s for rcdDriverFloat driver", i_Info.m_Name.c_str());
//		}
//
//		rcdDriverFloat * pDriver = new rcdDriverFloat( *channel, c_CTFR );
//		pDriver->SetDriverInfo(driver_info);
//
//		// Attach driver to the appropriate channels
//		channel->AddDriver( pDriver );
//		return pDriver;
//	}

	return NULL;
}
