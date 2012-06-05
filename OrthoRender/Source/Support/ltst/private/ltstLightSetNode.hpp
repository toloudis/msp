/********************************************************************************************\
**  ltstLightSetNode.hpp
**
**  A node represents a place in the scene graph that can have individual lighting.
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/

#ifdef LTST_LIGHTSETNODE_HPP
#error ltstLightSetNode.hpp multiply included
#endif
#define LTST_LIGHTSETNODE_HPP

#include <vector>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class g3dLight;
class ltstLightSet;
class g3dSceneNode;
struct g3dRenderState;


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
	// Sum up ambient color from light sets for this object and set this
	// ambient color into its render state
	//----------------------------------------------------------------------------
	void UpdateAmbient();

	//----------------------------------------------------------------------------
	// Add light to render state for this object
	//----------------------------------------------------------------------------
	void AddLightToRenderState(g3dLight *i_pLight);

	//----------------------------------------------------------------------------
	// Remove light from render state for this object
	//----------------------------------------------------------------------------
	void RemoveLightFromRenderState(g3dLight *i_pLight);

private:	
	g3dRenderState* m_pRenderState;
	g3dSceneNode* m_pNode; 
};
