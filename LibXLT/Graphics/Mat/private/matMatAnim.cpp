/****************************************************************************\
**  matMatAnim.cpp
**
**      matMatAnim.cpp defines the matMatAnim object, which represents a
**	material animation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/mat/matMatAnim.hpp"

//--------------------------------------------------------------------
//	The matMatAnim constructor requires a pointer to an anAnimation,
//	which will be henceforth owned by the matMatAnim.
//	The anAnimation is understood to operate
//	on the i_Param given.
//--------------------------------------------------------------------
matMatAnim::matMatAnim(	anAnimation* i_Anim,
						matMatParamIndex::matMatParamIndexType i_Param,
						float i_StartTime)
:	m_Animation(i_Anim),
	m_Param(i_Param),
	m_StartTime(i_StartTime),
	m_bActive(true),
	m_ColorAnim(NULL),
	m_FloatAnim(NULL),
	m_VectorAnim(NULL),
	m_bUseRealTime(false)
{
	setup_anim_type();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matMatAnim::matMatAnim(const matMatAnim& i_CopyFrom)
{
	this->m_Param = i_CopyFrom.m_Param;
	this->m_StartTime = i_CopyFrom.m_StartTime;
	this->m_bActive = i_CopyFrom.m_bActive;
	this->m_bUseRealTime = i_CopyFrom.m_bUseRealTime;

	this->m_Animation = i_CopyFrom.m_Animation->Clone();
	setup_anim_type();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matMatAnim::~matMatAnim()
{
	delete m_Animation;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matMatAnim::SetStartTime(float i_Time)
{
	m_StartTime = i_Time;
}

//--------------------------------------------------------------------
// Set whether to use real time for animation, or use the simulation
// time passed in to the renderer.
//--------------------------------------------------------------------
bool matMatAnim::GetUseRealTime() const
{
	return m_bUseRealTime;
}
void matMatAnim::SetUseRealTime(bool i_bUseRealTime)
{
	m_bUseRealTime = i_bUseRealTime;
}

//--------------------------------------------------------------------
// private - gets type of animation and sets up type variable
//--------------------------------------------------------------------
void matMatAnim::setup_anim_type()
{
	switch( m_Param )
	{
		case matMatParamIndex::e_Ambient:
		case matMatParamIndex::e_Diffuse:
		case matMatParamIndex::e_Specular:
		case matMatParamIndex::e_Emissive:
			m_ColorAnim = dynamic_cast<const anTypedAnimation<maFloatRGBA>*>(m_Animation);
			DBG_ASSERT(m_ColorAnim, "Wrong anim type for color");
		break;

		case matMatParamIndex::e_SpecularPower:
			m_FloatAnim = dynamic_cast<const anTypedAnimation<float>*>(m_Animation);
			DBG_ASSERT(m_FloatAnim, "Wrong anim type for specular power");
		break;

		case matMatParamIndex::e_TextureTranslation0:
		case matMatParamIndex::e_TextureTranslation1:
		case matMatParamIndex::e_TextureTranslation2:
		case matMatParamIndex::e_TextureTranslation3:
		case matMatParamIndex::e_TextureScale0:
		case matMatParamIndex::e_TextureScale1:
		case matMatParamIndex::e_TextureScale2:
		case matMatParamIndex::e_TextureScale3:
			m_VectorAnim = dynamic_cast<const anTypedAnimation<maVector2d>*>(m_Animation);
			DBG_ASSERT(m_VectorAnim, "Wrong anim type for texture translation or scale");
		break;

		case matMatParamIndex::e_TextureRotation0:
		case matMatParamIndex::e_TextureRotation1:
		case matMatParamIndex::e_TextureRotation2:
		case matMatParamIndex::e_TextureRotation3:
			m_FloatAnim = dynamic_cast<const anTypedAnimation<float>*>(m_Animation);
			DBG_ASSERT(m_FloatAnim, "Wrong anim type for texture rotation value");
		break;
	}
}

