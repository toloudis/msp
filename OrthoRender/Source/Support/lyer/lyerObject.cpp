/*****************************************************************************
**  lyerObject.cpp
**
**      A lyerObject is base class for objects that can be grouped
**	into layers and have active and draw style states set as a group.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Support/lyer/lyerObject.hpp"

#include "Core/env/envSTLHelpers.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
lyerObject::lyerObject()
{
	m_Flags.m_bLayerVisible  = true;
	m_Flags.m_bLayerPickable  = true;
	m_Flags.m_bLayerWireframe  = false;
	m_Flags.m_bLayerLowRes  = false;
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
//virtual 
void lyerObject::SetLayerVisible(bool i_bVisible)
{
	m_Flags.m_bLayerVisible = i_bVisible;
}
bool lyerObject::GetLayerVisible() const
{
	return m_Flags.m_bLayerVisible;
}

//--------------------------------------------------------------------
//	LayerPickable represents if objects in the layer can be picked.
//--------------------------------------------------------------------
//virtual 
void lyerObject::SetLayerPickable(bool i_bPickable)
{
	m_Flags.m_bLayerPickable = i_bPickable;
}
bool lyerObject::GetLayerPickable() const
{
	return m_Flags.m_bLayerPickable;
}

//--------------------------------------------------------------------
//	LayerWireframe represents if the objects are rendered
//	in a wireframe style.
//--------------------------------------------------------------------
//virtual 
void lyerObject::SetLayerWireframe(bool i_bWireframe)
{
	m_Flags.m_bLayerWireframe = i_bWireframe;
}
bool lyerObject::GetLayerWireframe() const
{
	return m_Flags.m_bLayerWireframe;
}

//--------------------------------------------------------------------
//	LayerLowRes represents if the objects are rendered
//	using a low resolution model.
//--------------------------------------------------------------------
//virtual 
void lyerObject::SetLayerLowRes(bool i_bLowRes)
{
	m_Flags.m_bLayerLowRes = i_bLowRes;
}
bool lyerObject::GetLayerLowRes() const
{
	return m_Flags.m_bLayerLowRes;
}
