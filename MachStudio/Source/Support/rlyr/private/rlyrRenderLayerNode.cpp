/****************************************************************************\
**  rlyrRenderLayerNode.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/rlyr/private/rlyrRenderLayerNode.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Tool/gpx/gpxRenderState.hpp"

//----------------------------------------------------------------------------
// Constructor takes node in which to alter the render state
//----------------------------------------------------------------------------
rlyrRenderLayerNode::rlyrRenderLayerNode(g3dSceneNode* i_pNode, 
								   const std::string& i_NodeName)
: m_NodeName(i_NodeName), 
  m_pSceneNode(i_pNode)
{

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
rlyrRenderLayerNode::~rlyrRenderLayerNode()
{
	// Proxy handles management of render state object now
	//delete m_pRenderStateProxy;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rlyrRenderLayerNode::SetNodeActive( bool i_bActive )
{
	if( m_pSceneNode )
		m_pSceneNode->SetActiveInRenderLayer( i_bActive );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool rlyrRenderLayerNode::GetNodeActive()
{
	if( m_pSceneNode )
		return m_pSceneNode->GetActiveInRenderLayer();	
	return false;
}