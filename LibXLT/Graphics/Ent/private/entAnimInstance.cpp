/*****************************************************************************
**  entAnimInstance.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/ent/entAnimInstance.hpp"
#include "Graphics/ent/entAnimation.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
entAnimInstance::entAnimInstance(float i_TimeOrigin,
								const entAnimation& i_Anim)
:	anFrameAnimInstance(i_TimeOrigin, i_Anim),
	m_Anim(i_Anim)
{
}

//--------------------------------------------------------------------
//	pure virtual destructor
//--------------------------------------------------------------------
entAnimInstance::~entAnimInstance()
{
}


//--------------------------------------------------------------------
//	Child classes must implement the clone function to provide a
//	copy of themselves.
//--------------------------------------------------------------------
anAnimInstance* entAnimInstance::Clone() const
{
	return m_Anim.CreateAnimInstance(this->GetTimeOrigin());
}
