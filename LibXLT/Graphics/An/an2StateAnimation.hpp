/*****************************************************************************
**  an2StateAnimation.hpp
**
**      an2StateAnimation is a simple animation type which oscillates
**	linearly between two values.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef AN_2STATEANIMATION_HPP
#error an2StateAnimation.hpp multiply included
#endif
#define AN_2STATEANIMATION_HPP

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#ifndef AN_TYPEDANIMATION_HPP
#include "Graphics/an/anTypedAnimation.hpp"
#endif

#include <map>
#include <vector>


//============================================================================
//	an2StateAnimation
//============================================================================
template <class T>
class an2StateAnimation : public anTypedAnimation<T>
{
	public:

		//--------------------------------------------------------------------
		//	an2StateAnimation constructor.  The an2StateAnimation requires
		//	the 2 values it oscillates between and the time length of the
		//	animation.  The user can set the looping and so on by using
		//	functions in the base anAnimation.
		//--------------------------------------------------------------------
		an2StateAnimation(const T& i_V1, const T& i_V2, float i_Length);

		//--------------------------------------------------------------------
		//	destructor - virtual
		//--------------------------------------------------------------------
		virtual ~an2StateAnimation();

		//--------------------------------------------------------------------
		//	These functions get the values that the animation oscillates
		//  between.
		//--------------------------------------------------------------------
		T GetFirstValue() const;
		T GetSecondValue() const;

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

		T m_V1, m_V2;
};


//============================================================================
//	an2StateAnimation implementation
//============================================================================

//--------------------------------------------------------------------
//	an2StateAnimation constructor.  The an2StateAnimation requires
//	the 2 values it oscillates between and the time length of the
//	animation.  The user can set the looping state and so on by using
//	functions in the base anAnimation.
//--------------------------------------------------------------------
template <class T>
an2StateAnimation<T>::an2StateAnimation(const T& i_V1, const T& i_V2, float i_Length)
:	m_V1(i_V1),
	m_V2(i_V2)
{
	this->SetLength(i_Length);
}

//----------------------------------------------------------------------------
//	destructor - virtual
//----------------------------------------------------------------------------
template <class T>
an2StateAnimation<T>::~an2StateAnimation()
{
}

//--------------------------------------------------------------------
//	These functions get the values that the animation oscillates
//  between.
//--------------------------------------------------------------------
template <class T>
T an2StateAnimation<T>::GetFirstValue() const
{
	return m_V1;
}

template <class T>
T an2StateAnimation<T>::GetSecondValue() const
{
	return m_V2;
}

//----------------------------------------------------------------------------
//	GetValue returns the value of the animation parameter at the
//	given time.
//----------------------------------------------------------------------------
template <class T>
T an2StateAnimation<T>::GetValue(float i_Time) const
{
	// convert to our time coordinate
	float anim_time = this->GetCorrectedTime(i_Time);

	// do a linear interpolation
	return T( m_V1 + (anim_time / this->GetLength()) * ( m_V2 - m_V1) );
}

//--------------------------------------------------------------------
//	Clone returns a copy of "this" allocated on the heap.
//--------------------------------------------------------------------
template <class T>
anAnimation* an2StateAnimation<T>::Clone() const
{
	return new an2StateAnimation<T>(*this);
}

//--------------------------------------------------------------------
//	Rescale changes the time scale of the animation by the given
//	factor.  For example, if the factor is 2.0, the animation will be
//	twice as long and appear to go half as fast.
//--------------------------------------------------------------------
template <class T>
void an2StateAnimation<T>::Rescale(float i_Scale)
{
	this->SetLength(this->GetLength() * i_Scale);
}
