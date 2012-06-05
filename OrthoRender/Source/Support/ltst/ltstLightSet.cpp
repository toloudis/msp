/****************************************************************************\
**  ltstLightSet.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/ltst/ltstLightSet.hpp"

#include "Support/ltst/private/ltstLightSetLight.hpp"
#include "Support/ltst/private/ltstLightSetNode.hpp"
#include "Support/ltst/private/ltstLightSetObject.hpp"
#include "Support/ltst/ltstLightSetsData.hpp"

#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Core/name/nameObject.hpp"

namespace
{
	void notify_add_light(std::vector<ltstLightSetNode*> &io_Nodes,
						  g3dLight* i_pLight)
	{
		std::vector<ltstLightSetNode*>::iterator it, end = io_Nodes.end();
		for (it = io_Nodes.begin(); it != end; ++it)
		{
			(*it)->AddLightToRenderState(i_pLight);
		}
	}


	void notify_remove_light(std::vector<ltstLightSetNode*> &io_Nodes,
						     g3dLight* i_pLight)
	{
		std::vector<ltstLightSetNode*>::iterator it, end = io_Nodes.end();
		for (it = io_Nodes.begin(); it != end; ++it)
		{
			(*it)->RemoveLightFromRenderState(i_pLight);
		}
	}

	bool all_nodes_equal(std::vector<bool>& io_bNodesLit,
						 bool i_bLit)
	{		
		const int num_nodes = io_bNodesLit.size();
		for (int i=0; i<num_nodes; ++i)
		{
			if (io_bNodesLit[i] != i_bLit)
				return false;
		}
		return true;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ltstLightSet::ltstLightSet(const nameString& i_Name)
: m_Name(i_Name) 
{

}

//----------------------------------------------------------------------------
// Add whole object to this set, updating the render state of the 
// root node of the object
//----------------------------------------------------------------------------
void ltstLightSet::AddWholeObjectToSet(ltstLightSetObject* i_pObject)
{
	// Check to see if this object is already in the list
	ObjectNodeMap::iterator it = m_Objects.find(i_pObject);
	if (it != m_Objects.end())
	{
		// Object already exists in our mapping
		sObjectNodeInfo &info = it->second;
		if (!info.m_bRootLit)
		{
			// If the object is not already list, turn off all child nodes
			// that are separately lit, and turn on the root node
			set_nodes_lit(false, i_pObject, info.m_bNodesLit);
			add_node_to_set(i_pObject->RootNode());
			info.m_bRootLit = true;
		}
	}
	else
	{
		// Turn on just the root node, creating a new info
		// structure for this object
		sObjectNodeInfo &info = m_Objects[i_pObject];
		info.m_bRootLit = true;
		info.m_bNodesLit.resize( i_pObject->GetNumNodes(), false);
		add_node_to_set(i_pObject->RootNode());
	}
}


//----------------------------------------------------------------------------
// Remove object from this set, updating the render state of the object
//----------------------------------------------------------------------------
void ltstLightSet::RemoveWholeObjectFromSet(ltstLightSetObject* i_pObject)
{
	// Confirm this object is in the list
	ObjectNodeMap::iterator it = m_Objects.find(i_pObject);
	if (it != m_Objects.end())
	{
		sObjectNodeInfo &info = it->second;
		if (info.m_bRootLit)
		{
			remove_node_from_set(i_pObject->RootNode());
			info.m_bRootLit = false;
		}

		// Set all individual nodes off
		set_nodes_lit(false, i_pObject, info.m_bNodesLit);

		// Remove it from the list
		m_Objects.erase(it);
	}
}

//----------------------------------------------------------------------------
// Add node for an individual fragment to this set, updating the render 
// state of the single node
//----------------------------------------------------------------------------
void ltstLightSet::AddNodeToSet(ltstLightSetObject* i_pObject, int i_Index)
{
	// Check to see if this object is already in the list
	ObjectNodeMap::iterator it = m_Objects.find(i_pObject);
	if (it != m_Objects.end())
	{
		// Object already exists in our mapping
		sObjectNodeInfo &info = it->second;

		// If the root is already lit, then we don't need to do anything
		if (!info.m_bRootLit)
		{
			if (i_Index < i_pObject->GetNumNodes())
			{
				// Turn on the individual node
				if (!info.m_bNodesLit[i_Index])
				{
					add_node_to_set(i_pObject->Node(i_Index));
					info.m_bNodesLit[i_Index] = true;
				}
			}
			else
			{
				DBG_ERROR2("Light Set fragment out of range, %d out of %d", i_Index, i_pObject->GetNumNodes());
			}
		}
	}
	else
	{
		// Turn on just the individual node, creating a new info
		// structure for this object
		sObjectNodeInfo &info = m_Objects[i_pObject];
		info.m_bRootLit = false;
		info.m_bNodesLit.resize( i_pObject->GetNumNodes(), false );

		add_node_to_set(i_pObject->Node(i_Index));
		info.m_bNodesLit[i_Index] = true;
	}
}

//----------------------------------------------------------------------------
// Remove node from this set, updating the render state of the node
//----------------------------------------------------------------------------
void ltstLightSet::RemoveNodeFromSet(ltstLightSetObject* i_pObject, int i_Index)
{
	// Confirm this object is in the list
	ObjectNodeMap::iterator it = m_Objects.find(i_pObject);
	if (it != m_Objects.end())
	{
		sObjectNodeInfo &info = it->second;

		// If we previously had just the root node lit,
		// then we have to turn off the root node and
		// turn on all the individual nodes in order to
		// then remove the one we are turning off
		if (info.m_bRootLit)
		{
			// turn off root node
			remove_node_from_set(i_pObject->RootNode());
			info.m_bRootLit = false;
			
			// Set all individual nodes on
			set_nodes_lit(true, i_pObject, info.m_bNodesLit);
		}

		// Now turn off the individual node
		if (info.m_bNodesLit[i_Index])
		{
			remove_node_from_set(i_pObject->Node(i_Index));
			info.m_bNodesLit[i_Index] = false;
		}

		// If that was the last node, we need to remove the object from our list
		if (all_nodes_equal(info.m_bNodesLit, false))
			m_Objects.erase(it);
	}
}

//----------------------------------------------------------------------------
// Add light to this set, updating the render states of the
// objects in this set
//----------------------------------------------------------------------------
void ltstLightSet::AddLightToSet(ltstLightSetLight* i_pLight)
{
	// Add light to new light set
	this->m_Lights.push_back(i_pLight);
	i_pLight->m_pContainingLightSet = this;

	// Notify all affected objects
	notify_add_light(this->m_Nodes, i_pLight->m_pLight);
}

//----------------------------------------------------------------------------
// Remove light from this set, updating the render states of the
// objects in this set
//----------------------------------------------------------------------------
void ltstLightSet::RemoveLightFromSet(ltstLightSetLight* i_pLight)
{
	// Remove light from light set
	envSTLHelpers::RemoveOneValue(this->m_Lights, i_pLight);

	// Notify all affected objects
	notify_remove_light(this->m_Nodes, i_pLight->m_pLight);

	// Set containing light set to NULL
	i_pLight->m_pContainingLightSet = NULL;
}

//--------------------------------------------------------------------
// Set ambient light for the given light set. This is added to
// the scene's ambient.
//--------------------------------------------------------------------
void ltstLightSet::SetAmbientLight(const maFloatRGBA &i_AmbientLight)
{
	this->m_AmbientLight = i_AmbientLight;

	// Now we have to go through all of the objects in this light set
	// and have them update the ambient light in their light set
	std::vector<ltstLightSetNode*>::iterator node_it, node_end = this->m_Nodes.end();
	for (node_it = this->m_Nodes.begin(); node_it != node_end; ++node_it)
	{
		(*node_it)->UpdateAmbient();
	}
}

//----------------------------------------------------------------------------
// Remove all connections for objects and lights in this set.
// Attach the lights to the given root render state.
//----------------------------------------------------------------------------
void ltstLightSet::Disconnect(g3dRenderState *i_pRootRenderState)
{
	// Remove all lights from this light set
	std::vector<ltstLightSetLight*>::iterator lt_it, lt_end = this->m_Lights.end();
	for (lt_it = this->m_Lights.begin(); lt_it != lt_end; ++lt_it)
	{
		// Notify all affected objects
		notify_remove_light(this->m_Nodes, (*lt_it)->m_pLight);
	
		(*lt_it)->m_pContainingLightSet = NULL;

		// Add light to root state when unassigned
		i_pRootRenderState->m_Lights.push_back((*lt_it)->m_pLight);
	}

	// Remove this light set from the nodes' containing light set list
	std::vector<ltstLightSetNode*>::iterator node_it, node_end = this->m_Nodes.end();
	for (node_it = this->m_Nodes.begin(); node_it != node_end; ++node_it)
	{
		envSTLHelpers::RemoveOneValue((*node_it)->m_ContainingLightSets, this);

		// sum ambient light for this node, 
		// now that light set has been removed from its list
		(*node_it)->UpdateAmbient();
	}

	// Clear the mapping
	m_Objects.clear();

	// Clear other lists
	m_Lights.clear();
	m_Nodes.clear();
}

//----------------------------------------------------------------------------
// Get list of names of the objects in the set
//----------------------------------------------------------------------------
void ltstLightSet::GetObjectsInSet(std::vector<nameString> &o_ObjectNames)
{
	ObjectNodeMap::const_iterator obj_it, obj_end = this->m_Objects.end();
	for (obj_it = this->m_Objects.begin(); obj_it != obj_end; ++obj_it)
	{
		o_ObjectNames.push_back( obj_it->first->m_pNameObj->GetName() );
	}
}

//----------------------------------------------------------------------------
// Get indices of fragments that are individually lit by this
// light set. If the whole object is lit, then the list will
// be returned empty.
//----------------------------------------------------------------------------
void ltstLightSet::GetLitFragmentIndices(ltstLightSetObject* i_pObject,
										 std::vector<int>& o_LitFragmentIndices)
{
	ObjectNodeMap::iterator it = m_Objects.find(i_pObject);
	if (it != m_Objects.end())
	{
		sObjectNodeInfo &info = it->second;

		// If whole object lighting, return empty list
		if (info.m_bRootLit)
			return;

		// Gather individual nodes that are turned on
		const int num_nodes = info.m_bNodesLit.size();
		for (int i=0; i<num_nodes; ++i)
		{
			if (info.m_bNodesLit[i])
				o_LitFragmentIndices.push_back(i);
		}
	}
}

//----------------------------------------------------------------------------
// Get data about the light set
//----------------------------------------------------------------------------
void ltstLightSet::GetData(ltstLightSetData &o_Data) const
{
	o_Data.m_Name = this->m_Name;
	o_Data.m_AmbientLight = this->m_AmbientLight;

	int i=0;
	o_Data.m_Lights.resize(this->m_Lights.size());
	std::vector<ltstLightSetLight*>::const_iterator lt_it, lt_end = this->m_Lights.end();
	for (lt_it = this->m_Lights.begin(); lt_it != lt_end; ++lt_it)
	{
		o_Data.m_Lights[i++] = (*lt_it)->m_pNameObj->GetName();
	}

	o_Data.m_Objects.clear();
	ObjectNodeMap::const_iterator obj_it, obj_end = this->m_Objects.end();
	for (obj_it = this->m_Objects.begin(); obj_it != obj_end; ++obj_it)
	{
		ltstLightSetObjectData obj_data;
		obj_data.m_Name = obj_it->first->m_pNameObj->GetName();

		if (!obj_it->second.m_bRootLit)
		{
			// Gather individual nodes that are turned on
			const int num_nodes = obj_it->second.m_bNodesLit.size();
			for (int i=0; i<num_nodes; ++i)
			{
				if (obj_it->second.m_bNodesLit[i])
					obj_data.m_LitFragmentIndices.push_back(i);
			}
		}

		o_Data.m_Objects.push_back(obj_data);
	}
}

//----------------------------------------------------------------------------
// Add node to this set, updating the render state of the object
//----------------------------------------------------------------------------
void ltstLightSet::add_node_to_set(ltstLightSetNode* i_pNode)
{
	this->m_Nodes.push_back(i_pNode);
	i_pNode->m_ContainingLightSets.push_back(this);

	// Add all lights to its render state 
	std::vector<ltstLightSetLight*>::iterator it, end = this->m_Lights.end();
	for (it = this->m_Lights.begin(); it != end; ++it)
	{
		i_pNode->AddLightToRenderState((*it)->m_pLight);
	}

	// set up ambient color for this object
	i_pNode->UpdateAmbient();
}


//----------------------------------------------------------------------------
// Remove node from this set, updating the render state of the object
//----------------------------------------------------------------------------
void ltstLightSet::remove_node_from_set(ltstLightSetNode* i_pNode)
{
	// Remove object from light set
	envSTLHelpers::RemoveOneValue(this->m_Nodes, i_pNode);
	envSTLHelpers::RemoveOneValue(i_pNode->m_ContainingLightSets, this);

	// Remove lights from its render state 
	std::vector<ltstLightSetLight*>::iterator it, end = this->m_Lights.end();
	for (it = this->m_Lights.begin(); it != end; ++it)
	{
		i_pNode->RemoveLightFromRenderState((*it)->m_pLight);
	}

	// set up ambient color for this object
	i_pNode->UpdateAmbient();
}

//----------------------------------------------------------------------------
// Remove node from this set, updating the render state of the object
//----------------------------------------------------------------------------
void ltstLightSet::set_nodes_lit(bool i_bLit,
					   ltstLightSetObject* i_pObject, 
					   std::vector<bool>& io_bNodesLit)
{
	const int num_nodes = io_bNodesLit.size();
	DBG_ASSERT2(num_nodes == i_pObject->GetNumNodes(), "Non-matching array lengths, %d versus %d", num_nodes, i_pObject->GetNumNodes());
	for (int i=0; i<num_nodes; ++i)
	{
		if (io_bNodesLit[i] != i_bLit)
		{
			if (i_bLit)
				add_node_to_set(i_pObject->Node(i));
			else
				remove_node_from_set(i_pObject->Node(i));

			io_bNodesLit[i] = i_bLit;
		}
	}
}