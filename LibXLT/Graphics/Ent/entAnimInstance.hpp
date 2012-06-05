/*****************************************************************************
**  entAnimInstance.hpp
**
**      entAnimInstance - represents an instance of a running animation,
**	it keeps track of its current frame in order to know which
**	events to trigger.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef ENT_ANIMINSTANCE_HPP
#error entAnimInstance.hpp multiply included
#endif
#define ENT_ANIMINSTANCE_HPP

#ifndef AN_FRAMEANIMATION_HPP
#include "Graphics/an/anFrameAnimation.hpp"
#endif

#include <vector>


//============================================================================
//	forward declarations
//============================================================================
class entAnimEvent;
class entAnimation;


//============================================================================
//	entAnimInstance
//============================================================================
class entAnimInstance : public anFrameAnimInstance
{
	public:
		//--------------------------------------------------------------------
		//	The entAnimInstance constructor requires the animation beginning
		// time and the animation used by this instance.
		//--------------------------------------------------------------------
		entAnimInstance(float i_TimeOrigin,
						const entAnimation& i_Anim);

		//--------------------------------------------------------------------
		//	pure virtual destructor
		//--------------------------------------------------------------------
		virtual ~entAnimInstance();

		//--------------------------------------------------------------------
		//	The Clone function creates a new copy of the entAnimInstance
		//	on the heap.
		//--------------------------------------------------------------------
		virtual anAnimInstance* Clone() const;

	private:
		const	entAnimation& m_Anim;
};
