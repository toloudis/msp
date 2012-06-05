/*****************************************************************************
**	prtclChannelAnimation.hpp
**
**	 Channel for playing baked particle animation
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_CHANNELANIMATION_HPP
#error prtclChannelAnimation.hpp multiply included
#endif
#define PRTCL_CHANNELANIMATION_HPP

#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif

//============================================================================
//============================================================================
class prtParticleGenerator;
class prtVertexAnimation;


//============================================================================
//============================================================================
class prtclChannelAnimation : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	prtclChannelAnimation( const char* i_Name, prtParticleGenerator* i_pGenerator );

	//--------------------------------------------------------------------
	//  Set the animation by name
	//--------------------------------------------------------------------
	virtual void  SetAnimation( prtVertexAnimation* i_pAnimation, float i_SimulationTime);

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline prtVertexAnimation* GetAnimation();

private:
	prtParticleGenerator* m_pPrtGenerator;
	prtVertexAnimation* m_pAnimation;
	float m_AnimTime;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline prtVertexAnimation* prtclChannelAnimation::GetAnimation()
{
	return m_pAnimation;
}
