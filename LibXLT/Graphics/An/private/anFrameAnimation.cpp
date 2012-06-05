/*****************************************************************************
**  anFrameAnimation.cpp
**
**      anFrameAnimation
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/an/anFrameAnimation.hpp"
#include "Graphics/g3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
anFrameAnimation::anFrameAnimation()
:	m_TransitionTime(0.5f),
	m_FrameRate(g3dConstants::c_fDefaultFrameRate),
	m_StartFrame(0),
	m_EndFrame(-1),
	m_NumFrames(0)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
anFrameAnimation::~anFrameAnimation()
{

}

//--------------------------------------------------------------------
//	SetTransitionTime - transition time(blend time) from source
//						animation to destination(this) animation.
//--------------------------------------------------------------------
void	anFrameAnimation::SetTransitionTime(float i_TransitionTime)
{
	m_TransitionTime = i_TransitionTime;
}

//--------------------------------------------------------------------
//	FrameRate - frames per second rate at which animation
//		moves through key data
//--------------------------------------------------------------------
void	anFrameAnimation::SetFrameRate(float i_FrameRate)
{
	m_FrameRate = i_FrameRate;
	reset_length();
}

//--------------------------------------------------------------------
//	StartFrame - frames at which to start sub animation
//--------------------------------------------------------------------
void	anFrameAnimation::SetStartFrame(float i_StartFrame)
{
	m_StartFrame = i_StartFrame;
	reset_length();
}

//--------------------------------------------------------------------
//	EndFrame - frames at which to end sub animation
//--------------------------------------------------------------------
void	anFrameAnimation::SetEndFrame(float i_EndFrame)
{
	m_EndFrame = i_EndFrame;
	reset_length();
}

//--------------------------------------------------------------------
//	GetNumFrames returns the total number of frames in
//	in the key set used by the animation.  This isn't
//	the number of frames played by this animation if
//	this animation uses a subset of those keys.
//--------------------------------------------------------------------
float anFrameAnimation::GetNumFrames() const
{
	return m_NumFrames;
}

//--------------------------------------------------------------------
//	CreateAnimInstance creates an anAnimInstance, of a type
//	appropriate to the type of the anAnimation.
//--------------------------------------------------------------------

anAnimInstance* anFrameAnimation::CreateAnimInstance(float i_TimeOrigin) const
{
	return new anFrameAnimInstance(i_TimeOrigin, *this);
}

//--------------------------------------------------------------------
//	Rescale changes the time scale of the animation by the given
//	factor.  For example, if the factor is 2.0, the animation will be
//	twice as long and appear to go half as fast.
//--------------------------------------------------------------------
void anFrameAnimation::Rescale(float i_Scale)
{
	// Scaling affects frame rate, not keys
	SetFrameRate(m_FrameRate / i_Scale);
}

//--------------------------------------------------------------------
//	SetNumFrames should be called by derived classes
//	to set the total number of frames in the key set.
//	This is used to compute when to loop when the
//	end frame is set to -1.
//--------------------------------------------------------------------
void anFrameAnimation::SetNumFrames(float i_NumFrames)
{
	m_NumFrames = i_NumFrames;
	reset_length();
}

//--------------------------------------------------------------------
//	reset animation length in seconds based on frame rate
//	and number of frames in this animation
//--------------------------------------------------------------------
void anFrameAnimation::reset_length()
{
	float start_frame = (m_StartFrame < 0) ? 0 : m_StartFrame;
	float end_frame = (m_EndFrame < 0) ? m_NumFrames : m_EndFrame;
	float anim_len = end_frame - start_frame;

	if (m_FrameRate == 0)
		this->SetLength(0);
	else
		this->SetLength(anim_len / m_FrameRate);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

anFrameAnimInstance::anFrameAnimInstance(float i_TimeOrigin,
											const anFrameAnimation& i_Anim)
:	anAnimInstance(i_TimeOrigin),
	m_Anim(i_Anim),
	m_FrameRate( i_Anim.GetFrameRate() )
{
}

//--------------------------------------------------------------------
//	pure virtual destructor
//--------------------------------------------------------------------

anFrameAnimInstance::~anFrameAnimInstance()
{
}


//--------------------------------------------------------------------
//	ComputeFrame converts from current time to frame index
//		within animation data
//--------------------------------------------------------------------
float anFrameAnimInstance::ComputeFrame(float i_SimulationTime) const
{
	float elapsed = i_SimulationTime - this->GetTimeOrigin();
	float start_frame = m_Anim.GetStartFrame();
	if (start_frame < 0) start_frame = 0.0f;
	if (elapsed < 0)
		return start_frame;

	float cur_frame = elapsed * m_FrameRate + start_frame;

	float anim_len = m_Anim.GetNumFrames();
	//float end_frame = (m_Anim.GetEndFrame() < 0) ? m_Anim.GetLength() : m_Anim.GetEndFrame();
	float end_frame = (m_Anim.GetEndFrame() < 0) ? anim_len : m_Anim.GetEndFrame();
	if (cur_frame > end_frame)
	{
		if (m_Anim.GetLooping())
		{
			// Need to handle reversing in here?

			float anim_len = end_frame - start_frame;
			if (anim_len == 0)
				return end_frame;

			//float loop_frame = fmodf ( cur_frame - m_StartFrame,
			//							anim_len ) + m_StartFrame;
			int num_cycles = int ((cur_frame - start_frame) / anim_len);
			float loop_frame = (cur_frame - (num_cycles * anim_len));

			return loop_frame;
		}
		else
		{
			// stopped at last frame
			return end_frame;
		}
	}

	return cur_frame;
}


//--------------------------------------------------------------------
//	Child classes must implement the clone function to provide a
//	copy of themselves.
//--------------------------------------------------------------------
anAnimInstance* anFrameAnimInstance::Clone() const
{
	return m_Anim.CreateAnimInstance(this->GetTimeOrigin());
}

//--------------------------------------------------------------------
//	GetLength returns the length of the animation that the
//	anFrameAnimInstance refers to.
//--------------------------------------------------------------------
float anFrameAnimInstance::GetLength() const
{
	return m_Anim.GetLength();
}

//--------------------------------------------------------------------
//	GetNumFrames returns the number of frames in the animation that the
//	anFrameAnimInstance refers to.
//--------------------------------------------------------------------
float anFrameAnimInstance::GetNumFrames() const
{
	return m_Anim.GetNumFrames();
}

//--------------------------------------------------------------------
//	FrameRate - frames per second rate at which animation
//	moves through key data.  Anim instance has its own copy
//	that can be modified.
//--------------------------------------------------------------------
void anFrameAnimInstance::SetFrameRate(float i_FrameRate)
{
	m_FrameRate = i_FrameRate;
}