/****************************************************************************\
**	mdlHierarchyImportParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlHierarchyImportParser.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/env/envString.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/G3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlHairInfo.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/private/mdlImportUtil.hpp"
#include "Graphics/mdl/private/mdlMaterialParser.hpp"
#include "Graphics/mdl/private/mdlMeshParser.hpp"

#include <sstream>
#include <string>


//------------------------------------------------------------------------
// Constructor takes material table by reference to fill in.
//------------------------------------------------------------------------
mdlHierarchyImportParser::mdlHierarchyImportParser(const fsLocator& i_Locator,
												   //const fsResourceFinder& i_TextureFinder,
												   mdlMatInfoTable& o_MaterialTable,
												   std::vector<mdlSkinInfo>& o_SkinnedSurfaces,
												   std::vector< shared_ptr<mdlHairInfo> >& o_HairSurfaces,
												   std::vector<g3dFragment*>& o_Fragments,
												   std::vector<matMaterial*>& o_Materials,
												   //std::vector<matTexture*>& o_Textures,
												   entFragInfoSink* o_Sink)
: m_Locator(i_Locator),
  //m_TextureFinder(i_TextureFinder),
  m_MaterialTable(o_MaterialTable),
  m_SkinnedSurfaces(o_SkinnedSurfaces),
  m_HairSurfaces(o_HairSurfaces),
  m_Fragments(o_Fragments),
  m_Materials(o_Materials),
  //m_Textures(o_Textures),
  m_Sink(o_Sink),
  m_pRootNode(NULL)
{
}

//------------------------------------------------------------------------
// Access to the root node that was parsed
//------------------------------------------------------------------------
g3dSceneNode* mdlHierarchyImportParser::GetRootNode()
{
	return m_pRootNode;
}

//------------------------------------------------------------------------
// Access to the root joints for each joint chain or skeleton
//	that was parsed
//------------------------------------------------------------------------
//std::vector< smdlJoint* >& mdlHierarchyImportParser::GetRootJoints()
//{
//	return m_RootJoints;
//}

//------------------------------------------------------------------------
// Parser has reached a node in the hierarchy, this could represent
// the root node or a child of the current node.
//------------------------------------------------------------------------
void mdlHierarchyImportParser::CreateLevel(bool i_bIsJoint)
{
	// Create new level
	g3dSceneNode *new_node = new g3dSceneNode();
	new_node->SetIsJoint( i_bIsJoint );
	//smdlJoint *new_joint = (i_bIsJoint) ? new smdlJoint(new_node) : NULL;

	if (m_NodeStack.empty())
	{
		// If we didn't have a root node yet, this is it
		if (!m_pRootNode)
		{
			m_pRootNode = new_node;
		}
		else 
		{
			// If the stack is empty and we already have a root node,
			// then this is a second root. This isn't allowed right now,
			// throw invalid file format exception
			DBG_ERROR("Parsed invalid second root node in mdlHierarchyImportParser::CreateLevel");
			throw mdlInvalidModelFileX(m_Locator);
		}

		//if (new_joint)
		//{
		//	// This is a root joint.
		//	m_RootJoints.push_back(new_joint);
		//}
	}
	else
	{
		// Add this level as a child of the current node
		m_NodeStack.top()->AddChild(new_node);

		// Connect up chains of joints - there can be multiple chains and
		// they can start and stop throughout the hierarchy.
		//if (new_joint)
		//{
		//	// If we have a joint on the stack, then set this as its child,
		//	// otherwise this is a root joint.
		//	if (m_JointStack.top())
		//		m_JointStack.top()->AddChild(new_joint);
		//	else
		//		m_RootJoints.push_back(new_joint);
		//}
	}

	// Push the new node on the stack, this is now the current node
	// for the parser functions.
	m_NodeStack.push(new_node);
//	m_JointStack.push(new_joint); // new_joint maybe NULL - that is okay.

}

//------------------------------------------------------------------------
// Parser has finished reading a node and all of its children
//	in the hierarchy, the new current node is now the parent of the 
//	old current node.
//------------------------------------------------------------------------
void mdlHierarchyImportParser::FinishLevel()
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty and we are asked to pop a level
		// then something has gone wrong,
		// throw invalid file format exception
		DBG_ERROR("FinishLevel call does not match with a CreateLevel call.");
		throw mdlInvalidModelFileX(m_Locator);
	}

	// Push the new node on the stack, this is now the current node
	// for the parser functions.
	m_NodeStack.pop();
//	m_JointStack.pop();
}

//------------------------------------------------------------------------
// Set the name of the current node.
//------------------------------------------------------------------------
void mdlHierarchyImportParser::SetNodeName(const std::string& i_NodeName)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the node name,
		// throw invalid file format exception
		DBG_ERROR("SetNodeName - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	m_NodeStack.top()->SetName(i_NodeName.c_str());
}

//------------------------------------------------------------------------
// Set the transformation of the current node.
//------------------------------------------------------------------------
void mdlHierarchyImportParser::SetNodeTransform(const maMatrix4x4& i_Transform)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the node transform,
		// throw invalid file format exception
		DBG_ERROR("SetNodeTransform - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	m_NodeStack.top()->SetTransform(i_Transform);
}

//------------------------------------------------------------------------
// Set the rotate and scale pivot points for transformation.
//------------------------------------------------------------------------
void mdlHierarchyImportParser::SetNodePivots(const maVector3d& i_RotatePivot,
											 const maVector3d& i_ScalePivot, 
											 const maVector3d& i_RotatePivotTranslation, 
											 const maVector3d& i_ScalePivotTranslation)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the node pivots,
		// throw invalid file format exception
		DBG_ERROR("SetNodePivots - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	m_NodeStack.top()->SetPivotPoints(i_RotatePivot, i_ScalePivot,
									  i_RotatePivotTranslation, i_ScalePivotTranslation);
}

//------------------------------------------------------------------------
// Set the joint orientation of the current node as euler angles.
//------------------------------------------------------------------------
void mdlHierarchyImportParser::SetNodeOrientation(float i_X, float i_Y, float i_Z)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the node orientation,
		// throw invalid file format exception
		DBG_ERROR("SetNodeOrientation - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	//DBG_LOG4("Joint %s orientation: %f %f %f", m_NodeStack.top()->GetName(), i_X, i_Y, i_Z);
	m_NodeStack.top()->SetOrientation( i_X, i_Y, i_Z );
}

//------------------------------------------------------------------------
// Set the orientation of the joint for applying scale animation.
//------------------------------------------------------------------------
void mdlHierarchyImportParser::SetScaleOrientation(float i_X, float i_Y, float i_Z)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the node orientation,
		// throw invalid file format exception
		DBG_ERROR("SetNodeOrientation - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	m_NodeStack.top()->SetScaleOrientation( i_X, i_Y, i_Z );
}

//------------------------------------------------------------------------
// Set the inverse bind pose for a joint, used when doing skin animation.
//------------------------------------------------------------------------
void mdlHierarchyImportParser::SetInverseBindPose(const maMatrix4x4 &i_InvBindPose)
{
	//if (m_JointStack.empty() || !m_JointStack.top())
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current joint
		// in order to set the scale orientation,
		// throw invalid file format exception
		DBG_ERROR("SetInverseBindPose - no current joint in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	//m_JointStack.top()->SetInvBindPose( i_InvBindPose );
	m_NodeStack.top()->SetInvBindPose( i_InvBindPose );
}

//------------------------------------------------------------------------
// Parser read a mesh fragment with the given geometry, assign it
//	to the current node. 
//------------------------------------------------------------------------
void mdlHierarchyImportParser::AddMesh(mdlFragInfo& i_FragInfo)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to add the mesh info
		// throw invalid file format exception
		DBG_ERROR("ReadMesh - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	g3dSceneNode *pSceneNode = m_NodeStack.top();

	if (pSceneNode->GetFragment())
	{
		// This is the second fragment for a single node, which
		// isn't allowed in this file format.
		// throw invalid file format exception
		DBG_ERROR("Parsed invalid second mesh for one node in mdlHierarchyNodeInfoParser::ReadMesh");
		throw mdlInvalidModelFileX(m_Locator);
	}

	std::vector<g3dFragment*> new_fragments;

	const bool bCreateMaterials = m_MaterialTable.empty();
	const bool bMorphable = false;	// meshes in hierarchy are static
	mdlImportUtil::MakeFragments(	i_FragInfo,
									//m_TextureFinder,
									new_fragments,
									m_Materials,
									//m_Textures,
									m_Sink,
									bMorphable,
									bCreateMaterials);

	// Mark scene node to signify that the contents are low-res, if needed
	g3dSceneNode::Resolution resolution;
	switch (i_FragInfo.m_ResolutionLevel)
	{
	default:
	case 0:
		resolution = g3dSceneNode::e_Mixed;
		break;
	case 1:
		resolution = g3dSceneNode::e_LowRes;
		break;
	case 2:
		resolution = g3dSceneNode::e_HighRes;
		break;
	}
	pSceneNode->SetContentResolution( resolution );

	if (new_fragments.size() == 1)
	{
		// Single fragment, just set into scene node
		m_Fragments.push_back(new_fragments[0]);
		pSceneNode->SetFragment(new_fragments[0]);
	}
	else if (new_fragments.size() > 1)
	{
		// Multiple fragments, make a new set of nodes
		// and add them as children to the current node
		int num_frags = new_fragments.size();
		for (int i=0; i<num_frags; i++)
		{
			//bga - These subnodes should not be separately named
			//std::stringstream ss;
			//ss << pSceneNode->GetName() << "_";
			//ss << i;
			g3dSceneNode *node = new g3dSceneNode;
			node->SetContentResolution( resolution );
			node->SetFragment(new_fragments[i]);
			//node->SetName( ss.str().c_str() );
			pSceneNode->AddChild(node);
			m_Fragments.push_back(new_fragments[i]);
		}
	}
}

//------------------------------------------------------------------------
// A Skin consists of a subdivision or mesh surface with morph targets  
// and/or joint influences that is attached to the joint skeleton.
//------------------------------------------------------------------------
void mdlHierarchyImportParser::AddSkin(shared_ptr<mdlSubdivInfo> &i_SubdivInfo,
										 shared_ptr<smdlCharacterSkin> &i_Skin)
{
	mdlSkinInfo surface;
	surface.m_SubdivInfo = i_SubdivInfo;
	surface.m_SkinInfo = i_Skin;
	// Location for skinned surface. If node stack is empty, then add it to root
	if (!m_NodeStack.empty())
		surface.m_SceneNodeName = m_NodeStack.top()->GetName();	
	m_SkinnedSurfaces.push_back(surface);
}
void mdlHierarchyImportParser::AddSkin(shared_ptr<mdlFragInfo> &i_MeshInfo,
										 shared_ptr<smdlCharacterSkin> &i_Skin)
{
	mdlSkinInfo surface;
	surface.m_MeshInfo = i_MeshInfo;
	surface.m_SkinInfo = i_Skin;
	// Location for skinned surface. If node stack is empty, then add it to root
	if (!m_NodeStack.empty())
		surface.m_SceneNodeName = m_NodeStack.top()->GetName();		
	m_SkinnedSurfaces.push_back(surface);
}

//------------------------------------------------------------------------
// Hair strands
//------------------------------------------------------------------------
void mdlHierarchyImportParser::AddHair(shared_ptr<mdlHairInfo> &i_HairInfo)
{
	// Location for hair surface. If node stack is empty, then add it to root
	if (!m_NodeStack.empty())
		i_HairInfo->m_SceneNodeName = m_NodeStack.top()->GetName();	

	m_HairSurfaces.push_back(i_HairInfo);
}

//------------------------------------------------------------------------
// Notification that a Material Table was read.
//------------------------------------------------------------------------
void mdlHierarchyImportParser::MaterialTableWasRead()
{	
	mdlMatInfoTable::iterator it = m_MaterialTable.begin();
	for (; it != m_MaterialTable.end(); ++it)
	{
		mdlMatInfo& mat_info = (*it->second);
		
		//bga - Not loading textures anymore, just creating the material pointer
		//mat_info.LoadTextures(m_TextureFinder, m_Textures);
		mat_info.CreateMaterial();

		m_Materials.push_back(mat_info.m_pMaterial);
	}
}

//------------------------------------------------------------------------
// The parser contains an internal material table to use when parsing
//	the file. The functions provide access to it.
//------------------------------------------------------------------------
mdlMatInfoTable& mdlHierarchyImportParser::GetMaterialTable()
{
	return m_MaterialTable;
}
const mdlMatInfoTable& mdlHierarchyImportParser::GetMaterialTable() const
{
	return m_MaterialTable;
}

//------------------------------------------------------------------------
// Parser read a mesh fragment with the given geometry, assign it
//	to the current node. 
//------------------------------------------------------------------------
void mdlHierarchyImportParser::AddMeshReference(mdlNodeInfoProxy& i_Proxy)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to add the mesh info
		// throw invalid file format exception
		DBG_ERROR("AddMeshReference - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}
	std::string &lastComponent = i_Proxy.m_Path.back();
	if( i_Proxy.m_Path.empty() )
	{
		// if the instace path that we are referring is
		// empty throw  invalid file format exception
		DBG_ERROR("AddMeshReference - instance reference path is empty: " << lastComponent );
		throw mdlInvalidModelFileX(m_Locator);
	}
	g3dSceneNode *pSceneNode = GetRootNode();
	g3dSceneNode *pReferredNode = pSceneNode->GetNamedNodeFromPath(  i_Proxy.m_Path.begin(), i_Proxy.m_Path.end() );
	if	( NULL == pReferredNode )
	{
		DBG_ERROR("AddMeshReference - Can't find instanced node: " << lastComponent );
		throw mdlInvalidModelFileX(m_Locator);
	}
	g3dSceneNode *pThisNode = m_NodeStack.top();
	//clone the children hierarchy of pReferredNode into thisNode
    pThisNode->CloneChildrenFrom( pReferredNode, m_Fragments );
	// enforce the same node name in order to detect instancing
	pThisNode->SetName( pReferredNode->GetName() );
	//Ideally pThisNode should be a clone of
	//pReferredNode, but it is not easily possible
	//since pThisNode is created before pReferredNode is resolved.
	//However the content resoolution flag is crucial.
	//Todo: a full copy of all the features.
	//kg: 08/07/09
	pThisNode->SetContentResolution( pReferredNode->GetContentResolution() );

}
