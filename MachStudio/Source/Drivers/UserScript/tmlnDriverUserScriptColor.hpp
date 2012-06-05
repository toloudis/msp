/*****************************************************************************
**	tmlnDriverUserScriptColor.hpp
**
**		Derived UserScript driver class from template for Color
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERUSERSCRIPTCOLOR_HPP
#error tmlnDriverUserScriptColor.hpp multiply included
#endif
#define TMLN_DRIVERUSERSCRIPTCOLOR_HPP

#ifndef TMLN_DRIVERUSERSCRIPTTEMPLATE_HPP
#include "Drivers/UserScript/tmlnDriverUserScriptTemplate.hpp"
#endif
#ifndef TMLN_CHANNELCOLOR_HPP
#include "Support/tmln/tmlnChannelColor.hpp"
#endif

//============================================================================
//============================================================================
class tmlnDriverUserScriptColor : 
	public tmlnDriverUserScriptTemplate<tmlnChannelColor>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverUserScriptColor(tmlnChannelColor &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time);
};

