/********************************************************************************************\
**  tmlnDriverAnimationFullInfo.hpp
**
**	Data structure for parsing animation drivers
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERANIMATIONFULLINFO_HPP
#error tmlnDriverAnimationInfo.hpp multiply included
#endif
#define TMLN_DRIVERANIMATIONFULLINFO_HPP


#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 
#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <vector>


class tmlnAnimationFullInfo
{
public:
	tmlnAnimationFullInfo();

	fsLocator	m_AnimFilename;
	bool		m_bDriverToAnimLength;
	bool		m_bUseAnimStart;

	// Anim Info that usually is in the .edf file
	std::string m_AnimName;
	//float m_TransitionTime;	// transition time between src and dst animations
	float m_FrameRate;
	float m_StartFrame;			// -1.0 is default / not set
	float m_EndFrame;			// -1.0 is default / not set
	bool  m_bLooping;
};

class tmlnDriverAnimationFullInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverAnimationFullInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverAnimationFullInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	tmlnAnimationFullInfo m_Info;
};

