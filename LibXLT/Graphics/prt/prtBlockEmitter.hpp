/*****************************************************************************
**	prtBlockEmitter.hpp
**
**		prtBlockEmitter is a prtEmitter which emits particles from a block
**	(rectangle in 3D).
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_BLOCKEMITTER_HPP
#error prtBlockEmitter.hpp multiply included
#endif
#define PRT_BLOCKEMITTER_HPP

#ifndef PRT_EMITTER_HPP
#include "Graphics/prt/prtEmitter.hpp"
#endif


//============================================================================
//============================================================================
class prtBlockEmitter : public prtEmitter
{
	public:
		//--------------------------------------------------------------------
		//	This constructor makes a block with all sides equal (a cube).
		//--------------------------------------------------------------------
		prtBlockEmitter(float i_Side);

		//--------------------------------------------------------------------
		//	This constructor makes a block with different length sides.
		//--------------------------------------------------------------------
		prtBlockEmitter(float i_XSide, float i_YSide, float i_ZSide);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtBlockEmitter();

		//--------------------------------------------------------------------
		//	GetPosition returns a random location in the emitter area
		//--------------------------------------------------------------------
		virtual maPoint3d GetPosition() const;

		//--------------------------------------------------------------------
		//	GetSides returns the lengths of the sides
		//--------------------------------------------------------------------
		void GetSides(float& o_X, float& o_Y, float& o_Z) const;

	private:
		float m_X, m_Y, m_Z;
};
