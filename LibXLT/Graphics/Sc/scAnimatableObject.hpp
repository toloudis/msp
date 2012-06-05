/*****************************************************************************
**	scAnimatableObject.hpp
**
**		scAnimatableObject is an abstract base for scene objects
**	which can be animated from frame animation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SC_ANIMATABLEOBJECT_HPP
#error scAnimatableObject.hpp multiply included
#endif
#define SC_ANIMATABLEOBJECT_HPP

#ifndef AN_FRAMEANIMATION_HPP
#include "Graphics/an/anFrameAnimation.hpp"
#endif
#ifndef SC_OBJECT_HPP
#include "Graphics/sc/scObject.hpp"
#endif


//============================================================================
//============================================================================
class scAnimatableObject : public scObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		scAnimatableObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~scAnimatableObject();

		//--------------------------------------------------------------------
		// CheckAnimation checks compatibility of this animation 
		// with the model. It will return true if the animation can
		// be played on this model.
		//--------------------------------------------------------------------
		virtual bool CheckAnimation( const anFrameAnimation& i_GeoAnimation ) const = 0;

		//--------------------------------------------------------------------
		//	SetAnimation makes the given animation into the current animation
		//	(but does not take ownership of it; the client must preserve it
		//	as long as the scAnimatableObject needs it).  Setting an animation
		//	with this function will not blend it - just clobber the old one.
		//	A pointer to the created animation instance is returned.
		//--------------------------------------------------------------------
		virtual anFrameAnimInstance* SetAnimation(
							const anFrameAnimation& i_GeoAnimation,
							float i_StartTime) = 0;

		//--------------------------------------------------------------------
		//	BlendAnimation causes a new animation to be blended to the current
		//	animation over "i_BlendTime" length of time.  At the end of
		//	i_BlendTime, the object's animation will be exactly the new
		//	animation.
		//	A pointer to the created animation instance is returned.
		//--------------------------------------------------------------------
		virtual anFrameAnimInstance* BlendAnimation(
								const anFrameAnimation& i_GeoAnimation,
								float i_StartTime,
								float i_BlendTime, 
								bool i_bSmoothBlend = false,
								float i_EaseInWeight = 1.0f, 
								float i_EaseOutWeight = 1.0f) = 0;

		//--------------------------------------------------------------------
		//	ClearAnimation causes all animation to stop.
		//--------------------------------------------------------------------
		virtual void ClearAnimation() = 0;

		//--------------------------------------------------------------------
		//	Adds sub animation on top of base animation.
		//	i_bPreserve - sets flag saying if the subanimation should
		//		remain during changes to the full animation.
		//--------------------------------------------------------------------
		virtual anFrameAnimInstance* AddSubAnimation(
							const anFrameAnimation& i_GeoAnimation,
							float i_StartTime,
							bool i_bPreserve) = 0;

		//--------------------------------------------------------------------
		//	Remove sub animation 
		//--------------------------------------------------------------------
		virtual void RemoveSubAnimation( anFrameAnimInstance* i_SubAnimInstance) = 0;

		//--------------------------------------------------------------------
		// SetSubAnimationBlend sets how the given subanimation should 
		// blend with the base anim. Values from 0-1. The given subanimation
		// should have been returned from a call to AddSubAnimation first.
		//--------------------------------------------------------------------
		virtual void SetSubAnimationBlend( const anFrameAnimInstance* i_SubAnim, 
							float i_Blend) = 0;

		//--------------------------------------------------------------------
		//	ClearAllSubAnimations causes all sub animations to stop.
		//--------------------------------------------------------------------
		virtual void ClearAllSubAnimations() = 0;

		//--------------------------------------------------------------------
		//	ClearSubAnimations causes non-preserved sub animations to stop.
		//--------------------------------------------------------------------
		virtual void ClearSubAnimations() = 0;
};
