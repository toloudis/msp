/****************************************************************************\
**	mdlSubdivParser.hpp
**
**		mdlSubdivParser.hpp supplies functions used to import subdivs from Maya
**	(written by our Maya plugin).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_SUBDIVPARSER_HPP
#error mdlSubdivParser.hpp multiply included
#endif
#define MDL_SUBDIVPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

#include <map>


//============================================================================
//	Forward References
//============================================================================
class chReader;
class chWriter;
class mdlSubdivInfo;


//============================================================================
//	Any of these mdlSubdivParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlSubdivParser
{
	//----------------------------------------------------------------------------
	//	read subdivision surface information
	//----------------------------------------------------------------------------
	void ReadSUBD(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlSubdivInfo& o_SubdivInfo,
					bool i_FillInVertexRemap,
					const mdlMatInfoTable *i_MaterialTable = NULL);

	//----------------------------------------------------------------------------
	//	Write subdivision surface information to file.
	//----------------------------------------------------------------------------
	void WriteSUBD(	chWriter& o_Writer,
					const mdlSubdivInfo& i_SubdivInfo);
}
