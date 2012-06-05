/*****************************************************************************
**	prtVertexAnimKeys.cpp
**
**		prtVertexAnimKeys - animation information for baked particle
**	animation.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtVertexAnimKeys.hpp"


//--------------------------------------------------------------------
//	Default constructor.
//--------------------------------------------------------------------
prtVertexAnimKeys::prtVertexAnimKeys()
:	m_Frames(NULL)
{
}

//--------------------------------------------------------------------
//	Mutators.  The prtVertexAnimKeys makes a copy of the anKeyData
//	part of the animation, but does not own the prtVertexFrame data
//	being pointed at. This allows sharing of vertex data between
//	animations.
//--------------------------------------------------------------------
void prtVertexAnimKeys::SetFrames(anKeyDataBase<prtVertexFrame*>& i_Anim)
{
	m_Frames = i_Anim;
}
