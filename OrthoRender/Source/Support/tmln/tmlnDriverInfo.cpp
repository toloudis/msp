/*****************************************************************************
**	tmlnDriverInfo.cpp
**
**	Base class for parsing data structures for drivers
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Support/tmln/tmlnDriverInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverInfo::tmlnDriverInfo(chDefs::Name i_ChunkName)
: chParsable(i_ChunkName), m_Name(""), m_BeginTime(0.0f), m_EndTime(0.0f),
	m_BlendType(0), m_BlendTime(0.0f), 
	m_EaseInWeight(1.0f), m_EaseOutWeight(1.0f), 
	m_bRestoreOriginal(false),
	m_DriverId(0)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverInfo::~tmlnDriverInfo()
{

}
