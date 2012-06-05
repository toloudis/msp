/*****************************************************************************
**	tmlnDriverConnectChannelBoolean.hpp
**
**		Derived ConnectChannel driver class from template for Boolean
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERCONNECTCHANNELBOOLEAN_HPP
#error tmlnDriverConnectChannelBoolean.hpp multiply included
#endif
#define TMLN_DRIVERCONNECTCHANNELBOOLEAN_HPP

#ifndef TMLN_DRIVERCONNECTCHANNELTEMPLATE_HPP
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelTemplate.hpp"
#endif
#ifndef TMLN_CHANNELBOOLEAN_HPP
#include "Support/tmln/tmlnChannelBoolean.hpp"
#endif

//============================================================================
//============================================================================
class tmlnDriverConnectChannelBoolean : 
	public tmlnDriverConnectChannelTemplate<tmlnChannelBoolean>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverConnectChannelBoolean(tmlnChannelBoolean &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time);
};

