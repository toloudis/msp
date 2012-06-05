/*****************************************************************************
**	tmlnDriverConnectChannelOrientation.hpp
**
**		Derived ConnectChannel driver class from template for Orientation
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERCONNECTCHANNELORIENTATION_HPP
#error tmlnDriverConnectChannelOrientation.hpp multiply included
#endif
#define TMLN_DRIVERCONNECTCHANNELORIENTATION_HPP

#ifndef TMLN_DRIVERCONNECTCHANNELTEMPLATE_HPP
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelTemplate.hpp"
#endif
#ifndef TMLN_BLENDDRIVER_HPP
#include "Support/tmln/tmlnBlendDriver.hpp"
#endif
#ifndef TMLN_CHANNELORIENTATION_HPP
#include "Support/tmln/tmlnChannelOrientation.hpp"
#endif


//============================================================================
//============================================================================
class tmlnDriverConnectChannelOrientation : 
	public tmlnDriverConnectChannelTemplate<tmlnChannelOrientation>,
	public tmlnBlendDriver<maRotation>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverConnectChannelOrientation(tmlnChannelOrientation &i_Channel, chDefs::Name i_ChunkName);

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
	virtual void GetBeginValue(tmlnChannel* i_pChannel, maRotation& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the driver at its end time for this channel.
	//--------------------------------------------------------------------
	virtual void GetEndValue(tmlnChannel* i_pChannel, maRotation& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the gradient of the driver at its
	// end time in order to maintain tangent continuity while blending.
	//--------------------------------------------------------------------
	virtual void GetEndGradient(tmlnChannel* i_pChannel, maRotation& o_Gradient);
};

