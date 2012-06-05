/****************************************************************************\
**	g3dDirectionalLight.hpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dDirectionalLight.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g3d/g3dJitterSettings.hpp"


//--------------------------------------------------------------------
//	The default constructor faces the light towards -y (straight
//	down).
//--------------------------------------------------------------------
g3dDirectionalLight::g3dDirectionalLight()
:	m_Direction(0.0f, -1.0f, 0.0f)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dDirectionalLight::~g3dDirectionalLight()
{
}

//--------------------------------------------------------------------
//	GetDirection returns the direction of the light.
//--------------------------------------------------------------------
const maVector3d& g3dDirectionalLight::GetDirection() const
{
	return m_Direction;
}

//--------------------------------------------------------------------
//	GetJitteredDirection returns the direction of the
//	light after jittering
//--------------------------------------------------------------------
maVector3d g3dDirectionalLight::GetJitteredDirection(int i_Pass) const
{
	float yaw, pitch;
	maFunctions::GetYawPitch(m_Direction, yaw, pitch);
	maVector3d vec = g3dJitterSettings::GetJitterNoise(i_Pass);
	vec *= maConstants::c_fAngleToRad * g3dJitterSettings::GetJitterDirScale(); // scale jitter to degrees
	float scale = 1.0f; // sqrtf(fabsf(vec[1]));
	yaw += (vec[0] * scale);
	pitch += (vec[2] * scale);

	float xz_len = cosf(pitch);
	maVector3d dir(xz_len * sinf(yaw), sinf(pitch), xz_len * cosf(yaw));
	return dir;
}

//--------------------------------------------------------------------
//	SetDirection sets the direction of the light.
//--------------------------------------------------------------------
void g3dDirectionalLight::SetDirection(const maVector3d& i_Direction)
{
	m_Direction = i_Direction;
	m_Direction.Normalize();
}
