/*****************************************************************************
**	prtPointEmitter.hpp
**
**		prtPointEmitter is a prtEmitter which emits all particles from the
**	single point at the origin.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_POINTEMITTER_HPP
#error prtPointEmitter.hpp multiply included
#endif
#define PRT_POINTEMITTER_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef PRT_EMITTER_HPP
#include "Graphics/prt/prtEmitter.hpp"
#endif


//============================================================================
//============================================================================
class prtPointEmitter : public prtEmitter
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtPointEmitter();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtPointEmitter();

		//--------------------------------------------------------------------
		//	GetPosition returns a random location in the emitter area
		//--------------------------------------------------------------------
		virtual maPoint3d GetPosition() const;
};
