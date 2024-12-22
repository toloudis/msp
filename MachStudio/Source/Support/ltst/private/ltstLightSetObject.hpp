/********************************************************************************************\
**  ltstLightSetObject.hpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/

#ifdef LTST_LIGHTSETOBJECT_HPP
#error ltstLightSetObject.hpp multiply included
#endif
#define LTST_LIGHTSETOBJECT_HPP

#include <string>
#include <vector>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class g3dLight;
class ltstLightSet;
class ltstLightSetNode;
class nameObject;
class api3dObjectSingle;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class ltstLightSetObject
{
public:
	nameObject* m_pNameObj;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	ltstLightSetObject(nameObject* i_pNameObj, api3dObjectSingle* i_pObject);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~ltstLightSetObject();

	//----------------------------------------------------------------------------
	// Get number of fragment nodes that can be separately lit
	//----------------------------------------------------------------------------
	int GetNumNodes() const;

	//----------------------------------------------------------------------------
	// Get name of fragment nodes with given index
	//----------------------------------------------------------------------------
	const std::string& GetNodeName(int i_Index) const;

	//----------------------------------------------------------------------------
	// Get fragment node with given index
	//----------------------------------------------------------------------------
	ltstLightSetNode* Node(int i_Index);

	//----------------------------------------------------------------------------
	// One special node represents the root of the object
	//----------------------------------------------------------------------------
	ltstLightSetNode* RootNode();

	//----------------------------------------------------------------------------
	// GetNodes()
	//----------------------------------------------------------------------------
	std::vector<ltstLightSetNode*> GetNodes();


private:
	//api3dObjectSingle* m_pObject;
	ltstLightSetNode* m_pRootNode;
	std::vector<ltstLightSetNode*> m_Nodes;
};
