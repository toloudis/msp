/*****************************************************************************
**	tmlnDriverUserScriptBoolean.hpp
**
**		Derived UserScript driver class from template for Boolean
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERUSERSCRIPTBOOLEAN_HPP
#error tmlnDriverUserScriptBoolean.hpp multiply included
#endif
#define TMLN_DRIVERUSERSCRIPTBOOLEAN_HPP

#ifndef TMLN_DRIVERUSERSCRIPTTEMPLATE_HPP
#include "Drivers/UserScript/tmlnDriverUserScriptTemplate.hpp"
#endif
#ifndef TMLN_CHANNELBOOLEAN_HPP
#include "Support/tmln/tmlnChannelBoolean.hpp"
#endif

//============================================================================
//============================================================================
class tmlnDriverUserScriptBoolean : 
	public tmlnDriverUserScriptTemplate<tmlnChannelBoolean>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverUserScriptBoolean(tmlnChannelBoolean &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time);
};

