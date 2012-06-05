/*****************************************************************************
**	prtSpiralParticleGenerator.hpp
**
**		prtSpiralParticleGenerator is a prtParticleGenerator whose particles
**	move in a spiral or circle.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_SPIRALPARTICLEGENERATOR_HPP
#error prtSpiralParticleGenerator.hpp multiply included
#endif
#define PRT_SPIRALPARTICLEGENERATOR_HPP

#ifndef PRT_SPRITEGROUPPARTICLEGENERATOR_HPP
#include "Graphics/prt/prtSpriteGroupParticleGenerator.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
struct prtSpiralParticleGeneratorImp;


//============================================================================
//============================================================================
class prtSpiralParticleGenerator : public prtSpriteGroupParticleGenerator
{
	public:
		//--------------------------------------------------------------------
		//	The i_CreationTime is the simulation time at which the particle
		//	generator was created.
		//--------------------------------------------------------------------
		prtSpiralParticleGenerator(float i_CreationTime);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtSpiralParticleGenerator();

		//--------------------------------------------------------------------
		//	Animate is called by the scScene for each object before it
		//	is rendered.  Objects should use this function to set their
		//	model's visibility (according to scObject::GetRenderable) and
		//	animate if necessary.  The prtSpiralParticleGenerator::PreRender should
		//	be called by child classes before they do their PreRender work.
		//--------------------------------------------------------------------
		virtual void Animate(float i_SimulationTime);

		//--------------------------------------------------------------------
		//	These parameters can be used with the 
		//	prtParticleGenerator::SetParameter function.
		//--------------------------------------------------------------------
		enum Parameter
		{
			e_MinEmitSpeed = prtSpriteGroupParticleGenerator::e_NextParameter, 
										// moving speed of the center which particles rotate around
			e_MaxEmitSpeed,
			e_EmitDirectionX,			// moving direction of the center
			e_EmitDirectionY,
			e_EmitDirectionZ,
			e_AccelerationX,			// acceleration of the center which particles rotate around
			e_AccelerationY,			// 
			e_AccelerationZ,			// 
			e_MinRotStartAngle,			// min angle on radius that particles can start
			e_MaxRotStartAngle,			// max angle on radius that particles can start
			e_MinRotAngularVel,			// rotating speed (degrees/second)
			e_MaxRotAngularVel,			// rotating speed (degrees/second)
			e_RotRadius,				// distance to the axis
			e_RotRadiusScaleRate,		// speed at which particles gradually receding from or approaching the axis
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

		prtSpiralParticleGeneratorImp* m_pImp;
};
