/*****************************************************************************
**	smdlGeoAnimatableObject.hpp
**
**		smdlGeoAnimatableObject is an abstract base for scene objects
**	which can be animated.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_GEOANIMATABLEOBJECT_HPP
#error smdlGeoAnimatableObject.hpp multiply included
#endif
#define SMDL_GEOANIMATABLEOBJECT_HPP

#ifndef SC_ANIMATABLEOBJECT_HPP
#include "Graphics/sc/scAnimatableObject.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class smdlGeoFrameAnimInstance;
class smdlSubAnimation;
class smdlSubAnimationIterator;


//============================================================================
//============================================================================
class smdlGeoAnimatableObject : public scAnimatableObject
{
	public:
		//--------------------------------------------------------------------
		//	The object will initially
		//	be placed at the origin with unit scale and no rotation.
		//--------------------------------------------------------------------
		smdlGeoAnimatableObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~smdlGeoAnimatableObject() = 0;

		//--------------------------------------------------------------------
		// CheckAnimation checks compatibility of this animation 
		// with the model. It will return true if the animation can
		// be played on this model.
		//--------------------------------------------------------------------
		virtual bool CheckAnimation( const anFrameAnimation& i_GeoAnimation ) const;

		//--------------------------------------------------------------------
		//	SetAnimation makes the given animation into the current animation
		//	(but does not take ownership of it; the client must preserve it
		//	as long as the smdlGeoAnimatableObject needs it).  Setting an animation
		//	with this function will not blend it - just clobber the old one.
		//	A pointer to the created animation instance is returned.
		//--------------------------------------------------------------------
		anFrameAnimInstance* SetAnimation(
							const anFrameAnimation& i_GeoAnimation,
							float i_StartTime);

		//--------------------------------------------------------------------
		//	BlendAnimation causes a new animation to be blended to the current
		//	animation over "i_BlendTime" length of time.  At the end of
		//	i_BlendTime, the object's animation will be exactly the new
		//	animation.
		//	A pointer to the created animation instance is returned.
		//--------------------------------------------------------------------
		anFrameAnimInstance* BlendAnimation(
								const anFrameAnimation& i_GeoAnimation,
								float i_StartTime,
								float i_BlendTime, 
								bool i_bSmoothBlend = false,
								float i_EaseInWeight = 1.0f, 
								float i_EaseOutWeight = 1.0f);

		//--------------------------------------------------------------------
		//	ClearBlend will remove the blend animation and cause the "main"
		//	animation to be the only animation applied to the object.
		//--------------------------------------------------------------------
		void ClearBlend();

		//--------------------------------------------------------------------
		//	ClearAnimation causes all animation to stop.
		//--------------------------------------------------------------------
		void ClearAnimation();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual anFrameAnimInstance* AddSubAnimation(
							const anFrameAnimation& i_GeoAnimation,
							float i_StartTime,
							bool i_bPreserve);

		//--------------------------------------------------------------------
		//	Remove sub animation 
		//--------------------------------------------------------------------
		virtual void RemoveSubAnimation( anFrameAnimInstance* i_SubAnimInstance);

		//--------------------------------------------------------------------
		// SetSubAnimationBlend sets how the given subanimation should 
		// blend with the base anim. Values from 0-1. The given subanimation
		// should have been returned from a call to AddSubAnimation first.
		//--------------------------------------------------------------------
		virtual void SetSubAnimationBlend( const anFrameAnimInstance* i_SubAnim, 
							float i_Blend);

		//--------------------------------------------------------------------
		//	ClearAllSubAnimations causes all sub animations to stop.
		//--------------------------------------------------------------------
		virtual void ClearAllSubAnimations();

		//--------------------------------------------------------------------
		//	ClearSubAnimations causes non-preserved sub animations to stop.
		//--------------------------------------------------------------------
		virtual void ClearSubAnimations();
		
		//--------------------------------------------------------------------
		// Set dirty bit to ensure animation the next frame.
		//--------------------------------------------------------------------
		void MarkDirty();

	protected:

		//--------------------------------------------------------------------
		//	GetCurAnimInstance returns the anim instance for the cur anim
		//--------------------------------------------------------------------
		const smdlGeoFrameAnimInstance* GetCurAnimInstance() const;

		//--------------------------------------------------------------------
		//	GetCurAnimInstance returns the anim instance for the cur anim
		//--------------------------------------------------------------------
		const smdlGeoFrameAnimInstance* GetBlendAnimInstance() const;

		//--------------------------------------------------------------------
		//	GetCurStartTime returns the start time for the current animation
		//--------------------------------------------------------------------
		float GetCurStartTime() const;

		//--------------------------------------------------------------------
		//	GetBlendStartTime returns the start time for the current blend
		//	animation.
		//--------------------------------------------------------------------
		float GetBlendStartTime() const;

		//--------------------------------------------------------------------
		//	GetBlendLength returns the length of time over which the blend
		//	animation will be introduced.
		//--------------------------------------------------------------------
		float GetBlendLength() const;

		//--------------------------------------------------------------------
		// Smooth blending parameters
		//--------------------------------------------------------------------
		inline bool IsSmoothBlend() const;
		inline float GetEaseInWeight() const;
		inline float GetEaseOutWeight() const;

		//--------------------------------------------------------------------
		//	GetBlendRatio will return a number from 0 to 1 representing the
		//	amount of the blend animation that should be present.  The
		//	amount of the original animation that should be present is 1 minus
		//	this number.  When this number is 1 SwitchToBlend should be
		//	called.
		//--------------------------------------------------------------------
		float GetBlendRatio(float i_Time) const;

		//--------------------------------------------------------------------
		//	SwitchToBlend makes the blend animation into the current animation
		//	and gets rid of the old current animation.
		//--------------------------------------------------------------------
		void SwitchToBlend(float i_Time);

		//--------------------------------------------------------------------
		//	GetNumSubAnims returns number of subanims currently running
		//--------------------------------------------------------------------
		int GetNumSubAnims() const;

		//--------------------------------------------------------------------
		//	GetSubAnimInstance returns given subanim index
		//--------------------------------------------------------------------
		const smdlGeoFrameAnimInstance* GetSubAnimInstance(int i_Index) const;

		//--------------------------------------------------------------------
		//	GetSubAnimation returns sub animtion pointer
		//--------------------------------------------------------------------
		const smdlSubAnimation* GetSubAnimation(int i_Index) const;

		//--------------------------------------------------------------------
		// CheckDirty - return true if the object needs to animate the model.
		//		Clears the dirty bit so that if the time is different
		//		it will be dirty next call.
		//--------------------------------------------------------------------
		bool CheckDirty(float i_SimTime);

	private:
		smdlGeoFrameAnimInstance* m_Cur;
		smdlGeoFrameAnimInstance* m_Blend;
		float m_CurStartTime;
		float m_BlendStartTime;
		float m_BlendLengthTime;

		// The next blend.  We queue exactly one next anim that is
		// replaced if the previous has not finished blending yet.
		smdlGeoFrameAnimInstance* m_BlendNext;
		// start time is set to the sim time at switch to blend
		float m_BlendNextStartTime;
		float m_BlendNextLengthTime;

		std::vector<smdlSubAnimation*> m_SubAnims;

		// dirty bits
		bool m_bDirty;
		float m_LastAnimateTime;
		
		// smooth blending attributes
		bool m_bSmoothBlend;
		float m_EaseInWeight; 
		float m_EaseOutWeight;
};

//--------------------------------------------------------------------
//	GetCurAnimInstance returns the anim instance for the cur anim
//--------------------------------------------------------------------
inline const smdlGeoFrameAnimInstance*
smdlGeoAnimatableObject::GetCurAnimInstance() const
{
	return m_Cur;
}

//--------------------------------------------------------------------
//	GetCurAnimInstance returns the anim instance for the cur anim
//--------------------------------------------------------------------
inline const smdlGeoFrameAnimInstance*
smdlGeoAnimatableObject::GetBlendAnimInstance() const
{
	return m_Blend;
}

//--------------------------------------------------------------------
//	GetCurStartTime returns the start time for the current animation
//--------------------------------------------------------------------
inline float smdlGeoAnimatableObject::GetCurStartTime() const
{
	return m_CurStartTime;
}

//--------------------------------------------------------------------
//	GetBlendStartTime returns the start time for the current blend
//	animation.
//--------------------------------------------------------------------
inline float smdlGeoAnimatableObject::GetBlendStartTime() const
{
	return m_BlendStartTime;
}

//--------------------------------------------------------------------
//	GetBlendLength returns the length of time over which the blend
//	animation will be introduced.
//--------------------------------------------------------------------
inline float smdlGeoAnimatableObject::GetBlendLength() const
{
	return m_BlendLengthTime;
}
//--------------------------------------------------------------------
// Smooth blending parameters
//--------------------------------------------------------------------
inline bool smdlGeoAnimatableObject::IsSmoothBlend() const
{
	return m_bSmoothBlend;
}
inline float smdlGeoAnimatableObject::GetEaseInWeight() const
{
	return m_EaseInWeight;
}
inline float smdlGeoAnimatableObject::GetEaseOutWeight() const
{
	return m_EaseOutWeight;
}
