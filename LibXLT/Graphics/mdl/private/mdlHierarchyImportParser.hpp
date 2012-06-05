/****************************************************************************\
**	mdlHierarchyImportParser.hpp
**
**		mdlHierarchyImportParser provides a derivation in order to
**	parser hierarchy files into mdlNodeInfo data structures.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_HIERARCHYIMPORTPARSER_HPP
#error mdlHierarchyImportParser.hpp multiply included
#endif
#define MDL_HIERARCHYIMPORTPARSER_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 
#ifndef MDL_HIERARCHYPARSER_HPP
#include "Graphics/mdl/private/mdlHierarchyParser.hpp"
#endif 
#ifndef MDL_SKININFO_HPP
#include "Graphics/mdl/mdlSkinInfo.hpp"
#endif 

#include <stack>
#include <vector>


//============================================================================
//============================================================================
class entFragInfoSink;
class fsResourceFinder;
class g3dFragment;
class g3dSceneNode;
class matMaterial;
class matTexture;
class mdlNodeInfoProxy;
//class smdlJoint;


//============================================================================
//============================================================================
class mdlHierarchyImportParser : public mdlHierarchyParser
{
public:
	//------------------------------------------------------------------------
	// Constructor takes material table by reference to fill in
	// and locator for use when throwing exceptions.
	//------------------------------------------------------------------------
	mdlHierarchyImportParser(const fsLocator& i_Locator,
							 //const fsResourceFinder& i_TextureFinder,
							 mdlMatInfoTable& o_MaterialTable,
							 std::vector<mdlSkinInfo>& o_SkinnedSurfaces,
							 std::vector< shared_ptr<mdlHairInfo> >& o_HairSurfaces,
							 std::vector<g3dFragment*>& o_Fragments,
							 std::vector<matMaterial*>& o_Materials,
							 //std::vector<matTexture*>& o_Textures,
							 entFragInfoSink* o_Sink = NULL);

	//------------------------------------------------------------------------
	// Access to the root node that was parsed
	//------------------------------------------------------------------------
	g3dSceneNode* GetRootNode();

	//------------------------------------------------------------------------
	// Access to the root joints for each joint chain or skeleton
	//	that was parsed
	//------------------------------------------------------------------------
	//std::vector< smdlJoint* >& GetRootJoints();

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
	virtual void SetInverseBindPose(const maMatrix4x4 &i_InvBindPose);

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
	// A Skin consists of a subdivision or mesh surface with morph targets  
	// and/or joint influences that is attached to the joint skeleton.
	//------------------------------------------------------------------------
	virtual void AddSkin(shared_ptr<mdlSubdivInfo> &i_SubdivInfo,
						 shared_ptr<smdlCharacterSkin> &i_Skin);
	virtual void AddSkin(shared_ptr<mdlFragInfo> &i_MeshInfo,
						 shared_ptr<smdlCharacterSkin> &i_Skin);

	//------------------------------------------------------------------------
	// Hair strands
	//------------------------------------------------------------------------
	virtual void AddHair(shared_ptr<mdlHairInfo> &i_HairInfo);

	//------------------------------------------------------------------------
	// Notification that a Material Table was read.
	//------------------------------------------------------------------------
	virtual void MaterialTableWasRead();

	//------------------------------------------------------------------------
	// The parser should contain an internal material table to use when parsing
	//	the file. The functions provide access to it.
	//------------------------------------------------------------------------
	virtual mdlMatInfoTable& GetMaterialTable();
	virtual const mdlMatInfoTable& GetMaterialTable() const;

private:
	fsLocator m_Locator;
	mdlMatInfoTable& m_MaterialTable;
	//const fsResourceFinder& m_TextureFinder;
	std::vector< mdlSkinInfo >& m_SkinnedSurfaces;
	std::vector< shared_ptr<mdlHairInfo> >& m_HairSurfaces;
	std::vector<g3dFragment*>& m_Fragments;
	std::vector<matMaterial*>& m_Materials;
	//std::vector<matTexture*>& m_Textures;
	entFragInfoSink* m_Sink;

	g3dSceneNode* m_pRootNode;
	//std::vector< smdlJoint* > m_RootJoints;
	std::stack< g3dSceneNode* > m_NodeStack;
	//std::stack< smdlJoint* > m_JointStack;
	
};

//----------------------------------------------------------------------------
//	Split the string or wstring into a vector
//	of components using the given delimiter
//----------------------------------------------------------------------------
template< typename TString >
void SplitComponents(const  TString &i_String, const TString &i_Delimiter, std::vector<TString> &components )
{
	size_t startPos = 0;
	size_t delimitPos =0;
	while ( delimitPos != TString::npos )
	{
		delimitPos = i_String.find_first_of( i_Delimiter, startPos);
		if ( delimitPos != TString::npos )
		{
			TString s1 = i_String.substr( startPos, delimitPos - startPos );
			components.push_back( s1 );
			startPos = delimitPos + 1;
		} else
		{
			TString s1 = i_String.substr( startPos );
			components.push_back( s1 );
		}
	}
}