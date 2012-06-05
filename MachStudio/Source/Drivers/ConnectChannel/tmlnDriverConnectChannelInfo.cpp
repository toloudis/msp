/*****************************************************************************
**	tmlnDriverConnectChannelInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverConnectChannelInfo::tmlnDriverConnectChannelInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName), 
	m_Value("")
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverConnectChannelInfo::~tmlnDriverConnectChannelInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverConnectChannelInfo::Clone()
{
	return new tmlnDriverConnectChannelInfo(*this);
}
