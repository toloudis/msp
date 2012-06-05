/*****************************************************************************
**	tmlnChannelAnimationSub.hpp
**
**	 Adapter for setting prop animation
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNELANIMATIONSUB_HPP
#error tmlnChannelAnimationSub.hpp multiply included
#endif
#define TMLN_CHANNELANIMATIONSUB_HPP

#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif

class api3dObjectEntity;
class entAnimation;

class tmlnChannelAnimationSub : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelAnimationSub( const char* i_Name, api3dObjectEntity* i_pObject );

	//--------------------------------------------------------------------
	// Add Sub animation by index
	//--------------------------------------------------------------------
	virtual void  AddSubAnimation(entAnimation* i_pAnimation, float i_SimulationTime);

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

private:
	api3dObjectEntity* m_pCharacter;
};
