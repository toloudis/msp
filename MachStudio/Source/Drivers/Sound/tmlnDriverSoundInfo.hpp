/********************************************************************************************\
**  tmlnDriverSoundInfo.hpp
**
**	Data structure for parsing sound drivers
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERSOUNDINFO_HPP
#error tmlnDriverSoundInfo.hpp multiply included
#endif
#define TMLN_DRIVERSOUNDINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 


//============================================================================
//============================================================================
class tmlnDriverSoundInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverSoundInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverSoundInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

public:
	fsLocator	m_SoundName;
	float		m_fSoundStartTime;
	float		m_fSoundEndTime;
	bool		m_bSetToSoundLength;
	
	bool		m_bNeedsLoad;
};

