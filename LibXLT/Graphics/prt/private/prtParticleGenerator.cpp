/*****************************************************************************
**	prtParticleGenerator.cpp
**
**		prtParticleGenerator is the base class for library particle
**	generators.  It stores a lot of different parameters which hopefully
**	apply to all library particle generators.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtParticleGenerator.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/prt/prtPointEmitter.hpp"
#include "Graphics/prt/prtVertexAnimation.hpp"

#include <algorithm>
#include <iterator>

//--------------------------------------------------------------------
//	The i_CreationTime is the simulation time at which the particle
//	generator was created.
//--------------------------------------------------------------------
prtParticleGenerator::prtParticleGenerator(float i_CreationTime)
:	scParticleGenerator(i_CreationTime),
	m_pEmitter(NULL),
	m_pAlphaProfile(NULL),
	m_Parameters(prtParticleGenerator::e_NextParameter),
	m_OffsetPosition(0, 0, 0),
	m_OffsetDirection(0, 1, 0),
	m_bPreSimSet(false),
	m_pAnimInstance(NULL),
	m_bAnimDirty(false)
{
	SetPeriod( 11 );

	this->SetParameter(e_MinParticleLifetime, 1.0f);
	this->SetParameter(e_MaxParticleLifetime, 2.0f);
	this->SetParameter(e_ParticleRate, 10.0f);
	this->SetParameter(e_MaxParticles, 1000.0f);
	this->SetParameter(e_PreSimTime, 0.0f);

//	m_pRandom = new maRand32( 100 );	// hard-code seed for now.
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtParticleGenerator::~prtParticleGenerator()
{
//	delete m_pRandom;
	delete m_pEmitter;
}

//--------------------------------------------------------------------
//	SetOffsetPosition affects the position of the particle generator.
//	If the generator is attached to a node of something, it will
//	be displaced by this value in the node space.  This is different
//	from the regular position which may be tracking a node exactly.
//	(scMovableObject::SetPosition).
//--------------------------------------------------------------------
void prtParticleGenerator::SetOffsetPosition(const maPoint3d& i_Point)
{
	m_OffsetPosition = i_Point;
}

//--------------------------------------------------------------------
//	SetOffsetDirection affects the direction of the particle
//	generator.
//--------------------------------------------------------------------
void prtParticleGenerator::SetOffsetDirection(const maVector3d& i_Direction)
{
	m_OffsetDirection = i_Direction;
}

//--------------------------------------------------------------------
//	GetOffsetPosition returns the offset position.
//--------------------------------------------------------------------
const maPoint3d& prtParticleGenerator::GetOffsetPosition()
{
	return m_OffsetPosition;
}

//--------------------------------------------------------------------
//	GetOffsetDirection returns the offset direction.
//--------------------------------------------------------------------
const maVector3d& prtParticleGenerator::GetOffsetDirection()
{
	return m_OffsetDirection;
}

//--------------------------------------------------------------------
//	SetEmitter sets the emitter for the particle generator.  The
//	particle generator takes ownership of the emitter, and it should
//	be allocated on the heap.
//--------------------------------------------------------------------
void prtParticleGenerator::SetEmitter(prtEmitter* i_Emitter)
{
	delete m_pEmitter;
	m_pEmitter = i_Emitter;
}

//--------------------------------------------------------------------
//	GetEmitter returns a pointer to the emitter being used by the
//	particle generator.  If no emitter has been set, this could
//	return NULL.  If PreRender is called and still no emitter has
//	been set, the particle generator will make a prtPointEmitter.
//--------------------------------------------------------------------
const prtEmitter* prtParticleGenerator::GetEmitter() const
{
	return m_pEmitter;
}

prtEmitter* prtParticleGenerator::GetEmitter()
{
	return m_pEmitter;
}

//--------------------------------------------------------------------
//	Do any generator specific calculations before the simulation
//	starts running.
//--------------------------------------------------------------------
//virtual 
void prtParticleGenerator::PreCalcSim( float i_fBeginTime, float i_fDuration )
{
	//float minlife	= this->GetParameter(e_MinParticleLifetime);
	//float maxlife	= this->GetParameter(e_MaxParticleLifetime);
	//float rateinc	= 1.0f / this->GetParameter(e_ParticleRate);
	//float maxpart	= this->GetParameter(e_MaxParticles);
	//float rate = 0.0f;

	//int simsize = ((int)(i_fDuration+1) * 30);
	//m_PreSimData.resize(simsize);
	//for (int i = 0; i<simsize; ++i)
	//{
	//	m_PreSimData[i].Seed = m_pRandom->Rand();

	//	float pct = m_pRandom->Rand() / m_pRandom->RandMax();
	//	m_PreSimData[i].StartTime = pct * 1.0f + i_fBeginTime;	// incomplete
	//	pct = m_pRandom->Rand() / m_pRandom->RandMax();
	//	m_PreSimData[i].LifeTime = 0;

	//	rate += rateinc;
	//}
	//maRand32* m_pRandom;
}


//--------------------------------------------------------------------
//	PreRender is called by the scScene for each object before it
//	is rendered.  Objects should use this function to set their
//	model's visibility (according to scObject::GetRenderable) and
//	animate if necessary.  The prtParticleGenerator::PreRender should
//	be called by child classes before they do their PreRender work.
//--------------------------------------------------------------------
void prtParticleGenerator::Animate(float i_SimulationTime)
{
	//	calculate the sim time
	//
	float simtime = i_SimulationTime;
	if (m_bPreSimSet)
	{
		simtime += GetParameter( e_PreSimTime );
	}

	//	animate the generator/particles
	scParticleGenerator::Animate( simtime );

	//	create an emitter if there isn't one already
	if( m_pEmitter == NULL )
	{
		m_pEmitter = new prtPointEmitter;
	}

	m_bAnimDirty = false;
}

//--------------------------------------------------------------------
//	PreSim handles simulating the generator for a set amount of time
//	before activation
//--------------------------------------------------------------------
//virtual 
void prtParticleGenerator::PreSim( float i_fBeginTime, float i_fDuration )
{
	m_bPreSimSet = false;

	float presimtime = GetParameter(e_PreSimTime);
	if ( presimtime <= 0.0f )
	{
		//	no presim time so jump out
		return;
	}

	//	update the total transform so it will be correct during this presim
	//
	this->GetBase()->UpdateTotalTransform();

	//	Calculate pre-sim info
	//
	//PreCalcSim( i_fBeginTime, i_fDuration );

	// save the generator lifetime so it can be reset
	//
	float lifetime;
	lifetime = this->GetGeneratorLifetime();

	float simtime = 0.0f;
	while ( simtime < presimtime )
	{
		Animate( simtime );

		simtime += 0.1f;						// increment some constant
		this->SetGeneratorLifetime(lifetime);	// reset the lifetime so it doesn't accidentally get set
	}

	m_bPreSimSet = true;
}

//--------------------------------------------------------------------
//	GetNumParameters returns the number of parameters used by the
//	particle generator.  This function should be overridden by
//	child classes which add parameters.
//--------------------------------------------------------------------
int prtParticleGenerator::GetNumParameters() const
{
	return e_NextParameter;
}

//--------------------------------------------------------------------
//	SetParameter is used to set a variety of properties, indexed
//	by prtParticleGenerator::Parameter.  Child classes can use this
//	function as interface to their own parameters, also, by
//	starting their enumerations at e_NextParameter.
//--------------------------------------------------------------------
void prtParticleGenerator::SetParameter(int i_Param, float i_Value)
{
	DBG_ASSERT(i_Param < 1000, "parameter index too large");
	if (i_Param >= 1000)
		return;

	if( i_Param >= m_Parameters.size() )
		std::fill_n(std::back_inserter(m_Parameters), i_Param - m_Parameters.size() + 1, 0.0f);

	m_Parameters[i_Param] = i_Value;
}

//--------------------------------------------------------------------
//	GetParameter is used to get one of the parameters of the particle
//	generator.  This function may also be used by child classes as an
//	interface to their parameters.
//--------------------------------------------------------------------
float prtParticleGenerator::GetParameter(int i_Param) const
{
	DBG_ASSERT(i_Param < 1000, "parameter index too large");
	if (i_Param >= 1000)
		return 0.0f;

	if( i_Param >= m_Parameters.size() )
		return 0.0f;
	else
		return m_Parameters[i_Param];
}

//--------------------------------------------------------------------
//	SetAlphaProfile sets the animation of the particle alpha over
//	its lifetime.  This animation should last 1 second (it will be
//	scaled to whatever the actual particle lifetime).
//	The animation will be cloned, so it is necessary to destroy the
//	one passed as a parameter.
//--------------------------------------------------------------------
void prtParticleGenerator::SetAlphaProfile(const anTypedAnimation<float>& i_Anim)
{
	m_pAlphaProfile = &i_Anim;
}

//--------------------------------------------------------------------
//	GetAlphaProfile returns the animation of the particle alpha.
//	If the user hasn't set a alpha profile, this could return NULL,
//	in which case some sort of default should be used.
//--------------------------------------------------------------------
const anTypedAnimation<float>* prtParticleGenerator::GetAlphaProfile() const
{
	return m_pAlphaProfile;
}

//--------------------------------------------------------------------
//	SetAnimation sets the given baked particle animation as the 
//	current animation (but does not take ownership of it).  This
//	will not blend the animation - just clobber the old one.
//	A pointer to the created animation instance is returned.
//--------------------------------------------------------------------
prtVertexAnimInstance* prtParticleGenerator::SetAnimation(
					const prtVertexAnimation& i_Animation,
					float i_StartTime)
{
	this->ClearAnimation();

	m_bAnimDirty = true;

	m_pAnimInstance = dynamic_cast<prtVertexAnimInstance*>(
					i_Animation.CreateAnimInstance(i_StartTime));

	m_AnimStartTime = i_StartTime;
	return m_pAnimInstance;
}


//--------------------------------------------------------------------
//	ClearAnimation removes the baked particle animation
//--------------------------------------------------------------------
void prtParticleGenerator::ClearAnimation()
{
	delete m_pAnimInstance;
	m_pAnimInstance = NULL;
}

//--------------------------------------------------------------------
//	Access to animation instance
//--------------------------------------------------------------------
prtVertexAnimInstance* prtParticleGenerator::GetAnimInstance()
{
	return m_pAnimInstance;
}

//--------------------------------------------------------------------
// Call this to force an animation of the particles even if the
//	time hasn't changed.
//--------------------------------------------------------------------
void prtParticleGenerator::MarkAnimDirty()
{
	m_bAnimDirty = true;
}
