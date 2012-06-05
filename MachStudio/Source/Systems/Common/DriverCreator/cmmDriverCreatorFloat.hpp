/*****************************************************************************
**	cmmDriverCreatorFloat.hpp
**
**	Creates common drivers and parsers for float channels
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_DRIVERCREATORFLOAT_HPP
#error cmmDriverCreatorFloat.hpp multiply included
#endif
#define CMM_DRIVERCREATORFLOAT_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

//============================================================================
//============================================================================
class tmlnChannelFloat;
class tmlnDriver;
class tmlnDriverCreator;
class tmlnDriverInfo;
class tmlnDriverNameList;

//============================================================================
//============================================================================
namespace cmmDriverCreatorFloat
{
	//--------------------------------------------------------------------
	// Create parsers for driver types created by this creator.
	// Supply two characters to uniquely define the system and channel
	//	for creating chunk names for the parsers.
	//--------------------------------------------------------------------
	void CreateParsers(chDefs::Name i_SystemCode,
						const char i_Code[2]);

	//--------------------------------------------------------------------
	// Helper function for finding what the chunk name will be
	//--------------------------------------------------------------------
	chDefs::Name GetKeyChunkName(const char i_Code[2]);
	chDefs::Name GetConnectChannelChunkName(const char i_Code[2]);

	//--------------------------------------------------------------------
	// Gather the possible types of drivers that can be created for
	//	for this channel.  Driver names are added through the
	//	DriverNameList's API
	//--------------------------------------------------------------------
	void GatherPossibleDrivers(const char* i_ChannelName,
							   tmlnDriverNameList& io_Drivers,
							   const char *i_DefaultCategory,
							   tmlnDriverCreator* i_Creator);

	//--------------------------------------------------------------------
	// Create static key driver for the given channel
	//--------------------------------------------------------------------
	tmlnDriver* CreateKeyForChannel( tmlnChannelFloat& io_Channel,
									 const char* i_ChannelName,
									 const char i_Code[2],
									 bool i_bShowIcons = false);

	//--------------------------------------------------------------------
	// Create driver for this channel based on the name used in
	//	GatherPossibleDrivers and attach it to the channel.
	//--------------------------------------------------------------------
	tmlnDriver* CreateDriverByName(  const char* i_DriverName,
									 tmlnChannelFloat& io_Channel,
									 const char* i_ChannelName,
									 const char i_Code[2],
									 bool i_bShowIcons = false );

	//--------------------------------------------------------------------
	// Create driver for this channel based on the the info structure.
	// Note, the chunk name normally comes from the i_Info, but needs
	// to be passed in here in order to support older file formats.
	//--------------------------------------------------------------------
	tmlnDriver* CreateDriverFromInfo( tmlnChannelFloat& io_Channel,
									  const char i_Code[2],
									  const tmlnDriverInfo& i_Info,
									  chDefs::Name i_ChunkName);
};
