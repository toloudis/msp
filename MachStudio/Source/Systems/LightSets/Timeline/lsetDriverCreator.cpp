/*****************************************************************************
**	lsetDriverCreator.cpp
**
**	Creates timeline channels for point lights
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Timeline/lsetDriverCreator.hpp"

#include "Systems/LightSets/Data/lsetLightSetsDataParser.hpp"
#include "Systems/LightSets/Object/lsetScriptObject.hpp"
#include "Systems/LightSets/Object/lsetObjectMgr.hpp"

#include "Systems/Common/DriverCreator/cmmDriverCreatorColor.hpp"

#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"


namespace
{
	//========================================================================
	//========================================================================
	const char *c_AmbientLightChannelName = "Ambient Light";
	const char c_AmbientLightChannelCode[2] = { 'A', 'L' };  // Ambient Light channel code

}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void lsetDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = lsetLightSetsDataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorColor::CreateParsers(sys_code, c_AmbientLightChannelCode); 

}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void lsetDriverCreator::GatherPossibleDrivers(	const tmlnScriptObject* i_pObject,
												tmlnDriverNameList& io_Drivers)
{
	if (dynamic_cast<const lsetScriptObject*>(i_pObject))
	{
//		cmmDriverCreatorColor::GatherPossibleDrivers(c_AmbientLightChannelName,
//			io_Drivers, "Parameters", this); 
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* lsetDriverCreator::CreateDriverByName(	const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	lsetScriptObject* lset_obj = dynamic_cast<lsetScriptObject*>(i_pObject);
	if (!lset_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Ambient Light Channel
//	pDriver = cmmDriverCreatorColor::CreateDriverByName(i_DriverName, 
//		lset_obj->AmbientLightChannel(), c_AmbientLightChannelName, c_AmbientLightChannelCode );
//	if (pDriver != NULL) 
//		return pDriver;

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* lsetDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	lsetScriptObject* lset_obj = dynamic_cast<lsetScriptObject*>(io_pObject);
	if (!lset_obj) return NULL;

//	if (i_pChannel == &lset_obj->AmbientLightChannel())
//	{
//		// Ambient Light Channel
//		return cmmDriverCreatorColor::CreateKeyForChannel(
//			lset_obj->AmbientLightChannel(), c_AmbientLightChannelName, c_AmbientLightChannelCode );
//	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* lsetDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	lsetScriptObject* lset_obj = dynamic_cast<lsetScriptObject*>(io_pObject);
	if (!lset_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Ambient Light channel
//	pDriver = cmmDriverCreatorColor::CreateDriverFromInfo(
//		lset_obj->AmbientLightChannel(), c_AmbientLightChannelCode, i_Info, name );
//	if (pDriver != NULL) 
//		return pDriver;

	return NULL;
}
