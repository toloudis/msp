/*****************************************************************************
**	tmlnDriverSoundInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Sound/tmlnDriverSoundInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverSoundInfo::tmlnDriverSoundInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName),
	m_fSoundStartTime(0.0f),
	m_fSoundEndTime(0.0f),
	m_bSetToSoundLength(true),
	m_bNeedsLoad(false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverSoundInfo::~tmlnDriverSoundInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverSoundInfo::Clone()
{
	return new tmlnDriverSoundInfo(*this);
}
