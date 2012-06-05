/*****************************************************************************
**	chtrDriverConnectChannelAnimationFull.hpp
**
**		Derived ConnectChannel driver class from template for AnimationFull
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_DRIVERCONNECTCHANNELANIMATIONFULL_HPP
#error chtrDriverConnectChannelAnimationFull.hpp multiply included
#endif
#define CHTR_DRIVERCONNECTCHANNELANIMATIONFULL_HPP

#ifndef CHTR_CHANNELANIMATIONFULL_HPP
#include "chtrChannelAnimationFull.hpp"
#endif

#ifndef TMLN_DRIVERCONNECTCHANNELTEMPLATE_HPP
#include "tmlnDriverConnectChannelTemplate.hpp"
#endif


//============================================================================
//============================================================================
class chtrDriverConnectChannelAnimationFull : 
	public tmlnDriverConnectChannelTemplate<chtrChannelAnimationFull>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	chtrDriverConnectChannelAnimationFull(chtrChannelAnimationFull &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);
};

