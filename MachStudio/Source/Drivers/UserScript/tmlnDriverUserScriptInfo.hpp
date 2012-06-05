/********************************************************************************************\
**  tmlnDriverUserScriptInfo.hpp
**
**	Data structure for parsing UserScripts.
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef TMLN_DRIVERUSERSCRIPTINFO_HPP
#error tmlnDriverUserScriptInfo.hpp multiply included
#endif
#define TMLN_DRIVERUSERSCRIPTINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif


//============================================================================
//============================================================================
class tmlnDriverUserScriptInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverUserScriptInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverUserScriptInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

public:
	std::string m_Value;
};

