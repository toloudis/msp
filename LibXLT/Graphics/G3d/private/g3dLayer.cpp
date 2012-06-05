/****************************************************************************\
**	g3dLayer.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dLayer.hpp"

#include "Core/env/envPlatform.hpp"


//----------------------------------------------------------------------------
// The root node is not owned by the layer, just pointed to
//----------------------------------------------------------------------------
g3dLayer::g3dLayer()
:	m_pRootNode(NULL),
	m_SortMethod( e_ZBuffer ),
	m_ModelSpace( e_World ),
	m_BlendMethod( e_Multiplicative ),
	m_bFogEnabled( true ),
	m_bShadows( false ),
	m_bPreLit( false ),
	m_bClearDepth( false )
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dLayer::g3dLayer(g3dSceneNode* i_Node)
:	m_pRootNode(i_Node),
	m_SortMethod( e_ZBuffer ),
	m_ModelSpace( e_World ),
	m_BlendMethod( e_Multiplicative ),
	m_bFogEnabled( true ),
	m_bShadows( false ),
	m_bPreLit( false ),
	m_bClearDepth( false )
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dLayer::g3dLayer( g3dSceneNode* i_Node,
				    SortMethod i_SortMethod,
					ModelSpace i_ModelSpace,
					BlendMethod i_BlendMethod,
					bool i_bFogEnabled,
					bool i_bShadows,
					bool i_bPreLit,
					bool i_bClearDepth)
:	m_pRootNode(i_Node),
	m_SortMethod( i_SortMethod ),
	m_ModelSpace( i_ModelSpace ),
	m_BlendMethod( i_BlendMethod ),
	m_bFogEnabled( i_bFogEnabled ),
	m_bShadows( i_bShadows ),
	m_bPreLit(i_bPreLit),
	m_bClearDepth(i_bClearDepth)
{
}
