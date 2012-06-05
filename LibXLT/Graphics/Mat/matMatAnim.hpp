/****************************************************************************\
**  matMatAnim.hpp
**
**      matMatAnim.hpp defines the matMatAnim object, which represents a
**	material animation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_MATANIM_HPP
#error matMatAnim.hpp multiply included
#endif
#define MAT_MATANIM_HPP

#ifndef AN_TYPEDANIMATION_HPP
#include "Graphics/an/anTypedAnimation.hpp"
#endif

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef MAT_MATPARAMINDEX_HPP
#include "Graphics/mat/matMatParamIndex.hpp"
#endif

#ifndef MA_VECTOR2D_HPP
#include "Core/ma/maVector2d.hpp"
#endif

class anAnimation;
class matMatAnimInstance;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class matMatAnim
{
	public:

		//--------------------------------------------------------------------
		//	The matMatAnim constructor requires a pointer to an anAnimation,
		//	which will be henceforth owned by the matMatAnim.
		//	The anAnimation is understood to operate
		//	on the i_Param given.
		//--------------------------------------------------------------------
		matMatAnim(	anAnimation* i_Anim,
					matMatParamIndex::matMatParamIndexType i_Param,
					float i_StartTime = 0.0f);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		matMatAnim(const matMatAnim& i_CopyFrom);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~matMatAnim();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		float GetStartTime() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetStartTime(float i_Time);

		//--------------------------------------------------------------------
		//	These Get functions return the various types of animations which
		//	are used by this matMatAnim.  If a given type is not used it is
		//	NULL.
		//--------------------------------------------------------------------
		const anTypedAnimation<float>* GetFloatAnimation() const;
		const anTypedAnimation<maFloatRGBA>* GetColorAnimation() const;
		const anTypedAnimation<maVector2d>* GetVectorAnimation() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		matMatParamIndex::matMatParamIndexType GetParamIndex() const;

		//--------------------------------------------------------------------
		// An inactive material animation does not update its material
		//--------------------------------------------------------------------
		inline bool IsActive() const;
		inline void SetActive(bool i_bActive);

		//--------------------------------------------------------------------
		// Set whether to use real time for animation, or use the simulation
		// time passed in to the renderer.
		//--------------------------------------------------------------------
		bool GetUseRealTime() const;
		void SetUseRealTime(bool i_bUseRealTime);

		//--------------------------------------------------------------------
		// Rescale will rescale based on the percentage given.
		// This number must be greater than zero, but can be as large as you
		// like if you want to triple or qudruple the length of the current
		// animation.
		//--------------------------------------------------------------------
		inline void Rescale( float i_fFactor );

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_anim_type();

		anAnimation* m_Animation;
		const anTypedAnimation<float>* m_FloatAnim;
		const anTypedAnimation<maFloatRGBA>* m_ColorAnim;
		const anTypedAnimation<maVector2d>* m_VectorAnim;
		matMatParamIndex::matMatParamIndexType m_Param;
		float m_StartTime;
		bool  m_bActive;
		bool  m_bUseRealTime;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline float matMatAnim::GetStartTime() const
{
	return m_StartTime;
}

//--------------------------------------------------------------------
//	These Get functions return the various types of animations which
//	are used by this matMatAnim.  If a given type is not used it is
//	NULL.
//--------------------------------------------------------------------
inline const anTypedAnimation<float>* matMatAnim::GetFloatAnimation() const
{
	return m_FloatAnim;
}

inline const anTypedAnimation<maFloatRGBA>* matMatAnim::GetColorAnimation() const
{
	return m_ColorAnim;
}

inline const anTypedAnimation<maVector2d>* matMatAnim::GetVectorAnimation() const
{
	return m_VectorAnim;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline matMatParamIndex::matMatParamIndexType matMatAnim::GetParamIndex() const
{
	return m_Param;
}

//--------------------------------------------------------------------
// An inactive material animation does not update its material
//--------------------------------------------------------------------
inline bool matMatAnim::IsActive() const
{
	return m_bActive;
}
inline void matMatAnim::SetActive(bool i_bActive)
{
	m_bActive = i_bActive;
}

//--------------------------------------------------------------------
// RescaleAnimation will rescale based on the percentage given.
// This number must be greater than zero, but can be as large as you
// like if you want to triple or qudruple the length of the current
// animation.
//--------------------------------------------------------------------
inline void matMatAnim::Rescale( float i_fFactor )
{
	m_Animation->Rescale(i_fFactor);
}
