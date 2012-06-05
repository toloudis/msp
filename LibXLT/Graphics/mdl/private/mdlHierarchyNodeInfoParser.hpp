/****************************************************************************\
**	mdlHierarchyNodeInfoParser.hpp
**
**		mdlHierarchyNodeInfoParser provides a derivation in order to
**	parser hierarchy files into mdlNodeInfo data structures.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_HIERARCHYNODEINFOPARSER_HPP
#error mdlHierarchyNodeInfoParser.hpp multiply included
#endif
#define MDL_HIERARCHYNODEINFOPARSER_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 
#ifndef MDL_HIERARCHYPARSER_HPP
#include "Graphics/mdl/private/mdlHierarchyParser.hpp"
#endif 

#include <stack>


//============================================================================
//============================================================================
class mdlNodeInfo;
class mdlNodeInfoProxy;


//============================================================================
//============================================================================
class mdlHierarchyNodeInfoParser : public mdlHierarchyParser
{
public:
	//------------------------------------------------------------------------
	// Constructor takes material table by reference to fill in
	// and locator for use when throwing exceptions.
	//------------------------------------------------------------------------
	mdlHierarchyNodeInfoParser(const fsLocator& i_Locator,
							   mdlMatInfoTable& o_MaterialTable);

	//------------------------------------------------------------------------
	// Access to the root node that was parsed
	//------------------------------------------------------------------------
	shared_ptr<mdlNodeInfo> GetRootNode();

	//------------------------------------------------------------------------
	// Parser has reached a node in the hierarchy, this could represent
	// the root node or a child of the current node.
	//------------------------------------------------------------------------
	virtual void CreateLevel(bool i_bIsJoint = false);

	//------------------------------------------------------------------------
	// Parser has finished reading a node and all of its children
	//	in the hierarchy, the new current node is now the parent of the 
	//	old current node.
	//------------------------------------------------------------------------
	virtual void FinishLevel();

	//------------------------------------------------------------------------
	// Set the name of the current node.
	//------------------------------------------------------------------------
	virtual void SetNodeName(const std::string& i_NodeName);

	//------------------------------------------------------------------------
	// Set the transformation of the current node.
	//------------------------------------------------------------------------
	virtual void SetNodeTransform(const maMatrix4x4& i_Transform);

	//------------------------------------------------------------------------
	// Set the rotate and scale pivot points for transformation.
	//------------------------------------------------------------------------
	virtual void SetNodePivots(const maVector3d& i_RotatePivot,
							   const maVector3d& i_ScalePivot, 
							   const maVector3d& i_RotatePivotTranslation, 
							   const maVector3d& i_ScalePivotTranslation);

	//------------------------------------------------------------------------
	// Set the joint orientation of the current node as euler angles.
	//------------------------------------------------------------------------
	virtual void SetNodeOrientation(float i_X, float i_Y, float i_Z);

	//------------------------------------------------------------------------
	// Set the orientation of the joint for applying scale animation.
	//------------------------------------------------------------------------
	virtual void SetScaleOrientation(float i_X, float i_Y, float i_Z);
	
	//------------------------------------------------------------------------
	// Set the inverse bind pose for a joint, used when doing skin animation.
	//------------------------------------------------------------------------
	virtual void SetInverseBindPose(const maMatrix4x4 &i_BindPose);

	//------------------------------------------------------------------------
	// Parser read a mesh fragment with the given geometry, assign it
	//	to the current node. 
	//------------------------------------------------------------------------
	virtual void AddMesh(mdlFragInfo& i_FragInfo);

	//------------------------------------------------------------------------
	// Parser read a node proxy which reference another node
	//------------------------------------------------------------------------
	virtual void AddMeshReference(mdlNodeInfoProxy& i_Proxy);
	//------------------------------------------------------------------------
	// This is an alternative to the "AddMesh()" function to be used when
	//	the derivation wants to control the how the mdlFragInfo structure
	//	is created. The default behavior is to make a local mdlFragInfo
	//	structure, read the frag info and then call "AddMesh()".
	//------------------------------------------------------------------------
	virtual void ReadMesh(chReader& i_Reader,
						  chDefs::Name i_Name,
						  chDefs::Version i_Version,
						  chDefs::Size i_Size);
	virtual void ReadNodeInfoProxy(chReader& i_Reader,
								  chDefs::Name i_Name,
								  chDefs::Version i_Version,
								  chDefs::Size i_Size);
	//------------------------------------------------------------------------
	// A Skin consists of a subdivision or mesh surface with morph targets  
	// and/or joint influences that is attached to the joint skeleton.
	//------------------------------------------------------------------------
	virtual void AddSkin(shared_ptr<mdlSubdivInfo> &i_SubdivInfo,
						 shared_ptr<smdlCharacterSkin> &i_SkinInfo);
	virtual void AddSkin(shared_ptr<mdlFragInfo> &i_MeshInfo,
						 shared_ptr<smdlCharacterSkin> &i_SkinInfo);

	//------------------------------------------------------------------------
	// Hair strands
	//------------------------------------------------------------------------
	virtual void AddHair(shared_ptr<mdlHairInfo> &i_HairInfo);

	//------------------------------------------------------------------------
	// The parser should contain an internal material table to use when parsing
	//	the file. The functions provide access to it.
	//------------------------------------------------------------------------
	virtual mdlMatInfoTable& GetMaterialTable();
	virtual const mdlMatInfoTable& GetMaterialTable() const;

private:
	fsLocator m_Locator;
	mdlMatInfoTable& m_MaterialTable;
	shared_ptr<mdlNodeInfo> m_RootNode;
	std::stack< shared_ptr<mdlNodeInfo> > m_NodeStack;
};
