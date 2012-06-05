/****************************************************************************\
**	mdlImportUtil.hpp
**
**		mdlImportUtil.hpp provides a utility for creating fragments
**	from fragment structures. Used in multiple file types, it is
**	provided here to avoid duplicating code.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_IMPORTUTIL_HPP
#error mdlImportUtil.hpp multiply included
#endif
#define MDL_IMPORTUTIL_HPP

#include <vector>


//============================================================================
//============================================================================
class entFragInfoSink;
class fsResourceFinder;
class g3dFragment;
class matMaterial;
class matTexture;
class mdlFragInfo;


//============================================================================
//	Any of these mdlImportUtil functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlImportUtil
{
	//------------------------------------------------------------------------
	// Make one or more fragments from the given mdlFragInfo
	//------------------------------------------------------------------------
	void MakeFragments( mdlFragInfo& i_FragInfo,
						 //const fsResourceFinder& i_TextureFinder,
						 std::vector<g3dFragment*>& o_Fragments,
						 std::vector<matMaterial*>& o_Materials,
						 //std::vector<matTexture*>& o_Textures,
						 entFragInfoSink* o_Sink,
						 bool i_Morphable,
						 bool i_CreateMaterials );

}
