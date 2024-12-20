/********************************************************************************************\
**  rlyrRenderLayerNode.hpp
**
**  A node represents a place in the scene graph that can been rendered or not rendered
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/

#ifdef RLYR_RENDERLAYERNODE_HPP
#error rlyrRenderLayerNode.hpp multiply included
#endif
#define RLYR_RENDERLAYERNODE_HPP

#include <string>
#include <vector>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class rlyrRenderLayer;
class g3dSceneNode;
struct g3dRenderState;
class gpxRenderState;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class rlyrRenderLayerNode
{
public:
	std::string m_NodeName;

	//----------------------------------------------------------------------------
	// Constructor takes node in which to alter the render state
	//----------------------------------------------------------------------------
	rlyrRenderLayerNode(g3dSceneNode* i_pNode, const std::string& i_NodeName);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~rlyrRenderLayerNode();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void SetNodeActive( bool i_bActive );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool GetNodeActive();

private:	
	g3dSceneNode* m_pSceneNode;
	bool m_bActive;
};
