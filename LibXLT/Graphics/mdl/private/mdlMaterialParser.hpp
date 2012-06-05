/****************************************************************************\
**	mdlMaterialParser.hpp
**
**		mdlMaterialParser.hpp supplies functions used to import 
**	material information.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MATERIALPARSER_HPP
#error mdlMaterialParser.hpp multiply included
#endif
#define MDL_MATERIALPARSER_HPP

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
class mdlMaterialInfo;


//============================================================================
//	Any of these mdlMaterialParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlMaterialParser
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadMATR(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlMaterialInfo& o_MatInfo);

	//----------------------------------------------------------------------------
	//  table of named materials to be shared by fragments later in file.
	//----------------------------------------------------------------------------
	void ReadMaterialTable(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							mdlMatInfoTable &o_MaterialTable);
}

