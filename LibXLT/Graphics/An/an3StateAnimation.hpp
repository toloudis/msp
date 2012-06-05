/*****************************************************************************
**  an3StateAnimation.hpp
**
**      an3StateAnimation is a simple animation type which oscillates
**	linearly between two values.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef AN_3STATEANIMATION_HPP
#error an3StateAnimation.hpp multiply included
#endif
#define AN_3STATEANIMATION_HPP

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#ifndef AN_TYPEDANIMATION_HPP
#include "Graphics/an/anTypedAnimation.hpp"
#endif

#include <map>
#include <vector>


//============================================================================
//	an3StateAnimation
//============================================================================
template <class T>
class an3StateAnimation : public anTypedAnimation<T>
{
	public:

		//--------------------------------------------------------------------
		//	an3StateAnimation constructor.  The an3StateAnimation requires
		//	3 values and 2 times.  (The first time is considered to be zero).
		//--------------------------------------------------------------------
		an3StateAnimation(	const T& i_V1,
							const T& i_V2,
							const T& i_V3,
							float i_MidTime,
							float i_EndTime);
		an3StateAnimation(	const T& i_V1,
							const T& i_V2,
							const T& i_V3,
							float i_MidTimeStart,
							float i_MidTimeEnd,
							float i_EndTime);

		//--------------------------------------------------------------------
		//	destructor - virtual
		//--------------------------------------------------------------------
		virtual ~an3StateAnimation();

		//--------------------------------------------------------------------
		//	These functions get the values that the animation oscillates
		//  between.
		//--------------------------------------------------------------------
		T GetFirstValue() const;
		T GetSecondValue() const;
		T GetThirdValue() const;

		//--------------------------------------------------------------------
		//	Returns the time which the second value occurs at.
		//--------------------------------------------------------------------
		float GetMidTimeStart() const;
		float GetMidTimeEnd() const;

		//--------------------------------------------------------------------
		//	GetValue returns the value of the animation parameter at the
		//	given time (referenced to the beginning of the animation at
		//	0 seconds).
		//--------------------------------------------------------------------
		virtual T GetValue(float i_Time) const;

		//--------------------------------------------------------------------
		//	Clone returns a copy of "this" allocated on the heap.
		//--------------------------------------------------------------------
		virtual anAnimation* Clone() const;

		//--------------------------------------------------------------------
		//	Rescale changes the time scale of the animation by the given
		//	factor.  For example, if the factor is 2.0, the animation will be
		//	twice as long and appear to go half as fast.
		//--------------------------------------------------------------------
		virtual void Rescale(float i_Scale);

	private:

		T m_V1, m_V2, m_V3;
		float m_MidTimeStart;
		float m_MidTimeEnd;
};


//============================================================================
//	an3StateAnimation implementation
//============================================================================

//--------------------------------------------------------------------
//	an3StateAnimation constructor.  The an3StateAnimation requires
//	the 2 values it oscillates between and the time length of the
//	animation.  The user can set the looping state and so on by using
//	functions in the base anAnimation.
//--------------------------------------------------------------------
template <class T>
an3StateAnimation<T>::an3StateAnimation(const T& i_V1,
										const T& i_V2,
										const T& i_V3,
										float i_MidTimeStart,
										float i_EndTime)
:	m_V1(i_V1),
	m_V2(i_V2),
	m_V3(i_V3),
	m_MidTimeStart(i_MidTimeStart),
	m_MidTimeEnd(i_MidTimeStart)
{
	this->SetLength(i_EndTime);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
template <class T>
an3StateAnimation<T>::an3StateAnimation(const T& i_V1,
										const T& i_V2,
										const T& i_V3,
										float i_MidTimeStart,
										float i_MidTimeEnd,
										float i_EndTime)
:	m_V1(i_V1),
	m_V2(i_V2),
	m_V3(i_V3),
	m_MidTimeStart(i_MidTimeStart),
	m_MidTimeEnd(i_MidTimeEnd)
{
	this->SetLength(i_EndTime);
}

//----------------------------------------------------------------------------
//	destructor - virtual
//----------------------------------------------------------------------------
template <class T>
an3StateAnimation<T>::~an3StateAnimation()
{
}

//--------------------------------------------------------------------
//	These functions get the values that the animation oscillates
//  between.
//--------------------------------------------------------------------
template <class T>
T an3StateAnimation<T>::GetFirstValue() const
{
	return m_V1;
}

template <class T>
T an3StateAnimation<T>::GetSecondValue() const
{
	return m_V2;
}

template <class T>
T an3StateAnimation<T>::GetThirdValue() const
{
	return m_V3;
}

//--------------------------------------------------------------------
//	Returns the time which the second value occurs at.
//--------------------------------------------------------------------
template <class T>
float an3StateAnimation<T>::GetMidTimeStart() const
{
	return m_MidTimeStart;
}

//--------------------------------------------------------------------
//	Returns the time which the second value occurs at.
//--------------------------------------------------------------------
template <class T>
float an3StateAnimation<T>::GetMidTimeEnd() const
{
	return m_MidTimeEnd;
}

//----------------------------------------------------------------------------
//	GetValue returns the value of the animation parameter at the
//	given time.
//----------------------------------------------------------------------------
template <class T>
T an3StateAnimation<T>::GetValue(float i_Time) const
{
	// convert to our time coordinate
	float anim_time = this->GetCorrectedTime(i_Time);

	// figure out if we're before,after or within the mid time
	if ( anim_time < m_MidTimeStart )
		return T( m_V1 + (anim_time / m_MidTimeStart) * ( m_V2 - m_V1) );
	else if (anim_time > m_MidTimeEnd)
		return T( m_V2 + ( (anim_time-m_MidTimeEnd) / (this->GetLength()-m_MidTimeEnd) ) * ( m_V3 - m_V2) );
	else
		return T( m_V2 );
}

//--------------------------------------------------------------------
//	Clone returns a copy of "this" allocated on the heap.
//--------------------------------------------------------------------
template <class T>
anAnimation* an3StateAnimation<T>::Clone() const
{
	return new an3StateAnimation<T>(*this);
}

//--------------------------------------------------------------------
//	Rescale changes the time scale of the animation by the given
//	factor.  For example, if the factor is 2.0, the animation will be
//	twice as long and appear to go half as fast.
//--------------------------------------------------------------------
template <class T>
void an3StateAnimation<T>::Rescale(float i_Scale)
{
	this->SetLength(this->GetLength() * i_Scale);
	m_MidTimeStart *= i_Scale;
	m_MidTimeEnd *= i_Scale;
}
