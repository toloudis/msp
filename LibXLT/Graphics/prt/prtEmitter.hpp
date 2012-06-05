/*****************************************************************************
**	prtEmitter.hpp
**
**		prtEmitter is a base class for objects that describe the emission
**	area for particle generators.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_EMITTER_HPP
#error prtEmitter.hpp multiply included
#endif
#define PRT_EMITTER_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class prtEmitter
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtEmitter();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtEmitter() = 0;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Think(float i_SimulationTime);

		//--------------------------------------------------------------------
		//	GetPosition returns a random location in the emitter area
		//--------------------------------------------------------------------
		virtual maPoint3d GetPosition() const = 0;
};
