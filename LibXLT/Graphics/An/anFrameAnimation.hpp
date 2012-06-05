/*****************************************************************************
**  anFrameAnimation.hpp
**
**      anFrameAnimation
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef AN_FRAMEANIMATION_HPP
#error anFrameAnimation.hpp multiply included
#endif
#define AN_FRAMEANIMATION_HPP

#ifndef AN_ANIMATION_HPP
#include "Graphics/an/anAnimation.hpp"
#endif


//============================================================================
//	anFrameAnimation
//============================================================================
class anFrameAnimation : public anAnimation
{
	public:
		//--------------------------------------------------------------------
		//	anFrameAnimation default constructor
		//--------------------------------------------------------------------
		anFrameAnimation();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~anFrameAnimation() = 0;

		//--------------------------------------------------------------------
		//	CreateAnimInstance creates an anAnimInstance, of a type
		//	appropriate to the type of the anAnimation.
		//--------------------------------------------------------------------
		virtual anAnimInstance* CreateAnimInstance(float i_TimeOrigin) const;

		//--------------------------------------------------------------------
		//	SetTransitionTime - transition time(blend time) from source
		//						animation to destination(this) animation.
		//--------------------------------------------------------------------
		inline float	GetTransitionTime() const;
		void			SetTransitionTime(float i_TransitionTime);

		//--------------------------------------------------------------------
		//	FrameRate - frames per second rate at which animation
		//		moves through key data
		//--------------------------------------------------------------------
		inline float	GetFrameRate() const;
		void			SetFrameRate(float i_FrameRate);

		//--------------------------------------------------------------------
		//	StartFrame - frames at which to start sub animation
		//--------------------------------------------------------------------
		inline float	GetStartFrame() const;
		void			SetStartFrame(float i_StartFrame);

		//--------------------------------------------------------------------
		//	EndFrame - frames at which to end sub animation
		//--------------------------------------------------------------------
		inline float	GetEndFrame() const;
		void			SetEndFrame(float i_EndFrame);

		//--------------------------------------------------------------------
		//	GetNumFrames returns the total number of frames in
		//	in the key set used by the animation.  This isn't
		//	the number of frames played by this animation if
		//	this animation uses a subset of those keys.
		//--------------------------------------------------------------------
		float			GetNumFrames() const;

		//--------------------------------------------------------------------
		//	Rescale changes the time scale of the animation by the given
		//	factor.  For example, if the factor is 2.0, the animation will be
		//	twice as long and appear to go half as fast.
		//--------------------------------------------------------------------
		virtual void Rescale(float i_Scale);

	protected:
		//--------------------------------------------------------------------
		//	SetNumFrames should be called by derived classes
		//	to set the total number of frames in the key set.
		//	This is used to compute when to loop when the
		//	end frame is set to -1.
		//--------------------------------------------------------------------
		void			SetNumFrames(float i_NumFrames);

	private:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void	reset_length();

		float	m_TransitionTime;
		float	m_FrameRate;
		float	m_StartFrame;
		float	m_EndFrame;
		float	m_NumFrames;
};


//--------------------------------------------------------------------
//	SetTransitionTime - transition time(blend time) from source
//						animation to destination(this) animation.
//--------------------------------------------------------------------
inline float anFrameAnimation::GetTransitionTime() const
{
	return m_TransitionTime;
}

//--------------------------------------------------------------------
//	FrameRate - frames per second rate at which animation
//		moves through key data
//--------------------------------------------------------------------
inline float anFrameAnimation::GetFrameRate() const
{
	return m_FrameRate;
}

//--------------------------------------------------------------------
//	StartFrame - frames at which to start sub animation
//--------------------------------------------------------------------
inline float anFrameAnimation::GetStartFrame() const
{
	return m_StartFrame;
}

//--------------------------------------------------------------------
//	EndFrame - frames at which to end sub animation
//--------------------------------------------------------------------
inline float anFrameAnimation::GetEndFrame() const
{
	return m_EndFrame;
}


//============================================================================
//	anFrameAnimInstance
//============================================================================
class anFrameAnimInstance : public anAnimInstance
{
	public:
		//--------------------------------------------------------------------
		//	The anFrameAnimInstance constructor requires the animation beginning
		// time and the animation used by this instance.
		//--------------------------------------------------------------------
		anFrameAnimInstance(float i_TimeOrigin,
							const anFrameAnimation& i_Anim);

		//--------------------------------------------------------------------
		//	pure virtual destructor
		//--------------------------------------------------------------------
		virtual ~anFrameAnimInstance();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline const anFrameAnimation& GetAnim() const;

		//--------------------------------------------------------------------
		//	The Clone function creates a new copy of the anFrameAnimInstance
		//	on the heap.
		//--------------------------------------------------------------------
		virtual anAnimInstance* Clone() const;

		//--------------------------------------------------------------------
		//	GetLength returns the length of the animation in seconds that the
		//	anFrameAnimInstance refers to.
		//--------------------------------------------------------------------
		float GetLength() const;

		//--------------------------------------------------------------------
		//	GetNumFrames returns the number of frames in the animation that the
		//	anFrameAnimInstance refers to.
		//--------------------------------------------------------------------
		float GetNumFrames() const;

		//--------------------------------------------------------------------
		//	FrameRate - frames per second rate at which animation
		//	moves through key data.  Anim instance has its own copy
		//	that can be modified.
		//--------------------------------------------------------------------
		inline float	GetFrameRate() const;
		void			SetFrameRate(float i_FrameRate);

		//--------------------------------------------------------------------
		//	ComputeFrame converts from current time to frame index
		//		within animation data
		//--------------------------------------------------------------------
		float ComputeFrame(float i_Time) const;

	private:

		const	anFrameAnimation& m_Anim;
		float	m_FrameRate;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
const anFrameAnimation& anFrameAnimInstance::GetAnim() const
{
	return m_Anim;
}

//--------------------------------------------------------------------
//	FrameRate - frames per second rate at which animation
//	moves through key data.  Anim instance has its own copy
//	that can be modified.
//--------------------------------------------------------------------
inline float anFrameAnimInstance::GetFrameRate() const
{
	return m_FrameRate;
}
