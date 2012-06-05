/*****************************************************************************
**	scParticleGenerator.hpp
**
**		scParticleGenerator is the base class for Terawatt particle
**	generators.  It stores a lot of different parameters which hopefully
**	apply to all Terawatt particle generators.
**	
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef SC_PARTICLEGENERATOR_HPP
#error scParticleGenerator.hpp multiply included
#endif
#define SC_PARTICLEGENERATOR_HPP

#ifndef SC_OBJECT_HPP
#include "Graphics/sc/scObject.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class scParticleGenerator : public scObject
{
	public:
		//--------------------------------------------------------------------
		//	The i_CreationTime is the simulation time at which the particle
		//	generator was created.
		//--------------------------------------------------------------------
		scParticleGenerator(float i_CreationTime);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~scParticleGenerator() = 0;

		//--------------------------------------------------------------------
		//	SetCreationTime allows for the resetting of the creation time
		//	to restart the generation of the particles.
		//--------------------------------------------------------------------
		void SetCreationTime( float i_CreationTime );

		//--------------------------------------------------------------------
		//	GetCreationTime returns the simulation time at which the particle
		//	generator was created.
		//--------------------------------------------------------------------
		float GetCreationTime() const;

		//--------------------------------------------------------------------
		//	SetGeneratorLifetime sets the length of time the generator is
		//	supposed to exist.  The default is 1.0.
		//--------------------------------------------------------------------
		void SetGeneratorLifetime(float i_Time);

		//--------------------------------------------------------------------
		//	GetGeneratorLifetime returns the length of time the generator is
		//	supposed to exist.
		//--------------------------------------------------------------------
		float GetGeneratorLifetime() const;

		//--------------------------------------------------------------------
		//	Pause suspends creation of new particles
		//--------------------------------------------------------------------
		void Pause();

		//--------------------------------------------------------------------
		//	UnPause resumes creation of new particles
		//--------------------------------------------------------------------
		void UnPause();

		//--------------------------------------------------------------------
		//	GetPaused returns true if the generator is paused
		//--------------------------------------------------------------------
		bool GetPaused();

		//--------------------------------------------------------------------
		//	IsFinished returns true if the generator has lived beyond its
		//	lifetime and is done rendering all of its particles and 
		//	so should be destroyed.
		//--------------------------------------------------------------------
		bool IsFinished();

		//--------------------------------------------------------------------
		//	IsExpired returns true if the generator should stop making
		//	particles (because it's lifetime is finished).
		//--------------------------------------------------------------------
		bool IsExpired();

		//--------------------------------------------------------------------
		//	Reset() resets the expired + finished flags
		//--------------------------------------------------------------------
		void Reset();

		//--------------------------------------------------------------------
		//	PreRender is called by the scScene for each object before it
		//	is rendered.  Objects should use this function to set their
		//	model's visibility (according to scObject::GetRenderable) and
		//	animate if necessary.  The scParticleGenerator::PreRender should
		//	be called by child classes before they do their PreRender work.
		//--------------------------------------------------------------------
		virtual void Animate(float i_SimulationTime);

		//--------------------------------------------------------------------
		//	DeleteAllParticles just take a guess what this one does. go ahead, guess
		//--------------------------------------------------------------------
		virtual void DeleteAllParticles() = 0;

		//--------------------------------------------------------------------
		//	ClearParticleAccumulation clears the variable that tracks the particle
		//	accumulation, essentially giving particle generation a clean slate
		//	to work with. Good for clearing out undesired accumulation that may
		//	occur during long pausessuch as mode changes
		//--------------------------------------------------------------------
		virtual void ClearParticleAccumulation() = 0;

	protected:
		//--------------------------------------------------------------------
		//	Child classes should call set finished when the generator is
		//	expired and additionally there are no more particles to animate
		//	(all particles have also expired).
		//--------------------------------------------------------------------
		void SetFinished();

		//--------------------------------------------------------------------
		//	GetTimeDelta returns the time elapsed since the last PreRender
		//--------------------------------------------------------------------
		float GetTimeDelta() const;

	private:
			
		float m_CreationTime;
		float m_GeneratorLifetime;
		float m_TimeDelta;
		float m_LastTime;
		
		struct
		{
			bool m_Expired		: 1;
			bool m_Finished		: 1;
			bool m_Paused		: 1;
		} m_Flags;
};


//--------------------------------------------------------------------
//	GetTimeDelta returns the time elapsed since the last PreRender
//--------------------------------------------------------------------
inline float scParticleGenerator::GetTimeDelta() const
{
	return m_TimeDelta;
}

