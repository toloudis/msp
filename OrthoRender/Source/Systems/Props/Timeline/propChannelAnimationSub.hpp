/*****************************************************************************
**	propChannelAnimationSub.hpp
**
**	 Adapter for setting prop animation
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef PROP_CHANNELANIMATIONSUB_HPP
#error propChannelAnimationSub.hpp multiply included
#endif
#define PROP_CHANNELANIMATIONSUB_HPP

#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif
#ifndef API3D_OBJECTENTITY_HPP
#include "Tool/api3d/api3dObjectEntity.hpp"
#endif

#include <map>


class propChannelAnimationSub : public tmlnChannel
{
public:
	typedef std::map<int,std::string> AnimMap;  // anim index + anim name

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	propChannelAnimationSub( const char* i_Name, api3dObjectEntity* i_pObject );

	//--------------------------------------------------------------------
	// Add Sub animation by index
	//--------------------------------------------------------------------
	virtual void  AddSubAnimation(int i_AnimIndex, float i_SimulationTime);

	//--------------------------------------------------------------------
	//	get the animation list from the object
	//--------------------------------------------------------------------
	void GetAnimationList( AnimMap& i_AnimMap );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float GetAnimationLength( int i_AnimIndex );

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

private:
	api3dObjectEntity* m_pProp;
};
