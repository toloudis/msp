/****************************************************************************\
**  ltstLightSetNode.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/ltst/private/ltstLightSetNode.hpp"
#include "Support/ltst/ltstLightSet.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"

//----------------------------------------------------------------------------
// Constructor takes node in which to alter the render state
//----------------------------------------------------------------------------
ltstLightSetNode::ltstLightSetNode(g3dSceneNode* i_pNode, 
								   const std::string& i_NodeName)
: m_pNode(i_pNode), 
  m_NodeName(i_NodeName), 
  m_pRenderState(NULL)
{

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ltstLightSetNode::~ltstLightSetNode()
{
	if (m_pRenderState)
	{
		delete m_pRenderState;
		this->m_pNode->SetRenderState(NULL);
	}
}

//----------------------------------------------------------------------------
// Sum up ambient color from light sets for this object and set this
// ambient color into its render state
//----------------------------------------------------------------------------
void ltstLightSetNode::UpdateAmbient()
{
	maFloatRGBA ambient;

	std::vector<ltstLightSet*>::iterator set_it, end = this->m_ContainingLightSets.end();
	for (set_it = this->m_ContainingLightSets.begin(); set_it != end; ++set_it)
	{
		ambient += (*set_it)->m_AmbientLight;
	}

	if (!m_pRenderState)
	{
		m_pRenderState = new g3dRenderState();
		this->m_pNode->SetRenderState(m_pRenderState);
	}
	m_pRenderState->m_AmbientLight = ambient;
}

//----------------------------------------------------------------------------
// Add light to render state for this object
//----------------------------------------------------------------------------
void ltstLightSetNode::AddLightToRenderState(g3dLight *i_pLight)
{
	if (!m_pRenderState)
	{
		m_pRenderState = new g3dRenderState();
		this->m_pNode->SetRenderState(m_pRenderState);
	}
	m_pRenderState->m_Lights.push_back(i_pLight);
}

//----------------------------------------------------------------------------
// Remove light from render state for this object
//----------------------------------------------------------------------------
void ltstLightSetNode::RemoveLightFromRenderState(g3dLight *i_pLight)
{
	if (m_pRenderState)
	{
		envSTLHelpers::RemoveOneValue(m_pRenderState->m_Lights, i_pLight);
	}
}
