/*****************************************************************************
**	prtVertexAnimation.cpp
**
**		A prtVertexAnimation
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtVertexAnimation.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	struct part_id_comp
	{
		int operator()(const prtParticleAttr& p1, const prtParticleAttr& p2) const
		{
			return (p1.m_ID < p2.m_ID);
		}
	};

	//----------------------------------------------------------------------------
	// a better search algorithm would be useful here
	//----------------------------------------------------------------------------
	int find_id(const prtVertexFrame *i_pFrame, envType::Int32 i_ID)
	{
		prtParticleAttr to_find;
		to_find.m_ID = i_ID;
		std::vector<prtParticleAttr>::const_iterator it =
			std::lower_bound(i_pFrame->m_Particles.begin(), i_pFrame->m_Particles.end(), to_find, part_id_comp());

		// If found,
		if ((it != i_pFrame->m_Particles.end()) && (it->m_ID == i_ID))
			return (it - i_pFrame->m_Particles.begin()); // convert iterator to index

		// linear search
		//int num_particles = i_pFrame->m_Particles.size();
		//for (int i=0; i<num_particles; i++)
		//{
		//	if (i_pFrame->m_Particles[i].m_ID == i_ID)
		//		return i;
		//	if (i_pFrame->m_Particles[i].m_ID > i_ID)
		//		break;
		//}
		return -1;
	}

} // end of namespace


//--------------------------------------------------------------------
//	The prtVertexAnimation requires references to animation
//	keys.  The frame animation does not own the keys in
//	order to allow sharing of key data between animations
//--------------------------------------------------------------------
prtVertexAnimation::prtVertexAnimation(const prtVertexAnimKeys& i_VertexKeys, 
	float i_FrameRate)
:	m_VertexKeys(i_VertexKeys)
{
	this->SetFrameRate(i_FrameRate);
	this->SetNumFrames( m_VertexKeys.GetFrames().GetLength() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtVertexAnimation::~prtVertexAnimation()
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const prtVertexAnimKeys& prtVertexAnimation::GetVertexKeys() const
{
	return m_VertexKeys;
}

//--------------------------------------------------------------------
//	CreateAnimInstance creates an anAnimInstance, of a type
//	appropriate to the type of the anAnimation.
//--------------------------------------------------------------------
anAnimInstance* prtVertexAnimation::CreateAnimInstance(float i_TimeOrigin) const
{
	return new prtVertexAnimInstance(i_TimeOrigin, *this);
}


//--------------------------------------------------------------------
//	Clone returns a copy of "this" allocated on the heap.
//--------------------------------------------------------------------
anAnimation* prtVertexAnimation::Clone() const
{
	return new prtVertexAnimation(*this);
}


//--------------------------------------------------------------------
//	The anFrameAnimInstance constructor requires the animation beginning
// time and the animation used by this instance.
//--------------------------------------------------------------------
prtVertexAnimInstance::prtVertexAnimInstance(float i_TimeOrigin,
									const prtVertexAnimation& i_Anim)
:	anFrameAnimInstance(i_TimeOrigin, i_Anim),
	m_Anim(i_Anim)
{

}

//--------------------------------------------------------------------
//	pure virtual destructor
//--------------------------------------------------------------------
prtVertexAnimInstance::~prtVertexAnimInstance()
{

}

//--------------------------------------------------------------------
//	The Clone function creates a new copy of the anFrameAnimInstance
//	on the heap.
//--------------------------------------------------------------------
anAnimInstance* prtVertexAnimInstance::Clone() const
{
	return m_Anim.CreateAnimInstance(this->GetTimeOrigin());
}

//--------------------------------------------------------------------
// Get frame data from simulation time.
//--------------------------------------------------------------------
prtVertexFrame* prtVertexAnimInstance::GetFrameForTime(float i_SimulationTime) const
{
	float cur_frame = this->ComputeFrame(i_SimulationTime);
	const prtVertexAnimKeys &keys = m_Anim.GetVertexKeys();

	if (keys.HasAnimation())
	{
		// Use baked animation to determine number and position of particles
		int key_index = keys.GetFrames().GetLowerKeyIndex(cur_frame);

		prtVertexFrame* pFrame = NULL;
		float key_time = 0;
		keys.GetFrames().GetKeyData(key_index, key_time, pFrame);

		return pFrame;
	}
	return NULL;
}

//--------------------------------------------------------------------
// Get frame data surrounding simulation time, used to interpolate
// positions between exact frames.
//--------------------------------------------------------------------
void prtVertexAnimInstance::GetFramesForTime(float i_SimulationTime,
											 prtVertexFrame* &o_pPrevFrame,
											 prtVertexFrame* &o_pNextFrame,
											 float &o_Alpha) const
{
	o_pPrevFrame = o_pNextFrame = NULL;
	float cur_frame = this->ComputeFrame(i_SimulationTime);
	const prtVertexAnimKeys &keys = m_Anim.GetVertexKeys();

	if (keys.HasAnimation())
	{
		// Use baked animation to determine number and position of particles
		int key1 = 0, key2 = 0;
		keys.GetFrames().GetBracketingKeyData(cur_frame, key1, key2);

		float key_time1 = 0, key_time2 = 0;
		keys.GetFrames().GetKeyData(key1, key_time1, o_pPrevFrame);
		keys.GetFrames().GetKeyData(key2, key_time2, o_pNextFrame);

		if ((key1 == key2) || (key_time2 == key_time1))
			o_Alpha = 1.0;
		else
			o_Alpha = (cur_frame - key_time1) / (key_time2 - key_time1);
	}
}

//--------------------------------------------------------------------
// Get position of particle with given ID at given time.
// Not as efficient as getting the bracketing frames and tracking 
// all the particles yourself. Returns false if a particle with
// given ID was not found.
//--------------------------------------------------------------------
bool prtVertexAnimInstance::GetPositionForParticle(float i_SimulationTime,
												   envType::Int32 i_ID,
												   maPoint3d& o_Position) const
{
	prtVertexFrame *pPrevFrame = NULL, *pNextFrame = NULL;
	float alpha = 1.0f;
	GetFramesForTime(i_SimulationTime, pPrevFrame, pNextFrame, alpha);

	return GetPositionForParticle(i_ID, pPrevFrame, pNextFrame, alpha, o_Position);
}
//--------------------------------------------------------------------
// This variation can be called after calling GetFramesForTime()
// once for the whole generator. This would be more efficient
// if you know that the streak length is the same for all particles.
//--------------------------------------------------------------------
bool prtVertexAnimInstance::GetPositionForParticle( envType::Int32 i_ID,
													const prtVertexFrame* i_pPrevFrame,
													const prtVertexFrame* i_pNextFrame,
													float i_Alpha,
													maPoint3d& o_Position) const
{
	bool bInterpolate = (i_pPrevFrame != NULL && i_pPrevFrame != i_pNextFrame);

	int next_index = find_id(i_pNextFrame, i_ID);
	if (next_index >= 0)
	{
		if (bInterpolate)
		{
			int prev_index = find_id(i_pPrevFrame, i_ID);
			if (prev_index >= 0)
			{
				o_Position = i_pNextFrame->m_Particles[next_index].m_Position * i_Alpha + 
							i_pPrevFrame->m_Particles[prev_index].m_Position * (1 - i_Alpha);
				return true;
			}
		}

		o_Position = i_pNextFrame->m_Particles[next_index].m_Position;
		return true;
	}

	return false;
}

