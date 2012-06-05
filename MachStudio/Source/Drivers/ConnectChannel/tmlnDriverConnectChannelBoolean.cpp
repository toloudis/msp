/*****************************************************************************
**	tmlnDriverConnectChannelBoolean.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelBoolean.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverConnectChannelBoolean::tmlnDriverConnectChannelBoolean(tmlnChannelBoolean &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverConnectChannelTemplate<tmlnChannelBoolean>(i_Channel, i_ChunkName)
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverConnectChannelBoolean::Operate(const maTime& i_Time)
{
	if (!IsBefore(i_Time))
	{
		//	check to make sure master name and channel are valid
		//	if not, try to correct it.
		//
		CheckMasterNameAndChannel();

		//	if master channel grab its value
		if (m_pMasterChannel)
		{
			bool goal = m_pMasterChannel->GetState();
			m_Channel.SetState(goal);
		}
	}

	// Because the master channel might change at any time without telling us,
	// we need to always operate
	this->MarkDirty();
}
