/*****************************************************************************
**	tmlnChannelAnimationSub.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/tmln/tmlnChannelAnimationSub.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Graphics/ent/entAnimation.hpp"
#include "Graphics/ent/entEntity.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelAnimationSub::tmlnChannelAnimationSub( const char* i_Name, api3dObjectEntity* i_pObject )
: tmlnChannel(i_Name, true), m_pCharacter(i_pObject)
{
}


//--------------------------------------------------------------------
// Add Sub animation by index
//--------------------------------------------------------------------
//virtual
void  tmlnChannelAnimationSub::AddSubAnimation( entAnimation* i_pAnimation, float i_SimulationTime )
{
	//DBG_LOG2( "AddSubAnim (%d)(%6.2f)", i_AnimIndex, i_SimulationTime );

	//	set the animation
	if (m_pCharacter && m_pCharacter->GetEntity())
	{
		const bool bPreserve = false;
		m_pCharacter->GetEntity()->AddSubAnimation( i_pAnimation, i_SimulationTime, bPreserve );
	}
}



//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelAnimationSub::Reset()
{
	// Because of the "Preserve" flag states in AddSubAnimation, this will stop
	// the custom subanimations, but will not remove the expressions.
	m_pCharacter->GetEntity()->ClearSubAnimations();
}

