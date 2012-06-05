/*****************************************************************************
**	xfrmTransformMgr.hpp
**
**	Keeps track of parent transform nodes that can apply a transformation
**  to child objects and other child transform nodes.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef XFRM_TRANSFORMMGR_HPP
#error xfrmTransformMgr.hpp multiply included
#endif
#define XFRM_TRANSFORMMGR_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/Ma/maMatrix4x4.hpp"
#endif 

#include <boost/function.hpp>
#include <vector>


//============================================================================
//============================================================================
class nameObject;
class xfrmTransformGroup;
class xfrmTransformNodeCallback;
class g3dSceneNode;

//============================================================================
// Handle to refer to layer
//============================================================================
//typedef void* xfrmTransformHandle;


//============================================================================
//============================================================================
namespace xfrmTransformMgr
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize();

	//--------------------------------------------------------------------
	//	Clear all transforms and objects
	//--------------------------------------------------------------------
	void Clear();


//------------------------------------------------------------------------
// Systems should call these functions in order to submit
// and organize objects from different systems.
//------------------------------------------------------------------------

	//--------------------------------------------------------------------
	//  Add named object to list of things that can be transformed.
	//	One version is for geometry passing in the g3dSceneNode,
	//	The other version passes in a function for updating the object.
	//--------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
			    g3dSceneNode* i_pSceneNode,
				xfrmTransformNodeCallback* i_pCallback);
	void  AddObject(nameObject* i_pNameObj, 
				xfrmTransformNodeCallback* i_pCallback);

	//--------------------------------------------------------------------
	//	Remove object from manager (removing from all transforms)
	//--------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj);

	//--------------------------------------------------------------------
	//	Change assignments from OldNameObj to NewNameObj (both of which
	//	should be registered with the manager at the point of calling 
	//	this function). This is used when reloading or replacing 
	//	geometry in a system.
	//--------------------------------------------------------------------
	void ReplaceObject(nameObject* i_pOldNameObj, 
					   nameObject* i_pNewNameObj);

//
//------------------------------------------------------------------------
// The user interface can then manipulate the transformings with the
// following functions by using the names of the transforms and objects
//------------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Returns true if non-empty and unique name for a transform.
	//--------------------------------------------------------------------
	bool IsValidTransformName(const std::string &i_Name);

	//--------------------------------------------------------------------
	//	Create a named transform
	//--------------------------------------------------------------------
	xfrmTransformGroup*  CreateTransform( nameObject* i_pNameObject, 
					g3dSceneNode* i_pSceneNode,
					xfrmTransformNodeCallback* i_pCallback );

	//--------------------------------------------------------------------
	//	Destroy named transform, all objects in this set become 
	//	"unassigned"
	//--------------------------------------------------------------------
	void  DeleteTransform(const nameString& i_Name);

	//--------------------------------------------------------------------
	//	Destroy named transform, using std::string
	//	Note: Try to use the nameString function.
	//--------------------------------------------------------------------
	//void  DeleteTransform(const std::string& i_Name);

	//--------------------------------------------------------------------
	//	Clear named transform of objects, but keep the transform
	//--------------------------------------------------------------------
	void  ClearTransform(const nameString& i_Name );

	//--------------------------------------------------------------------
	//	Rename a transform
	//--------------------------------------------------------------------
	//void  RenameTransform(const nameString& i_OldName, const nameString& i_NewName);

	//--------------------------------------------------------------------
	//	Add object to the given transform. Returns true if successful.
	//  Is i_bPreserveTransform is true, the node's local transform is
	//	changed in order to preserve its world space position.
	//--------------------------------------------------------------------
	bool  AddNodeToTransform(const nameString& i_TransformName, 
							 const nameString& i_ObjectName,
							 bool i_bPreserveTransform = false);

	//------------------------------------------------------------------------
	//	Remove object from its parent transform
	//------------------------------------------------------------------------
	void  RemoveNodeFromParent(const nameString& i_ObjectName );

	//--------------------------------------------------------------------
	//  Convert from handle to name
	//--------------------------------------------------------------------
	//const nameString& GetTransformName(xfrmTransformHandle i_Handle);
	//void SetTransformName(xfrmTransformHandle i_Handle, const nameString& i_Name);

//--------------------------------------------------------------------
// These functions get the current state of the transform transformings
// in order to be displayed to the user.
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Return number of transforms
	//--------------------------------------------------------------------
	int GetNumTransforms();

	//--------------------------------------------------------------------
	//  Get names of transforms
	//--------------------------------------------------------------------
	void GetTransformNames(std::vector<nameString> &o_Names);

	//--------------------------------------------------------------------
	//  Get names of objects in a transform (including child transforms). 
	//--------------------------------------------------------------------
	void GetNodesInTransform(const nameString& i_TransformName, 
						 std::vector<nameString> &o_ObjectNames);

	//--------------------------------------------------------------------
	// Get parent transform for the given object or transform
	//--------------------------------------------------------------------
	bool GetParentForNode(const nameString &i_ObjectName, 
							 nameString &o_TransformName);

	//--------------------------------------------------------------------
	// Return true if i_PotentialParent is a parent node of i_NodeName
	// recursively searching up the tree to the root.
	// This should be used to make sure that you do not attempt to add 
	// parent node under a child node and create a circular graph.
	//--------------------------------------------------------------------
	bool IsAncestorOfNode(const nameString &i_NodeName, 
						const nameString &i_PotentialParent);

	//--------------------------------------------------------------------
	//  Get list of all names of leaf objects registered in the manager
	//--------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames);

	//--------------------------------------------------------------------
	//	Test if name is component that can be added to light sets
	//--------------------------------------------------------------------
	bool  IsTransform(const nameString& i_Name);
	bool  IsObject(const nameString& i_Name);

//------------------------------------------------------------------------
// These functions compute state values related to the hierarchy
//------------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Return whether the inherited pickable state from the parent nodes 
	// of this node is pickable or not.
	//--------------------------------------------------------------------
	bool GetParentPickable(const nameString &i_ObjectName);

	//--------------------------------------------------------------------
	//  Get sum of matrices of all parents of this node. 
	//  Returns false and identity matrix if this node 
	//	does not have a parent transform.
	//--------------------------------------------------------------------
	bool ComputeExclusiveMatrixForNode(const nameString &i_ObjectName, 
										 maMatrix4x4 &o_Transformation);

	//--------------------------------------------------------------------
	// Call update function for all nodes with a callback function.
	// This should be called once per frame between the timeline think 
	// and the proxy update.
	//--------------------------------------------------------------------
	void UpdateTransforms();

	//--------------------------------------------------------------------
	// Return true if this group's children have a pivot point based 
	// icon geometry instead of true geometry in the scnee graph.
	// If so, return pivot point in the o_Pivot argument.
	//--------------------------------------------------------------------
	bool GetIconPivotPoint(xfrmTransformGroup &i_Group, maPoint3d& o_Pivot); 

//--------------------------------------------------------------------
//	Access to scene hierarchy
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Get vector of all top level group nodes 
	//	(group nodes with no parent)
	//--------------------------------------------------------------------
	void GetRootTransforms(std::vector<xfrmTransformGroup*> &o_RootNodes);

	//--------------------------------------------------------------------
	// Get vector of all top level objects (leaf nodes with no parent)
	//--------------------------------------------------------------------
	void GetUngroupedObjects(std::vector<nameString> &o_ObjectNames);

//--------------------------------------------------------------------
//	Utilities
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Set editor visibility for all children of this node
	//--------------------------------------------------------------------
	void SetChildrenVisibleInEditor(xfrmTransformGroup &i_Group, bool i_bVisible);

}	// end of namespace
