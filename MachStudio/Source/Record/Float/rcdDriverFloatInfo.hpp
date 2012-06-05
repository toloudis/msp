/********************************************************************************************\
**  rcdDriverFloatInfo.hpp
**
**	Data structure for parsing sound drivers
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef RCD_DRIVERFLOATINFO_HPP
#error rcdDriverFloatInfo.hpp multiply included
#endif
#define RCD_DRIVERFLOATINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif


class rcdDriverFloatInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rcdDriverFloatInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~rcdDriverFloatInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();


	anKeyData<float> m_Keys;

};

