/*****************************************************************************
**	tmlnDriverUserScriptPosition.hpp
**
**		Derived UserScript driver class from template for Position
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERUSERSCRIPTPOSITION_HPP
#error tmlnDriverUserScriptPosition.hpp multiply included
#endif
#define TMLN_DRIVERUSERSCRIPTPOSITION_HPP

#ifndef TMLN_DRIVERUSERSCRIPTTEMPLATE_HPP
#include "Drivers/UserScript/tmlnDriverUserScriptTemplate.hpp"
#endif
#ifndef TMLN_CHANNELPOSITION_HPP
#include "Support/tmln/tmlnChannelPosition.hpp"
#endif
#ifndef TMLN_BLENDDRIVER_HPP
#include "Support/tmln/tmlnBlendDriver.hpp"
#endif 

//============================================================================
//============================================================================
class tmlnDriverUserScriptPosition : 
	public tmlnDriverUserScriptTemplate<tmlnChannelPosition>,
	public tmlnBlendDriver<maPoint3d>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverUserScriptPosition(tmlnChannelPosition &i_Channel, chDefs::Name i_ChunkName);

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

