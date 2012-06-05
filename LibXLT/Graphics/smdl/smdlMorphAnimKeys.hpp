/*****************************************************************************
**	smdlMorphAnimKeys.hpp
**
**		smdlMorphAnimKeys - animation information for blending
**	between morph targets over time.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_MORPHANIMKEYS_HPP
#error smdlMorphAnimKeys.hpp multiply included
#endif
#define SMDL_MORPHANIMKEYS_HPP

#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif


//============================================================================
//============================================================================
class smdlMorphAnimKeys
{
	public:
		//--------------------------------------------------------------------
		//	Default constructor.
		//--------------------------------------------------------------------
		smdlMorphAnimKeys();

		//--------------------------------------------------------------------
		//	HasAnimation - returns true if some channel has animation
		//--------------------------------------------------------------------
		inline bool	HasAnimation() const;

		//--------------------------------------------------------------------
		// GetBlend	- get linear interpolated translation key data
		//--------------------------------------------------------------------
		float GetBlend(float i_Time) const;

		//--------------------------------------------------------------------
		//	Accessors - const and non-const.
		//--------------------------------------------------------------------
		anKeyData<float>* GetBlend();
		const anKeyData<float>* GetBlend() const;

		//--------------------------------------------------------------------
		//	Mutators.  The smdlMorphAnimKeys does not own the animations passed
		//	into these functions, nor does it copy them, so make sure you
		//	don't delete the objects out from under it.  Because of this,
		//	it is possible to reuse and share animation channels.
		//--------------------------------------------------------------------
		void SetBlend(anKeyData<float>* i_Anim);

	private:
		anKeyData<float>* m_Blend;
};


//--------------------------------------------------------------------
//	HasAnimation - returns true if some channel has animation
//--------------------------------------------------------------------
inline bool	smdlMorphAnimKeys::HasAnimation() const
{
	return (m_Blend != NULL);
}

//--------------------------------------------------------------------
//	Accessors - const and non-const.
//--------------------------------------------------------------------
inline anKeyData<float>*
smdlMorphAnimKeys::GetBlend()
{
	return m_Blend;
}

inline const anKeyData<float>*
smdlMorphAnimKeys::GetBlend() const
{
	return m_Blend;
}

