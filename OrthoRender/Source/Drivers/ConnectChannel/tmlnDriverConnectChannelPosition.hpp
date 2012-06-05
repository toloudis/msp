/*****************************************************************************
**	tmlnDriverConnectChannelPosition.hpp
**
**		Derived ConnectChannel driver class from template for Position
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERCONNECTCHANNELPOSITION_HPP
#error tmlnDriverConnectChannelPosition.hpp multiply included
#endif
#define TMLN_DRIVERCONNECTCHANNELPOSITION_HPP

#ifndef TMLN_DRIVERCONNECTCHANNELTEMPLATE_HPP
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelTemplate.hpp"
#endif
#ifndef TMLN_BLENDDRIVER_HPP
#include "Support/tmln/tmlnBlendDriver.hpp"
#endif
#ifndef TMLN_CHANNELPOSITION_HPP
#include "Support/tmln/tmlnChannelPosition.hpp"
#endif


//============================================================================
//============================================================================
class tmlnDriverConnectChannelPosition : 
	public tmlnDriverConnectChannelTemplate<tmlnChannelPosition>,
	public tmlnBlendDriver<maPoint3d>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverConnectChannelPosition(tmlnChannelPosition &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

//============================================================================
//	tmlnBlendDriver interface
//============================================================================

	//--------------------------------------------------------------------
	// Get the value of the driver at its begin time for this channel.
	//--------------------------------------------------------------------
	virtual void GetBeginValue(tmlnChannel* i_pChannel, maPoint3d& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the driver at its end time for this channel.
	//--------------------------------------------------------------------
	virtual void GetEndValue(tmlnChannel* i_pChannel, maPoint3d& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the gradient of the driver at its
	// end time in order to maintain tangent continuity while blending.
	//--------------------------------------------------------------------
	virtual void GetEndGradient(tmlnChannel* i_pChannel, maPoint3d& o_Gradient);
};

