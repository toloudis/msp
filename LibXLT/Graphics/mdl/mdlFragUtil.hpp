/****************************************************************************\
**	mdlFragUtil.hpp
**
**		mdlFragUtil supplies functions for manipulating between
**	our different fragment data structures.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_FRAGUTIL_HPP
#error mdlFragUtil.hpp multiply included
#endif
#define MDL_FRAGUTIL_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <map>
#include <vector>


//============================================================================
//============================================================================
class maMatrix4x4;
class matMaterial;
class mdlFragInfo;
struct mdlMatInfo;
struct mdlSplitFragInfo;


//============================================================================
//============================================================================
namespace mdlFragUtil
{
	//------------------------------------------------------------------------
	//	Make mdlSplitFragInfo structs from a single mdlFragInfo struct,
	//  one for each material.
	//	A variation of this function allows you to override some
	//	material assignments when splitting the fragments.
	//------------------------------------------------------------------------
	void SplitFragments(const mdlFragInfo &i_FragInfo,
						std::vector<mdlSplitFragInfo> &o_SplitFrags);
	void SplitFragments(const mdlFragInfo &i_FragInfo,
						std::vector<mdlSplitFragInfo> &o_SplitFrags,
						const std::map<const matMaterial*,matMaterial*>& i_MaterialRemapping);

	//------------------------------------------------------------------------
	// Creates basis vectors for the given fragment info
	//------------------------------------------------------------------------
	void CreateBasisVectors(mdlFragInfo &io_FragInfo);

	//------------------------------------------------------------------------
	// Convert remapping from "old index->new index" map to a vector
	//	which maps "new index->old index"
	//------------------------------------------------------------------------
	void ConvertRemapping(const std::multimap<int, int>& i_OrgRemapping,
						 std::vector<int>& i_NewRemapping,
						 int i_NumExpandedVertices);

	//------------------------------------------------------------------------
	// Combine the remappings in the original MayFragInfo with the 
	//	remapping arrays from the split fragments generated in order
	//	to create new arrays which go all the way back to the Maya arrays.
	//------------------------------------------------------------------------
	void GenerateFullRemappings(const mdlFragInfo& i_FragInfo,
								const std::vector<mdlSplitFragInfo>& i_SplitFrags,
								std::vector< std::vector<int> >& o_FullVertexRemapping,
								std::vector< std::vector<int> >& o_FullNormalRemapping);

	//------------------------------------------------------------------------
	//	Spatially partition the triangles in this fragment so that
	//	no fragment has more than i_MaxNumTris number of triangles.
	//------------------------------------------------------------------------
	void SpatiallyPartition(const mdlSplitFragInfo &i_FragInfo,
							std::vector<mdlSplitFragInfo> &o_SplitFrags,
							int i_MaxNumTris);
}
