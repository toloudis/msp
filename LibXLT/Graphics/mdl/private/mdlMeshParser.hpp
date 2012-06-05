/****************************************************************************\
**	mdlMeshParser.hpp
**
**		mdlMeshParser.hpp supplies functions used to read and write meshes
**	in geometry files. 
**
**	In this format, the vertex information has already been sorted so
**	there are equal numbers of positions and normals and there is only
**	one set of indices. This format also includes an optional map to
**	tell how the vertices were sorted in order to use shorter lists of 
**	vertices when animating.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MESHPARSER_HPP
#error mdlMeshParser.hpp multiply included
#endif
#define MDL_MESHPARSER_HPP

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
class mdlFragInfo;
class mdlNodeInfoProxy;
struct mdlMatInfo;


//============================================================================
//	Any of these mdlMeshParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
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
					bool i_FillInVertexRemap,
					mdlMatInfoTable *i_MaterialTable = NULL);
	
	//----------------------------------------------------------------------------
	//	Write geometry fragment to file.
	//----------------------------------------------------------------------------
	void WriteGFRG(	chWriter& o_Writer,
					const mdlFragInfo& i_FragInfo);

	//----------------------------------------------------------------------------
	//	Write node proxy to file.
	//----------------------------------------------------------------------------
	void WriteGNDP(	chWriter& o_Writer,
					const mdlNodeInfoProxy& i_NodeInfo);

	//----------------------------------------------------------------------------
	//	Read node proxy from file.
	//----------------------------------------------------------------------------
	bool ReadGNDP(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlNodeInfoProxy& o_NodeInfoProxy );
}

