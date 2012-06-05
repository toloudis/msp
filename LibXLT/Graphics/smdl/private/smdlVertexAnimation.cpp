/*****************************************************************************
**	smdlVertexAnimation.cpp
**
**		A smdlVertexAnimation
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlVertexAnimation.hpp"


//============================================================================
//============================================================================
namespace
{

//----------------------------------------------------------------------------
// Need to get length of animation from key data.
// This could be just the first node, or a search through
// all keys to find longest.
//----------------------------------------------------------------------------
float get_key_length(const std::vector<smdlVertexAnimKeys>& i_VertexKeys)
{
	float max_len = 0.0f;

	std::vector<smdlVertexAnimKeys>::const_iterator it, end = i_VertexKeys.end();
	for (it = i_VertexKeys.begin(); it != end; ++it)
	{
		const smdlVertexAnimKeys &keys = (*it);
		float len = keys->GetAnimLength();
		if (len > max_len)
			max_len = len;
	}

	return max_len;
}

} // end of namespace


//--------------------------------------------------------------------
//	The smdlVertexAnimation requires references to animation
//	keys.  The frame animation does not own the keys in
//	order to allow sharing of key data between animations
//--------------------------------------------------------------------
smdlVertexAnimation::smdlVertexAnimation(const std::vector<smdlVertexAnimKeys>& i_VertexKeys, 
	float i_FrameRate)
:	m_VertexKeys(i_VertexKeys)
{
	this->SetFrameRate(i_FrameRate);
	this->SetNumFrames( get_key_length(m_VertexKeys) );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlVertexAnimation::~smdlVertexAnimation()
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::vector<smdlVertexAnimKeys>& 
smdlVertexAnimation::GetVertexKeys() const
{
	return m_VertexKeys;
}

//--------------------------------------------------------------------
//	CreateAnimInstance creates an anAnimInstance, of a type
//	appropriate to the type of the anAnimation.
//--------------------------------------------------------------------

anAnimInstance* smdlVertexAnimation::CreateAnimInstance(float i_TimeOrigin) const
{
	return new smdlVertexAnimInstance(i_TimeOrigin, *this);
}


//--------------------------------------------------------------------
//	Clone returns a copy of "this" allocated on the heap.
//--------------------------------------------------------------------
anAnimation* smdlVertexAnimation::Clone() const
{
	return new smdlVertexAnimation(*this);
}


//--------------------------------------------------------------------
//	The anFrameAnimInstance constructor requires the animation beginning
// time and the animation used by this instance.
//--------------------------------------------------------------------
smdlVertexAnimInstance::smdlVertexAnimInstance(float i_TimeOrigin,
									const smdlVertexAnimation& i_Anim)
:	entAnimInstance(i_TimeOrigin, i_Anim),
	m_Anim(i_Anim)
{
}

//--------------------------------------------------------------------
//	pure virtual destructor
//--------------------------------------------------------------------
smdlVertexAnimInstance::~smdlVertexAnimInstance()
{
}

//--------------------------------------------------------------------
//	The Clone function creates a new copy of the anFrameAnimInstance
//	on the heap.
//--------------------------------------------------------------------
anAnimInstance* smdlVertexAnimInstance::Clone() const
{
	return m_Anim.CreateAnimInstance(this->GetTimeOrigin());
}

