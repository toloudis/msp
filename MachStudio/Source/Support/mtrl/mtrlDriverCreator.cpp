/*****************************************************************************
**	mtrlDriverCreator.cpp
**
**	Handles parsing of animation drivers on material properties
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/mtrlDriverCreator.hpp"

#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"

#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFilePath.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnChannelTextureFileName.hpp"
#include "Drivers/Enable/tmlnDriverEnable.hpp"
#include "Drivers/Enable/tmlnDriverEnableInfo.hpp"
#include "Drivers/Enable/tmlnDriverEnableParser.hpp"
#include "Drivers/Color/tmlnDriverColor.hpp"
#include "Drivers/Color/tmlnDriverColorInfo.hpp"
#include "Drivers/Color/tmlnDriverColorParser.hpp"
#include "Drivers/FilePath/tmlnDriverAnimatedFilePath.hpp"
#include "Drivers/FilePath/tmlnDriverAnimatedFilePathInfo.hpp"
#include "Drivers/FilePath/tmlnDriverAnimatedFilePathParser.hpp"
#include "Drivers/FilePath/tmlnDriverFilePath.hpp"
#include "Drivers/FilePath/tmlnDriverFilePathInfo.hpp"
#include "Drivers/FilePath/tmlnDriverFilePathParser.hpp"
#include "Drivers/Float/tmlnDriverFloat.hpp"
#include "Drivers/Float/tmlnDriverFloatInfo.hpp"
#include "Drivers/Float/tmlnDriverFloatParser.hpp"
#include "Drivers/TextureFileName/tmlnDriverTextureFileName.hpp"
#include "Drivers/TextureFileName/tmlnDriverTextureFileNameInfo.hpp"
#include "Drivers/TextureFileName/tmlnDriverTextureFileNameParser.hpp"
#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileName.hpp"
#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileNameInfo.hpp"
#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileNameParser.hpp"
#include "Drivers/Position/tmlnDriverPosition.hpp"
#include "Drivers/Position/tmlnDriverPositionInfo.hpp"
#include "Drivers/Position/tmlnDriverPositionParser.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"

#include "Core/Fs/fsAbsolutePathMgr.hpp"

#include <algorithm>
#include <iterator>

namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_MTFK = chDefs::MakeName('M', 'T', 'F', 'K');  // MaTerial Float Key
	const chDefs::Name c_MTCK = chDefs::MakeName('M', 'T', 'C', 'K');  // MaTerial Color Key
	//const chDefs::Name c_MTVK = chDefs::MakeName('M', 'T', 'V', 'K');  // MaTerial Vector3 Key
	const chDefs::Name c_MTBK = chDefs::MakeName('M', 'T', 'B', 'K');  // MaTerial Boolean Key
	const chDefs::Name c_MTXK = chDefs::MakeName('M', 'T', 'X', 'K');  // MaTerial teXture (FilePath) Key
	const chDefs::Name c_MTXA = chDefs::MakeName('M', 'T', 'X', 'A');  // MaTerial teXture (FilePath) Animation
	const chDefs::Name c_MTCY = chDefs::MakeName('M', 'T', 'C', 'Y');  // MaTerial teture Control (TextureFileName) keY
	const chDefs::Name c_MTCA = chDefs::MakeName('M', 'T', 'C', 'A');  // MaTerial teture Control (TextureFileName) Animation
	const chDefs::Name c_MTVK = chDefs::MakeName('M', 'T', 'V', 'K');  // MaTerial Vector (position) Key

	tmlnChannel* get_channel_by_name(const std::string &i_Name,
									 tmlnScriptObject* i_pObject)
	{
		tmlnChannelSet &channel_set = i_pObject->ChannelSet();
		const int num_channels = channel_set.GetNumChannels();
		for (int i=0; i<num_channels; i++)
		{
			if (i_Name == channel_set.GetChannel(i).GetName())
				return &channel_set.Channel(i);
		}
		return NULL;
	}
	
	//--------------------------------------------------------------------
	// Internal function for keying an individual material channel
	//--------------------------------------------------------------------
	tmlnDriver* create_key_for_channel( tmlnChannel* i_pChannel )
	{
		tmlnChannelFloat* pFloatChannel = dynamic_cast<tmlnChannelFloat*>(i_pChannel);
		if (pFloatChannel)
		{
			tmlnDriverFloat * pDriver = new tmlnDriverFloat( *pFloatChannel, c_MTFK );
			pDriver->SetName(i_pChannel->GetName().c_str());
			pDriver->SetInitialTime( 0.0f ); // short key
			i_pChannel->AddDriver( pDriver );
			return pDriver;
		}

		tmlnChannelColor* pColorChannel = dynamic_cast<tmlnChannelColor*>(i_pChannel);
		if (pColorChannel)
		{
			tmlnDriverColor * pDriver = new tmlnDriverColor( *pColorChannel, c_MTCK );
			pDriver->SetName(i_pChannel->GetName().c_str());
			pDriver->SetInitialTime( 0.0f ); // short key
			i_pChannel->AddDriver( pDriver );
			return pDriver;
		}

		tmlnChannelBoolean* pEnableChannel = dynamic_cast<tmlnChannelBoolean*>(i_pChannel);
		if (pEnableChannel)
		{
			bool bState = pEnableChannel->GetState();
			tmlnDriverEnable * pDriver = new tmlnDriverEnable( *pEnableChannel, c_MTBK );
			pDriver->SetEnabled(bState);
			pDriver->SetName(i_pChannel->GetName().c_str());
			pDriver->SetInitialTime( 0.0f ); // short key
			i_pChannel->AddDriver( pDriver );
			return pDriver;
		}

		// Switched the "key" driver for textures to the texture animation driver,
		// which can handle a single texture key as a simple case, but can also be expanded
		// into a full texture anima by the user.
		tmlnChannelFilePath* pFilePathChannel = dynamic_cast<tmlnChannelFilePath*>(i_pChannel);
		if (pFilePathChannel)
		{
			//tmlnDriverFilePath * pDriver = new tmlnDriverFilePath( *pFilePathChannel, c_MTXK );
			tmlnDriverAnimatedFilePath * pDriver = new tmlnDriverAnimatedFilePath( *pFilePathChannel, c_MTXA );
			pDriver->SetName(i_pChannel->GetName().c_str());
			pDriver->SetInitialTime( 0.0f ); // short key
			i_pChannel->AddDriver( pDriver );
			return pDriver;
		}

		tmlnChannelTextureFileName* pTextureFileNameChannel = dynamic_cast<tmlnChannelTextureFileName*>(i_pChannel);
		if (pTextureFileNameChannel)
		{
			tmlnDriverAnimatedTextureFileName * pDriver = new tmlnDriverAnimatedTextureFileName( *pTextureFileNameChannel, c_MTCA );
			pDriver->SetName(i_pChannel->GetName().c_str());
			pDriver->SetInitialTime( 0.0f ); // short key
			i_pChannel->AddDriver( pDriver );
			return pDriver;
		}

		tmlnChannelPosition* pPositionChannel = dynamic_cast<tmlnChannelPosition*>(i_pChannel);
		if (pPositionChannel)
		{
			tmlnDriverPosition * pDriver = new tmlnDriverPosition( *pPositionChannel, c_MTVK );
			pDriver->SetName(i_pChannel->GetName().c_str());
			pDriver->SetInitialTime( 0.0f ); // short key
			i_pChannel->AddDriver( pDriver );
			return pDriver;
		}
		return 0;
	}

}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void mtrlDriverCreator::CreateParsers()
{
	tmlnParser::AddDriverParser(c_MTFK, new tmlnDriverFloatParser(c_MTFK));
	tmlnParser::AddDriverParser(c_MTCK, new tmlnDriverColorParser(c_MTCK));
	tmlnParser::AddDriverParser(c_MTVK, new tmlnDriverPositionParser(c_MTVK));
	tmlnParser::AddDriverParser(c_MTBK, new tmlnDriverEnableParser(c_MTBK));
	tmlnParser::AddDriverParser(c_MTXK, new tmlnDriverFilePathParser(c_MTXK));
	tmlnParser::AddDriverParser(c_MTXA, new tmlnDriverAnimatedFilePathParser(c_MTXA));
	tmlnParser::AddDriverParser(c_MTCY, new tmlnDriverTextureFileNameParser(c_MTCY));
	tmlnParser::AddDriverParser(c_MTCA, new tmlnDriverAnimatedTextureFileNameParser(c_MTCA));
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void mtrlDriverCreator::GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers)
{
	const mtrlScriptObject* pObject = dynamic_cast<const mtrlScriptObject*>(i_pObject);
	if (pObject)
	{
		const int num_mats = pObject->GetNumMaterials();
		for (int i=0; i<num_mats; i++)
		{
			std::string ctrl("Material ");
			ctrl += pObject->GetMaterialName(i);
			io_Drivers.AddDriver( ctrl.c_str(), "Material Animation", this);
		}
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* mtrlDriverCreator::CreateDriverByName( const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	mtrlScriptObject* mtrl_obj = dynamic_cast<mtrlScriptObject*>(i_pObject);
	if (!mtrl_obj) return NULL;

	if (!::_strnicmp(i_DriverName, "Material ", ::strlen("Material ")))
	{
		std::string material_name = i_DriverName + ::strlen("Material ");
		int index = mtrl_obj->GetIndexForName(material_name);

		if (index >= 0)
		{
			// Key all material channels at once
			std::vector<tmlnChannel*> channels_to_key;
			mtrlShaderObject *pShaderObject = mtrl_obj->GetShaderDataObject(index);
			std::copy(pShaderObject->Channels().begin(), pShaderObject->Channels().end(),
							std::back_inserter(channels_to_key));

			mtrlShaderObject *pUVTransformObject = mtrl_obj->GetUVTransformDataObject(index);
			std::copy(pUVTransformObject->Channels().begin(), pUVTransformObject->Channels().end(),
				std::back_inserter(channels_to_key));

			if (mtrl_obj->GetMaterialData(index).GetHasGlow())
			{
				mtrlShaderObject *pGlowObject = mtrl_obj->GetGlowDataObject(index);
				std::copy(pGlowObject->Channels().begin(), pGlowObject->Channels().end(),
								std::back_inserter(channels_to_key));
			}
			if (mtrl_obj->GetMaterialData(index).GetHasOutline())
			{
				mtrlShaderObject *pOutlineObject = mtrl_obj->GetOutlineDataObject(index);
				std::copy(pOutlineObject->Channels().begin(), pOutlineObject->Channels().end(),
					std::back_inserter(channels_to_key));
			}
			if (mtrl_obj->GetMaterialData(index).GetHasReflection())
			{
				mtrlShaderObject *pReflectionObject = mtrl_obj->GetReflectionDataObject(index);
				std::copy(pReflectionObject->Channels().begin(), pReflectionObject->Channels().end(),
					std::back_inserter(channels_to_key));
			}
			if (mtrl_obj->GetMaterialData(index).GetHasDisplacement())
			{
				mtrlShaderObject *pDisplacementObject = mtrl_obj->GetDisplacementDataObject(index);
				std::copy(pDisplacementObject->Channels().begin(), pDisplacementObject->Channels().end(),
					std::back_inserter(channels_to_key));
			}

			mtrlShaderObject *pNormalsObject = mtrl_obj->GetNormalsDataObject(index);
			std::copy(pNormalsObject->Channels().begin(), pNormalsObject->Channels().end(),
				std::back_inserter(channels_to_key));

			const int num_channels = channels_to_key.size();
			tmlnDriver* driver = NULL;
			for (int i=0; i<num_channels; i++)
			{
				// In order to handle multiple drivers created in this function,
				// we need to add all but the last one into the script object ourselves
				// and return jsut the last one unadded.
				if (driver) i_pObject->AddDriver(driver); 
				driver = create_key_for_channel(channels_to_key[i]);
			}
			return driver;
		}
	}
	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* mtrlDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	mtrlScriptObject* mtrl_obj = dynamic_cast<mtrlScriptObject*>(io_pObject);
	if (!mtrl_obj) return NULL;

	std::string channel_name = i_pChannel->GetName();
	std::string::iterator it = std::find(channel_name.begin(), channel_name.end(), '.');
	if (it != channel_name.end())
	{
		std::string material_name(channel_name.begin(), it);
		std::string property_name(it+1, channel_name.end());

		int mat_index = mtrl_obj->GetIndexForName(material_name);
		if (mat_index >= 0)
		{
			// We know now that this channel belongs to a material shader object.
			// It isn't really important to know which shader or property though.
			//DBG_LOG2("Looking to key %s on material %s", property_name.c_str(), material_name.c_str());

			return create_key_for_channel(i_pChannel);
		}
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* mtrlDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	mtrlScriptObject* mtrl_obj = dynamic_cast<mtrlScriptObject*>(io_pObject);
	if (!mtrl_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();
	if (name == c_MTFK)
	{
		const tmlnDriverFloatInfo& driver_info = dynamic_cast<const tmlnDriverFloatInfo&>(i_Info);
		tmlnChannel *channel = ::get_channel_by_name(i_Info.m_Name, io_pObject);
		//DBG_ASSERT(channel, "Couldn't find channel for driver");
		if (!channel) return NULL;

		tmlnChannelFloat *pFloatChannel = dynamic_cast<tmlnChannelFloat*>(channel);
		//DBG_ASSERT(channel, "Channel is incorrect type (expected float)");
		if (!pFloatChannel) return NULL;

		tmlnDriverFloat * pDS = new tmlnDriverFloat( *pFloatChannel, c_MTFK );
		pDS->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		channel->AddDriver( pDS );
		return pDS;
	}	
	else if (name == c_MTXA)
	{
		//// make copy of data in order to alter full filepath
		//tmlnDriverAnimatedFilePathInfo driver_info = dynamic_cast<const tmlnDriverAnimatedFilePathInfo&>(i_Info);
		//
		//// resolve absolute path
		//if (driver_info.m_FirstFilePath.GetNumNames() > 1)
		//	fsAbsolutePathMgr::ResolvePath(driver_info.m_FirstFilePath, "Textures");

		//tmlnChannel *channel = ::get_channel_by_name(i_Info.m_Name, io_pObject);
		////DBG_ASSERT(channel, "Couldn't find channel for driver");
		//if (!channel) return NULL;

		//tmlnChannelFilePath *pFilePathChannel = dynamic_cast<tmlnChannelFilePath*>(channel);
		////DBG_ASSERT(channel, "Channel is incorrect type (expected float)");
		//if (!pFilePathChannel) return NULL;

		//tmlnDriverAnimatedFilePath * pDS = new tmlnDriverAnimatedFilePath( *pFilePathChannel, c_MTXA );
		//pDS->SetDriverInfo(driver_info);

		//// Attach driver to the appropriate channels
		//channel->AddDriver( pDS );
		//return pDS;

		// make copy of data in order to alter full filepath
		tmlnDriverAnimatedFilePathInfo driver_info = dynamic_cast<const tmlnDriverAnimatedFilePathInfo&>(i_Info);
		
		// resolve absolute path
		if (driver_info.m_FirstFilePath.GetNumNames() > 1)
			fsAbsolutePathMgr::ResolvePath(driver_info.m_FirstFilePath, "Textures");

		tmlnChannel *channel = ::get_channel_by_name(i_Info.m_Name, io_pObject);
		//DBG_ASSERT(channel, "Couldn't find channel for driver");
		if (!channel) return NULL;

		tmlnChannelTextureFileName *pTextureFileNameChannel = dynamic_cast<tmlnChannelTextureFileName*>(channel);
		//DBG_ASSERT(channel, "Channel is incorrect type (expected float)");
		if (!pTextureFileNameChannel) return NULL;

		tmlnDriverAnimatedTextureFileName * pDS = new tmlnDriverAnimatedTextureFileName( *pTextureFileNameChannel, c_MTCA );
		pDS->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		channel->AddDriver( pDS );
		return pDS;
	}	
	else if (name == c_MTXK)
	{
		//// make copy of data in order to alter full filepath
		//tmlnDriverFilePathInfo driver_info = dynamic_cast<const tmlnDriverFilePathInfo&>(i_Info);
		//
		//// resolve absolute path
		//if (driver_info.m_Value.GetNumNames() > 1)
		//	fsAbsolutePathMgr::ResolvePath(driver_info.m_Value, "Textures");

		//tmlnChannel *channel = ::get_channel_by_name(i_Info.m_Name, io_pObject);
		////DBG_ASSERT(channel, "Couldn't find channel for driver");
		//if (!channel) return NULL;

		//tmlnChannelFilePath *pFilePathChannel = dynamic_cast<tmlnChannelFilePath*>(channel);
		////DBG_ASSERT(channel, "Channel is incorrect type (expected float)");
		//if (!pFilePathChannel) return NULL;

		//tmlnDriverFilePath * pDS = new tmlnDriverFilePath( *pFilePathChannel, c_MTXK );
		//pDS->SetDriverInfo(driver_info);

		//// Attach driver to the appropriate channels
		//channel->AddDriver( pDS );
		//return pDS;

		// make copy of data in order to alter full filepath
		tmlnDriverFilePathInfo driver_info = dynamic_cast<const tmlnDriverFilePathInfo&>(i_Info);
		
		// resolve absolute path
		if (driver_info.m_Value.GetNumNames() > 1)
			fsAbsolutePathMgr::ResolvePath(driver_info.m_Value, "Textures");

		tmlnChannel *channel = ::get_channel_by_name(i_Info.m_Name, io_pObject);
		//DBG_ASSERT(channel, "Couldn't find channel for driver");
		if (!channel) return NULL;

		tmlnChannelTextureFileName *pTextureFileNameChannel = dynamic_cast<tmlnChannelTextureFileName*>(channel);
		//DBG_ASSERT(channel, "Channel is incorrect type (expected float)");
		if (!pTextureFileNameChannel) return NULL;

		tmlnDriverTextureFileName * pDS = new tmlnDriverTextureFileName( *pTextureFileNameChannel, c_MTCY );
		pDS->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		channel->AddDriver( pDS );
		return pDS;
	}	
	else if (name == c_MTCA)
	{
		// make copy of data in order to alter full filepath
		tmlnDriverAnimatedTextureFileNameInfo driver_info = dynamic_cast<const tmlnDriverAnimatedTextureFileNameInfo&>(i_Info);
		
		// resolve absolute path
		if (driver_info.m_FirstTextureFileName.GetValue().GetNumNames() > 1)
		{
			prtyTextureFileData val = driver_info.m_FirstTextureFileName.GetFullValue();
			fsAbsolutePathMgr::ResolvePath(val.m_TextureLocator, "Textures");
		}

		tmlnChannel *channel = ::get_channel_by_name(i_Info.m_Name, io_pObject);
		//DBG_ASSERT(channel, "Couldn't find channel for driver");
		if (!channel) return NULL;

		tmlnChannelTextureFileName *pTextureFileNameChannel = dynamic_cast<tmlnChannelTextureFileName*>(channel);
		//DBG_ASSERT(channel, "Channel is incorrect type (expected float)");
		if (!pTextureFileNameChannel) return NULL;

		tmlnDriverAnimatedTextureFileName * pDS = new tmlnDriverAnimatedTextureFileName( *pTextureFileNameChannel, c_MTCA );
		pDS->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		channel->AddDriver( pDS );
		return pDS;
	}	
	else if (name == c_MTCY)
	{
		// make copy of data in order to alter full filepath
		tmlnDriverTextureFileNameInfo driver_info = dynamic_cast<const tmlnDriverTextureFileNameInfo&>(i_Info);
		
		// resolve absolute path
		if (driver_info.m_Value.GetValue().GetNumNames() > 1)
		{
			prtyTextureFileData val = driver_info.m_Value.GetFullValue();
			fsAbsolutePathMgr::ResolvePath(val.m_TextureLocator, "Textures");
		}

		tmlnChannel *channel = ::get_channel_by_name(i_Info.m_Name, io_pObject);
		//DBG_ASSERT(channel, "Couldn't find channel for driver");
		if (!channel) return NULL;

		tmlnChannelTextureFileName *pTextureFileNameChannel = dynamic_cast<tmlnChannelTextureFileName*>(channel);
		//DBG_ASSERT(channel, "Channel is incorrect type (expected float)");
		if (!pTextureFileNameChannel) return NULL;

		tmlnDriverTextureFileName * pDS = new tmlnDriverTextureFileName( *pTextureFileNameChannel, c_MTCY );
		pDS->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		channel->AddDriver( pDS );
		return pDS;
	}	
	else if (name == c_MTCK)
	{
		const tmlnDriverColorInfo& driver_info = dynamic_cast<const tmlnDriverColorInfo&>(i_Info);
		tmlnChannel *channel = ::get_channel_by_name(i_Info.m_Name, io_pObject);
		//DBG_ASSERT(channel, "Couldn't find channel for driver");
		if (!channel) return NULL;

		tmlnChannelColor *pColorChannel = dynamic_cast<tmlnChannelColor*>(channel);
		//DBG_ASSERT(channel, "Channel is incorrect type (expected color)");
		if (!pColorChannel) return NULL;

		tmlnDriverColor * pDS = new tmlnDriverColor( *pColorChannel, c_MTCK );
		pDS->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		channel->AddDriver( pDS );
		return pDS;
	}
	else if (name == c_MTVK)
	{
		const tmlnDriverPositionInfo& driver_info = dynamic_cast<const tmlnDriverPositionInfo&>(i_Info);
		tmlnChannel *channel = ::get_channel_by_name(i_Info.m_Name, io_pObject);
		//DBG_ASSERT(channel, "Couldn't find channel for driver");
		if (!channel) return NULL;

		tmlnChannelPosition *pPositionChannel = dynamic_cast<tmlnChannelPosition*>(channel);
		//DBG_ASSERT(channel, "Channel is incorrect type (expected vector3d)");
		if (!pPositionChannel) return NULL;

		tmlnDriverPosition * pDS = new tmlnDriverPosition( *pPositionChannel, c_MTVK );
		pDS->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		channel->AddDriver( pDS );
		return pDS;
	}
	else if (name == c_MTBK)
	{
		const tmlnDriverEnableInfo& driver_info = dynamic_cast<const tmlnDriverEnableInfo&>(i_Info);
		tmlnChannel *channel = ::get_channel_by_name(i_Info.m_Name, io_pObject);
		//DBG_ASSERT(channel, "Couldn't find channel for driver");
		if (!channel) return NULL;

		tmlnChannelBoolean *pEnableChannel = dynamic_cast<tmlnChannelBoolean*>(channel);
		//DBG_ASSERT(channel, "Channel is incorrect type (expected boolean)");
		if (!pEnableChannel) return NULL;

		tmlnDriverEnable * pDS = new tmlnDriverEnable( *pEnableChannel, c_MTBK );
		pDS->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		channel->AddDriver( pDS );
		return pDS;
	}

	return NULL;
}
