/*****************************************************************************
**	smdlGeoAnimatableObject.cpp
**
**		smdlGeoAnimatableObject is an abstract base for scene objects
**	which can be animated.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlGeoAnimatableObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/sc/scControlAnim.hpp"
#include "Graphics/smdl/private/smdlSubAnimation.hpp"
#include "Graphics/smdl/smdlGeoFrameAnimation.hpp"


//--------------------------------------------------------------------
//	The object will initially
//	be placed at the origin with unit scale and no rotation.
//--------------------------------------------------------------------
smdlGeoAnimatableObject::smdlGeoAnimatableObject()
:	m_Cur(NULL),
	m_Blend(NULL),
	m_CurStartTime(0.0f),
	m_BlendStartTime(0.0f),
	m_BlendLengthTime(0.0f),
	m_BlendNext(NULL),
	m_BlendNextStartTime(0.0f),
	m_BlendNextLengthTime(0.0f),
	m_bDirty(true),
	m_LastAnimateTime(-1.0f)
{
	SetPeriod( 11 );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlGeoAnimatableObject::~smdlGeoAnimatableObject()
{
	delete m_Cur;
	delete m_Blend;

	envSTLHelpers::DeleteContainer(m_SubAnims);
}

//--------------------------------------------------------------------
// CheckAnimation checks compatibility of this animation 
// with the model. It will return true if the animation can
// be played on this model.
//--------------------------------------------------------------------
//virtual 
bool smdlGeoAnimatableObject::CheckAnimation( const anFrameAnimation& i_GeoAnimation ) const
{
	// Check for correct type of animation
	const smdlGeoFrameAnimation* pAnimation = dynamic_cast<const smdlGeoFrameAnimation*>(&i_GeoAnimation);
	return (pAnimation != NULL);
}

//--------------------------------------------------------------------
//	SetAnimation makes the given animation into the current animation
//	(but does not take ownership of it the client must preserve it
//	as long as the smdlGeoAnimatableObject needs it).  Setting an animation
//	with this function will not blend it - just clobber the old one.
//--------------------------------------------------------------------
anFrameAnimInstance* smdlGeoAnimatableObject::SetAnimation(
										const anFrameAnimation& i_GeoAnimation,
										float i_StartTime)
{
	m_bDirty = true;
	this->ClearAnimation();

	m_Cur = dynamic_cast<smdlGeoFrameAnimInstance*>(
					i_GeoAnimation.CreateAnimInstance(i_StartTime));

	m_CurStartTime = i_StartTime;
	return m_Cur;
}

//--------------------------------------------------------------------
//	BlendAnimation causes a new animation to be blended to the current
//	animation over "i_BlendTime" length of time.  At the end of
//	i_BlendTime, the object's animation will be exactly the new
//	animation.
//--------------------------------------------------------------------
anFrameAnimInstance* smdlGeoAnimatableObject::BlendAnimation(
										const anFrameAnimation& i_GeoAnimation,
										float i_StartTime,
										float i_BlendTime, 
										bool i_bSmoothBlend,
										float i_EaseInWeight, 
										float i_EaseOutWeight)
{
	m_bDirty = true;
	if ( m_Cur == NULL )
	{
		//	if we don't have any current animation,
		//	just set the current animation to the blend
		return this->SetAnimation(i_GeoAnimation, i_StartTime);
	}
	else if ( !m_Blend )
	{
		delete m_Blend;

		m_Blend = dynamic_cast<smdlGeoFrameAnimInstance*>(
					i_GeoAnimation.CreateAnimInstance(i_StartTime));

		m_BlendStartTime = i_StartTime;
		m_BlendLengthTime = i_BlendTime;

		m_bSmoothBlend = i_bSmoothBlend;
		m_EaseInWeight = i_EaseInWeight; 
		m_EaseOutWeight = i_EaseOutWeight;

		return m_Blend;
	}
	else
	{
		// queue the incoming blend, if anything was there, replace it.
		delete m_BlendNext;

		m_BlendNext = dynamic_cast<smdlGeoFrameAnimInstance*>(
					i_GeoAnimation.CreateAnimInstance(i_StartTime));

		m_BlendNextStartTime = i_StartTime;  // reset in SwitchToBlend
		m_BlendNextLengthTime = i_BlendTime;

		m_bSmoothBlend = i_bSmoothBlend;
		m_EaseInWeight = i_EaseInWeight; 
		m_EaseOutWeight = i_EaseOutWeight;

		return m_BlendNext;
	}
}

//--------------------------------------------------------------------
//	ClearBlend will remove the blend animation and cause the "main"
//	animation to be the only animation applied to the object.
//--------------------------------------------------------------------
void smdlGeoAnimatableObject::ClearBlend()
{
	m_bDirty = true;
	delete m_Blend;
	m_Blend = NULL;
	delete m_BlendNext;
	m_BlendNext = NULL;
}

//--------------------------------------------------------------------
//	GetBlendRatio will return a number from 0 to 1 representing the
//	amount of the blend animation that should be present.  The
//	amount of the original animation that should be present is 1 minus
//	this number.  When this number is 1 SwitchToBlend should be
//	called.
//--------------------------------------------------------------------
float smdlGeoAnimatableObject::GetBlendRatio(float i_Time) const
{
	if ( m_Cur == NULL ) return 0.0f;
	if ( m_Blend == NULL ) return 0.0f;

	if (m_BlendLengthTime == 0.0f)
		return 1.0f;

	// This was the original blend, the blend occurred after
	// the second animation already started, so that they blended
	// while the animation played.
	//if ( (m_BlendStartTime - i_Time) >= m_BlendLengthTime )
	//	return 1.0f;
	//
	//return (i_Time - m_BlendStartTime) / m_BlendLengthTime;

	// The new blend method blends before the second animation starts
	if (i_Time >= m_BlendStartTime)
		return 1.0f;

	if ( (m_BlendStartTime - i_Time) >= m_BlendLengthTime )
		return 0.0f;

	return 1.0f - (m_BlendStartTime - i_Time) / m_BlendLengthTime;
}

//--------------------------------------------------------------------
//	SwitchToBlend makes the blend animation into the current animation
//	and gets rid of the old current animation.
//--------------------------------------------------------------------
void smdlGeoAnimatableObject::SwitchToBlend(float i_Time)
{
	m_bDirty = true;
	delete m_Cur;
	m_Cur = m_Blend;
	m_CurStartTime = m_BlendStartTime;
	m_Blend = m_BlendNext;
	m_BlendNext = NULL;
	m_BlendNextStartTime = i_Time;
}

//--------------------------------------------------------------------
//	ClearAnimation causes all animation to stop.
//--------------------------------------------------------------------
void smdlGeoAnimatableObject::ClearAnimation()
{
	m_bDirty = true;
	this->ClearBlend();
	delete m_Cur;
	m_Cur = NULL;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
anFrameAnimInstance* smdlGeoAnimatableObject::AddSubAnimation(
					const anFrameAnimation& i_GeoAnimation,
					float i_StartTime,
					bool i_bPreserve)
{
	m_bDirty = true;
	smdlGeoFrameAnimInstance* anim_instance = dynamic_cast<smdlGeoFrameAnimInstance*>(
					i_GeoAnimation.CreateAnimInstance(i_StartTime));

	smdlSubAnimation *sub_anim = new smdlSubAnimation(anim_instance);
	sub_anim->SetPreserve(i_bPreserve);
	m_SubAnims.push_back(sub_anim);
	return anim_instance;
}

//--------------------------------------------------------------------
//	Remove sub animation 
//--------------------------------------------------------------------
void smdlGeoAnimatableObject::RemoveSubAnimation( anFrameAnimInstance* i_SubAnimInstance )
{
	std::vector<smdlSubAnimation*>::iterator it, end = m_SubAnims.end();
	for (it = m_SubAnims.begin(); it != end; ++it)
	{
		if ((*it)->GetAnimInstance() == i_SubAnimInstance)
		{
			m_bDirty = true;
			m_SubAnims.erase(it);
			break;
		}
	}
}

//--------------------------------------------------------------------
// SetSubAnimationBlend sets how the given subanimation should 
// blend with the base anim. Values from 0-1. The given subanimation
// should have been given in a call to AddSubAnimation first.
//--------------------------------------------------------------------
void smdlGeoAnimatableObject::SetSubAnimationBlend( const anFrameAnimInstance* i_SubAnim, 
					float i_Blend )
{
	std::vector<smdlSubAnimation*>::iterator it, end = m_SubAnims.end();
	for (it = m_SubAnims.begin(); it != end; ++it)
	{
		if ((*it)->GetAnimInstance() == i_SubAnim)
		{
			(*it)->SetBlend(i_Blend);
			m_bDirty = true;
			return;
		}
	}

	if (i_Blend > 0.0)
	{
		DBG_LOG("Could not find subanimation to set blend.");
	}
}

//--------------------------------------------------------------------
//	ClearAllSubAnimations causes all sub animations to stop.
//--------------------------------------------------------------------
void smdlGeoAnimatableObject::ClearAllSubAnimations()
{
	m_bDirty = true;
	envSTLHelpers::DeleteContainer(m_SubAnims);
}

//--------------------------------------------------------------------
//	ClearSubAnimations causes non-preserved sub animations to stop.
//--------------------------------------------------------------------
void smdlGeoAnimatableObject::ClearSubAnimations()
{
	m_bDirty = true;
	for (int i=m_SubAnims.size()-1; i>=0; i--)
	{
		if (!m_SubAnims[i]->GetPreserve())
		{
			delete m_SubAnims[i];
			m_SubAnims.erase(m_SubAnims.begin() + i);
		}
	}
}

//--------------------------------------------------------------------
// Set dirty bit to ensure animation the next frame.
//--------------------------------------------------------------------
void smdlGeoAnimatableObject::MarkDirty()
{	
	m_bDirty = true;
}

//--------------------------------------------------------------------
//	GetNumSubAnims returns number of subanims currently running
//--------------------------------------------------------------------
int smdlGeoAnimatableObject::GetNumSubAnims() const
{
	return m_SubAnims.size();
}

//--------------------------------------------------------------------
//	GetSubAnimInstance returns given subanim index
//--------------------------------------------------------------------
const smdlGeoFrameAnimInstance* smdlGeoAnimatableObject::GetSubAnimInstance(int i_Index) const
{
	return m_SubAnims[i_Index]->GetAnimInstance();
}

//--------------------------------------------------------------------
//	GetSubAnimation returns sub animtion pointer
//--------------------------------------------------------------------
const smdlSubAnimation* smdlGeoAnimatableObject::GetSubAnimation(int i_Index) const
{
	return m_SubAnims[i_Index];
}

//--------------------------------------------------------------------
// CheckDirty - return true if the object needs to animate the model.
//		Clears the dirty bit so that if the time is different
//		it will be dirty next call.
//--------------------------------------------------------------------
bool smdlGeoAnimatableObject::CheckDirty(float i_SimTime)
{
	// Check if control animations are dirty
	bool controls_dirty = false;
	std::vector<scControlAnim*>::iterator it, end = this->m_ControlAnims.end();
	for (it = m_ControlAnims.begin(); it != end; ++it)
	{
		controls_dirty |= (*it)->CheckDirty(i_SimTime);
	}

	if ((!m_bDirty) && (!controls_dirty) && (i_SimTime == m_LastAnimateTime))
		return false;

	m_bDirty = false;
	m_LastAnimateTime = i_SimTime;
	return true;
}
