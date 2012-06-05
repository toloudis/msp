/****************************************************************************\
**	mdlMeshMayaParser.hpp
**
**		mdlMeshMayaParser.hpp supplies functions used to import meshes 
**	from an older version of our Maya plugin. 
**
**	In this format, the vertices and normals have different indices
**	and they have to be sorted together in order to be rendered in D3D.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MESHMAYAPARSER_HPP
#error mdlMeshMayaParser.hpp multiply included
#endif
#define MDL_MESHMAYAPARSER_HPP

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
class mdlFragInfo;
struct mdlMatInfo;


//============================================================================
//	Any of these mdlMeshMayaParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlMeshMayaParser
{
	//----------------------------------------------------------------------------
	//	Read geometry fragment - returns true if the fragment load was
	//	successful, false if the fragment should be skipped.
	//----------------------------------------------------------------------------
	bool ReadGFRG(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlFragInfo& o_FragInfo,
					bool i_FillInVertexRemap,
					bool i_bSkipLowRes = false,
					bool i_bSkipHighRes = false,
					mdlMatInfoTable *i_MaterialTable = NULL);
}

