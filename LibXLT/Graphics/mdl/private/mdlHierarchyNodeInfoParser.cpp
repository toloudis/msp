/****************************************************************************\
**  mdlHierarchyNodeInfoParser.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlHierarchyNodeInfoParser.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/mdl/private/mdlMaterialParser.hpp"
#include "Graphics/mdl/private/mdlMeshParser.hpp"


//------------------------------------------------------------------------
// Constructor takes material table by reference to fill in.
//------------------------------------------------------------------------
mdlHierarchyNodeInfoParser::mdlHierarchyNodeInfoParser(const fsLocator& i_Locator,
													   mdlMatInfoTable& o_MaterialTable)
:	m_Locator(i_Locator),
	m_MaterialTable(o_MaterialTable),
	m_RootNode(new mdlNodeInfo())		// start with root in order to group nodes from file
{
	m_RootNode->m_NodeName = "root";
}

//------------------------------------------------------------------------
// Access to the root node that was parsed
//------------------------------------------------------------------------
shared_ptr<mdlNodeInfo> mdlHierarchyNodeInfoParser::GetRootNode()
{
	// m_RootNode was not parsed, it is a grouper in case multiple
	// root nodes were read from a file. If it has a single child, then we
	// can use that root node that we parsed as the root node to return here.
	//
	if (m_RootNode->m_Children.size() == 1)
		return m_RootNode->m_Children[0];
	else
		return m_RootNode;
}

//------------------------------------------------------------------------
// Parser has reached a node in the hierarchy, this could represent
// the root node or a child of the current node.
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::CreateLevel(bool i_bIsJoint)
{
	// Create new level
	shared_ptr<mdlNodeInfo> new_node(new mdlNodeInfo(i_bIsJoint));

	if (m_NodeStack.empty())
	{
		// Parsed a root node from the file, put it under our grouping node.
		m_RootNode->m_Children.push_back(new_node);
	}
	else
	{
		// Add this level as a child of the current node
		m_NodeStack.top()->m_Children.push_back(new_node);
	}

	// Push the new node on the stack, this is now the current node
	// for the parser functions.
	m_NodeStack.push(new_node);

}

//------------------------------------------------------------------------
// Parser has finished reading a node and all of its children
//	in the hierarchy, the new current node is now the parent of the 
//	old current node.
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::FinishLevel()
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
}

//------------------------------------------------------------------------
// Set the name of the current node.
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::SetNodeName(const std::string& i_NodeName)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the node name,
		// throw invalid file format exception
		DBG_ERROR("SetNodeName - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	m_NodeStack.top()->m_NodeName = i_NodeName;
}

//------------------------------------------------------------------------
// Set the transformation of the current node.
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::SetNodeTransform(const maMatrix4x4& i_Transform)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the node transform,
		// throw invalid file format exception
		DBG_ERROR("SetNodeTransform - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	m_NodeStack.top()->m_Transform = i_Transform;

}

//------------------------------------------------------------------------
// Set the rotate and scale pivot points for transformation.
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::SetNodePivots(const maVector3d& i_RotatePivot,
											   const maVector3d& i_ScalePivot, 
											   const maVector3d& i_RotatePivotTranslation, 
											   const maVector3d& i_ScalePivotTranslation)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the node orientation,
		// throw invalid file format exception
		DBG_ERROR("SetNodePivots - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	shared_ptr<mdlNodeInfo> &node = m_NodeStack.top();
	node->m_RotatePivot = i_RotatePivot;
	node->m_ScalePivot = i_ScalePivot;
	node->m_RotatePivotTranslation = i_RotatePivotTranslation;
	node->m_ScalePivotTranslation = i_ScalePivotTranslation;
}

//------------------------------------------------------------------------
// Set the joint orientation of the current node as euler angles.
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::SetNodeOrientation(float i_X, float i_Y, float i_Z)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the node orientation,
		// throw invalid file format exception
		DBG_ERROR("SetNodeOrientation - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	m_NodeStack.top()->m_JointOrientation.Set( i_X, i_Y, i_Z );
}

//------------------------------------------------------------------------
// Set the orientation of the joint for applying scale animation.
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::SetScaleOrientation(float i_X, float i_Y, float i_Z)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the node orientation,
		// throw invalid file format exception
		DBG_ERROR("SetScaleOrientation - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	m_NodeStack.top()->m_JointScaleOrientation.Set( i_X, i_Y, i_Z );
}

//------------------------------------------------------------------------
// Set the inverse bind pose for a joint, used when doing skin animation.
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::SetInverseBindPose(const maMatrix4x4 &i_InvBindPose)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the node orientation,
		// throw invalid file format exception
		DBG_ERROR("SetInverseBindPose - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	m_NodeStack.top()->m_InverseBindPose = i_InvBindPose;
}

//------------------------------------------------------------------------
// Parser read a mesh fragment with the given geometry, assign it
//	to the current node. 
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::AddMesh(mdlFragInfo& i_FragInfo)
{
	DBG_ASSERT(false, "AddMesh should not be called because ReadMesh was overriden");
}

//------------------------------------------------------------------------
// This is an alternative to the "AddMesh()" function to be used when
//	the derivation wants to control the how the mdlFragInfo structure
//	is created. The default behavior is to make a local mdlFragInfo
//	structure, read the frag info and then call "AddMesh()".
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::ReadMesh(chReader& i_Reader,
								  chDefs::Name i_Name,
								  chDefs::Version i_Version,
								  chDefs::Size i_Size)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to add the mesh info
		// throw invalid file format exception
		DBG_ERROR("ReadMesh - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	if (!m_NodeStack.top()->m_MeshInfo)
	{
		// We only need the vertex remap when the mesh will be vertex animated,
		// which is not the case in this file format.
		const bool c_bFillInVertexRemap = false;

		// Create our own frag info in order to attach it to 
		// our node info tree
		shared_ptr<mdlFragInfo> new_mesh(new mdlFragInfo);
		mdlMeshParser::ReadGFRG(i_Reader, i_Version, i_Size, 
			*new_mesh, c_bFillInVertexRemap, &this->GetMaterialTable());
		m_NodeStack.top()->m_MeshInfo = new_mesh;
	}
	else
	{
		// This is the second fragment for a single node, which
		// isn't allowed in this file format.
		// throw invalid file format exception
		DBG_ERROR("Parsed invalid second mesh for one node in mdlHierarchyNodeInfoParser::ReadMesh");
		throw mdlInvalidModelFileX(m_Locator);
	}
}
void mdlHierarchyNodeInfoParser::ReadNodeInfoProxy(chReader& i_Reader,
								  chDefs::Name i_Name,
								  chDefs::Version i_Version,
								  chDefs::Size i_Size)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to add the mesh info
		// throw invalid file format exception
		DBG_ERROR("ReadMesh - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}
	//----------------------------------------------------------------------------
	//	Read Node Proxy
	//----------------------------------------------------------------------------

	if (!m_NodeStack.top()->m_InstanceInfo)
	{
		shared_ptr<mdlNodeInfoProxy> new_node_info_proxy(new mdlNodeInfoProxy);
		mdlMeshParser::ReadGNDP(i_Reader, i_Version, i_Size, 
			*new_node_info_proxy);
		m_NodeStack.top()->m_InstanceInfo = new_node_info_proxy;
	}
	else
	{
		// This is the second fragment for a single node, which
		// isn't allowed in this file format.
		// throw invalid file format exception
		DBG_ERROR("Parsed invalid second mesh for one node in mdlHierarchyNodeInfoParser::ReadMesh");
		throw mdlInvalidModelFileX(m_Locator);
	}
}
//------------------------------------------------------------------------
// A Skin consists of a subdivision or mesh surface with morph targets  
// and/or joint influences that is attached to the joint skeleton.
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::AddSkin(shared_ptr<mdlSubdivInfo> &i_SubdivInfo,
										 shared_ptr<smdlCharacterSkin> &i_SkinInfo)
{
	shared_ptr<mdlNodeInfo> skin_node;
	if (m_NodeStack.empty())
	{
		// If we are at the root, then add a new node to the root node to hold the skin
		skin_node.reset(new mdlNodeInfo());
		skin_node->m_NodeName = i_SubdivInfo->m_Name;
		m_RootNode->m_Children.push_back(skin_node);
	}
	else
		skin_node = m_NodeStack.top();


	skin_node->m_SubdivInfo = i_SubdivInfo;
	skin_node->m_SkinInfo = i_SkinInfo;
}
void mdlHierarchyNodeInfoParser::AddSkin(shared_ptr<mdlFragInfo> &i_MeshInfo,
										 shared_ptr<smdlCharacterSkin> &i_SkinInfo)
{
	shared_ptr<mdlNodeInfo> skin_node;
	if (m_NodeStack.empty())
	{
		// If we are at the root, then add a new node to the root node to hold the skin
		skin_node.reset(new mdlNodeInfo());
		skin_node->m_NodeName = i_MeshInfo->m_Name;
		m_RootNode->m_Children.push_back(skin_node);
	}
	else
		skin_node = m_NodeStack.top();


	skin_node->m_MeshInfo = i_MeshInfo;
	skin_node->m_SkinInfo = i_SkinInfo;
}

//------------------------------------------------------------------------
// Hair strands
//------------------------------------------------------------------------
void mdlHierarchyNodeInfoParser::AddHair(shared_ptr<mdlHairInfo> &i_HairInfo)
{
	if (m_NodeStack.empty())
	{
		// If the stack is empty we don't have a current node
		// in order to set the hair info,
		// throw invalid file format exception
		DBG_ERROR("AddHair - no current node in stack");
		throw mdlInvalidModelFileX(m_Locator);
	}

	m_NodeStack.top()->m_HairInfo = i_HairInfo;
}

//------------------------------------------------------------------------
// The parser contains an internal material table to use when parsing
//	the file. The functions provide access to it.
//------------------------------------------------------------------------
mdlMatInfoTable& mdlHierarchyNodeInfoParser::GetMaterialTable()
{
	return m_MaterialTable;
}
const mdlMatInfoTable& mdlHierarchyNodeInfoParser::GetMaterialTable() const
{
	return m_MaterialTable;
}

void mdlHierarchyNodeInfoParser::AddMeshReference(mdlNodeInfoProxy& i_Proxy)
{
	DBG_ERROR( "Not Implementted" );
	throw  mdlInvalidModelFileX( m_Locator );
}
