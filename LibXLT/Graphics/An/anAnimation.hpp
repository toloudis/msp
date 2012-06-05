/*****************************************************************************
**  anAnimation.hpp
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

#ifdef AN_ANIMATION_HPP
#error anAnimation.hpp multiply included
#endif
#define AN_ANIMATION_HPP

//============================================================================
//	Forward References
//============================================================================
class anAnimInstance;


//============================================================================
//	anAnimation
//============================================================================
class anAnimation
{
	public:
		//--------------------------------------------------------------------
		//	anAnimation default constructor
		//--------------------------------------------------------------------
		anAnimation();

		//--------------------------------------------------------------------
		//	destructor - pure virtual
		//--------------------------------------------------------------------
		virtual ~anAnimation() = 0;

		//--------------------------------------------------------------------
		//	GetLength returns the total length of the animation in seconds, or
		//	the period if it is looping.
		//--------------------------------------------------------------------
		float GetLength() const;

		//--------------------------------------------------------------------
		//	GetLooping returns true if the animation should repeat
		//--------------------------------------------------------------------
		bool GetLooping() const;

		//--------------------------------------------------------------------
		//	GetReversing returns true if the animation should reverse and go
		//	backwards when it reaches it's end.
		//--------------------------------------------------------------------
		bool GetReversing() const;

		//--------------------------------------------------------------------
		//	CreateAnimInstance creates an anAnimInstance, of a type
		//	appropriate to the type of the anAnimation.
		//--------------------------------------------------------------------
		virtual anAnimInstance* CreateAnimInstance(float i_TimeOrigin) const = 0;

		//--------------------------------------------------------------------
		//	Clone returns a copy of "this" allocated on the heap.
		//--------------------------------------------------------------------
		virtual anAnimation* Clone() const = 0;

		//--------------------------------------------------------------------
		//	SetLooping should be called by child classes to set the looping
		//	property of the animation.
		//--------------------------------------------------------------------
		void SetLooping(bool i_Looping);

		//--------------------------------------------------------------------
		//	SetReversing should be called by child classes to set the
		//	reversing property of the animation.
		//--------------------------------------------------------------------
		void SetReversing(bool i_Reversing);

		//--------------------------------------------------------------------
		//	Rescale changes the time scale of the animation by the given
		//	factor.  For example, if the factor is 2.0, the animation will be
		//	twice as long and appear to go half as fast.
		//--------------------------------------------------------------------
		virtual void Rescale(float i_Scale) = 0;

	protected:

		//--------------------------------------------------------------------
		//	SetLength should be called by child classes to set the length of the
		//	animation (as soon as they know it).
		//--------------------------------------------------------------------
		void SetLength(float i_Length);

		//--------------------------------------------------------------------
		//	GetCorrectedTime is a convenience for child classes which
		//	computes the animation time parameter based on the time given
		//	and whether the animation is looping or reversing.
		//	This time is guaranteed to be in the range [0, GetLength()]
		//--------------------------------------------------------------------
		float GetCorrectedTime(float i_Time) const;

	private:

		float m_Length;

		struct
		{
			bool m_bLooping		: 1;
			bool m_bReversing	: 1;
		} m_Flags;
};


//============================================================================
//	anAnimInstance
//============================================================================
class anAnimInstance
{
	public:

		//--------------------------------------------------------------------
		//	The anAnimInstance constructor requires the animation beginning
		// time.
		//--------------------------------------------------------------------
		anAnimInstance(float i_TimeOrigin);

		//--------------------------------------------------------------------
		//	pure virtual destructor
		//--------------------------------------------------------------------
		virtual ~anAnimInstance() = 0;

		//--------------------------------------------------------------------
		//	Child classes must implement the clone function to provide a
		//	copy of themselves.
		//--------------------------------------------------------------------
		virtual anAnimInstance* Clone() const = 0;

		//--------------------------------------------------------------------
		//	GetTimeOrigin returns the absolute time when the animation began.
		//--------------------------------------------------------------------
		float GetTimeOrigin() const;

		//--------------------------------------------------------------------
		//	SetTimeOrigin changes the time origin of the animation
		//--------------------------------------------------------------------
		void SetTimeOrigin(float i_Time);

	private:

		float m_TimeOrigin;
};


//============================================================================
//	anAnimation implementation
//============================================================================

//----------------------------------------------------------------------------
//	GetLength returns the total length of the animation in seconds, or
//	the period if it is looping.
//----------------------------------------------------------------------------
inline float anAnimation::GetLength() const
{
	return m_Length;
}

//----------------------------------------------------------------------------
//	GetLooping returns true if the animation should repeat
//----------------------------------------------------------------------------
inline bool anAnimation::GetLooping() const
{
	return m_Flags.m_bLooping;
}

//----------------------------------------------------------------------------
//	GetReversing returns true if the animation should reverse and go
//	backwards when it reaches it's end.
//----------------------------------------------------------------------------
inline bool anAnimation::GetReversing() const
{
	return m_Flags.m_bReversing;
}


//============================================================================
//	anAnimInstance implementation
//============================================================================

//----------------------------------------------------------------------------
//	GetTimeOrigin returns the absolute time when the animation began.
//----------------------------------------------------------------------------
inline float anAnimInstance::GetTimeOrigin() const
{
	return m_TimeOrigin;
}



