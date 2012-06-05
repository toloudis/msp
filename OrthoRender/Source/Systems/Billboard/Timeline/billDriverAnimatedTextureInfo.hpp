/********************************************************************************************\
**  billDriverAnimatedTextureInfo.hpp
**
**	Data structure for parsing drivers
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef BILL_DRIVERANIMATEDTEXTUREINFO_HPP
#error billDriverAnimatedTextureInfo.hpp multiply included
#endif
#define BILL_DRIVERANIMATEDTEXTUREINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif


class billDriverAnimatedTextureInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	billDriverAnimatedTextureInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~billDriverAnimatedTextureInfo();

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



