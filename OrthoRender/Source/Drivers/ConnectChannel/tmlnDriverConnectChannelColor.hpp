/*****************************************************************************
**	tmlnDriverConnectChannelColor.hpp
**
**		Derived ConnectChannel driver class from template for Color
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERCONNECTCHANNELCOLOR_HPP
#error tmlnDriverConnectChannelColor.hpp multiply included
#endif
#define TMLN_DRIVERCONNECTCHANNELCOLOR_HPP

#ifndef TMLN_DRIVERCONNECTCHANNELTEMPLATE_HPP
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelTemplate.hpp"
#endif
#ifndef TMLN_CHANNELCOLOR_HPP
#include "Support/tmln/tmlnChannelColor.hpp"
#endif

//============================================================================
//============================================================================
class tmlnDriverConnectChannelColor : 
	public tmlnDriverConnectChannelTemplate<tmlnChannelColor>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverConnectChannelColor(tmlnChannelColor &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);
};

