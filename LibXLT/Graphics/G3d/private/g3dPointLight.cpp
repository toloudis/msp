/****************************************************************************\
**	g3dPointLight.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dPointLight.hpp"

#include "Graphics/g3d/g3dJitterSettings.hpp"


//============================================================================
//	l_DefaultRange is the default range for the point light
//============================================================================
float l_DefaultRange = 1000000.0f;


//--------------------------------------------------------------------
//	The default constructor places the light at the origin.
//--------------------------------------------------------------------
g3dPointLight::g3dPointLight()
:	m_Position(0, 0, 0),
	m_f0(1.0f),
	m_f1(0.0f),
	m_f2(0.0f),
	m_f3(0.0f),
	m_fStart(0.0f),
	m_fRange(l_DefaultRange)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dPointLight::~g3dPointLight()
{
}

//--------------------------------------------------------------------
//	GetPosition returns the position of the light.
//--------------------------------------------------------------------
const maPoint3d& g3dPointLight::GetPosition() const
{
	return m_Position;
}

//--------------------------------------------------------------------
//	GetPosition returns the position of the light.
//--------------------------------------------------------------------
maPoint3d g3dPointLight::GetJitteredPosition(int i_Pass) const
{
	maVector3d vec = g3dJitterSettings::GetJitterNoise(i_Pass);
	vec *= 0.1f * g3dJitterSettings::GetJitterPointScale(); // scale down jitter
	return m_Position + vec;
}

//--------------------------------------------------------------------
//	SetPosition sets the position of the light.
//--------------------------------------------------------------------
void g3dPointLight::SetPosition(const maPoint3d& i_Position)
{
	m_Position = i_Position;
}

//--------------------------------------------------------------------
//	GetFalloff0, GetFalloff1, and GetFalloff2 all return
//	coefficients that describe the falloff of the pointlight.
//	A point light is attenuated according to this formula:
//
//		A =				1
//			-------------------------
//			a0 + a1 * D + a2 * D^2,
//
//	where D is the distance from the light to the surface it is
//	illuminating and a0, a1, and a2 are the falloff coefficients.
//	Some (but not all) of the coefficients may be zero.
//--------------------------------------------------------------------
float g3dPointLight::GetFalloff0() const
{
	return m_f0;
}

float g3dPointLight::GetFalloff1() const
{
	return m_f1;
}

float g3dPointLight::GetFalloff2() const
{
	return m_f2;
}

float g3dPointLight::GetFalloff3() const
{
	return m_f3;
}

float g3dPointLight::GetFalloffStart() const
{
	return m_fStart;
}

//--------------------------------------------------------------------
//	The SetFalloff0-2 functions allow the user to set the falloff
//	curve.
//--------------------------------------------------------------------
void g3dPointLight::SetFalloff0(float i_Val)
{
	DBG_ASSERT(i_Val != 0, "This value can't be zero");

	if (i_Val == 0.0f)
		i_Val = 1.0f;

	m_f0 = i_Val;
}

void g3dPointLight::SetFalloff1(float i_Val)
{
	DBG_ASSERT(i_Val >= 0, "This value can't be negative");

	if (i_Val < 0)
		i_Val = 0;

	m_f1 = i_Val;
}

void g3dPointLight::SetFalloff2(float i_Val)
{
	DBG_ASSERT(i_Val >= 0, "This value can't be negative");

	if (i_Val < 0)
		i_Val = 0;

	m_f2 = i_Val;
}

void g3dPointLight::SetFalloff3(float i_Val)
{
	DBG_ASSERT(i_Val >= 0, "This value can't be negative");

	if (i_Val < 0)
		i_Val = 0;

	m_f3 = i_Val;
}

void g3dPointLight::SetFalloffStart(float i_Val)
{
	DBG_ASSERT(i_Val >= 0, "This value can't be negative");

	if (i_Val < 0)
		i_Val = 0;

	m_fStart = i_Val;
}

//--------------------------------------------------------------------
//	GetRange returns the range of the light
//--------------------------------------------------------------------
float g3dPointLight::GetRange() const
{
	return m_fRange;
}

//--------------------------------------------------------------------
//	SetRange sets the the range of the light
//--------------------------------------------------------------------
void g3dPointLight::SetRange(float i_Range)
{
	m_fRange = i_Range;
}

