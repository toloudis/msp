/****************************************************************************\
**	mdlNodeUtil.cpp
**
**		mdlNodeUtil supplies functions for manipulating between
**	our different fragment data structures.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlNodeUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/geo/geoKDTree.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlMaterialMerge.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"

#include <iostream>
#include <set>


//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	//	Base class of all the visit classes used
	//----------------------------------------------------------------------------
	struct VisitBase
	{
		// various results returnd by Pre, Post or VisitMdlHierarchy.
		//	eOK = everything  is OK 
		//	ePrune = everything is OK, but no nneed to visit the children of this node
		//	eStop = everything is OK, we found something, we can stop the visiting
		//	eError = an error occured stop the visiting
		typedef enum { eOK, ePrune, eStop, eError } EWalkRes;
		//Does something before visiting the children
		virtual EWalkRes Pre( shared_ptr< mdlNodeInfo > &i_MdlNode )=0;
		//Does something after visiting the children
		virtual EWalkRes Post( shared_ptr< mdlNodeInfo > &i_MdlNode, EWalkRes res)=0;
	};

	//----------------------------------------------------------------------------
	//	A function template for visiting the mdlNode hierarchy.
	//	The visit class should not disrupt the parent child relationship 
	//	in the hierarchy.
	//----------------------------------------------------------------------------
	template< class Visit >
	VisitBase::EWalkRes  VisitMdlHierarchyRec(shared_ptr<mdlNodeInfo> io_MdlNode, Visit &visit )
	{	
		bool bVisitChildren = true;
		//Does the stuff that need
		//be done prior to visiting children
		VisitBase::EWalkRes res = visit.Pre(io_MdlNode);	
		//If the result of Pre-visit was ePrune or eStop or eError
		//no need to visit the children
		if ( res > VisitBase::eOK )
		{
			bVisitChildren = false;
		}
		if ( bVisitChildren )
		{
			std::vector< shared_ptr<mdlNodeInfo> >::iterator chit;
			//make a copy of the current stack
			Visit::stack_type visitStackCopy( visit.m_Stack );
			for ( chit = io_MdlNode->m_Children.begin(); chit != io_MdlNode->m_Children.end(); ++chit )
			{
				shared_ptr<mdlNodeInfo> ch = *chit;		
				//visit the child
				res = VisitMdlHierarchyRec( ch, visit );			
				//restore the stack
				visit.m_Stack =  visitStackCopy;
				//If the result of visitinbg the child is eStop or eError
				//break here
				if ( res  >= VisitBase::eStop )
				{
					break;
				}
			}
		}
		//does the stuff that need be done
		//after visiting children
		//but dont do it if the current result is eError
		if ( res <= VisitBase::eStop )
		{
			res = visit.Post(io_MdlNode, res);
		}
		return res;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	shared_ptr<mdlNodeInfo> find_instance(shared_ptr<mdlNodeInfo>& i_Node,
		const std::deque< std::string > &i_PathToDereference,
		int i_IndexInPath)
	{
		const int num_paths = i_PathToDereference.size();
		if (i_IndexInPath >= num_paths)
			return shared_ptr<mdlNodeInfo>(); // return null

		const std::string name = i_PathToDereference[i_IndexInPath];
		if (i_Node->m_NodeName != name)
			return shared_ptr<mdlNodeInfo>(); // return null

		int new_index = i_IndexInPath+1;
		if (new_index == num_paths) 
			return i_Node; // this node is the end of the path

		// recurse on children
		const int num_kids = i_Node->m_Children.size();
		for (int k=0; k<num_kids; ++k)
		{
			shared_ptr<mdlNodeInfo> result = find_instance(i_Node->m_Children[k], i_PathToDereference, new_index);
			if (result)
				return result;
		}

		return shared_ptr<mdlNodeInfo>(); // return null
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	int count_num_nodes(const shared_ptr<mdlNodeInfo>& i_Node )
	{
		int numNodes =0;
		const int num_kids = i_Node->m_Children.size();
		for (int k=0; k<num_kids; ++k)
		{
			numNodes += count_num_nodes( i_Node->m_Children[k] );
		}

		return numNodes + 1; // return null
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	class InstanceResolver
	{
		public:
			//----------------------------------------------------------------------------
			//----------------------------------------------------------------------------
			InstanceResolver(shared_ptr<mdlNodeInfo>& i_RootNode)
				: m_RootNode(i_RootNode)
			{
			}

			//----------------------------------------------------------------------------
			//----------------------------------------------------------------------------
			shared_ptr<mdlNodeInfo> ResolveInstance(const shared_ptr<mdlPathReference>& i_InstanceInfo)
			{
				const std::deque< std::string > &pathToDereference = i_InstanceInfo->m_Path;
				std::map< std::deque< std::string >, shared_ptr<mdlNodeInfo> >::iterator it =
					m_FoundInstances.find(pathToDereference);
				if (it != m_FoundInstances.end())
				{
					return it->second;
				}

				// New instance, search the root node for it
				shared_ptr<mdlNodeInfo> found = find_instance(m_RootNode, pathToDereference, 0);
				m_FoundInstances[pathToDereference] = found;
				return found;
			}

		private:
			shared_ptr<mdlNodeInfo>& m_RootNode;
			std::map< std::deque< std::string >, shared_ptr<mdlNodeInfo> > m_FoundInstances;
	};

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	struct sMeshAndTransform
	{
		shared_ptr<mdlFragInfo> m_MeshInfo;
		maMatrix4x4 m_Matrix;
	};

	//----------------------------------------------------------------------------
	// gather surfaces from the hierarchy
	//----------------------------------------------------------------------------
	void gather_fragments(const shared_ptr<mdlNodeInfo>& i_Node,
		const maMatrix4x4& i_TotalMatrix,
		std::list<sMeshAndTransform>& o_Fragments,
		std::list< shared_ptr< mdlNodeInfo > > &o_NonMergedNodes,
		InstanceResolver &io_InstanceResolver)
	{
		const std::string &nodeName = i_Node->m_NodeName;
		maMatrix4x4 total_matrix = i_Node->m_Transform * i_TotalMatrix;

		if (i_Node->m_MeshInfo)
		{
			sMeshAndTransform info = { i_Node->m_MeshInfo, total_matrix };
			o_Fragments.push_back( info );
		} else if (i_Node->m_InstanceInfo)
		{
			shared_ptr<mdlNodeInfo> instance = io_InstanceResolver.ResolveInstance(i_Node->m_InstanceInfo);
			if (instance && instance->m_MeshInfo )
			{
				sMeshAndTransform info = { instance->m_MeshInfo, total_matrix };
				o_Fragments.push_back( info );
			}
		} else if ( mdlNodeUtil::HasGeometryContent( i_Node ) )
		{
			i_Node->m_Transform = total_matrix;
			o_NonMergedNodes.push_back( i_Node );
		}
		int numKids = i_Node->m_Children.size();

		for (int k=0; k < numKids; ++k)
			gather_fragments(i_Node->m_Children[k], total_matrix, o_Fragments, o_NonMergedNodes, io_InstanceResolver);
	}

}


//============================================================================
//============================================================================
namespace mdlNodeUtil
{
	//------------------------------------------------------------------------
	//returns true if the node contains a valid geometry
	//or a reference to it
	//(m_MeshInfo or m_SubdivInfo or m_InstanceInfo or m_SkinInfo should be non-null)	
	//------------------------------------------------------------------------
	bool HasGeometryContent( const shared_ptr< mdlNodeInfo > &i_Node )
	{
		return i_Node->m_MeshInfo || i_Node->m_SubdivInfo || i_Node->m_InstanceInfo || i_Node->m_SkinInfo;
	}

	//------------------------------------------------------------------------
	//	For the input subtree of the mdl node hiearcchy given,
	//	merge all nodes with a mesh or an instance to a mesh based on 
	//	the materials assigned to the mdlFragInfo-s
	//	and connect the merged nodes under the root node as children.
	//	Any non-mergable node containing geometry content is put under the
	//	root node with its total cumulative transform.
	// i_MergedPrefix = a prefix string that can be prefixed to generate the name of a merged fragment.
	//					The name of a marged fragment = prefix + "_" + material name
	//	io_RootSceneNode = the  root node of the mdl node hierarchy, to be merged
	//  
	//	o_NumberOfMeshesSubjectedToMerge = number of meshes in the scene that were be 
	//					subjected to  merge
	//	o_NumberOfMergedMeshesResulted = number of merged fragments produced.
	//------------------------------------------------------------------------
	void DoMerge( const std::string &i_MergePrefix,
				shared_ptr< mdlNodeInfo > & io_RootSceneNode,
				int i_MaxNumTrisInMergedMesh,
				int &o_NumberOfMeshesSubjectedToMerge,
				int &o_NumberOfMergedMeshesResulted )
	{
		if ( i_MaxNumTrisInMergedMesh < 1000 )
		{
			DBG_TRACE( "max number of triangles in merged mesh too low : " << i_MaxNumTrisInMergedMesh );
		}
		int numNodesBeforeMerge = count_num_nodes( io_RootSceneNode );
		std::list<sMeshAndTransform> fragments;
		std::list< shared_ptr< mdlNodeInfo > > nonMergedNodes;
		{
			// scope added so that instance_resolver releases its shared_ptrs
			InstanceResolver instance_resolver(io_RootSceneNode);
			maMatrix4x4 identity;
			gather_fragments(io_RootSceneNode, identity, fragments, nonMergedNodes, instance_resolver);
		}

		// Replace root node with new node, releases old hierarchy completely.
		shared_ptr< mdlNodeInfo > new_root_node(new mdlNodeInfo());
		new_root_node->m_NodeName = io_RootSceneNode->m_NodeName;
		new_root_node->m_Transform = io_RootSceneNode->m_Transform;
		io_RootSceneNode = new_root_node;

		// Merge all of the fragments in the list
		mdlMaterialMerge  mdlMerge( i_MergePrefix , i_MaxNumTrisInMergedMesh );

		std::list<sMeshAndTransform>::iterator it;
		for (it = fragments.begin(); it != fragments.end(); ++it)
		{
			// do the material merge
			const maMatrix4x4 & transformToBeAppliedBeforeMerging = it->m_Matrix;
			mdlMerge.DoMerge( it->m_MeshInfo, &transformToBeAppliedBeforeMerging );

			// release fragment info memory
			it->m_MeshInfo.reset();
		}

		std::vector< shared_ptr< mdlFragInfo > > mergedResult;
		//collect all the resulting material merged frags
		mdlMerge.DepleteResult( mergedResult );

		io_RootSceneNode->m_Children.reserve( mergedResult.size() + nonMergedNodes.size() );
		std::vector< shared_ptr< mdlFragInfo > >::iterator git;
		for ( git = mergedResult.begin(); git != mergedResult.end(); ++git )
		{
			//attache them to a fresh nodeInfo
			//give it the same nam e as the material name
			shared_ptr< mdlFragInfo > &newFrag = *git;
			shared_ptr< mdlNodeInfo > newNodeForMaterial( new mdlNodeInfo );
			newNodeForMaterial->m_MeshInfo = newFrag;
			newNodeForMaterial->m_NodeName = newFrag->m_Name;
			//push the node as the direct child of root node
			io_RootSceneNode->m_Children.push_back (  newNodeForMaterial );
		}
		std::list< shared_ptr< mdlNodeInfo > >::iterator lit;
		for ( lit = nonMergedNodes.begin(); lit != nonMergedNodes.end(); ++lit )
		{
			shared_ptr< mdlNodeInfo > &nonMergedNode = *lit;
			io_RootSceneNode->m_Children.push_back( nonMergedNode );
		}
		o_NumberOfMeshesSubjectedToMerge = mdlMerge.GetNumMeshesSubjectedToMergeSoFar();
		o_NumberOfMergedMeshesResulted = static_cast< int > ( mergedResult.size() );
		int numNodesAfterMerge = count_num_nodes( io_RootSceneNode );
		DBG_LOG( "numNodesBeforeMerge = " << numNodesBeforeMerge );
		DBG_LOG( "numNodesAfterMerge " << numNodesAfterMerge );
	}
}
