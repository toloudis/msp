/*****************************************************************************
**	smdlGeoFrameAnimation.cpp
**
**		A smdlGeoFrameAnimation
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlGeoFrameAnimation.hpp"


//============================================================================
//============================================================================
namespace
{
//----------------------------------------------------------------------------
// Need to get length of animation from key data.
// This could be just the first node, or a search through
// all keys to find longest.
//----------------------------------------------------------------------------
float get_key_length(smdlTree<smdlGeoAnimKeys>::const_iterator& io_Instance)
{
	float max_len = 0.0f;

	const smdlGeoAnimKeys& instance = io_Instance.GetData();
	if (instance.GetTranslate() && instance.GetTranslate()->GetLength() > max_len)
		max_len = instance.GetTranslate()->GetLength();
	if (instance.GetRotate() && instance.GetRotate()->GetLength() > max_len)
		max_len = instance.GetRotate()->GetLength();
	if (instance.GetScale() && instance.GetScale()->GetLength() > max_len)
		max_len = instance.GetScale()->GetLength();

	//DBG_ASSERT(false, "Could not get key length from root, needs search.");

	int num_children = io_Instance.NumChildren();
	for (int i = 0 ; i < num_children ; i++ )
	{
		io_Instance.MoveToChild(i);
		float len = get_key_length(io_Instance);
		io_Instance.MoveToParent();
		if (len > max_len)
			max_len = len;
	}

	return max_len;
}

} // end of namespace


//--------------------------------------------------------------------
//	The smdlGeoFrameAnimation requires a reference to animation
//	keys.  The frame animation does not own the keys in
//	order to allow sharing of key data between animations
//--------------------------------------------------------------------
smdlGeoFrameAnimation::smdlGeoFrameAnimation(const smdlKeyRootMap& i_KeyRoots, 
											 float i_FrameRate)
: m_KeyRoots(i_KeyRoots), m_bAdditive(false), m_bAttachToRootJoint(false), m_bIgnoreJointOrientation(false)
{
	this->SetFrameRate(i_FrameRate);
	
	// Find maximum length of animation data
	float max_len = 0.0f;
	smdlKeyRootMap::const_iterator it;
	for (it = i_KeyRoots.begin(); it != i_KeyRoots.end(); ++it)
	{
		const smdlTree<smdlGeoAnimKeys> &keys = (*it->second);
		float anim_len = get_key_length(keys.GetIterator());
		if (anim_len > max_len)
			max_len = anim_len;
	}
	this->SetNumFrames( max_len );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlGeoFrameAnimation::~smdlGeoFrameAnimation()
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const smdlKeyRootMap& smdlGeoFrameAnimation::GetKeyRoots() const
{
	return m_KeyRoots;
}

//--------------------------------------------------------------------
//	CreateAnimInstance creates an anAnimInstance, of a type
//	appropriate to the type of the anAnimation.
//--------------------------------------------------------------------
anAnimInstance* smdlGeoFrameAnimation::CreateAnimInstance(float i_TimeOrigin) const
{
	return new smdlGeoFrameAnimInstance(i_TimeOrigin, *this);
}

//--------------------------------------------------------------------
//	Clone returns a copy of "this" allocated on the heap.
//--------------------------------------------------------------------
anAnimation* smdlGeoFrameAnimation::Clone() const
{
	return new smdlGeoFrameAnimation(*this);
}

//--------------------------------------------------------------------
// Name of joint that is root of this subanimation
//--------------------------------------------------------------------
//void smdlGeoFrameAnimation::SetNameOfRoot(const std::string& i_Name)
//{
//	m_NameOfRoot = i_Name;
//}
//const std::string& smdlGeoFrameAnimation::GetNameOfRoot() const
//{
//	return m_NameOfRoot;
//}

//--------------------------------------------------------------------
// Return true if this is a subanimation with a defined name of root
//--------------------------------------------------------------------
//bool smdlGeoFrameAnimation::HasRootName() const
//{
//	return (!m_NameOfRoot.empty());
//}

//--------------------------------------------------------------------
// AdditiveAnimation is true if the transformations are deltas
//	from the base pose and can be used in additive subanimations.
//--------------------------------------------------------------------
void smdlGeoFrameAnimation::SetAdditiveAnimation(bool i_Delta)
{
	m_bAdditive = i_Delta;
}
bool smdlGeoFrameAnimation::GetAdditiveAnimation() const
{
	return m_bAdditive;
}

//--------------------------------------------------------------------
// AttachToRootJoint is used to support older file formats
//	on newer hierarchies. It means that the animation
//	should attach to the first joint it finds in the hierarchy.
//--------------------------------------------------------------------
void smdlGeoFrameAnimation::SetAttachToRootJoint(bool i_Attach)
{
	m_bAttachToRootJoint = i_Attach;
}
bool smdlGeoFrameAnimation::GetAttachToRootJoint() const
{
	return m_bAttachToRootJoint;
}

//--------------------------------------------------------------------
// Some animation that isn't coming from Maya should ignore the
//	joint orientation built into the scene graph.
//--------------------------------------------------------------------
void smdlGeoFrameAnimation::SetIgnoreJointOrientation(bool i_bIgnore)
{
	m_bIgnoreJointOrientation = i_bIgnore;
}
bool smdlGeoFrameAnimation::GetIgnoreJointOrientation() const
{
	return m_bIgnoreJointOrientation;
}

//--------------------------------------------------------------------
//	The anFrameAnimInstance constructor requires the animation beginning
// time and the animation used by this instance.
//--------------------------------------------------------------------
smdlGeoFrameAnimInstance::smdlGeoFrameAnimInstance(float i_TimeOrigin,
									const smdlGeoFrameAnimation& i_Anim)
:	entAnimInstance(i_TimeOrigin, i_Anim),
	m_Anim(i_Anim)
{
}

//--------------------------------------------------------------------
//	pure virtual destructor
//--------------------------------------------------------------------
smdlGeoFrameAnimInstance::~smdlGeoFrameAnimInstance()
{
}

//--------------------------------------------------------------------
//	The Clone function creates a new copy of the anFrameAnimInstance
//	on the heap.
//--------------------------------------------------------------------
anAnimInstance* smdlGeoFrameAnimInstance::Clone() const
{
	return m_Anim.CreateAnimInstance(this->GetTimeOrigin());
}

