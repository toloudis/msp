/*****************************************************************************
**	prtStaticParticleGenerator.hpp
**
**		prtStaticParticleGenerator is a scParticleGenerator whose particles
**	don't move in space (but may still rotate, scale, etc.)
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_STATICPARTICLEGENERATOR_HPP
#error prtStaticParticleGenerator.hpp multiply included
#endif
#define PRT_STATICPARTICLEGENERATOR_HPP

#ifndef PRT_SPRITEGROUPPARTICLEGENERATOR_HPP
#include "Graphics/prt/prtSpriteGroupParticleGenerator.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
struct prtStaticParticleGeneratorImp;


//============================================================================
//============================================================================
class prtStaticParticleGenerator : public prtSpriteGroupParticleGenerator
{
	public:
		//--------------------------------------------------------------------
		//	The i_CreationTime is the simulation time at which the particle
		//	generator was created.
		//--------------------------------------------------------------------
		prtStaticParticleGenerator(float i_CreationTime);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtStaticParticleGenerator();

		//--------------------------------------------------------------------
		//	Animate is called by the scScene for each object before it
		//	is rendered.  Objects should use this function to set their
		//	model's visibility (according to scObject::GetRenderable) and
		//	animate if necessary.  The prtStaticParticleGenerator::PreRender should
		//	be called by child classes before they do their PreRender work.
		//--------------------------------------------------------------------
		virtual void Animate(float i_SimulationTime);

		//--------------------------------------------------------------------
		//	DeleteAllParticles just take a guess what this one does. go ahead, guess
		//--------------------------------------------------------------------
		virtual void DeleteAllParticles();

		//--------------------------------------------------------------------
		//	ClearParticleAccumulation clears the variable that tracks the particle
		//	accumulation, essentially giving particle generation a clean slate
		//	to work with. Good for clearing out undesired accumulation that may
		//	occur during long pausessuch as mode changes
		//--------------------------------------------------------------------
		virtual void ClearParticleAccumulation();

	private:

		prtStaticParticleGeneratorImp* m_pImp;
};

