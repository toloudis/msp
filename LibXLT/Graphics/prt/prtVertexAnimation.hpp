/*****************************************************************************
**	prtVertexAnimation.hpp
**
**		A prtVertexAnimation represents baked vertex animation.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef prt_VERTEXANIMATION_HPP
#error prtVertexAnimation.hpp multiply included
#endif
#define prt_VERTEXANIMATION_HPP

#ifndef AN_FRAMEANIMATION_HPP
#include "Graphics/an/anFrameAnimation.hpp"
#endif
#ifndef G3D_CONSTANTS_HPP
#include "Graphics/g3d/g3dConstants.hpp"
#endif
#ifndef PRT_VERTEXANIMKEYS_HPP
#include "Graphics/prt/prtVertexAnimKeys.hpp"
#endif


//============================================================================
//============================================================================
class prtVertexAnimation : public anFrameAnimation
{
	public:
		//--------------------------------------------------------------------
		//	The prtVertexAnimation requires references to animation
		//	keys.  The frame animation does not own the keys in
		//	order to allow sharing of key data between animations
		//--------------------------------------------------------------------
		prtVertexAnimation(const prtVertexAnimKeys& i_VertexKeys, 
			float i_FrameRate = g3dConstants::c_fDefaultFrameRate);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~prtVertexAnimation();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const prtVertexAnimKeys& GetVertexKeys() const;

		//--------------------------------------------------------------------
		//	CreateAnimInstance creates an anAnimInstance, of a type
		//	appropriate to the type of the anAnimation.
		//--------------------------------------------------------------------
		virtual anAnimInstance* CreateAnimInstance(float i_TimeOrigin) const;

		//--------------------------------------------------------------------
		//	Clone returns a copy of "this" allocated on the heap.
		//--------------------------------------------------------------------
		virtual anAnimation* Clone() const;

	private:

		const prtVertexAnimKeys& m_VertexKeys;

};


//============================================================================
//	prtVertexAnimInstance
//============================================================================
class prtVertexAnimInstance : public anFrameAnimInstance
{
	public:
		//--------------------------------------------------------------------
		//	The anFrameAnimInstance constructor requires the animation beginning
		// time and the animation used by this instance.
		//--------------------------------------------------------------------
		prtVertexAnimInstance(	float i_TimeOrigin,
								const prtVertexAnimation& i_Anim);

		//--------------------------------------------------------------------
		//	pure virtual destructor
		//--------------------------------------------------------------------
		virtual ~prtVertexAnimInstance();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline const prtVertexAnimation& GetAnim() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline const prtVertexAnimKeys& GetVertexKeys() const;

		//--------------------------------------------------------------------
		// Get frame data from simulation time.
		//--------------------------------------------------------------------
		prtVertexFrame* GetFrameForTime(float i_SimulationTime) const;

		//--------------------------------------------------------------------
		// Get frame data surrounding simulation time, used to interpolate
		// positions between exact frames. Alpha value returned is value
		// from 0 to 1 which defines time value between previous and next frames.
		//--------------------------------------------------------------------
		void GetFramesForTime(float i_SimulationTime,
							  prtVertexFrame* &o_pPrevFrame,
							  prtVertexFrame* &o_pNextFrame,
							  float &o_Alpha) const;

		//--------------------------------------------------------------------
		// Get position of particle with given ID at given time.
		// Not as efficient as getting the bracketing frames and tracking 
		// all the particles yourself. Returns false if a particle with
		// given ID was not found.
		//--------------------------------------------------------------------
		bool GetPositionForParticle(float i_SimulationTime,
								   envType::Int32 i_ID,
								   maPoint3d& o_Position) const;

		//--------------------------------------------------------------------
		// This variation can be called after calling GetFramesForTime()
		// once for the whole generator. This would be more efficient
		// if you know that the streak length is the same for all particles.
		//--------------------------------------------------------------------
		bool GetPositionForParticle( envType::Int32 i_ID,
									 const prtVertexFrame* i_pPrevFrame,
									 const prtVertexFrame* i_pNextFrame,
									 float i_Alpha,
									 maPoint3d& o_Position) const;

		//--------------------------------------------------------------------
		//	The Clone function creates a new copy of the anFrameAnimInstance
		//	on the heap.
		//--------------------------------------------------------------------
		virtual anAnimInstance* Clone() const;

	private:
		const	prtVertexAnimation& m_Anim;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const prtVertexAnimation& prtVertexAnimInstance::GetAnim() const
{
	return m_Anim;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const prtVertexAnimKeys& prtVertexAnimInstance::GetVertexKeys() const
{
	return m_Anim.GetVertexKeys();
}
