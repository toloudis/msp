/****************************************************************************\
**	g3dSpotLight.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dSpotLight.hpp"


//--------------------------------------------------------------------
//	The default constructor places the light at the origin.
//--------------------------------------------------------------------
g3dSpotLight::g3dSpotLight()
:	m_Direction(0, 0, 1),
	m_fInnerAngle(0.5f),
	m_fOuterAngle(0.6f)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dSpotLight::~g3dSpotLight()
{
}

//--------------------------------------------------------------------
//	GetDirection returns the position of the light.
//--------------------------------------------------------------------
const maVector3d& g3dSpotLight::GetDirection() const
{
	return m_Direction;
}

//--------------------------------------------------------------------
//	SetDirection sets the position of the light.
//--------------------------------------------------------------------
void g3dSpotLight::SetDirection(const maVector3d& i_Direction)
{
	m_Direction = i_Direction;
}

//--------------------------------------------------------------------
//	GetInnerAngle returns the inner angle of the light.
//--------------------------------------------------------------------
float g3dSpotLight::GetInnerAngle() const
{
	return m_fInnerAngle;
}

//--------------------------------------------------------------------
//	SetInnerAngle sets the the inner angle of the light.  Must be
//	between 0 and the outer angle.
//--------------------------------------------------------------------
void g3dSpotLight::SetInnerAngle(float i_InnerAngle)
{
	m_fInnerAngle = i_InnerAngle;
}

//--------------------------------------------------------------------
//	GetOuterAngle returns the outer angle of the light.
//--------------------------------------------------------------------
float g3dSpotLight::GetOuterAngle() const
{
	return m_fOuterAngle;
}

//--------------------------------------------------------------------
//	SetOuterAngle sets the the outer angle of the light.  Must be
//  greater than the inner angle
//--------------------------------------------------------------------
void g3dSpotLight::SetOuterAngle(float i_OuterAngle)
{
	m_fOuterAngle = i_OuterAngle;
}

