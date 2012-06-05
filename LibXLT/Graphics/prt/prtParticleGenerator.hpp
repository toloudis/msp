/*****************************************************************************
**	prtParticleGenerator.hpp
**
**		prtParticleGenerator is the base class for Terawatt particle
**	generators.  It stores a lot of different parameters which hopefully
**	apply to all Terawatt particle generators.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_PARTICLEGENERATOR_HPP
#error prtParticleGenerator.hpp multiply included
#endif
#define PRT_PARTICLEGENERATOR_HPP

#ifndef AN_TYPEDANIMATION_HPP
#include "Graphics/an/anTypedAnimation.hpp"
#endif
#ifndef SC_PARTICLEGENERATOR_HPP
#include "Graphics/sc/scParticleGenerator.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class prtEmitter;
class prtVertexAnimation;
class prtVertexAnimInstance;


//============================================================================
//============================================================================
class prtParticleGenerator : public scParticleGenerator
{
	public:
		//--------------------------------------------------------------------
		//	The i_CreationTime is the simulation time at which the particle
		//	generator was created.
		//--------------------------------------------------------------------
		prtParticleGenerator(float i_CreationTime);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtParticleGenerator() = 0;

		//--------------------------------------------------------------------
		//	SetOffsetPosition affects the position of the particle generator.
		//	If the generator is attached to a node of something, it will
		//	be displaced by this value in the node space.  This is different
		//	from the regular position which may be tracking a node exactly.
		//	(scMovableObject::SetPosition).
		//--------------------------------------------------------------------
		void SetOffsetPosition(const maPoint3d& i_Point);

		//--------------------------------------------------------------------
		//	SetOffsetDirection affects the direction of the particle
		//	generator.
		//--------------------------------------------------------------------
		void SetOffsetDirection(const maVector3d& i_Direction);

		//--------------------------------------------------------------------
		//	GetOffsetPosition returns the offset position.
		//--------------------------------------------------------------------
		const maPoint3d& GetOffsetPosition();

		//--------------------------------------------------------------------
		//	GetOffsetDirection returns the offset direction.
		//--------------------------------------------------------------------
		const maVector3d& GetOffsetDirection();

		//--------------------------------------------------------------------
		//	SetEmitter sets the emitter for the particle generator.  The
		//	particle generator takes ownership of the emitter, and it should
		//	be allocated on the heap.
		//--------------------------------------------------------------------
		void SetEmitter(prtEmitter* i_Emitter);

		//--------------------------------------------------------------------
		//	GetEmitter returns a pointer to the emitter being used by the
		//	particle generator.  If no emitter has been set, this could
		//	return NULL.  If PreRender is called and still no emitter has
		//	been set, the particle generator will make a prtPointEmitter.
		//--------------------------------------------------------------------
		const prtEmitter* GetEmitter() const;
		prtEmitter* GetEmitter();

		//--------------------------------------------------------------------
		//	PreRender is called by the scScene for each object before it
		//	is rendered.  Objects should use this function to set their
		//	model's visibility (according to scObject::GetRenderable) and
		//	animate if necessary.  The prtParticleGenerator::PreRender should
		//	be called by child classes before they do their PreRender work.
		//--------------------------------------------------------------------
		virtual void Animate(float i_SimulationTime);

		//--------------------------------------------------------------------
		//	PreSim handles simulating the generator for a set amount of time
		//	before activation
		//--------------------------------------------------------------------
		virtual void PreSim( float i_fBeginTime, float i_fDuration );

		//--------------------------------------------------------------------
		//	Do any generator specific calculations before the simulation
		//	starts running.
		//--------------------------------------------------------------------
		virtual void PreCalcSim( float i_fBeginTime, float i_fDuration );

		//--------------------------------------------------------------------
		//	The Parameter enum is a convenient way of handling some of the
		//	parameters of the particle generator.
		//--------------------------------------------------------------------
		enum Parameter
		{
			e_MinParticleLifetime = 0,		//	default 1.0
			e_MaxParticleLifetime,			//	default 2.0
			e_ParticleRate,					//	default 10.0
			e_MaxParticles,					//	default 1000.0
			e_PreSimTime,					//	default 0.0
			e_NextParameter
		};

		//--------------------------------------------------------------------
		//	GetNumParameters returns the number of parameters used by the
		//	particle generator.  This function should be overridden by 
		//	child classes which add parameters.
		//--------------------------------------------------------------------
		virtual int GetNumParameters() const;

		//--------------------------------------------------------------------
		//	SetParameter is used to set a variety of properties, indexed
		//	by prtParticleGenerator::Parameter.  Child classes can use this
		//	function as interface to their own parameters, also, by
		//	starting their enumerations at e_NextParameter.
		//--------------------------------------------------------------------
		void SetParameter(int i_Param, float i_Value);

		//--------------------------------------------------------------------
		//	GetParameter is used to get one of the parameters of the particle
		//	generator.  This function may also be used by child classes as an
		//	interface to their parameters.
		//--------------------------------------------------------------------
		float GetParameter(int i_Param) const;

		//--------------------------------------------------------------------
		//	SetAlphaProfile sets the animation of the particle alpha over
		//	its lifetime.  This animation should last 1 second (it will be
		//	scaled to whatever the actual particle lifetime).
		//	The animation will be cloned, so it is necessary to destroy the
		//	one passed as a parameter.
		//--------------------------------------------------------------------
		void SetAlphaProfile(const anTypedAnimation<float>& i_Anim);

		//--------------------------------------------------------------------
		//	GetAlphaProfile returns the animation of the particle alpha.
		//	If the user hasn't set a alpha profile, this could return NULL,
		//	in which case some sort of default should be used.
		//--------------------------------------------------------------------
		const anTypedAnimation<float>* GetAlphaProfile() const;

		//--------------------------------------------------------------------
		//	SetAnimation sets the given baked particle animation as the 
		//	current animation (but does not take ownership of it).  This
		//	will not blend the animation - just clobber the old one.
		//	A pointer to the created animation instance is returned.
		//--------------------------------------------------------------------
		prtVertexAnimInstance* SetAnimation(
							const prtVertexAnimation& i_Animation,
							float i_StartTime);

		//--------------------------------------------------------------------
		//	ClearAnimation removes the baked particle animation
		//--------------------------------------------------------------------
		void ClearAnimation();

		//--------------------------------------------------------------------
		//	Access to animation instance
		//--------------------------------------------------------------------
		prtVertexAnimInstance* GetAnimInstance();

		//--------------------------------------------------------------------
		// Call this to force an animation of the particles even if the
		//	time hasn't changed.
		//--------------------------------------------------------------------
		void MarkAnimDirty();

	protected:
		//	random number generator
		//maRand32*	m_pRandomGenerator;
		//int			m_RandomSeed;

	private:
		const anTypedAnimation<float>* m_pAlphaProfile;
		prtEmitter* m_pEmitter;
		std::vector<float> m_Parameters;
		maPoint3d m_OffsetPosition;
		maVector3d m_OffsetDirection;
		bool m_bPreSimSet;

	protected:
		prtVertexAnimInstance* m_pAnimInstance;
		float m_AnimStartTime;
		bool m_bAnimDirty;

	//	//	testing pre-calc'ing sim
	//private:
	//	struct psdata
	//	{
	//		float	StartTime;
	//		float	LifeTime;
	//		int		Seed;
	//	};
	//	std::vector<psdata> m_PreSimData;
};

