/****************************************************************************\
**  mdlHierarchyParser.hpp
**
**      mdlHierarchyParser defines a way to parse hierarchies in files 
**	such that the reader can decide how to handle the levels (creates
**	graphics objects, convert and write to a new format, read into
**	data structures, etc.)
**
**	A user should derive from the class mdlHierarchyParser and then
**	pass the object to the static function LoadModel().
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_HIERARCHYPARSER_HPP
#error mdlHierarchyParser.hpp multiply included
#endif
#define MDL_HIERARCHYPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

#include <map>
#include <string>


//============================================================================
//============================================================================
class chReader;
class chWriter;
class fsLocator;
class maMatrix4x4;
class maVector3d;
class mdlFragInfo;
class mdlNodeInfoProxy;
class mdlSubdivInfo;
struct mdlHairInfo;
struct smdlCharacterSkin;


//============================================================================
//	Any of these mdlHierarchyParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
class mdlHierarchyParser
{
public:
	//------------------------------------------------------------------------
	//	LoadModel loads a hierarchical model from a file.  It will make 
	//	calls to the HierarchyParser object when it reaches each level in
	//	in the hierarchy.
	//------------------------------------------------------------------------
	static void LoadModel(	const fsLocator& i_File,
							mdlHierarchyParser& io_Parser );

	//------------------------------------------------------------------------
	// Reads a HLEV chunk of an existing open reader.
	//------------------------------------------------------------------------
	static void LoadHierarchy(mdlHierarchyParser& io_Parser,
							  chReader& i_Reader,
							  chDefs::Name i_Name,
							  chDefs::Version i_Version,
							  chDefs::Size i_Size);

	//------------------------------------------------------------------------
	// Parser has reached a node in the hierarchy, this could represent
	// the root node or a child of the current node.
	//------------------------------------------------------------------------
	virtual void CreateLevel(bool i_bIsJoint = false) = 0;

	//------------------------------------------------------------------------
	// Parser has finished reading a node and all of its children
	//	in the hierarchy, the new current node is now the parent of the 
	//	old current node.
	//------------------------------------------------------------------------
	virtual void FinishLevel() = 0;

	//------------------------------------------------------------------------
	// Set the name of the current node.
	//------------------------------------------------------------------------
	virtual void SetNodeName(const std::string& i_NodeName) = 0;

	//------------------------------------------------------------------------
	// Set the transformation of the current node.
	//------------------------------------------------------------------------
	virtual void SetNodeTransform(const maMatrix4x4& i_Transform) = 0;

	//------------------------------------------------------------------------
	// Set the rotate and scale pivot points for transformation.
	//------------------------------------------------------------------------
	virtual void SetNodePivots(const maVector3d& i_RotatePivot,
							   const maVector3d& i_ScalePivot, 
							   const maVector3d& i_RotatePivotTranslation, 
							   const maVector3d& i_ScalePivotTranslation) = 0;

	//------------------------------------------------------------------------
	// Set the joint orientation of the current node as euler angles.
	//------------------------------------------------------------------------
	virtual void SetNodeOrientation(float i_X, float i_Y, float i_Z) = 0;

	//------------------------------------------------------------------------
	// Set the orientation of the joint for applying scale animation.
	//------------------------------------------------------------------------
	virtual void SetScaleOrientation(float i_X, float i_Y, float i_Z) = 0;
	
	//------------------------------------------------------------------------
	// Set the inverse bind pose for a joint, used when doing skin animation.
	//------------------------------------------------------------------------
	virtual void SetInverseBindPose(const maMatrix4x4 &i_BindPose) = 0;

	//------------------------------------------------------------------------
	// Parser read a mesh fragment with the given geometry, assign it
	//	to the current node. 
	//------------------------------------------------------------------------
	virtual void AddMesh(mdlFragInfo& i_FragInfo) = 0;

	//------------------------------------------------------------------------
	// Assign the nodeInfoProxy to the current node
	//------------------------------------------------------------------------
	virtual void AddMeshReference(mdlNodeInfoProxy& i_Proxy) = 0;
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
	
	//------------------------------------------------------------------------
	// Similar to ReadMesh,
	// but calls AddMeshReference instead
	//------------------------------------------------------------------------
	virtual void ReadNodeInfoProxy(chReader& i_Reader,
		chDefs::Name i_Name,
		chDefs::Version i_Version,
		chDefs::Size i_Size);

	//------------------------------------------------------------------------
	// A Skin consists of a subdivision or mesh surface with morph targets  
	// and/or joint influences that is attached to the joint skeleton.
	//------------------------------------------------------------------------
	virtual void AddSkin(shared_ptr<mdlSubdivInfo> &i_SubdivInfo,
						 shared_ptr<smdlCharacterSkin> &i_Skin) = 0;
	virtual void AddSkin(shared_ptr<mdlFragInfo> &i_MeshInfo,
						 shared_ptr<smdlCharacterSkin> &i_Skin) = 0;

	//------------------------------------------------------------------------
	// Hair strands
	//------------------------------------------------------------------------
	virtual void AddHair(shared_ptr<mdlHairInfo> &i_HairInfo) = 0;

	//------------------------------------------------------------------------
	// Notification that a Material Table was read.
	//------------------------------------------------------------------------
	virtual void MaterialTableWasRead();

	//------------------------------------------------------------------------
	// The parser should contain an internal material table to use when parsing
	//	the file. The functions provide access to it.
	//------------------------------------------------------------------------
	virtual mdlMatInfoTable& GetMaterialTable() = 0;
	virtual const mdlMatInfoTable& GetMaterialTable() const = 0;

	//------------------------------------------------------------------------
	// Static write functions to write hierarchical models
	//------------------------------------------------------------------------ 
	static void WriteLevelChunkHeader(chWriter& o_Writer);
	static void WriteJointChunkHeader(chWriter& o_Writer);
	static void WriteNodeName(chWriter& o_Writer, const std::string& i_NodeName);
	static void WriteNodeTransform(chWriter& o_Writer, const maMatrix4x4& i_Transform);
	static void WriteNodePivots(chWriter& o_Writer, 
								const maVector3d &i_RotatePivot, 
								const maVector3d &i_ScalePivot, 
								const maVector3d &i_RotatePivotTranslation, 
								const maVector3d &i_ScalePivotTranslation);
	static void WriteNodeOrientation(chWriter& o_Writer, const maVector3d &i_JointOrientation);
	static void WriteJointScaleOrientation(chWriter& o_Writer, const maVector3d &i_JointScaleOrientation);
	static void WriteInverseBindPose(chWriter& o_Writer, const maMatrix4x4& i_InverseBindPose);
};
