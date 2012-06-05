/********************************************************************************************\
**  tmlnDriverAnimatedFilePathInfo.hpp
**
**	Data structure for parsing drivers
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERANIMATEDFILEPATHINFO_HPP
#error tmlnDriverAnimatedFilePathInfo.hpp multiply included
#endif
#define TMLN_DRIVERANIMATEDFILEPATHINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


class tmlnDriverAnimatedFilePathInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverAnimatedFilePathInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverAnimatedFilePathInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	// Texture filenames
	fsLocator	m_FirstFilePath;
	int			m_NumberOfFrames;

	float		m_FrameRate;
	bool		m_bLooping;
	bool		m_bReversing;
	bool		m_bDriverToAnimLength;
};



