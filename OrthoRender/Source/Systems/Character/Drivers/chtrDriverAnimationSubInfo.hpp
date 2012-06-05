/********************************************************************************************\
**  chtrDriverAnimationSubInfo.hpp
**
**	Data structure for parsing animation drivers
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CHTR_DRIVERANIMATIONSUBINFO_HPP
#error chtrDriverAnimationInfo.hpp multiply included
#endif
#define CHTR_DRIVERANIMATIONSUBINFO_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <vector>


class chtrAnimationSubInfo
{
public:
	chtrAnimationSubInfo();

	itString	m_AnimFilename;
	bool		m_bDriverToAnimLength;

	// Anim Info that usually is in the .edf file
	std::string m_AnimName;
	//float m_TransitionTime;	// transition time between src and dst animations
	float m_FrameRate;
	float m_StartFrame;			// -1.0 is default / not set
	float m_EndFrame;			// -1.0 is default / not set
	bool  m_bLooping;
};

class chtrDriverAnimationSubInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	chtrDriverAnimationSubInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~chtrDriverAnimationSubInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	chtrAnimationSubInfo m_Info;
};

