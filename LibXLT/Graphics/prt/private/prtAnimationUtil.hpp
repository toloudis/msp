/*****************************************************************************
**	prtAnimationUtil.hpp
**
**	prtAnimationUtil is a private helper for particle generator classes.
**	It provides a template class which has functions for animating particle
**	systems from baked data.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef	PRT_ANIMATIONUTIL_HPP
#error	prtAnimationUtil.hpp multiply included
#endif
#define	PRT_ANIMATIONUTIL_HPP

#ifndef PRT_PARTICLEMANAGER_HPP
#include "Graphics/prt/private/prtParticleManager.hpp"
#endif


//============================================================================
//============================================================================
namespace prtAnimationUtil
{
	//----------------------------------------------------------------------------
	// Animates particles according to the baked vertex animation keys.
	// Will alter the particles in the manager so that the correct ids exist
	// for this time. The creator should be a class that implements
	// ParticleType* CreateParticle(envType::Int32 i_ID, const maPoint3d& i_Position) const
	// in order to create a new particle of the correct type.
	//----------------------------------------------------------------------------
	template <class ParticleType, class ParticleCreator>
	maAxisBox Animate(const prtVertexAnimInstance& i_AnimInstance,
					  prtParticleManager<ParticleType>& io_Manager,
					  const ParticleCreator& i_Creator,
					  float i_SimulationTime,
					  float i_StreakTime);
};


//----------------------------------------------------------------------------
// Animates particles according to the baked vertex animation keys.
//----------------------------------------------------------------------------
template <class ParticleType, class ParticleCreator>
maAxisBox prtAnimationUtil::Animate(const prtVertexAnimInstance& i_AnimInstance,
								  prtParticleManager<ParticleType>& io_Manager,
								  const ParticleCreator& i_Creator,
								  float i_SimulationTime,
								  float i_StreakTime)
{
	maAxisBox anim_box;
	prtVertexFrame* pPrevFrame = NULL, *pNextFrame = NULL;
	float alpha = 1.0;
	i_AnimInstance.GetFramesForTime(i_SimulationTime, pPrevFrame, pNextFrame, alpha);
	if (pNextFrame)
	{
		// Get existing list and make it match with the current 
		// particle list 
		ParticleType* cur = io_Manager.GetHead();

		bool bInterpolate = (pPrevFrame != NULL && pPrevFrame != pNextFrame);
		int prev_index = 0;

		// If doing render streaks, get some frame info for this
		prtVertexFrame* pStreakPrevFrame = NULL, *pStreakNextFrame = NULL;
		float streak_alpha = 1.0;
		if (i_StreakTime > 0)
		{
			// By going back in time, we may need to interpolate between different frames
			// then for the current time
			i_AnimInstance.GetFramesForTime(i_SimulationTime-i_StreakTime, 
				pStreakPrevFrame, pStreakNextFrame, streak_alpha);
		}

		int num_particles = pNextFrame->m_Particles.size();
		for (int i=0; i<num_particles; i++)
		{
			const prtParticleAttr &part = pNextFrame->m_Particles[i];

			// Remove the ids before current id we are looking for (expired particles)
			while (cur && (cur->GetID() < part.m_ID))
			{
				ParticleType* temp = cur;
				cur = static_cast<ParticleType*>(cur->GetNext());
				io_Manager.RemoveParticle(temp);
			}

			// Get position and age of particle in animation, can be altered by interpolation later
			maPoint3d part_pos = part.m_Position;
			float part_age = part.m_Age;
			
			// If interpolating, find this particle id in the previous frame
			if (bInterpolate)
			{
				int num_prev = pPrevFrame->m_Particles.size();
				while ((prev_index <  num_prev)&& (pPrevFrame->m_Particles[prev_index].m_ID < part.m_ID))
				{
					prev_index++;
				}
				if ((prev_index <  num_prev) && (pPrevFrame->m_Particles[prev_index].m_ID == part.m_ID))
				{
					// Have previous position, interpolate position and age
					const prtParticleAttr &prev_part = pPrevFrame->m_Particles[prev_index];
					part_pos = part.m_Position * alpha + prev_part.m_Position * (1 - alpha);
					part_age = part.m_Age * alpha + prev_part.m_Age * (1 - alpha);

					// Note, lifetime does not need to be interpolated
				}
			}

			// If we don't have an existing particle with the correct ID
			// create a new one
			if (!cur || cur->GetID() > part.m_ID)
			{
				ParticleType* new_particle = i_Creator.CreateParticle(part.m_ID,
																	  part_pos);
				io_Manager.InsertParticle(cur, new_particle);
				cur = new_particle;
			}

			// Refresh the particle to age from Maya
			cur->SetAge(i_SimulationTime-part_age, part_age, part.m_Lifespan);

			// Think() updates aspects of the particle based on its age and lifespan
			cur->Think(i_SimulationTime, 0.0f);		

			// Overwrite the position that was calculated in Think()
			// with the interpolated baked value
			cur->SetPosition( part_pos );
			anim_box.Union( part_pos );

			// If streaking, set the last position to a point back in time slightly
			maPoint3d last_pos = part_pos;
			if (i_StreakTime > 0)
			{
				i_AnimInstance.GetPositionForParticle(part.m_ID, pStreakPrevFrame, pStreakNextFrame, streak_alpha, last_pos);
			}
			cur->SetLastPosition( last_pos ); 

			// Go to next particle
			cur = static_cast<ParticleType*>(cur->GetNext());
		}

		// If we have any particles left over, delete them
		if (cur != NULL)
		{
			// Get rid of expired particles
			io_Manager.DeleteParticlesAfter(cur);
		}
	}

	return anim_box;
}
