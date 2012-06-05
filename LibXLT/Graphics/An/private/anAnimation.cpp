/*****************************************************************************
**  anAnimation.cpp
**
**      anAnimation contains the base anAnimation class and the base
**	anAnimInstance.  An anAnimation represents an animation and the
**	anAnimInstance represents a specific occurrence of that animation.  For
**	instance, an animation might encode a color slowly changing from red to
**	yellow, and an anim instance might represent a specific color on a GUI
**	button changing to yellow.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/an/anAnimation.hpp"


//============================================================================
//	anAnimation
//============================================================================

//--------------------------------------------------------------------
//	anAnimation default constructor
//--------------------------------------------------------------------
anAnimation::anAnimation()
:	m_Length(0)
{
	m_Flags.m_bLooping = false;
	m_Flags.m_bReversing = false;
}

//--------------------------------------------------------------------
//	destructor - pure virtual
//--------------------------------------------------------------------
anAnimation::~anAnimation()
{
}

//--------------------------------------------------------------------
//	SetLength should be called by child classes to set the length of the
//	animation (as soon as they know it).
//--------------------------------------------------------------------
void anAnimation::SetLength(float i_Length)
{
	m_Length = i_Length;
}

//--------------------------------------------------------------------
//	SetLooping should be called by child classes to set whether the
//	animation loops.
//--------------------------------------------------------------------
void anAnimation::SetLooping(bool i_Looping)
{
	m_Flags.m_bLooping = i_Looping;
}

//--------------------------------------------------------------------
//	SetReversing should be called by child classes to set whether the
//	animation reverses when it is finished.
//--------------------------------------------------------------------
void anAnimation::SetReversing(bool i_Reversing)
{
	m_Flags.m_bReversing = i_Reversing;
}

//--------------------------------------------------------------------
//	GetCorrectedTime is a convenience for child classes which
//	computes the animation time parameter based on the time given
//	and whether the animation is looping or reversing.
//--------------------------------------------------------------------
float anAnimation::GetCorrectedTime(float i_Time) const
{
	// convert to our time coordinate
	float anim_time = i_Time;

	if( m_Flags.m_bLooping )
	{
		int num_iterations = int(anim_time / m_Length);
		anim_time = anim_time - float(num_iterations) * m_Length;

		if( m_Flags.m_bReversing && (num_iterations & 0x01) )
		{
			//	odd number of iterations, so we are going backwards
			//	reflect the time backwards
			anim_time = m_Length - anim_time;
		}
	}
	else
	{
		if( anim_time > m_Length )
		{
			if( m_Flags.m_bReversing )
			{
				float length2 = m_Length * 2.0f;

				if( anim_time > length2 )
					anim_time = 0.0f;
				else
					anim_time = 2.0f * m_Length - anim_time;
			}
			else
				anim_time = m_Length;
		}
	}

	//	paranoid numerical correction
	if( anim_time > m_Length )
		anim_time = m_Length;
	else if( anim_time < 0.0f )
		anim_time = 0.0f;

	return anim_time;
}


//============================================================================
//	anAnimInstance
//============================================================================

//--------------------------------------------------------------------
//	The anAnimInstance constructor requires the animation beginning
// time.
//--------------------------------------------------------------------
anAnimInstance::anAnimInstance(float i_TimeOrigin)
:	m_TimeOrigin(i_TimeOrigin)
{
}

//--------------------------------------------------------------------
//	pure virtual destructor
//--------------------------------------------------------------------
anAnimInstance::~anAnimInstance()
{
}

//--------------------------------------------------------------------
//	SetTimeOrigin changes the time origin of the animation
//--------------------------------------------------------------------
void anAnimInstance::SetTimeOrigin(float i_Time)
{
	m_TimeOrigin = i_Time;
}
