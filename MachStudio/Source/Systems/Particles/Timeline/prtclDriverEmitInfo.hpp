/********************************************************************************************\
**  prtclDriverEmitInfo.hpp
**
**	Data structure for parsing animation drivers
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef PRTCL_DRIVEREMITINFO_HPP
#error prtclDriverAnimationInfo.hpp multiply included
#endif
#define PRTCL_DRIVEREMITINFO_HPP

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


class prtclEmitInfo
{
public:
	int			m_AnimIndex;
	itString	m_AnimName;
	bool		m_bDriverToAnimLength;
};

class prtclDriverEmitInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	prtclDriverEmitInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~prtclDriverEmitInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	prtclEmitInfo m_Info;
};

