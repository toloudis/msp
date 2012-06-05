/*****************************************************************************
**	smdlCharacterAnimation.cpp
**
**		A smdlCharacterAnimation
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlCharacterAnimation.hpp"

#include "Core/ma/maFunctions.hpp"


//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	// Need to get length of animation from key data.
	// This could be just the first node, or a search through
	// all keys to find longest.
	//----------------------------------------------------------------------------
	float get_key_length(const std::map<std::string, smdlVertexAnimKeys>& i_VertexKeys)
	{
		float max_len = 0.0f;

		std::map<std::string, smdlVertexAnimKeys>::const_iterator it;
		for (it = i_VertexKeys.begin(); it != i_VertexKeys.end(); ++it)
		{
			const smdlVertexAnimKeys &keys = (it->second);
			max_len = maFunctions::Highest(keys->GetAnimLength(), max_len);
		}
		return max_len;
	}	

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float get_key_length(const std::map<std::string, smdlMorphAnimKeys>& i_MorphKeys)
	{
		float max_len = 0.0f;

		std::map<std::string, smdlMorphAnimKeys>::const_iterator it;
		for (it = i_MorphKeys.begin(); it != i_MorphKeys.end(); ++it)
		{
			const smdlMorphAnimKeys &keys = (it->second);
			if (keys.GetBlend())
				max_len = maFunctions::Highest(keys.GetBlend()->GetLength(), max_len);
		}
		return max_len;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float get_key_length(const std::map<std::string, smdlGeoAnimKeys>& i_SkinKeys)
	{
		float max_len = 0.0f;

		std::map<std::string, smdlGeoAnimKeys>::const_iterator it;
		for (it = i_SkinKeys.begin(); it != i_SkinKeys.end(); ++it)
		{
			const smdlGeoAnimKeys &keys = (it->second);
			if (keys.GetVisible())
				max_len = maFunctions::Highest(keys.GetVisible()->GetLength(), max_len);
		}
		return max_len;
	}	

} // end of namespace


//--------------------------------------------------------------------
//	The smdlCharacterAnimation requires references to animation
//	keys.  The frame animation does not own the keys in
//	order to allow sharing of key data between animations
//--------------------------------------------------------------------
smdlCharacterAnimation::smdlCharacterAnimation(const smdlKeyRootMap& i_KeyRoots,
	const std::map<std::string, smdlMorphAnimKeys>& i_MorphKeys,
	const std::map<std::string, smdlVertexAnimKeys>& i_VertexKeys, 
	const std::map<std::string, smdlGeoAnimKeys>& i_SkinKeys, 
	float i_FrameRate)
: smdlGeoFrameAnimation(i_KeyRoots, i_FrameRate),
	m_MorphKeys(i_MorphKeys),
	m_VertexKeys(i_VertexKeys),
	m_SkinKeys(i_SkinKeys)
{
	// See if our morph animation is longer than the jointed animation
	float morph_len = get_key_length(m_MorphKeys);
	if (morph_len > this->GetNumFrames())
		this->SetNumFrames( morph_len );

	// See if our vertex animation is longer than the jointed animation
	float vert_len = get_key_length(m_VertexKeys);
	if (vert_len > this->GetNumFrames())
		this->SetNumFrames( vert_len );

	// See if our skin animation is longer than the jointed animation
	float skin_len = get_key_length(m_SkinKeys);
	if (skin_len > this->GetNumFrames())
		this->SetNumFrames( skin_len );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlCharacterAnimation::~smdlCharacterAnimation()
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::map<std::string, smdlMorphAnimKeys>& 
smdlCharacterAnimation::GetMorphKeys() const
{
	return m_MorphKeys;
}

//--------------------------------------------------------------------
// Look up name in map and return keys for this target. May return
// NULL if no keys for the given target name exist.
//--------------------------------------------------------------------
const smdlMorphAnimKeys* 
smdlCharacterAnimation::GetMorphKeysForTarget(const std::string &i_Name) const
{
	std::map<std::string, smdlMorphAnimKeys>::const_iterator it = m_MorphKeys.find(i_Name);
	if (it != m_MorphKeys.end())
	{
		return &(it->second);
	}
	return NULL;
}

//--------------------------------------------------------------------
//	Access to baked vertex animation keys
//--------------------------------------------------------------------
const std::map<std::string, smdlVertexAnimKeys>& smdlCharacterAnimation::GetVertexKeys() const
{
	return m_VertexKeys;
}

//--------------------------------------------------------------------
// Look up name in map and return vertex keys for this target. May return
// NULL if no keys for the given surface name exist.
//--------------------------------------------------------------------
const smdlVertexAnimKeys* smdlCharacterAnimation::GetVertexKeysForSurface(const std::string &i_Name) const
{
	std::map<std::string, smdlVertexAnimKeys>::const_iterator it = m_VertexKeys.find(i_Name);
	if (it != m_VertexKeys.end())
	{
		return &(it->second);
	}
	return NULL;
}

//--------------------------------------------------------------------
//	Access to skin animation keys
//--------------------------------------------------------------------
const std::map<std::string, smdlGeoAnimKeys>& smdlCharacterAnimation::GetSkinKeys() const
{
	return m_SkinKeys;
}

//--------------------------------------------------------------------
// Look up name in map and return skin keys for this target. May return
// NULL if no keys for the given surface name exist.
//--------------------------------------------------------------------
const smdlGeoAnimKeys* smdlCharacterAnimation::GetSkinKeysForSurface(const std::string &i_Name) const
{
	std::map<std::string, smdlGeoAnimKeys>::const_iterator it = m_SkinKeys.find(i_Name);
	if (it != m_SkinKeys.end())
	{
		return &(it->second);
	}
	return NULL;
}

//--------------------------------------------------------------------
//	CreateAnimInstance creates an anAnimInstance, of a type
//	appropriate to the type of the anAnimation.
//--------------------------------------------------------------------
anAnimInstance* smdlCharacterAnimation::CreateAnimInstance(float i_TimeOrigin) const
{
	return new smdlCharacterAnimInstance(i_TimeOrigin, *this);
}


//--------------------------------------------------------------------
//	Clone returns a copy of "this" allocated on the heap.
//--------------------------------------------------------------------
anAnimation* smdlCharacterAnimation::Clone() const
{
	return new smdlCharacterAnimation(*this);
}

//--------------------------------------------------------------------
//	The anFrameAnimInstance constructor requires the animation beginning
// time and the animation used by this instance.
//--------------------------------------------------------------------
smdlCharacterAnimInstance::smdlCharacterAnimInstance(float i_TimeOrigin,
									const smdlCharacterAnimation& i_Anim)
:	smdlGeoFrameAnimInstance(i_TimeOrigin, i_Anim),
	m_Anim(i_Anim)
{

}

//--------------------------------------------------------------------
//	pure virtual destructor
//--------------------------------------------------------------------
smdlCharacterAnimInstance::~smdlCharacterAnimInstance()
{

}

//--------------------------------------------------------------------
//	The Clone function creates a new copy of the anFrameAnimInstance
//	on the heap.
//--------------------------------------------------------------------
anAnimInstance* smdlCharacterAnimInstance::Clone() const
{
	return m_Anim.CreateAnimInstance(this->GetTimeOrigin());
}

