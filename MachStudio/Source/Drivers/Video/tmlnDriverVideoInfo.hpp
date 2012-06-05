/********************************************************************************************\
**  tmlnDriverVideoInfo.hpp
**
**	Data structure for parsing drivers
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERVIDEOINFO_HPP
#error tmlnDriverVideoInfo.hpp multiply included
#endif
#define TMLN_DRIVERVIDEOINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


class tmlnDriverVideoInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverVideoInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverVideoInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	fsLocator	m_FileName;

	float		m_FrameRate;
	bool		m_bLooping;
	bool		m_bReversing;
	bool		m_bDriverToAnimLength;
};



