/********************************************************************************************\
**  tmlnDriverConnectChannelInfo.hpp
**
**	Data structure for parsing ConnectChannels.
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef TMLN_DRIVERCONNECTCHANNELINFO_HPP
#error tmlnDriverConnectChannelInfo.hpp multiply included
#endif
#define TMLN_DRIVERCONNECTCHANNELINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif


//============================================================================
//============================================================================
class tmlnDriverConnectChannelInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverConnectChannelInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverConnectChannelInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

public:
	nameString m_Value;
};

