/****************************************************************************\
**  mdlMeshParser.hpp
**
**      mdlSubdivParser.hpp supplies functions used to import meshes from Maya
**	(written by our Maya plugin).
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MESHPARSER_HPP
#error mdlMeshParser.hpp multiply included
#endif
#define MDL_MESHPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
//#ifndef MDL_MATINFOTABLE_HPP
//#include "Graphics/mdl/mdlMatInfoTable.hpp"
//#endif 

#include <map>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class chReader;
class mdlFragInfo;
//struct mdlMatInfo;

//----------------------------------------------------------------------------
//	Any of these mdlMeshParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace mdlMeshParser
{
	//----------------------------------------------------------------------------
	// Set the loader to skip the loading of low resolution or high resolution
	//	models. Can save memory when it is known a certain version isn't needed.
	//----------------------------------------------------------------------------
	void SetSkipLowRes(bool i_bSkip);
	void SetSkipHighRes(bool i_bSkip);

	//----------------------------------------------------------------------------
	//	Read geometry fragment - returns true if the fragment load was
	//	successful, false if the fragment should be skipped.
	//----------------------------------------------------------------------------
	bool ReadGFRG(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlFragInfo& o_FragInfo,
					bool i_FillInVertexRemap);
					//,mdlMatInfoTable *i_MaterialTable = NULL);

}

