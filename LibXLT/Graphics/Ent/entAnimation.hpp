/*****************************************************************************
**  entAnimation.hpp
**
**      entAnimation - represents an animation that has animation
**	events attached to key frames.
**
**	[bga] Animation event functionality has been removed. I am 
**		keeping this class around because it encapsulates things
**		nicely within the ent package to have animation, entity
**		and template all have classes with similar names.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef ENT_ANIMATION_HPP
#error entAnimation.hpp multiply included
#endif
#define ENT_ANIMATION_HPP

#ifndef AN_FRAMEANIMATION_HPP
#include "Graphics/an/anFrameAnimation.hpp"
#endif


//============================================================================
//	entAnimation
//============================================================================
class entAnimation : public anFrameAnimation
{
	public:
		//--------------------------------------------------------------------
		//	entAnimation default constructor
		//--------------------------------------------------------------------
		explicit entAnimation();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~entAnimation();

		//--------------------------------------------------------------------
		//	CreateAnimInstance creates an anAnimInstance, of a type
		//	appropriate to the type of the anAnimation.
		//--------------------------------------------------------------------
		virtual anAnimInstance* CreateAnimInstance(float i_TimeOrigin) const = 0;
};
