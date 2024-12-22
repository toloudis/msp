/********************************************************************************************\
**  ltstLightSetNode.hpp
**
**  A node represents a place in the scene graph that can have individual lighting.
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/

#ifdef LTST_LIGHTSETNODE_HPP
#error ltstLightSetNode.hpp multiply included
#endif
#define LTST_LIGHTSETNODE_HPP

#include <string>
#include <vector>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class g3dLight;
class ltstLightSet;
class g3dSceneNode;
struct g3dRenderState;
class gpxRenderState;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class ltstLightSetNode
{
public:
	std::vector<ltstLightSet*> m_ContainingLightSets;
	std::string m_NodeName;

	//----------------------------------------------------------------------------
	// Constructor takes node in which to alter the render state
	//----------------------------------------------------------------------------
	ltstLightSetNode(g3dSceneNode* i_pNode, const std::string& i_NodeName);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~ltstLightSetNode();

	//----------------------------------------------------------------------------
	// Add light to render state for this object
	//----------------------------------------------------------------------------
	void AddLightToRenderState(g3dLight *i_pLight);

	//----------------------------------------------------------------------------
	// Remove light from render state for this object
	//----------------------------------------------------------------------------
	void RemoveLightFromRenderState(g3dLight *i_pLight);

	//----------------------------------------------------------------------------
	// GetNodeName()
	//----------------------------------------------------------------------------
	std::string GetNodeName();

	//----------------------------------------------------------------------------
	// GetContainingLightSets()
	//----------------------------------------------------------------------------
	std::vector<ltstLightSet*> GetContainingLightSets();

private:	
	gpxRenderState* m_pRenderStateProxy;
	//g3dRenderState* m_pRenderState;
	//g3dSceneNode* m_pNode; 
};
