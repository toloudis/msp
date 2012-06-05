/*****************************************************************************
**  matShaderEffect.cpp
**
**    This is an abstract base class for a programmatic effect that may
**	involve multiple passes with different render settings.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Graphics/mat/matShaderEffect.hpp"

#include "Core/ma/maConstants.hpp"

//====================================================================
//====================================================================
matShaderEffect::matShaderEffect()
{
}

//====================================================================
//====================================================================
matShaderEffect::~matShaderEffect()
{
}

effUVTransform::effUVTransform(const effUVTransform& i_CopyFrom)
:	m_UScale(i_CopyFrom.m_UScale),
	m_VScale(i_CopyFrom.m_VScale),
	m_UTrans(i_CopyFrom.m_UTrans),
	m_VTrans(i_CopyFrom.m_VTrans),
	m_UVAngle(i_CopyFrom.m_UVAngle)
{
}
effUVTransform& effUVTransform::operator = (const effUVTransform& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	m_UScale = i_CopyFrom.m_UScale;
	m_VScale = i_CopyFrom.m_VScale;
	m_UTrans = i_CopyFrom.m_UTrans;
	m_VTrans = i_CopyFrom.m_VTrans;
	m_UVAngle = i_CopyFrom.m_UVAngle;

	return *this;
}

// uv transformation parameters are a feature of all material shaders!
maMatrix4x4 effUVTransform::MakeUVTransform() const
{
	// scale, then rotate, then translate.
	maMatrix4x4 m;

	m.MakeScale(m_UScale, m_VScale, 1);
	m.RotateBy(m_UVAngle * maConstants::c_fAngleToRad, maVector3d(0,0,1));
	m.TranslateBy(m_UTrans, m_VTrans, 0);

	m.Transpose();
	return m;
}

