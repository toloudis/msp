/*****************************************************************************
**	smdlMorphFrameAnimation.cpp
**
**		A smdlMorphFrameAnimation
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlMorphFrameAnimation.hpp"


//============================================================================
//============================================================================
namespace
{

//----------------------------------------------------------------------------
// Need to get length of animation from key data.
// This could be just the first node, or a search through
// all keys to find longest.
//----------------------------------------------------------------------------
float get_key_length(const std::map<std::string, smdlMorphAnimKeys>& i_Map)
{
	float max_len = 0.0f;

	std::map<std::string, smdlMorphAnimKeys>::const_iterator it, end = i_Map.end();
	for (it = i_Map.begin(); it!= end; ++it)
	{
		const smdlMorphAnimKeys& instance = it->second;
		if (instance.GetBlend() && instance.GetBlend()->GetLength() > max_len)
			max_len = instance.GetBlend()->GetLength();
	}
	return max_len;
}

} // end of namespace


//--------------------------------------------------------------------
//	The smdlMorphFrameAnimation requires a reference to animation
//	keys.  The frame animation does not own the keys in
//	order to allow sharing of key data between animations
//--------------------------------------------------------------------
smdlMorphFrameAnimation::smdlMorphFrameAnimation(const std::map<std::string, smdlMorphAnimKeys>& i_Keys, float i_FrameRate)
:	m_Keys(i_Keys)
{
	this->SetFrameRate(i_FrameRate);
	this->SetNumFrames( get_key_length(i_Keys) );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlMorphFrameAnimation::~smdlMorphFrameAnimation()
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::map<std::string, smdlMorphAnimKeys>& 
smdlMorphFrameAnimation::GetKeys() const
{
	return m_Keys;
}

//--------------------------------------------------------------------
// Look up name in map and return keys for this target. May return
// NULL if no keys for the given target name exist.
//--------------------------------------------------------------------
const smdlMorphAnimKeys* 
smdlMorphFrameAnimation::GetKeysForTarget(const std::string &i_Name) const
{
	std::map<std::string, smdlMorphAnimKeys>::const_iterator it = m_Keys.find(i_Name);
	if (it != m_Keys.end())
	{
		return &(it->second);
	}
	return NULL;
}

//--------------------------------------------------------------------
//	CreateAnimInstance creates an anAnimInstance, of a type
//	appropriate to the type of the anAnimation.
//--------------------------------------------------------------------
anAnimInstance* smdlMorphFrameAnimation::CreateAnimInstance(float i_TimeOrigin) const
{
	return new smdlMorphFrameAnimInstance(i_TimeOrigin, *this);
}

//--------------------------------------------------------------------
//	Clone returns a copy of "this" allocated on the heap.
//--------------------------------------------------------------------
anAnimation* smdlMorphFrameAnimation::Clone() const
{
	return new smdlMorphFrameAnimation(*this);
}

//--------------------------------------------------------------------
//	The anFrameAnimInstance constructor requires the animation beginning
// time and the animation used by this instance.
//--------------------------------------------------------------------
smdlMorphFrameAnimInstance::smdlMorphFrameAnimInstance(float i_TimeOrigin,
									const smdlMorphFrameAnimation& i_Anim)
:	entAnimInstance(i_TimeOrigin, i_Anim),
	m_Anim(i_Anim)
{

}

//--------------------------------------------------------------------
//	pure virtual destructor
//--------------------------------------------------------------------
smdlMorphFrameAnimInstance::~smdlMorphFrameAnimInstance()
{

}

//--------------------------------------------------------------------
//	The Clone function creates a new copy of the anFrameAnimInstance
//	on the heap.
//--------------------------------------------------------------------
anAnimInstance* smdlMorphFrameAnimInstance::Clone() const
{
	return m_Anim.CreateAnimInstance(this->GetTimeOrigin());
}
