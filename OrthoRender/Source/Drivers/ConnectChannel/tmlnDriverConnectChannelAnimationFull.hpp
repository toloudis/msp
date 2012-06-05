/*****************************************************************************
**	tmlnDriverConnectChannelAnimationFull.hpp
**
**		Derived ConnectChannel driver class from template for AnimationFull
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERCONNECTCHANNELANIMATIONFULL_HPP
#error tmlnDriverConnectChannelAnimationFull.hpp multiply included
#endif
#define TMLN_DRIVERCONNECTCHANNELANIMATIONFULL_HPP

#ifndef TMLN_DRIVERCONNECTCHANNELTEMPLATE_HPP
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelTemplate.hpp"
#endif
#ifndef TMLN_CHANNELAnimationFull_HPP
#include "Support/tmln/tmlnChannelAnimationFull.hpp"
#endif

//============================================================================
//============================================================================
class tmlnDriverConnectChannelAnimationFull : 
	public tmlnDriverConnectChannelTemplate<tmlnChannelAnimationFull>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverConnectChannelAnimationFull(tmlnChannelAnimationFull &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void Operate(float i_Time);
};

