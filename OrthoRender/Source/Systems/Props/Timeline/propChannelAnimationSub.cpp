/*****************************************************************************
**	propChannelAnimationSub.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Props/Timeline/propChannelAnimationSub.hpp"

#include "Core/Dbg/dbgLog.hpp"
#include "Graphics/Ent/entAnimation.hpp"
#include "Graphics/Ent/entEntity.hpp"
#include "Graphics/Ent/entEntityTemplate.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
propChannelAnimationSub::propChannelAnimationSub( const char* i_Name, api3dObjectEntity* i_pObject )
: tmlnChannel(i_Name, true), m_pProp(i_pObject)
{
}


//--------------------------------------------------------------------
// Add Sub animation by index
//--------------------------------------------------------------------
//virtual
void  propChannelAnimationSub::AddSubAnimation( int i_AnimIndex, float i_SimulationTime )
{
	//DBG_LOG2( "AddSubAnim (%d)(%6.2f)", i_AnimIndex, i_SimulationTime );

	//	set the animation
	entEntityTemplate* pEntityTemplate = m_pProp->GetEntityTemplate();
	if (pEntityTemplate)
	{
		const bool bPreserve = false;
		m_pProp->GetEntity()->AddSubAnimation( pEntityTemplate->GetAnimation(i_AnimIndex), 
			i_SimulationTime, bPreserve );
	}
}

//--------------------------------------------------------------------
//	get the animation list from the object
//--------------------------------------------------------------------
void propChannelAnimationSub::GetAnimationList( AnimMap& i_AnimMap )
{
	entEntity* pEntity = m_pProp->GetEntity();
	entEntityTemplate* pEntityTemplate = m_pProp->GetEntityTemplate();
	if (pEntityTemplate)
	{
		for ( int i = 0 ; i < pEntityTemplate->GetNumAnimations() ; i++ )
		{
			// HACK: hacky way to get the animation list
			//if ( pEntity->DoesAnimationExist(i) )
			{
				i_AnimMap[i] = pEntityTemplate->GetAnimName(i);
			}
		}
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
float propChannelAnimationSub::GetAnimationLength( int i_AnimIndex )
{
	if (m_pProp->GetEntityTemplate())
		return m_pProp->GetEntityTemplate()->GetAnimation(i_AnimIndex)->GetLength();
	else
		return 0.0f;
}


//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void propChannelAnimationSub::Reset()
{
	//m_pProp->GetEntity()->ClearAnimation();
}

