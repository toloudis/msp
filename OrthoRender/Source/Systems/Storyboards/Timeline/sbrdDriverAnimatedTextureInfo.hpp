/********************************************************************************************\
**  sbrdDriverAnimatedTextureInfo.hpp
**
**	Data structure for parsing drivers
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/

#ifdef SBRD_DRIVERANIMATEDTEXTUREINFO_HPP
#error sbrdDriverAnimatedTextureInfo.hpp multiply included
#endif
#define SBRD_DRIVERANIMATEDTEXTUREINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif


class sbrdDriverAnimatedTextureInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	sbrdDriverAnimatedTextureInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~sbrdDriverAnimatedTextureInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	// Texture filenames
	itString	m_FirstTextureFilename;
	int			m_NumberOfFrames;

	float		m_FrameRate;
	bool		m_bLooping;
	bool		m_bDriverResizeByAnimLength;
};



