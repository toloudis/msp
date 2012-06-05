/*****************************************************************************
**	scParticleGenerator.cpp
**
**		scParticleGenerator is the base class for Terawatt particle
**	generators.  It stores a lot of different parameters which hopefully
**	apply to all Terawatt particle generators.
**	
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scParticleGenerator.hpp"


//--------------------------------------------------------------------
//	The i_CreationTime is the simulation time at which the particle
//	generator was created.
//--------------------------------------------------------------------
scParticleGenerator::scParticleGenerator( float i_CreationTime )
:	m_CreationTime(i_CreationTime),
	m_TimeDelta(0.0f),
	m_LastTime(-1.0f)
{
	SetPeriod( 11 );

	m_Flags.m_Expired	= false;
	m_Flags.m_Paused	= false;
	m_Flags.m_Finished	= false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
scParticleGenerator::~scParticleGenerator()
{
}

//--------------------------------------------------------------------
//	SetCreationTime allows for the resetting of the creation time
//	to restart the generation of the particles.
//--------------------------------------------------------------------
void scParticleGenerator::SetCreationTime( float i_CreationTime )
{
	m_CreationTime = i_CreationTime;
}

//--------------------------------------------------------------------
//	GetCreationTime returns the simulation time at which the particle
//	generator was created.
//--------------------------------------------------------------------
float scParticleGenerator::GetCreationTime() const
{
	return m_CreationTime;
}

//--------------------------------------------------------------------
//	SetGeneratorLifetime sets the length of time the generator is
//	supposed to exist.  The default is 1.0.
//--------------------------------------------------------------------
void scParticleGenerator::SetGeneratorLifetime(float i_Time)
{
	m_GeneratorLifetime = i_Time;
}

//--------------------------------------------------------------------
//	GetGeneratorLifetime returns the length of time the generator is
//	supposed to exist.
//--------------------------------------------------------------------
float scParticleGenerator::GetGeneratorLifetime() const
{
	return m_GeneratorLifetime;
}

//--------------------------------------------------------------------
//	Pause suspends creation of new particles
//--------------------------------------------------------------------
void scParticleGenerator::Pause()
{
	m_Flags.m_Paused = true;
}

//--------------------------------------------------------------------
//	UnPause resumes creation of new particles
//--------------------------------------------------------------------
void scParticleGenerator::UnPause()
{
	m_Flags.m_Paused = false;
}

//--------------------------------------------------------------------
//	GetPaused returns true if the generator is paused
//--------------------------------------------------------------------
bool scParticleGenerator::GetPaused()
{
	return m_Flags.m_Paused;
}

//--------------------------------------------------------------------
//	IsExpired returns true if the generator has lived beyond its
//	lifetime and should be destroyed.
//--------------------------------------------------------------------
bool scParticleGenerator::IsExpired()
{
	return m_Flags.m_Expired;
}

//--------------------------------------------------------------------
//	Reset() resets the expired + finished flags
//--------------------------------------------------------------------
void scParticleGenerator::Reset()
{
	m_Flags.m_Expired	= false;
	m_Flags.m_Finished	= false;
}

//--------------------------------------------------------------------
//	IsFinished returns true if the generator has lived beyond its
//	lifetime and is done rendering all of its particles and 
//	so should be destroyed.
//--------------------------------------------------------------------
bool scParticleGenerator::IsFinished()
{
	return m_Flags.m_Finished;
}

//--------------------------------------------------------------------
//	PreRender is called by the scScene for each object before it
//	is rendered.  Objects should use this function to set their
//	model's visibility (according to scObject::GetRenderable) and
//	animate if necessary.  The scParticleGenerator::PreRender should
//	be called by child classes before they do their PreRender work.
//--------------------------------------------------------------------
void scParticleGenerator::Animate(float i_SimulationTime)
{
	scObject::Animate( i_SimulationTime );

	if ( m_LastTime < 0 )
	{
		m_LastTime = i_SimulationTime;
	}

	m_TimeDelta	= i_SimulationTime - m_LastTime;
	m_LastTime	= i_SimulationTime;

	//	Pausing the generator effectively causes it's lifetime to 
	//	increase, so that it lasts that much longer
	if ( m_Flags.m_Paused )
	{
		m_GeneratorLifetime += m_TimeDelta;
	}

	if ( i_SimulationTime > (m_CreationTime + m_GeneratorLifetime) )
	{
		m_Flags.m_Expired = true;
	}
}

//--------------------------------------------------------------------
//	Child classes should call set finished when the generator is
//	expired and additionally there are no more particles to animate
//	(all particles have also expired).
//--------------------------------------------------------------------
void scParticleGenerator::SetFinished()
{
	m_Flags.m_Finished = true;
	//scObject::SetExpired();
}
