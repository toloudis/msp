/********************************************************************************************\
**  cmraDriverMayaScriptInfo.hpp
**
**	Data structure for parsing drivers
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERMAYASCRIPTINFO_HPP
#error cmraDriverMayaScriptInfo.hpp multiply included
#endif
#define CMRA_DRIVERMAYASCRIPTINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


class cmraDriverMayaScriptInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverMayaScriptInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverMayaScriptInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	fsLocator	m_AnimFilename;
	float		m_FrameRate;
	float		m_StartFrame;		// -1.0 is default / not set
	float		m_EndFrame;			// -1.0 is default / not set
	bool		m_bLooping;
	bool		m_bDriverToAnimLength;
	bool		m_bUseAnimStart;
	maPoint3d	m_Offset;
	float		m_CutThreshold;
	bool		m_bHasStereoChannels;
	bool		m_bAlterPosition;
	bool		m_bAlterTarget;
	bool		m_bAlterFov;
	bool		m_bAlterTilt;
	bool		m_bAlterFocalLength;
	bool		m_bAlterHorizontalAperture;
	bool		m_bAlterStereoFD;
	bool		m_bAlterStereoIOD;
};



