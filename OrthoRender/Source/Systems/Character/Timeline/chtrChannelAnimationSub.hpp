/*****************************************************************************
**	chtrChannelAnimationSub.hpp
**
**	 Adapter for setting prop animation
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef CHTR_CHANNELANIMATIONSUB_HPP
#error chtrChannelAnimationSub.hpp multiply included
#endif
#define CHTR_CHANNELANIMATIONSUB_HPP

#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif

class api3dObjectEntity;
class entAnimation;

class chtrChannelAnimationSub : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	chtrChannelAnimationSub( const char* i_Name, api3dObjectEntity* i_pObject );

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
