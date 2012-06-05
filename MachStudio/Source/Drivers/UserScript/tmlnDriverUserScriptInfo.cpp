/*****************************************************************************
**	tmlnDriverUserScriptInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/UserScript/tmlnDriverUserScriptInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverUserScriptInfo::tmlnDriverUserScriptInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName), 
	m_Value("")
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverUserScriptInfo::~tmlnDriverUserScriptInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverUserScriptInfo::Clone()
{
	return new tmlnDriverUserScriptInfo(*this);
}
