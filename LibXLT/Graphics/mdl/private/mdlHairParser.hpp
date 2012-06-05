/****************************************************************************\
**  mdlHairParser.hpp
**
**      mdlHairParser provides functions to read/write hairinfo chunks from 
**		a binary file
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_HAIRPARSER_HPP
#error mdlHairParser.hpp multiply included
#endif
#define MDL_HAIRPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class chReader;
class chWriter;
struct mdlHairInfo;


//============================================================================
//============================================================================
namespace mdlHairParser
{
	//----------------------------------------------------------------------------
	//	Read HairInfo chunk from a binary file
	//	Returns true on success, false if the chunk need be skipped
	//	On error, throws an exception
	//----------------------------------------------------------------------------
	//Read the HairInfo chunk
	bool ReadHRFO(
		chReader& i_Reader,
		chDefs::Version i_Version,
		chDefs::Size i_Size,
		mdlHairInfo& o_HairInfo,
		mdlMatInfoTable *i_MaterialTable
		);

	//----------------------------------------------------------------------------
	//	Write HairInfo chunk to a binary file.
	//----------------------------------------------------------------------------
	void WriteHRFO( 
		chWriter& o_Writer, 
		const mdlHairInfo& i_HairInfo 
		);

}//namespace mdlHairParser
