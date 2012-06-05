/*****************************************************************************
**  anConstantAnimation.hpp
**
**      anConstantAnimation is an animation which isn't an animation at all -
**	it always has a particular constant value.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef AN_CONSTANTANIMATION_HPP
#error anConstantAnimation.hpp multiply included
#endif
#define AN_CONSTANTANIMATION_HPP

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#ifndef AN_TYPEDANIMATION_HPP
#include "Graphics/an/anTypedAnimation.hpp"
#endif


//============================================================================
//	anConstantAnimation
//============================================================================
template <class T>
class anConstantAnimation : public anTypedAnimation<T>
{
	public:
		//--------------------------------------------------------------------
		//	anConstantAnimation constructor.  The anConstantAnimation requires
		//	the values it returns.  Since the value is constant, the time
		//	length is immaterial and is set to a default.
		//--------------------------------------------------------------------
		anConstantAnimation(const T& i_Val);

		//--------------------------------------------------------------------
		//	destructor - virtual
		//--------------------------------------------------------------------
		virtual ~anConstantAnimation();

		//--------------------------------------------------------------------
		//	CreateAnimInstance creates an anConstantAnimInstance which can be
		//	used to animate a parameter.
		//--------------------------------------------------------------------
		virtual anAnimInstance* CreateAnimInstance(float i_TimeOrigin) const;

		//--------------------------------------------------------------------
		//	GetValue returns the value of the animation.
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

		T m_Val;
};


//============================================================================
//	anConstantAnimation implementation
//============================================================================

//----------------------------------------------------------------------------
//	anConstantAnimation constructor.  The animation _must_ contain a value
//	at zero time; this is the constructor parameter.
//----------------------------------------------------------------------------
template <class T>
anConstantAnimation<T>::anConstantAnimation(const T& i_Val)
:	m_Val(i_Val)
{
	this->SetLength(1.0f);
}

//----------------------------------------------------------------------------
//	destructor - virtual
//----------------------------------------------------------------------------
template <class T>
anConstantAnimation<T>::~anConstantAnimation()
{
}

//----------------------------------------------------------------------------
//	CreateAnimInstance creates an anTypedAnimInstance that refers to the
//	anConstantAnimation.
//----------------------------------------------------------------------------
template <class T>
anAnimInstance* anConstantAnimation<T>::CreateAnimInstance(float i_TimeOrigin) const
{
	return new anTypedAnimInstance<T>(i_TimeOrigin, *this);
}

//--------------------------------------------------------------------
//	GetValue returns the value of the animation.
//--------------------------------------------------------------------
template <class T>
T anConstantAnimation<T>::GetValue(float i_Time) const
{
	return m_Val;
}

//--------------------------------------------------------------------
//	Clone returns a copy of "this" allocated on the heap.
//--------------------------------------------------------------------
template <class T>
anAnimation* anConstantAnimation<T>::Clone() const
{
	return new anConstantAnimation<T>(*this);
}

//--------------------------------------------------------------------
//	Rescale changes the time scale of the animation by the given
//	factor.  For example, if the factor is 2.0, the animation will be
//	twice as long and appear to go half as fast.
//--------------------------------------------------------------------
template <class T>
void anConstantAnimation<T>::Rescale(float i_Scale)
{
	this->SetLength(this->GetLength() * i_Scale);
}

