/********************************************************************************************\
**  tmlnDriverColorFlickerInfo.hpp
**
**	Data structure for parsing sound drivers
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERCOLORFLICKERINFO_HPP
#error tmlnDriverColorFlickerInfo.hpp multiply included
#endif
#define TMLN_DRIVERCOLORFLICKERINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif


class tmlnDriverColorFlickerInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverColorFlickerInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverColorFlickerInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();


	//========================================================================
	//	variables
	//========================================================================
	maFloatRGBA m_StartColor1;
	maFloatRGBA m_StartColor2;
	maFloatRGBA m_EndColor1;
	maFloatRGBA m_EndColor2;
	float		m_fFrequency;		// in seconds
};

