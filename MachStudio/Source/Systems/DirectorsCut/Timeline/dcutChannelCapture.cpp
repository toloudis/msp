/*****************************************************************************
**	dcutChannelCapture.cpp
**
**	 Channel to enable/disable dir lights
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Timeline/dcutChannelCapture.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Systems/DirectorsCut/Object/dcutDirectorsCutObject.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCapture.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutChannelCapture::dcutChannelCapture(const char* i_Name)//, dcutDirectorsCutObject* i_pCamera)
:	tmlnChannelBoolean(i_Name)//, 
//	m_pCamera(i_pCamera)
{

}

//--------------------------------------------------------------------
//  Set new state for object, needs to be implemented in
// derived class
//--------------------------------------------------------------------
void  dcutChannelCapture::SetState(bool i_bVal)
{
}

//--------------------------------------------------------------------
//  Get current state of object
//--------------------------------------------------------------------
bool  dcutChannelCapture::GetState() const
{
	return true;
}

//--------------------------------------------------------------------
//	Get a list of all the cameras and their in/out times
//--------------------------------------------------------------------
void dcutChannelCapture::GetInOutTimes(tmlnTimeInOutDataList& o_List)
{
	int i, count;
	count = GetNumDrivers();
	for (i=0; i<count; ++i)
	{
		dcutDriverCapture* pDriver = dynamic_cast<dcutDriverCapture*>(&Driver(i));
		if (pDriver != 0)
		{
			DBG_LOG3("in-out #%02d begin %6.3f end %6.3f", i, pDriver->GetBeginTime(), pDriver->GetEndTime() );
			o_List.AddTimeInOut( pDriver->GetBeginTime(),
								 pDriver->GetEndTime() );
		}
	}
}


