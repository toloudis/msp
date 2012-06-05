/*****************************************************************************
**	prtCircleEmitter.cpp
**
**		prtCircleEmitter is a prtEmitter which emits all particles from the
**	single point at the origin.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtCircleEmitter.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"

#include <math.h>


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtCircleEmitter::prtCircleEmitter(float i_Radius)
:	m_Radius(i_Radius)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtCircleEmitter::~prtCircleEmitter()
{
}

//--------------------------------------------------------------------
//	GetPosition returns a random location in the emitter area
//--------------------------------------------------------------------
maPoint3d prtCircleEmitter::GetPosition() const
{
	//	We must use a square root distribution for the radius
	//	to get a area-constant particle distribution
	float radius_param = maFunctions::FloatRand(0.0f, 1.0f);
	radius_param = sqrtf(radius_param);
	radius_param *= m_Radius;
	float angle_param = maFunctions::FloatRand(0.0f, maConstants::c_fPI_Times_2);
	float x = radius_param * float(cos(angle_param));
	float y = radius_param * float(sin(angle_param));

	return maPoint3d(x, y, 0);
}
