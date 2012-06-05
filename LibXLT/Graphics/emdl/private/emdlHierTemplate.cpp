/*****************************************************************************
**  emdlHierTemplate.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/emdl/emdlHierTemplate.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"


//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
emdlHierTemplate::emdlHierTemplate()
:	m_pModel(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
emdlHierTemplate::~emdlHierTemplate()
{
	delete m_pModel;
}

//--------------------------------------------------------------------
//	SetModel sets the scene node representing the root of the
//	object.
//--------------------------------------------------------------------
void emdlHierTemplate::SetModel( g3dSceneNode* i_pNode )
{
	m_pModel = i_pNode;
}
