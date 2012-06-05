/*****************************************************************************
**	prtConeParticleGenerator.hpp
**
**		prtConeParticleGenerator is a prtParticleGenerator whose particles
**	travel outwards in a cone shape.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_CONEPARTICLEGENERATOR_HPP
#error prtConeParticleGenerator.hpp multiply included
#endif
#define PRT_CONEPARTICLEGENERATOR_HPP

#ifndef PRT_SPRITEGROUPPARTICLEGENERATOR_HPP
#include "Graphics/prt/prtSpriteGroupParticleGenerator.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
struct prtConeParticleGeneratorImp;


//============================================================================
//============================================================================
class prtConeParticleGenerator : public prtSpriteGroupParticleGenerator
{
	public:
		//--------------------------------------------------------------------
		//	The i_CreationTime is the simulation time at which the particle
		//	generator was created.
		//--------------------------------------------------------------------
		prtConeParticleGenerator(float i_CreationTime);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtConeParticleGenerator();

		//--------------------------------------------------------------------
		//	Animate is called by the scScene for each object before it
		//	is rendered.  Objects should use this function to set their
		//	model's visibility (according to scObject::GetRenderable) and
		//	animate if necessary.  The prtConeParticleGenerator::PreRender should
		//	be called by child classes before they do their PreRender work.
		//--------------------------------------------------------------------
		virtual void Animate(float i_SimulationTime);

		//--------------------------------------------------------------------
		//	These parameters can be used with the 
		//	prtParticleGenerator::SetParameter function.
		//--------------------------------------------------------------------
		enum Parameter
		{
			e_ConeAngle	= prtSpriteGroupParticleGenerator::e_NextParameter,	// defaults to Pi/2
			e_MinSpeed,				// defaults to 5
			e_MaxSpeed,				// defaults to 5
			e_AccelerationX,		// defaults to 0
			e_AccelerationY,		// defaults to 0
			e_AccelerationZ,		// defaults to 0
			e_NextParameter
		};

		//--------------------------------------------------------------------
		//	GetNumParameters returns the number of parameters used by the
		//	particle generator.  This function should be overridden by 
		//	child classes which add parameters.
		//--------------------------------------------------------------------
		virtual int GetNumParameters() const;

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

		prtConeParticleGeneratorImp* m_pImp;
};
