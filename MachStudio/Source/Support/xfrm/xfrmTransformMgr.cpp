/*****************************************************************************
**	xfrmTransformMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/xfrm/xfrmTransformMgr.hpp"
#include "Support/xfrm/xfrmTransformGroup.hpp"

#include "Support/vis/visMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameMgr.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Tool/api3d/api3dScene.hpp"

#include <list>
#include <set>


//============================================================================
//============================================================================
namespace xfrmTransformMgr
{
	namespace
	{

		// We want to have a mapping from nameString to xfrmTransformNode*
		// but only really the nameObject* is constant - its nameString may change.
		// So, a std::map from nameObject* to xfrmTransformNode* doesn't work well.
		std::vector<xfrmTransformNode*>  l_Objects;
		std::vector<xfrmTransformGroup*> l_Transforms;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		template<class S> 
		class nameobj_search
		{
		public:
			nameobj_search(const nameString& i_Name) : m_Name(i_Name) {};
			bool operator () ( S* i_Set )
			{
				if (i_Set->m_pNameObject != 0)
                    return (m_Name == i_Set->m_pNameObject->GetName());
				else
					return false;
			}
			nameString m_Name;
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		template<class S> 
		class objptr_search
		{
		public:
			objptr_search(nameObject* i_pObject) : m_pObject(i_pObject) {};
			bool operator () ( S* i_Set )
			{
				return (m_pObject == i_Set->m_pNameObject);
			}
			nameObject* m_pObject;
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//template<class S> 
		//class scenenodeptr_search
		//{
		//public:
		//	scenenodeptr_search(g3dSceneNode* i_pObject) : m_pObject(i_pObject) {};
		//	bool operator () ( S* i_Set )
		//	{
		//		return (m_pObject == i_Set->m_pSceneNode);
		//	}
		//	g3dSceneNode* m_pObject;
		//};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		template<class S, class Pred>
		S* Find(const std::vector<S*>& i_Container, Pred i_Predicate)
		{
			std::vector<S*>::const_iterator transform_it = 
				std::find_if(i_Container.begin(), i_Container.end(), i_Predicate);
			if  (transform_it != i_Container.end())
			{
				S *pTransform = (*transform_it);
				return pTransform;
			}
			return NULL;
		}

		//--------------------------------------------------------------------
		// Find transform node by name. This could be a leaf object or
		// a transform node.
		//--------------------------------------------------------------------
		xfrmTransformNode* find_node(const nameString &i_NodeName)
		{
			if (xfrmTransformGroup *pTransform = Find(l_Transforms, nameobj_search<xfrmTransformGroup>(i_NodeName)))
				return pTransform;
			if (xfrmTransformNode *pNode = Find(l_Objects, nameobj_search<xfrmTransformNode>(i_NodeName)))
				return pNode;
			return NULL;
		}

		//--------------------------------------------------------------------
		// Return true if i_pNode is a parent of i_pGroup.
		// Used to prevent circular parenting relationships.
		//--------------------------------------------------------------------
		bool is_parent(xfrmTransformGroup *i_pTransform, xfrmTransformNode *i_pNode)
		{
			//xfrmTransformGroup *pParent = i_pTransform->m_pParentGroup;
			xfrmTransformGroup *pParent = i_pTransform; // start with node to check for identical - also circular problem
			while (pParent)
			{
				if (pParent == i_pNode)
					return true;
				pParent = pParent->m_pParentGroup;
			}
			return false;
		}

		//--------------------------------------------------------------------
		// Add object to transform - this involves removing the 
		// scene node from its old parent and adding it to the new one
		// in addition to reorganizing the xfrmTransformNode members.
		//--------------------------------------------------------------------
		void add_object_to_transform(xfrmTransformGroup &i_Transform, 
									 xfrmTransformNode &i_Node,
									 bool i_bPreserveTransform = false)
		{
			// Remove from old parent if it exists
			xfrmTransformGroup *pOldTransform = i_Node.m_pParentGroup;
			maMatrix4x4 old_xform, new_xform;
			if (pOldTransform != &i_Transform)
			{
				if (pOldTransform)
				{
					envSTLHelpers::RemoveOneValue(pOldTransform->m_ChildNodes, &i_Node);
					old_xform = pOldTransform->GetTotalTransformation(); 
				}

				// Reparent the g3dSceneNode, if it is non-NULL (lights and cameras have NULL scene nodes)
				if (i_Node.m_pSceneNode)
				{
					DBG_ASSERT(i_Transform.m_pSceneNode, "NULL transform nodes should not have been added to manager");
					g3dSceneNode *pSceneNode = i_Node.m_pSceneNode;
					g3dSceneNode *pParent = pSceneNode->GetParent();
					pParent->RemoveChild(pSceneNode);
					i_Transform.m_pSceneNode->AddChild(pSceneNode);
				}

				// Add record keeping to the xfrm classes
				i_Transform.m_ChildNodes.push_back(&i_Node);
				i_Node.m_pParentGroup = &i_Transform;
				
				// Move object from old transformation space into new transformation space
				if (i_bPreserveTransform)
				{
					new_xform = i_Transform.GetTotalTransformation(); 
					new_xform.Invert();
					maMatrix4x4 total_offset = old_xform * new_xform; // or new_xform * old_xform ?
					if (!total_offset.IsIdentity())
					{
						// Apply a transformation to preserve the world space 
						// position of the node/object.
						//i_Node.ApplyTransform(total_offset);
						if (i_Node.m_pCallback)
							i_Node.m_pCallback->ApplyTransformation(total_offset);
					}
				}
			}
		}

		//--------------------------------------------------------------------
		// Remove object from transform - in this case, the object is
		// not being reparented to a new node, they need to be put back
		// under the scene world root.
		//--------------------------------------------------------------------
		void remove_object_from_transform(xfrmTransformNode &i_Node)
		{
			// Remove from old parent if it exists
			xfrmTransformGroup *pOldParent = i_Node.m_pParentGroup;
			if (pOldParent)
			{
				envSTLHelpers::RemoveOneValue(pOldParent->m_ChildNodes, &i_Node);
				i_Node.m_pParentGroup = NULL;

				// Reparent the g3dSceneNode, if it is non-NULL
				if (i_Node.m_pSceneNode)
				{
					g3dSceneNode *pSceneNode = i_Node.m_pSceneNode;
					g3dSceneNode *root_node = api3dScene::GetRoot( api3dScene::WorldLayerIndex() );
					g3dSceneNode *pParent = pSceneNode->GetParent();
					if (pParent != root_node)
					{
						pParent->RemoveChild(pSceneNode);
						root_node->AddChild(pSceneNode);	
					}
				}
			}
		}

		//--------------------------------------------------------------------
		// Compute total transformation of this node plus its parents
		//--------------------------------------------------------------------
		//void compute_total_matrix(const xfrmTransformGroup *i_pTransform, 
		//						  maMatrix4x4 &io_Transformation)
		//{
		//	const xfrmTransformGroup* cur_node = i_pTransform;
		//	while( cur_node )
		//	{
		//		io_Transformation *= cur_node->m_Matrix;
		//		cur_node = cur_node->m_pParentGroup;
		//	}
		//}
		
		//--------------------------------------------------------------------
		// Return true if i_PotentialParent is a parent node of i_NodeName
		// recursively searching up the tree to the root.
		// This should be used to make sure that you do not attempt to add 
		// parent node under a child node and create a circular graph.
		//--------------------------------------------------------------------
		bool is_ancestor_of_node(const xfrmTransformNode* i_pNode, 
							     const xfrmTransformNode* i_pPotentialParent)
		{
			xfrmTransformNode *pParent = i_pNode->m_pParentGroup;
			if (!pParent)
				return false;
			else if (pParent == i_pPotentialParent)
				return true;
			else
			{
				return is_ancestor_of_node(pParent, i_pPotentialParent);
			}
			return false;
		}

	}	// end of namespace


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize()
	{
		// We could alternatively create an "unassigned" transform with
		// an empty name. Then all objects that are not in a set would belong
		// to that xfrmTransformGroup.  So far, I don't see that to be an advantage.
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeInitialize()
	{
		Clear();
	}

	//--------------------------------------------------------------------
	//	Clear all transforms and objects
	//--------------------------------------------------------------------
	void Clear()
	{
		// Clean up
		envSTLHelpers::DeleteContainer(l_Transforms);
		envSTLHelpers::DeleteContainer(l_Objects);
	}

	//------------------------------------------------------------------------
	//  Add named object to list of things that can be transformed,
	//	This version is for geometry passing in the g3dSceneNode,
	//------------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				    g3dSceneNode* i_pSceneNode,
					xfrmTransformNodeCallback* i_pCallback)
				    //boost::function0<void> i_UpdateFunction,
					//boost::function1<void, const maMatrix4x4&> i_TransformFunction)
	{
		if (i_pNameObj == NULL)
			return;
		// Only geometry will have i_pObject, lights and cameras will have NULL pointers
		//if (i_pSceneNode == NULL)
		//	return;

		l_Objects.push_back(new xfrmTransformNode(i_pNameObj, i_pSceneNode, i_pCallback));
			//i_UpdateFunction, i_TransformFunction));
	}
	//--------------------------------------------------------------------
	//  Add named object to list of things that can be transformed.
	//	This version passes in a function for updating the object.
	//--------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
					xfrmTransformNodeCallback* i_pCallback)
				    //boost::function0<void> i_UpdateFunction,
					//boost::function1<void, const maMatrix4x4&> i_TransformFunction)
	{
		if (i_pNameObj == NULL)
			return;
		// No g3dSceneNode is this variation
		l_Objects.push_back(new xfrmTransformNode(i_pNameObj, NULL, i_pCallback));
													//i_UpdateFunction, i_TransformFunction));
	}

	//--------------------------------------------------------------------
	//	Remove object from manager  (removing from all transforms)
	//--------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj)
	{
		if (i_pNameObj == NULL)
			return;

		// Find named object
		// (A name search is not enough here, needs to use object pointer)
		//
		if (xfrmTransformNode *pObject = Find(l_Objects, objptr_search<xfrmTransformNode>(i_pNameObj)))
		{
			//	Found the name, so now remove from containing transform
			remove_object_from_transform(*pObject);

			envSTLHelpers::RemoveOneValue(l_Objects, pObject);
			delete pObject;
		}
	}

	//------------------------------------------------------------------------
	//	Change assignments from OldNameObj to NewNameObj (both of which
	//	should be registered with the manager at the point of calling 
	//	this function). This is used when reloading or replacing 
	//	geometry in a system.
	//------------------------------------------------------------------------
	void ReplaceObject(nameObject* i_pOldObject, 
					   nameObject* i_pNewObject)
	{
		if (i_pOldObject == NULL)
			return;
		if (i_pNewObject == NULL)
			return;

		// Find named objects (using pointer, since they both have same nameString)
		std::vector<xfrmTransformNode*>::iterator obj1_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), objptr_search<xfrmTransformNode>(i_pOldObject));
		std::vector<xfrmTransformNode*>::iterator obj2_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), objptr_search<xfrmTransformNode>(i_pNewObject));

		if (obj1_it != l_Objects.end() && obj2_it != l_Objects.end())
		{
			xfrmTransformGroup* pTransform = (*obj1_it)->m_pParentGroup; 
			xfrmTransformNode* pNewObject = (*obj2_it);

			if (pTransform && pNewObject)
			{
				// Add new object to transform
				const bool bPreserveTransform = false;
				add_object_to_transform(*pTransform, *pNewObject, bPreserveTransform);
			}
		}
	}

	//------------------------------------------------------------------------
	// Returns true if non-empty and unique name for a transform.
	//------------------------------------------------------------------------
	bool IsValidTransformName(const std::string &i_Name)
	{
		if (i_Name.empty()) return false;

		std::vector<xfrmTransformGroup*>::iterator it, end = l_Transforms.end();
		for (it = l_Transforms.begin(); it != end; ++it)
		{
			if ((*it)->m_pNameObject->GetName().GetString() == i_Name)
				return false;
		}
		return true;
	}

	//------------------------------------------------------------------------
	//	Create a named transform
	//------------------------------------------------------------------------
	xfrmTransformGroup*  CreateTransform( nameObject* i_pNameObject, 
										  g3dSceneNode* i_pSceneNode,
										  xfrmTransformNodeCallback* i_pCallback)
	{
		DBG_ASSERT(i_pNameObject, "A transform needs a valid name object");
		DBG_ASSERT(i_pSceneNode, "A transform needs a valid scene node");
		//bga - empty name is valid when creating a transform from user interface, it will get a name soon.
		//DBG_ASSERT(!i_pNameObject->GetName().IsEmpty(), "A transform needs a valid name");

		// Check for unique name, GUI should use IsValidTransformName
		std::vector<xfrmTransformGroup*>::iterator transform_it = 
			std::find_if(l_Transforms.begin(), l_Transforms.end(), nameobj_search<xfrmTransformGroup>(i_pNameObject->GetName()));
		if (transform_it != l_Transforms.end())
		{
			DBG_WARNING("Transform names should be unique: " << i_pNameObject->GetName().GetString());
		}

		xfrmTransformGroup *pTransform = new xfrmTransformGroup(i_pNameObject, i_pSceneNode, i_pCallback);
		l_Transforms.push_back(pTransform);
		//nameMgr::RegisterName( pTransform->m_Name );

		//return reinterpret_cast<xfrmTransformHandle>(pTransform);
		return pTransform;
	}

	
	//------------------------------------------------------------------------
	//	Destroy named transform, all objects in this set become 
	//	"unassigned"
	//------------------------------------------------------------------------
	void  DeleteTransform(const nameString& i_Name)
	{
		// Find named transform
		std::vector<xfrmTransformGroup*>::iterator transform_it = 
			std::find_if(l_Transforms.begin(), l_Transforms.end(), nameobj_search<xfrmTransformGroup>(i_Name));
		if  (transform_it != l_Transforms.end())
		{
			xfrmTransformGroup *pTransform = (*transform_it);

			if (pTransform)
			{
				// Remove this transform from its parent, if non-NULL
				if (pTransform->m_pParentGroup)
					remove_object_from_transform(*pTransform);

				// Make a copy of the child nodes list in order to remove them
				// because the lines below will alter the transform's list itself
				// and invalid the iterator if we did it directly
				std::vector<xfrmTransformNode*> child_nodes = pTransform->m_ChildNodes;

				// Remove this transform from the objects' containing transforms
				std::vector<xfrmTransformNode*>::iterator obj_it, obj_end = child_nodes.end();
				for (obj_it = child_nodes.begin(); obj_it != obj_end; ++obj_it)
				{
					xfrmTransformNode* pNode = (*obj_it);
					remove_object_from_transform(*pNode);
				}
			}

			l_Transforms.erase(transform_it);
			delete pTransform;
		}
	}

	//------------------------------------------------------------------------
	//	Destroy named transform, using std::string
	//------------------------------------------------------------------------
	//void  DeleteTransform(const std::string& i_Name)
	//{
	//	std::vector<xfrmTransformGroup*>::iterator it, end = l_Transforms.end();
	//	for (it = l_Transforms.begin(); it != end; ++it)
	//	{
	//		xfrmTransformGroup *pTransform = (*it);
	//		if (pTransform->m_pNameObject->GetName().GetString() == i_Name)
	//		{
	//			DeleteTransform(pTransform->m_pNameObject->GetName());
	//			break;
	//		}
	//	}
	//}

	//--------------------------------------------------------------------
	//	Clear named transform of objects, but keep the transform
	//--------------------------------------------------------------------
	void ClearTransform(const nameString& i_TransformName )
	{
		if (xfrmTransformGroup *pTransform = Find(l_Transforms, nameobj_search<xfrmTransformGroup>(i_TransformName)))
		{
			// Remove this transform from the objects' containing transforms
			std::vector<xfrmTransformNode*>::iterator obj_it, obj_end = pTransform->m_ChildNodes.end();
			for (obj_it = pTransform->m_ChildNodes.begin(); obj_it != obj_end; ++obj_it)
			{
				xfrmTransformNode* pNode = (*obj_it);
				remove_object_from_transform(*pNode);
			}

			// Now, clear the list
			pTransform->m_ChildNodes.clear();
		}
	}

	//------------------------------------------------------------------------
	//	Rename a transform
	//------------------------------------------------------------------------
	//void  RenameTransform(const nameString& i_OldName, const nameString& i_NewName)
	//{
	//	std::vector<xfrmTransformGroup*>::iterator it, end = l_Transforms.end();
	//	for (it = l_Transforms.begin(); it != end; ++it)
	//	{
	//		xfrmTransformGroup *pTransform = (*it);
	//		if (pTransform->m_pNameObject->GetName() == i_OldName)
	//		{
	//			pTransform->m_Name = i_NewName;
	//			break;
	//		}
	//	}
	//}

	//------------------------------------------------------------------------
	// Remove all transforms (preparing for a new scene)
	//------------------------------------------------------------------------
	//void ClearAllTransforms()
	//{
	//	// Disconnect all transforms before destroying
	//	std::vector<xfrmTransformGroup*>::iterator it, end = l_Transforms.end();
	//	for (it = l_Transforms.begin(); it != end; ++it)
	//	{
	//		xfrmTransformGroup *pTransform = (*it);

	//		// TODO - anything to do here? the destructor takes care of deleting all the transforms + objects
	//	}

	//	//	delete all the transforms
	//	envSTLHelpers::DeleteContainer(l_Transforms);
	//}

	//------------------------------------------------------------------------
	//	Add object to the given transform
	//------------------------------------------------------------------------
	//void  AddNodeToTransform(	const nameString& i_TransformName,
	//						g3dSceneNode* i_pPickObject )
	//{
	//	// Find named transform
	//	if (xfrmTransformGroup *pTransform = Find(l_Transforms, nameobj_search<xfrmTransformGroup>(i_TransformName)))
	//	{
	//		// Find named object
	//		if (xfrmTransformNode *pNode = Find(l_Objects, scenenodeptr_search<xfrmTransformNode>(i_pPickObject)))
	//		{
	//			// Add object to transform
	//			add_object_to_transform(*pTransform, *pNode);
	//		}
	//	}
	//}

	//--------------------------------------------------------------------
	//	Add object to the given transform
	//  Is i_bPreserveTransform is true, the node's local transform is
	//	changed in order to preserve its world space position.
	//--------------------------------------------------------------------
	bool AddNodeToTransform(const nameString& i_TransformName, 
							const nameString& i_ObjectName,
							bool i_bPreserveTransform )
	{
		// Find named transform
		if (xfrmTransformGroup *pTransform = Find(l_Transforms, nameobj_search<xfrmTransformGroup>(i_TransformName)))
		{
			// Find named object or transform
			if (xfrmTransformNode *pNode = find_node(i_ObjectName))
			{
				// Make sure that the new parent is not already a child of this node
				// (which would make a circular graph).
				if (!is_parent(pTransform, pNode))
				{
					// Add object to transform
					add_object_to_transform(*pTransform, *pNode, i_bPreserveTransform);
					return true;
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------
	//	Remove object from its parent transform
	//------------------------------------------------------------------------
	void  RemoveNodeFromParent(const nameString& i_ObjectName )
	{
		// Find named object or transform
		if (xfrmTransformNode *pNode = find_node(i_ObjectName))
		{
			// Remove object from transform
			remove_object_from_transform(*pNode);
		}
	}


	//--------------------------------------------------------------------
	//  Convert from handle to name
	//--------------------------------------------------------------------
	//const nameString& GetTransformName(xfrmTransformHandle i_Handle)
	//{
	//	xfrmTransformGroup *pTransform = reinterpret_cast<xfrmTransformGroup*>(i_Handle);
	//	return pTransform->m_pNameObject->GetName();
	//}
	//void SetTransformName(xfrmTransformHandle i_Handle, const nameString& i_Name)
	//{
	//	xfrmTransformGroup *pTransform = reinterpret_cast<xfrmTransformGroup*>(i_Handle);
	//	pTransform->m_Name = i_Name;
	//}

	//------------------------------------------------------------------------
	// Return number of transforms
	//------------------------------------------------------------------------
	int GetNumTransforms()
	{
		return l_Transforms.size();
	}

	//------------------------------------------------------------------------
	//  Get names of transforms
	//------------------------------------------------------------------------
	void GetTransformNames(std::vector<nameString> &o_Names)
	{
		std::vector<xfrmTransformGroup*>::iterator it, end = l_Transforms.end();
		for (it = l_Transforms.begin(); it != end; ++it)
		{
			o_Names.push_back((*it)->m_pNameObject->GetName());		
		}
	}

	//------------------------------------------------------------------------
	//  Get names of objects in a transform (including child transforms). 
	//------------------------------------------------------------------------
	void GetNodesInTransform(const nameString& i_TransformName, 
						 std::vector<nameString> &o_ObjectNames)
	{
		// Find named transform	
		if (xfrmTransformGroup *pTransform = Find(l_Transforms, nameobj_search<xfrmTransformGroup>(i_TransformName)))
		{
			o_ObjectNames.resize(pTransform->m_ChildNodes.size());
			int i=0;
			std::vector<xfrmTransformNode*>::iterator it, end = pTransform->m_ChildNodes.end();
			for (it = pTransform->m_ChildNodes.begin(); it != end; ++it)
			{
				if ((*it)->m_pNameObject != 0)
					o_ObjectNames[i++] = (*it)->m_pNameObject->GetName();
			}
		}
	}

	//--------------------------------------------------------------------
	// Get parent transform for the given object
	//--------------------------------------------------------------------
	bool GetParentForNode(const nameString &i_ObjectName, 
							 nameString &o_TransformName)
	{	
		// Find named object or transform
		if (xfrmTransformNode *pNode = find_node(i_ObjectName))
		{
			if (pNode->m_pParentGroup)
			{
				o_TransformName = pNode->m_pParentGroup->m_pNameObject->GetName();
				return true;
			}
		}
		return false;
	}
	
	//--------------------------------------------------------------------
	// Return true if i_PotentialParent is a parent node of i_NodeName
	// recursively searching up the tree to the root.
	// This should be used to make sure that you do not attempt to add 
	// parent node under a child node and create a circular graph.
	//--------------------------------------------------------------------
	bool IsAncestorOfNode(const nameString &i_NodeName, 
						  const nameString &i_PotentialParent)
	{
		// Find named object or transform
		xfrmTransformNode *pNode = find_node(i_NodeName);
		xfrmTransformNode *pPotentialParent = find_node(i_PotentialParent);
		if (pNode && pPotentialParent)
		{
			return is_ancestor_of_node(pNode, pPotentialParent);
		}
		return false;
	}

	//------------------------------------------------------------------------
	//  Get list of all names of leaf objects registered in the manager
	//------------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames)
	{
		std::vector<xfrmTransformNode*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			if ((*it)->m_pNameObject != 0)
				o_ObjectNames.push_back((*it)->m_pNameObject->GetName());
		}
	}

	//--------------------------------------------------------------------
	//	Test if name is component that can be added to light sets
	//--------------------------------------------------------------------
	bool  IsTransform(const nameString& i_Name)
	{
		xfrmTransformGroup *pTransform = Find(l_Transforms, nameobj_search<xfrmTransformGroup>(i_Name));
		return (pTransform != NULL);
	}
	bool  IsObject(const nameString& i_Name)
	{
		xfrmTransformNode *pNode = Find(l_Objects, nameobj_search<xfrmTransformNode>(i_Name));
		return (pNode != NULL);
	}


	//--------------------------------------------------------------------
	// Return whether the inherited pickable state from the parent nodes 
	// of this node is pickable or not.
	//--------------------------------------------------------------------
	bool GetParentPickable(const nameString &i_ObjectName)
	{
		// Find named object or transform
		if (xfrmTransformNode *pNode = find_node(i_ObjectName))
		{
			if (pNode->m_pParentGroup)
			{
				return pNode->m_pParentGroup->GetPickable();
			}
		}
		// If no parent, then default to "pickable == true"
		return true;
	}

	//--------------------------------------------------------------------
	//  Get sum of matrices of all parents of this node. 
	//  Returns false and identity matrix if this node 
	//	does not have a parent transform.
	//--------------------------------------------------------------------
	bool ComputeExclusiveMatrixForNode(const nameString &i_ObjectName, 
										 maMatrix4x4 &o_Transformation)
	{
		// Find named object or transform
		if (xfrmTransformNode *pNode = find_node(i_ObjectName))
		{
			if (pNode->m_pParentGroup)
			{
				//compute_total_matrix(pNode->m_pParentGroup, xform);
				o_Transformation = pNode->m_pParentGroup->GetTotalTransformation(); // caches result
				return true;
			}
		}
		o_Transformation = maMatrix4x4();
		return false;
	}

	//--------------------------------------------------------------------
	// Call update function for all nodes with a callback function.
	// This should be called once per frame between the timeline think 
	// and the proxy update.
	//--------------------------------------------------------------------
	void UpdateTransforms()
	{
		std::vector<xfrmTransformGroup*>::iterator git;
		for (git = l_Transforms.begin(); git != l_Transforms.end(); ++git)
		{
			if ((*git)->m_pCallback)
				(*git)->m_pCallback->UpdateParentTransform();
			//if ((*git)->m_UpdateFunction != NULL)
			//	((*git)->m_UpdateFunction)();
		}
		std::vector<xfrmTransformNode*>::iterator nit;
		for (nit = l_Objects.begin(); nit != l_Objects.end(); ++nit)
		{
			if ((*nit)->m_pCallback)
				(*nit)->m_pCallback->UpdateParentTransform();
			//if ((*nit)->m_UpdateFunction != NULL)
			//	((*nit)->m_UpdateFunction)();
		}
	}
		
	//--------------------------------------------------------------------
	// Return true if this group's children have a pivot point based 
	// icon geometry instead of true geometry in the scnee graph.
	// If so, return pivot point in the o_Pivot argument.
	//--------------------------------------------------------------------
	bool GetIconPivotPoint(xfrmTransformGroup &i_Group, maPoint3d& o_Pivot)
	{
		maPoint3d group_pvt(0,0,0);
		int num_child_pivots = 0;

		int num_kids = i_Group.m_ChildNodes.size();
		for (int k=0; k<num_kids; ++k)
		{
			xfrmTransformNode *pChildNode = i_Group.m_ChildNodes[k];

			maPoint3d icon_pvt;
			bool bChildHasIconPivot = false;

			// could organize the child node related functions into the base class
			// and avoid the dynamic cast...
			xfrmTransformGroup *pChildGroup = dynamic_cast<xfrmTransformGroup*>(pChildNode);
			if (pChildGroup)
			{
				bChildHasIconPivot = GetIconPivotPoint(*pChildGroup, icon_pvt);
			}
			else if (pChildNode->m_pCallback)
			{
				bChildHasIconPivot = pChildNode->m_pCallback->HasIconPivotPoint(icon_pvt);
			}

			if (bChildHasIconPivot)
			{
				group_pvt += icon_pvt;
				num_child_pivots++;
			}
		}

		if (num_child_pivots > 0)
		{
			o_Pivot = group_pvt / (float)num_child_pivots;
			return true;
		}
		else 
			return false;
	}

	//--------------------------------------------------------------------
	// Get vector of all top level group nodes 
	//	(group nodes with no parent)
	//--------------------------------------------------------------------
	void GetRootTransforms(std::vector<xfrmTransformGroup*> &o_RootNodes)
	{
		std::vector<xfrmTransformGroup*>::iterator it, end = l_Transforms.end();
		for (it = l_Transforms.begin(); it != end; ++it)
		{
			if (((*it)->m_pNameObject != 0) && ((*it)->m_pParentGroup == 0))
				o_RootNodes.push_back(*it);
		}
	}

	//--------------------------------------------------------------------
	// Get vector of all top level objects (leaf nodes with no parent)
	//--------------------------------------------------------------------
	void GetUngroupedObjects(std::vector<nameString> &o_ObjectNames)
	{
		std::vector<xfrmTransformNode*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			if (((*it)->m_pNameObject != 0) && ((*it)->m_pParentGroup == 0))
				o_ObjectNames.push_back((*it)->m_pNameObject->GetName());
		}
	}

	//--------------------------------------------------------------------
	// Set editor visibility for all children of this node
	//--------------------------------------------------------------------
	void SetChildrenVisibleInEditor(xfrmTransformGroup &i_Group, bool i_bVisible)
	{
		int num_kids = i_Group.m_ChildNodes.size();
		for (int k=0; k<num_kids; ++k)
		{
			xfrmTransformNode *pChildNode = i_Group.m_ChildNodes[k];

			// could organize the child node related functions into the base class
			// and avoid the dynamic cast...
			xfrmTransformGroup *pChildGroup = dynamic_cast<xfrmTransformGroup*>(pChildNode);
			if (pChildGroup)
			{
				SetChildrenVisibleInEditor(*pChildGroup, i_bVisible);
			}
			else
			{
				// visMgr should take the nameString, not std::string
				visMgr::SetVisibleInEditor(pChildNode->m_pNameObject->GetName().GetString(), i_bVisible);
			}
		}
	}

}	// end of namespace
