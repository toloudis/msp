/*****************************************************************************
**	prtCircleEmitter.hpp
**
**		prtCircleEmitter is a prtEmitter which emits particles from a 2D
**	circle.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_CIRCLEEMITTER_HPP
#error prtCircleEmitter.hpp multiply included
#endif
#define PRT_CIRCLEEMITTER_HPP

#ifndef PRT_EMITTER_HPP
#include "Graphics/prt/prtEmitter.hpp"
#endif


//============================================================================
//============================================================================
class prtCircleEmitter : public prtEmitter
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtCircleEmitter(float i_Radius);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtCircleEmitter();

		//--------------------------------------------------------------------
		//	GetPosition returns a random location in the emitter area
		//--------------------------------------------------------------------
		virtual maPoint3d GetPosition() const;

		//--------------------------------------------------------------------
		//	GetRadius returns the radius of the circle.
		//--------------------------------------------------------------------
		float GetRadius() const { return m_Radius; }

	private:
		float m_Radius;
};
