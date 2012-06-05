/****************************************************************************\
**	mdlModelingPackageParser.hpp
**
**		A parser for the modeling package chunk.
**
**	This data contains what
**	package the exported data came from, what SDK was used, what version of
**	the exporter was used, and the date.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MODELINGPACKAGEPARSER_HPP
#error mdlModelingPackageParser.hpp multiply included
#endif
#define MDL_MODELINGPACKAGEPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class chReader;
struct mdlModelingPackageData;


//============================================================================
//	Any of these mdlModelingPackageParser functions might throw 
//	one of the fs exceptions.
//============================================================================
namespace mdlModelingPackageParser
{
	//-------------------------------------------------------------------------
	//	Read model package chunk
	//-------------------------------------------------------------------------
	void ReadMODV(	chReader& io_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlModelingPackageData& o_Data );
}

