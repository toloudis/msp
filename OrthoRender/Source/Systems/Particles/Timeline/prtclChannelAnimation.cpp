/*****************************************************************************
**	prtclChannelAnimation.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#include "Systems/Particles/Timeline/prtclChannelAnimation.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Graphics/prt/prtParticleGenerator.hpp"
#include "Graphics/prt/prtVertexAnimation.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclChannelAnimation::prtclChannelAnimation( const char* i_Name, prtParticleGenerator* i_pGenerator )
: tmlnChannel(i_Name), 
	m_pPrtGenerator(i_pGenerator), 
	m_pAnimation(NULL), 
	m_AnimTime(0)
{
}


//--------------------------------------------------------------------
//  Set the animation by name
//--------------------------------------------------------------------
//virtual
void  prtclChannelAnimation::SetAnimation( prtVertexAnimation* i_pAnimation, float i_SimulationTime )
{
	//	set the animation
	if (   (i_pAnimation != m_pAnimation) 
		|| (m_AnimTime != i_SimulationTime) )
	{
		if (i_pAnimation)
		{
			m_pPrtGenerator->SetAnimation( *i_pAnimation, i_SimulationTime);

			//bga - changing this so that emit channel is allowed to control pause/unpause
			// when we do not have any drivers
			if (this->GetNumDrivers() > 0)
			{
				m_pPrtGenerator->UnPause();
			}
		}
		else
			m_pPrtGenerator->ClearAnimation();
	}

	m_pAnimation = i_pAnimation;
	m_AnimTime = i_SimulationTime;

	// The user may be altering the frame rate while the
	// animation is currently playing, so we need to
	// make sure the anim_instance is always updated.
	prtVertexAnimInstance* pAnimInstance = m_pPrtGenerator->GetAnimInstance();
	if (pAnimInstance && m_pAnimation)
	{
		pAnimInstance->SetFrameRate( m_pAnimation->GetFrameRate() );
	}
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void prtclChannelAnimation::Reset()
{
	//DBG_LOG0( "ClearAnimation - Reset() channel " );
	m_pPrtGenerator->ClearAnimation();
	
	//bga - changing this so that emit channel is allowed to control pause/unpause
	// when we do not have any drivers
	if (this->GetNumDrivers() > 0)
	{
		m_pPrtGenerator->Pause();
	}
	
	m_pAnimation = NULL;
	m_AnimTime = 0;
}

