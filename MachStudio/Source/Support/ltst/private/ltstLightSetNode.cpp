/****************************************************************************\
**  ltstLightSetNode.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/ltst/private/ltstLightSetNode.hpp"
#include "Support/ltst/ltstLightSet.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Tool/gpx/gpxRenderState.hpp"

//----------------------------------------------------------------------------
// Constructor takes node in which to alter the render state
//----------------------------------------------------------------------------
ltstLightSetNode::ltstLightSetNode(g3dSceneNode* i_pNode, 
								   const std::string& i_NodeName)
: //m_pNode(i_pNode), 
  m_NodeName(i_NodeName), 
  //m_pRenderState(NULL), 
  m_pRenderStateProxy(NULL)
{

	m_pRenderStateProxy = new gpxRenderState(*i_pNode);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ltstLightSetNode::~ltstLightSetNode()
{
	// Proxy handles management of render state object now
	delete m_pRenderStateProxy;
	//if (m_pRenderState)
	//{
	//	delete m_pRenderState;
	//	this->m_pNode->SetRenderState(NULL);
	//}
}

//----------------------------------------------------------------------------
// Add light to render state for this object
//----------------------------------------------------------------------------
void ltstLightSetNode::AddLightToRenderState(g3dLight *i_pLight)
{
	// Proxy handles management of render state object now
	m_pRenderStateProxy->AddLightToRenderState(i_pLight);
	//if (!m_pRenderState)
	//{
	//	m_pRenderState = new g3dRenderState();
	//	this->m_pNode->SetRenderState(m_pRenderState);
	//}
	//m_pRenderState->m_Lights.push_back(i_pLight);
}

//----------------------------------------------------------------------------
// Remove light from render state for this object
//----------------------------------------------------------------------------
void ltstLightSetNode::RemoveLightFromRenderState(g3dLight *i_pLight)
{
	// Proxy handles management of render state object now
	m_pRenderStateProxy->RemoveLightFromRenderState(i_pLight);
	//if (m_pRenderState)
	//{
	//	envSTLHelpers::RemoveOneValue(m_pRenderState->m_Lights, i_pLight);
	//}
}

//----------------------------------------------------------------------------
// GetNodeName()
//----------------------------------------------------------------------------
std::string ltstLightSetNode::GetNodeName()
{
	return m_NodeName;
}

//----------------------------------------------------------------------------
// GetContainingLightSets()
//----------------------------------------------------------------------------
std::vector<ltstLightSet*> ltstLightSetNode::GetContainingLightSets()
{
	return m_ContainingLightSets;
}