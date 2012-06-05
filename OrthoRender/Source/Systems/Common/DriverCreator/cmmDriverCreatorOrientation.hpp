/*****************************************************************************
**	cmmDriverCreatorOrientation.hpp
**
**	Creates common drivers and parsers for Orientation channels
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_DRIVERCREATORORIENTATION_HPP
#error cmmDriverCreatorOrientation.hpp multiply included
#endif
#define CMM_DRIVERCREATORORIENTATION_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


//============================================================================
//============================================================================
class pick3dPickObject;
class tmlnChannelOrientation;
class tmlnDriver;
class tmlnDriverCreator;
class tmlnDriverInfo;
class tmlnDriverNameList;


//============================================================================
//============================================================================
namespace cmmDriverCreatorOrientation
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
	chDefs::Name GetAttachChunkName(const char i_Code[2]);
	chDefs::Name GetSplineChunkName(const char i_Code[2]);
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
	tmlnDriver* CreateKeyForChannel( tmlnChannelOrientation& io_Channel,
									 const char* i_ChannelName,
									 const char i_Code[2],
									 bool i_bShowIcons);


	//--------------------------------------------------------------------
	// Create driver for this channel based on the name used in
	//	GatherPossibleDrivers and attach it to the channel.
	//--------------------------------------------------------------------
	tmlnDriver* CreateDriverByName(  const char* i_DriverName,
									 tmlnChannelOrientation& io_Channel,
									 const char* i_ChannelName,
									 const char i_Code[2],
									 bool i_bShowIcons );

	//--------------------------------------------------------------------
	// Create driver for this channel based on the the info structure.
	// Note, the chunk name normally comes from the i_Info, but needs
	// to be passed in here in order to support older file formats.
	//--------------------------------------------------------------------
	tmlnDriver* CreateDriverFromInfo( tmlnChannelOrientation& io_Channel,
									  const char i_Code[2],
									  const tmlnDriverInfo& i_Info,
									  chDefs::Name i_ChunkName );
};
