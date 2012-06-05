/********************************************************************************************\
**  ltstLightSet.hpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/

#ifdef LTST_LIGHTSET_HPP
#error ltstLightSet.hpp multiply included
#endif
#define LTST_LIGHTSET_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include <map>
#include <vector>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
struct g3dRenderState;
class ltstLightSetLight;
class ltstLightSetNode;
class ltstLightSetObject;
class ltstLightSetData;

// Need to keep track of which nodes are expanded in 
// the given object
struct sObjectNodeInfo
{
	bool m_bRootLit;
	std::vector<bool> m_bNodesLit;
};
typedef std::map<ltstLightSetObject*, sObjectNodeInfo> ObjectNodeMap;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class ltstLightSet
{
public:
	nameString m_Name;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	ltstLightSet(const nameString& i_Name);

	//----------------------------------------------------------------------------
	// Add whole object to this set, updating the render state of the 
	// root node of the object
	//----------------------------------------------------------------------------
	void AddWholeObjectToSet(ltstLightSetObject* i_pObject);

	//----------------------------------------------------------------------------
	// Remove whole object from this set
	//----------------------------------------------------------------------------
	void RemoveWholeObjectFromSet(ltstLightSetObject* i_pObject);

	//----------------------------------------------------------------------------
	// Add node for an individual fragment to this set, updating the render 
	// state of the single node
	//----------------------------------------------------------------------------
	void AddNodeToSet(ltstLightSetObject* i_pObject, int i_NodeIndex);

	//----------------------------------------------------------------------------
	// Remove fragment node from this set, updating the render state of the node
	//----------------------------------------------------------------------------
	void RemoveNodeFromSet(ltstLightSetObject* i_pObject, int i_NodeIndex);

	//----------------------------------------------------------------------------
	// Add light to this set, updating the render states of the
	// objects in this set
	//----------------------------------------------------------------------------
	void AddLightToSet(ltstLightSetLight* i_pLight);

	//----------------------------------------------------------------------------
	// Remove light from this set, updating the render states of the
	// objects in this set
	//----------------------------------------------------------------------------
	void RemoveLightFromSet(ltstLightSetLight* i_pLight);
					
	//----------------------------------------------------------------------------
	// Remove all connections for objects and lights in this set.
	// Attach the lights to the given root render state.
	//----------------------------------------------------------------------------
	void Disconnect(g3dRenderState *i_pRootRenderState);

	//----------------------------------------------------------------------------
	// Accessors
	//----------------------------------------------------------------------------
	const std::vector<ltstLightSetLight*>& Lights() const 	{ return m_Lights; }

	//----------------------------------------------------------------------------
	// Get list of names of the objects in the set
	//----------------------------------------------------------------------------
	void GetObjectsInSet(std::vector<nameString> &o_ObjectNames);

	//----------------------------------------------------------------------------
	// Return true if the given object is in this light set.
	//----------------------------------------------------------------------------
	bool IsObjectInSet(ltstLightSetObject* i_pObject);

	//----------------------------------------------------------------------------
	// Get indices of fragments that are individually lit by this
	// light set. If the whole object is lit, then the list will
	// be returned empty.
	//----------------------------------------------------------------------------
	void GetLitFragmentIndices(ltstLightSetObject* i_pObject,
							   std::vector<int>& o_LitFragmentIndices);

	//----------------------------------------------------------------------------
	// Get data about the light set
	//----------------------------------------------------------------------------
	void GetData(ltstLightSetData &o_Data) const;

	//----------------------------------------------------------------------------
	// GetNodes()
	//----------------------------------------------------------------------------
	std::vector<ltstLightSetNode*> GetNodes();

	//----------------------------------------------------------------------------
	// GetLights()
	//----------------------------------------------------------------------------
	std::vector<ltstLightSetLight*> GetLights();

	//----------------------------------------------------------------------------
	// GetObjects()
	//----------------------------------------------------------------------------
	std::map<ltstLightSetObject*, sObjectNodeInfo> GetObjects();

private:
	std::vector<ltstLightSetLight*> m_Lights;
	std::vector<ltstLightSetNode*> m_Nodes;

	ObjectNodeMap m_Objects;

	//----------------------------------------------------------------------------
	// Private functions, only handle updating the render states
	//----------------------------------------------------------------------------
	void add_node_to_set(ltstLightSetNode* i_pNode);
	void remove_node_from_set(ltstLightSetNode* i_pNode);

	//----------------------------------------------------------------------------
	// Set state of all nodes in the list to match the i_bLit state
	//----------------------------------------------------------------------------
	void set_nodes_lit(bool i_bLit,
						ltstLightSetObject* i_pObject, 
						std::vector<bool>& io_bNodesLit);
};
