/*****************************************************************************
**	chtrChannelAnimationSub.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Character/Timeline/chtrChannelAnimationSub.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Graphics/ent/entAnimation.hpp"
#include "Graphics/ent/entEntity.hpp"
#include "Graphics/ent/entEntityTemplate.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrChannelAnimationSub::chtrChannelAnimationSub( const char* i_Name, api3dObjectEntity* i_pObject )
: tmlnChannel(i_Name, true), m_pCharacter(i_pObject)
{
}


//--------------------------------------------------------------------
// Add Sub animation by index
//--------------------------------------------------------------------
//virtual
void  chtrChannelAnimationSub::AddSubAnimation( entAnimation* i_pAnimation, float i_SimulationTime )
{
	//DBG_LOG2( "AddSubAnim (%d)(%6.2f)", i_AnimIndex, i_SimulationTime );

	//	set the animation
	entEntityTemplate* pEntityTemplate = m_pCharacter->GetEntityTemplate();
	if (pEntityTemplate)
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
void chtrChannelAnimationSub::Reset()
{
	// Because of the "Preserve" flag states in AddSubAnimation, this will stop
	// the custom subanimations, but will not remove the expressions.
	m_pCharacter->GetEntity()->ClearSubAnimations();
}

