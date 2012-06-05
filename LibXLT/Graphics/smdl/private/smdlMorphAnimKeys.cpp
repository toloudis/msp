/*****************************************************************************
**	smdlMorphAnimKeys.cpp
**
**		smdlMorphAnimKeys - animation information for blending
**	between morph targets over time.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlMorphAnimKeys.hpp"


//--------------------------------------------------------------------
//	Default constructor.
//--------------------------------------------------------------------
smdlMorphAnimKeys::smdlMorphAnimKeys()
:	m_Blend(NULL)
{
}


//--------------------------------------------------------------------
// GetBlend	- get linear interpolated translation key data
//--------------------------------------------------------------------
float smdlMorphAnimKeys::GetBlend(float i_Time) const
{
	if (m_Blend)
		return m_Blend->GetValue(i_Time);
	else
		return 0;
}

//--------------------------------------------------------------------
//	Mutators.  The smdlMorphAnimKeys does not own the animations passed
//	into these functions, nor does it copy them, so make sure you
//	don't delete the objects out from under it.  Because of this,
//	it is possible to reuse and share animation channels.
//--------------------------------------------------------------------
void smdlMorphAnimKeys::SetBlend(anKeyData<float>* i_Anim)
{
	m_Blend = i_Anim;
}
