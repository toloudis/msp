/********************************************************************************************\
**  tmlnChannelInfoParser.hpp
**
**  reads and writes common Channel information, like begin time,
**		end time and name
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef TMLN_CHANNElINFOPARSER_HPP
#error tmlnChannelInfoParser.hpp multiply included
#endif
#define TMLN_CHANNElINFOPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <string>
#include <vector>
#include <map>


//============================================================================
//============================================================================
class tmlnChannelInfo;
class chReader;
class chWriter;
class fsLocator;


//============================================================================
//============================================================================
namespace tmlnChannelInfoParser
{
	//------------------------------------------------------------------------
	// Chunk name used for Channel base information
	//------------------------------------------------------------------------
	chDefs::Name GetChunkName();

	//------------------------------------------------------------------------
	//   ReadChannels 
	//------------------------------------------------------------------------
	void ReadChannels(	chReader& i_Reader,
					std::map<std::string, tmlnChannelInfo> &o_Channels,
					const char* i_IndexedStringNames[] = NULL,
					const int i_NumIndexedStringNames = 0);

	//------------------------------------------------------------------------
	//   WriteChannels
	//------------------------------------------------------------------------
	void WriteChannels(	chWriter& o_Writer,
		const std::map<std::string, tmlnChannelInfo> &i_Channels );
}

