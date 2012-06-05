/*****************************************************************************
**  anTypedAnimation.hpp
**
**      anTypedAnimation specializes animations to provide an interface for
**	an animation that works on a specific type of parameter - for instance,
**	a float, a color, or a matrix.
**		The anTypedAnimation contains little of its own functionality but
**	it is convenient to define it to facilitate placing animations in
**	containers, etc.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef AN_TYPEDANIMATION_HPP
#error anTypedAnimation.hpp multiply included
#endif
#define AN_TYPEDANIMATION_HPP

#ifndef AN_ANIMATION_HPP
#include "Graphics/an/anAnimation.hpp"
#endif


//============================================================================
//	anTypedAnimation
//============================================================================
template <class T>
class anTypedAnimation : public anAnimation
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~anTypedAnimation() = 0;

		//--------------------------------------------------------------------
		//	GetValue returns the value of the animation parameter at the
		//	given time (referenced to the beginning of the animation at
		//	0 seconds).
		//--------------------------------------------------------------------
		virtual T GetValue(float i_Time) const = 0;

		//--------------------------------------------------------------------
		//	CreateAnimInstance creates an anAnimInstance, of a type
		//	appropriate to the type of the anAnimation.
		//--------------------------------------------------------------------
		virtual anAnimInstance* CreateAnimInstance(float i_TimeOrigin) const;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
anTypedAnimation<T>::~anTypedAnimation()
{
}


//============================================================================
//	anTypedAnimInstance
//============================================================================
template <class T>
class anTypedAnimInstance : public anAnimInstance
{
	public:
		//--------------------------------------------------------------------
		//	The anTypedAnimInstance constructor requires the animation beginning
		// time and the animation used by this instance.
		//--------------------------------------------------------------------
		anTypedAnimInstance(float i_TimeOrigin,
							const anTypedAnimation<T>& i_Anim);

		//--------------------------------------------------------------------
		//	pure virtual destructor
		//--------------------------------------------------------------------
		virtual ~anTypedAnimInstance();

		//--------------------------------------------------------------------
		//	GetValue returns the value of the animation parameter at the
		//	given time.
		//--------------------------------------------------------------------
		T GetValue(float i_Time) const;

		//--------------------------------------------------------------------
		//	The Clone function creates a new copy of the anTypedAnimInstance
		//	on the heap.
		//--------------------------------------------------------------------
		virtual anAnimInstance* Clone() const;

		//--------------------------------------------------------------------
		//	GetLength returns the length of the animation that the
		//	anTypedAnimInstance refers to.
		//--------------------------------------------------------------------
		float GetLength() const;

	private:

		const anTypedAnimation<T>& m_Anim;
};

//--------------------------------------------------------------------
//	CreateAnimInstance creates an anAnimInstance, of a type
//	appropriate to the type of the anAnimation.
//--------------------------------------------------------------------
template <class T>
anAnimInstance* anTypedAnimation<T>::CreateAnimInstance(float i_TimeOrigin) const
{
	return new anTypedAnimInstance<T>(i_TimeOrigin, *this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
anTypedAnimInstance<T>::anTypedAnimInstance(float i_TimeOrigin,
											const anTypedAnimation<T>& i_Anim)
:	anAnimInstance(i_TimeOrigin),
	m_Anim(i_Anim)
{
}

//--------------------------------------------------------------------
//	pure virtual destructor
//--------------------------------------------------------------------
template <class T>
anTypedAnimInstance<T>::~anTypedAnimInstance()
{
}

//--------------------------------------------------------------------
//	GetValue returns the value of the animation parameter at the
//	given time.
//--------------------------------------------------------------------
template <class T>
inline T anTypedAnimInstance<T>::GetValue(float i_Time) const
{
	return m_Anim.GetValue(i_Time - this->GetTimeOrigin());
}

//--------------------------------------------------------------------
//	Child classes must implement the clone function to provide a
//	copy of themselves.
//--------------------------------------------------------------------
template <class T>
anAnimInstance* anTypedAnimInstance<T>::Clone() const
{
	return m_Anim.CreateAnimInstance(this->GetTimeOrigin());
}

//--------------------------------------------------------------------
//	GetLength returns the length of the animation that the
//	anTypedAnimInstance refers to.
//--------------------------------------------------------------------
template <class T>
inline float anTypedAnimInstance<T>::GetLength() const
{
	return m_Anim.GetLength();
}

