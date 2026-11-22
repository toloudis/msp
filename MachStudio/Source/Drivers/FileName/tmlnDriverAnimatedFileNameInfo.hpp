/********************************************************************************************\
**  tmlnDriverAnimatedFileNameInfo.hpp
**
**	Data structure for parsing drivers
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERANIMATEDFILENAMEINFO_HPP
#error tmlnDriverAnimatedFileNameInfo.hpp multiply included
#endif
#define TMLN_DRIVERANIMATEDFILENAMEINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#include <string>


class tmlnDriverAnimatedFileNameInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverAnimatedFileNameInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverAnimatedFileNameInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	// Texture filenames
	std::string	m_FirstFileName;
	int			m_NumberOfFrames;

	float		m_FrameRate;
	bool		m_bLooping;
	bool		m_bDriverToAnimLength;
};



