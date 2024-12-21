/****************************************************************************\
**	mdlNodeUtil.hpp
**
**		mdlNodeUtil supplies functions for manipulating between
**	our different fragment data structures.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_NODEUTIL_HPP
#error mdlNodeUtil.hpp multiply included
#endif
#define MDL_NODEUTIL_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif

#include <map>
#include <string>
#include <vector>


//============================================================================
//============================================================================
class matMaterial;
class mdlFragInfo;
struct mdlSplitFragInfo;
struct mdlMatInfo;
class mdlNodeInfo;
class maMatrix4x4;


//============================================================================
//============================================================================
namespace mdlNodeUtil
{
	//----------------------------------------------------------------------------
	//	For the input subtree of the mdl node hiearcchy given,
	//	merge all nodes with a mesh or an instance to a mesh based on 
	//	the materials assigned to the mdlFragInfo-s
	//	and connect the merged nodes under the root node as children.
	//	Any non-mergable nopde containing geometry content is put under the
	//	root node with its total cumulative transform.
	// i_MergedPrefix = a prefix string that can be prefixed to generate the name of a merged fragment.
	//					The name of a marged fragment = prefix + "_" + material name
	//	io_RootSceneNode = the  root node of the mdl node hierarchy, to be merged
	//  
	//	o_NumberOfMeshesSubjectedToMerge = number of meshes in the scene that were be 
	//					subjected to  merge
	//  o_NumberOfMergedMeshesResulted = number of merged fragments produced.
	//------------------------------------------------------------------------
	void DoMerge(
		const std::string &i_MergePrefix, 
		shared_ptr< mdlNodeInfo >  &i_RootNode,
		int i_MaxNumTrisInMergedMesh,		
		int &o_NumberOfMeshesSubjectedToMerge,
		int &o_NumberOfMergedMeshesResulted);
	
	//------------------------------------------------------------------------
	//returns true if the node contains a valid geometry
	//or a reference to it
	//(m_MeshInfo or m_SubdivInfo or m_InstanceInfo or m_SkinInfo should be non-null)	
	//------------------------------------------------------------------------
	bool HasGeometryContent( const shared_ptr< mdlNodeInfo > &i_Node );
}
