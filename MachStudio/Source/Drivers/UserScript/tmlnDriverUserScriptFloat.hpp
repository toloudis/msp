/*****************************************************************************
**	tmlnDriverUserScriptFloat.hpp
**
**		Derived UserScript driver class from template for Float
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERUSERSCRIPTFLOAT_HPP
#error tmlnDriverUserScriptFloat.hpp multiply included
#endif
#define TMLN_DRIVERUSERSCRIPTFLOAT_HPP

#ifndef TMLN_DRIVERUSERSCRIPTTEMPLATE_HPP
#include "Drivers/UserScript/tmlnDriverUserScriptTemplate.hpp"
#endif
#ifndef TMLN_CHANNELFLOAT_HPP
#include "Support/tmln/tmlnChannelFloat.hpp"
#endif
#ifndef TMLN_BLENDDRIVER_HPP
#include "Support/tmln/tmlnBlendDriver.hpp"
#endif 

//============================================================================
//============================================================================
class tmlnDriverUserScriptFloat : 
	public tmlnDriverUserScriptTemplate<tmlnChannelFloat>,
	public tmlnBlendDriver<float>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverUserScriptFloat(tmlnChannelFloat &i_Channel, chDefs::Name i_ChunkName);

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
	virtual void GetBeginValue(tmlnChannel* i_pChannel, float& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the driver at its end time for this channel.
	//--------------------------------------------------------------------
	virtual void GetEndValue(tmlnChannel* i_pChannel, float& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the gradient of the driver at its
	// end time in order to maintain tangent continuity while blending.
	//--------------------------------------------------------------------
	virtual void GetEndGradient(tmlnChannel* i_pChannel, float& o_Gradient);
};

