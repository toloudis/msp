/********************************************************************************************\
**  tmlnDriverAnimatedTextureFileNameInfo.hpp
**
**	Data structure for parsing drivers
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERANIMATEDTEXTUREFILENAMEINFO_HPP
#error tmlnDriverAnimatedTextureFileNameInfo.hpp multiply included
#endif
#define TMLN_DRIVERANIMATEDTEXTUREFILENAMEINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif

class tmlnDriverAnimatedTextureFileNameInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverAnimatedTextureFileNameInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverAnimatedTextureFileNameInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	// Texture filenames
	prtyTextureFileName	m_FirstTextureFileName;
	int			m_NumberOfFrames;

	float		m_FrameRate;
	bool		m_bLooping;
	bool		m_bReversing;
	bool		m_bDriverToAnimLength;
};



