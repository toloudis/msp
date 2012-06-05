/*****************************************************************************
**	xtraDriverCreator.cpp
**
**	Handles parsing of animation drivers on control animations
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/xtra/xtraDriverCreator.hpp"

#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFilePath.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnChannelTextureFileName.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Support/xtra/xtraScriptObject.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorBoolean.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorColor.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorOrientation.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorPosition.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorTexture.hpp"


//============================================================================
//============================================================================
namespace
{
	//========================================================================
	// Chunk name for representing general parsers, not limited to scope of a system
	//========================================================================
	const chDefs::Name c_XXXX = chDefs::MakeName('X', 'X', 'X', 'X');

	//========================================================================
	//========================================================================
	const char c_CustomBooleanChannelCode[2] = { 'X', 'B' };  
	const char c_CustomColorChannelCode[2] = { 'X', 'C' };  
	const char c_CustomFloatChannelCode[2] = { 'X', 'F' };  
	const char c_CustomOrientationChannelCode[2] = { 'X', 'O' };  
	const char c_CustomPositionChannelCode[2] = { 'X', 'P' };  
	const char c_CustomTextureChannelCode[2] = { 'X', 'T' };  
	const char c_CustomTextureFileChannelCode[2] = { 'X', 'E' }; //tExture file control  
	

	//--------------------------------------------------------------------
	// Internal function for keying an individual channel
	//--------------------------------------------------------------------
	tmlnDriver* create_key_for_channel( tmlnChannel* i_pChannel )
	{
		const bool bShowIcons = false; //bga - how do we get this per system?

		tmlnChannelBoolean* pEnableChannel = dynamic_cast<tmlnChannelBoolean*>(i_pChannel);
		if (pEnableChannel)
		{
			tmlnDriver *pDriver = cmmDriverCreatorBoolean::CreateKeyForChannel(
				*pEnableChannel, i_pChannel->GetName().c_str(), c_CustomBooleanChannelCode, bShowIcons);
			if (pDriver)
				pDriver->SetChannelName( i_pChannel->GetName().c_str() );
			return pDriver;
		}

		tmlnChannelColor* pColorChannel = dynamic_cast<tmlnChannelColor*>(i_pChannel);
		if (pColorChannel)
		{
			tmlnDriver *pDriver = cmmDriverCreatorColor::CreateKeyForChannel(
				*pColorChannel, i_pChannel->GetName().c_str(), c_CustomColorChannelCode, bShowIcons);
			if (pDriver)
				pDriver->SetChannelName( i_pChannel->GetName().c_str() );
			return pDriver;
		}

		tmlnChannelFloat* pFloatChannel = dynamic_cast<tmlnChannelFloat*>(i_pChannel);
		if (pFloatChannel)
		{
			tmlnDriver *pDriver = cmmDriverCreatorFloat::CreateKeyForChannel(
				*pFloatChannel, i_pChannel->GetName().c_str(), c_CustomFloatChannelCode, bShowIcons);
			if (pDriver)
				pDriver->SetChannelName( i_pChannel->GetName().c_str() );
			return pDriver;
		}

		tmlnChannelOrientation* pOrientationChannel = dynamic_cast<tmlnChannelOrientation*>(i_pChannel);
		if (pOrientationChannel)
		{
			tmlnDriver *pDriver = cmmDriverCreatorOrientation::CreateKeyForChannel(
				*pOrientationChannel, i_pChannel->GetName().c_str(), c_CustomOrientationChannelCode, bShowIcons);
			if (pDriver)
				pDriver->SetChannelName( i_pChannel->GetName().c_str() );
			return pDriver;
		}

		tmlnChannelPosition* pPositionChannel = dynamic_cast<tmlnChannelPosition*>(i_pChannel);
		if (pPositionChannel)
		{
			tmlnDriver *pDriver = cmmDriverCreatorPosition::CreateKeyForChannel(
				*pPositionChannel, i_pChannel->GetName().c_str(), c_CustomPositionChannelCode, bShowIcons);
			if (pDriver)
				pDriver->SetChannelName( i_pChannel->GetName().c_str() );
			return pDriver;
		}

		tmlnChannelFilePath* pTextureChannel = dynamic_cast<tmlnChannelFilePath*>(i_pChannel);
		if (pTextureChannel)
		{
			tmlnDriver *pDriver = cmmDriverCreatorTexture::CreateKeyForChannel(
				*pTextureChannel, i_pChannel->GetName().c_str(), c_CustomTextureChannelCode, bShowIcons);
			if (pDriver)
				pDriver->SetChannelName( i_pChannel->GetName().c_str() );
			return pDriver;
		}
		tmlnChannelTextureFileName* pTextureFileChannel = dynamic_cast<tmlnChannelTextureFileName*>(i_pChannel);
		if (pTextureFileChannel)
		{
			tmlnDriver *pDriver = cmmDriverCreatorTexture::CreateKeyForChannel(
				*pTextureFileChannel, i_pChannel->GetName().c_str(), c_CustomTextureFileChannelCode, bShowIcons);
			if (pDriver)
				pDriver->SetChannelName( i_pChannel->GetName().c_str() );
			return pDriver;
		}

		return 0;
	}

}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void xtraDriverCreator::CreateParsers()
{
	cmmDriverCreatorBoolean::CreateParsers(c_XXXX, c_CustomBooleanChannelCode); 
	cmmDriverCreatorColor::CreateParsers(c_XXXX, c_CustomColorChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(c_XXXX, c_CustomFloatChannelCode); 
	cmmDriverCreatorOrientation::CreateParsers(c_XXXX, c_CustomOrientationChannelCode); 
	cmmDriverCreatorPosition::CreateParsers(c_XXXX, c_CustomPositionChannelCode); 
	cmmDriverCreatorTexture::CreateParsers(c_XXXX, c_CustomTextureChannelCode); 
	cmmDriverCreatorTexture::CreateParsers(c_XXXX, c_CustomTextureFileChannelCode); 
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void xtraDriverCreator::GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers)
{
	const xtraScriptObject* xtra_obj = dynamic_cast<const xtraScriptObject*>(i_pObject);
	if (xtra_obj)
	{
		const int num_attrs = xtra_obj->GetNumCustomProperties();
		for (int i=0; i<num_attrs; ++i)
		{
			const tmlnChannel *pChannel = xtra_obj->GetCustomChannel(i);
			if (!pChannel) continue;

			const tmlnChannelBoolean* pBooleanChannel = dynamic_cast<const tmlnChannelBoolean*>(pChannel);
			if (pBooleanChannel)
			{		
				cmmDriverCreatorBoolean::GatherPossibleDrivers(pChannel->GetName().c_str(),
					io_Drivers, "Extras", this); 
				continue;
			}
			const tmlnChannelColor* pColorChannel = dynamic_cast<const tmlnChannelColor*>(pChannel);
			if (pColorChannel)
			{		
				cmmDriverCreatorColor::GatherPossibleDrivers(pChannel->GetName().c_str(),
					io_Drivers, "Extras", this); 
				continue;
			}
			const tmlnChannelFloat* pFloatChannel = dynamic_cast<const tmlnChannelFloat*>(pChannel);
			if (pFloatChannel)
			{		
				cmmDriverCreatorFloat::GatherPossibleDrivers(pChannel->GetName().c_str(),
					io_Drivers, "Extras", this); 
				continue;
			}
			const tmlnChannelOrientation* pOrientationChannel = dynamic_cast<const tmlnChannelOrientation*>(pChannel);
			if (pOrientationChannel)
			{		
				cmmDriverCreatorOrientation::GatherPossibleDrivers(pChannel->GetName().c_str(),
					io_Drivers, "Extras", this); 
				continue;
			}
			const tmlnChannelPosition* pPositionChannel = dynamic_cast<const tmlnChannelPosition*>(pChannel);
			if (pPositionChannel)
			{		
				cmmDriverCreatorPosition::GatherPossibleDrivers(pChannel->GetName().c_str(),
					io_Drivers, "Extras", this); 
				continue;
			}
			const tmlnChannelFilePath* pTextureChannel = dynamic_cast<const tmlnChannelFilePath*>(pChannel);
			if (pTextureChannel)
			{		
				cmmDriverCreatorTexture::GatherPossibleDrivers(pChannel->GetName().c_str(),
					io_Drivers, "Extras", this); 
				continue;
			}
			const tmlnChannelTextureFileName* pTextureFileChannel = dynamic_cast<const tmlnChannelTextureFileName*>(pChannel);
			if (pTextureFileChannel)
			{		
				cmmDriverCreatorTexture::GatherPossibleDrivers(pChannel->GetName().c_str(),
					io_Drivers, "Extras", this); 
				continue;
			}
		}
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* xtraDriverCreator::CreateDriverByName(	const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	xtraScriptObject* xtra_obj = dynamic_cast<xtraScriptObject*>(i_pObject);
	if (xtra_obj)
	{
		tmlnDriver* pDriver = NULL;
		const bool bShowIcons = false; //bga - how do we get this per system?

		const int num_attrs = xtra_obj->GetNumCustomProperties();
		for (int i=0; i<num_attrs; ++i)
		{
			tmlnChannel *pChannel = xtra_obj->GetCustomChannel(i);
			if (!pChannel) continue;

			tmlnChannelBoolean* pBooleanChannel = dynamic_cast<tmlnChannelBoolean*>(pChannel);
			if (pBooleanChannel)
			{		
				pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
					*pBooleanChannel, pChannel->GetName().c_str(), c_CustomBooleanChannelCode, bShowIcons );
				if (pDriver != NULL) 
				{
					pDriver->SetChannelName( pChannel->GetName().c_str() );
					return pDriver;
				}
			}
			tmlnChannelColor* pColorChannel = dynamic_cast<tmlnChannelColor*>(pChannel);
			if (pColorChannel)
			{		
				pDriver = cmmDriverCreatorColor::CreateDriverByName(i_DriverName, 
					*pColorChannel, pChannel->GetName().c_str(), c_CustomColorChannelCode, bShowIcons );
				if (pDriver != NULL) 
				{
					pDriver->SetChannelName( pChannel->GetName().c_str() );
					return pDriver;
				}
			}
			tmlnChannelFloat* pFloatChannel = dynamic_cast<tmlnChannelFloat*>(pChannel);
			if (pFloatChannel)
			{		
				pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
					*pFloatChannel, pChannel->GetName().c_str(), c_CustomFloatChannelCode, bShowIcons );
				if (pDriver != NULL) 
				{
					pDriver->SetChannelName( pChannel->GetName().c_str() );
					return pDriver;
				}
			}
			tmlnChannelOrientation* pOrientationChannel = dynamic_cast<tmlnChannelOrientation*>(pChannel);
			if (pOrientationChannel)
			{		
				pDriver = cmmDriverCreatorOrientation::CreateDriverByName(i_DriverName, 
					*pOrientationChannel, pChannel->GetName().c_str(), c_CustomOrientationChannelCode, bShowIcons );
				if (pDriver != NULL) 
				{
					pDriver->SetChannelName( pChannel->GetName().c_str() );
					return pDriver;
				}
			}
			tmlnChannelPosition* pPositionChannel = dynamic_cast<tmlnChannelPosition*>(pChannel);
			if (pPositionChannel)
			{		
				pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
					*pPositionChannel, pChannel->GetName().c_str(), c_CustomPositionChannelCode, bShowIcons );
				if (pDriver != NULL) 
				{
					pDriver->SetChannelName( pChannel->GetName().c_str() );
					return pDriver;
				}
			}
			tmlnChannelFilePath* pTextureChannel = dynamic_cast<tmlnChannelFilePath*>(pChannel);
			if (pTextureChannel)
			{		
				pDriver = cmmDriverCreatorTexture::CreateDriverByName(i_DriverName, 
					*pTextureChannel, pChannel->GetName().c_str(), c_CustomTextureChannelCode, bShowIcons );
				if (pDriver != NULL) 
				{
					pDriver->SetChannelName( pChannel->GetName().c_str() );
					return pDriver;
				}
			}
			tmlnChannelTextureFileName* pTextureFileChannel = dynamic_cast<tmlnChannelTextureFileName*>(pChannel);
			if (pTextureFileChannel)
			{		
				pDriver = cmmDriverCreatorTexture::CreateDriverByName(i_DriverName, 
					*pTextureFileChannel, pChannel->GetName().c_str(), c_CustomTextureFileChannelCode, bShowIcons );
				if (pDriver != NULL) 
				{
					pDriver->SetChannelName( pChannel->GetName().c_str() );
					return pDriver;
				}
			}
		}
	}

	return 0;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* xtraDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												   tmlnChannel* i_pChannel)
{
	xtraScriptObject* xtra_obj = dynamic_cast<xtraScriptObject*>(io_pObject);
	if (!xtra_obj) return NULL;


	std::string channel_name = i_pChannel->GetName();
	int xtra_prty_index = xtra_obj->GetIndexForName(channel_name);
	if (xtra_prty_index >= 0)
	{
		// We know now that this channel belongs to a material shader object.
		// It isn't really important to know which shader or property though.
		//DBG_LOG2("Looking to key %s on material %s", property_name.c_str(), material_name.c_str());

		return create_key_for_channel(i_pChannel);
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* xtraDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	xtraScriptObject* xtra_obj = dynamic_cast<xtraScriptObject*>(io_pObject);
	if (!xtra_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();
	std::string channel_name = i_Info.m_ChannelName;

	const int num_attrs = xtra_obj->GetNumCustomProperties();
	for (int i=0; i<num_attrs; ++i)
	{
		tmlnChannel *pChannel = xtra_obj->GetCustomChannel(i);
		if (!pChannel) continue;

		if (pChannel->GetName() == channel_name)
		{
			tmlnChannelBoolean* pBooleanChannel = dynamic_cast<tmlnChannelBoolean*>(pChannel);
			if (pBooleanChannel)
			{		
				tmlnDriver *pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
					*pBooleanChannel, c_CustomBooleanChannelCode, i_Info, name );
				if (pDriver != NULL) 
					return pDriver;
			}
			tmlnChannelColor* pColorChannel = dynamic_cast<tmlnChannelColor*>(pChannel);
			if (pColorChannel)
			{		
				tmlnDriver *pDriver = cmmDriverCreatorColor::CreateDriverFromInfo(
					*pColorChannel, c_CustomColorChannelCode, i_Info, name );
				if (pDriver != NULL) 
					return pDriver;
			}
			tmlnChannelFloat* pFloatChannel = dynamic_cast<tmlnChannelFloat*>(pChannel);
			if (pFloatChannel)
			{		
				tmlnDriver *pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
					*pFloatChannel, c_CustomFloatChannelCode, i_Info, name );
				if (pDriver != NULL) 
					return pDriver;
			}
			tmlnChannelOrientation* pOrientationChannel = dynamic_cast<tmlnChannelOrientation*>(pChannel);
			if (pOrientationChannel)
			{		
				tmlnDriver *pDriver = cmmDriverCreatorOrientation::CreateDriverFromInfo(
					*pOrientationChannel, c_CustomOrientationChannelCode, i_Info, name );
				if (pDriver != NULL) 
					return pDriver;
			}
			tmlnChannelPosition* pPositionChannel = dynamic_cast<tmlnChannelPosition*>(pChannel);
			if (pPositionChannel)
			{		
				tmlnDriver *pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
					*pPositionChannel, c_CustomPositionChannelCode, i_Info, name );
				if (pDriver != NULL) 
					return pDriver;
			}
			tmlnChannelFilePath* pTextureChannel = dynamic_cast<tmlnChannelFilePath*>(pChannel);
			if (pTextureChannel)
			{		
				tmlnDriver *pDriver = cmmDriverCreatorTexture::CreateDriverFromInfo(
					*pTextureChannel, c_CustomTextureChannelCode, i_Info, name );
				if (pDriver != NULL) 
					return pDriver;
			}
			tmlnChannelTextureFileName* pTextureFileChannel = dynamic_cast<tmlnChannelTextureFileName*>(pChannel);
			if (pTextureFileChannel)
			{		
				tmlnDriver *pDriver = cmmDriverCreatorTexture::CreateDriverFromInfo(
					*pTextureFileChannel, c_CustomTextureFileChannelCode, i_Info, name );
				if (pDriver != NULL) 
					return pDriver;
			}

			DBG_WARNING("Found channel by name, but could not create driver from data");
			break;
		}
	}

	return NULL;
}
