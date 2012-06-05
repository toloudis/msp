/*****************************************************************************
**	tmlnDriverUserScriptOrientation.hpp
**
**		Derived UserScript driver class from template for Orientation
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERUSERSCRIPTORIENTATION_HPP
#error tmlnDriverUserScriptOrientation.hpp multiply included
#endif
#define TMLN_DRIVERUSERSCRIPTORIENTATION_HPP

#ifndef TMLN_DRIVERUSERSCRIPTTEMPLATE_HPP
#include "Drivers/UserScript/tmlnDriverUserScriptTemplate.hpp"
#endif
#ifndef TMLN_CHANNELORIENTATION_HPP
#include "Support/tmln/tmlnChannelOrientation.hpp"
#endif
#ifndef TMLN_BLENDDRIVER_HPP
#include "Support/tmln/tmlnBlendDriver.hpp"
#endif

//============================================================================
//============================================================================
class tmlnDriverUserScriptOrientation : 
	public tmlnDriverUserScriptTemplate<tmlnChannelOrientation>,
	public tmlnBlendDriver<maRotation>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverUserScriptOrientation(tmlnChannelOrientation &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time);

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

