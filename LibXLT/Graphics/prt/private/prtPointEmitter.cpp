/*****************************************************************************
**	prtPointEmitter.cpp
**
**		prtPointEmitter is a prtEmitter which emits all particles from the
**	single point at the origin.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtPointEmitter.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtPointEmitter::prtPointEmitter()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtPointEmitter::~prtPointEmitter()
{
}

//--------------------------------------------------------------------
//	GetPosition returns a random location in the emitter area
//--------------------------------------------------------------------
maPoint3d prtPointEmitter::GetPosition() const
{
	return maPoint3d(0, 0, 0);
}