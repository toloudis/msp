/********************************************************************************************\
**  prtclDataParser.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef PRTCL_DATAPARSER_HPP
#error prtclDataParser.hpp multiply included
#endif
#define PRTCL_DATAPARSER_HPP

#ifndef PRTCL_SCRIPTDATA_HPP
#include "Systems/Particles/Data/prtclScriptData.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class chReader;
class chWriter;
class fsLocator;


//============================================================================
//============================================================================
class prtclDataParser
{
public:
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	static chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadParticleChunk
	//------------------------------------------------------------------------
	static void ReadParticleChunk(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						prtclScriptData& o_Data );

	//------------------------------------------------------------------------
	//   WriteParticleChunk
	//------------------------------------------------------------------------
	static void WriteParticleChunk(	chWriter& o_Writer,
						const prtclScriptData& i_Data );

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	static void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					prtclParticlesData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	static void WriteData(	chWriter& o_Writer,
					const prtclParticlesData& i_Data );



};


